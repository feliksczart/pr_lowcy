#include "hunters.h"
#include "monitor.h"
#include <iostream>

HuntersState Hunters::state = HuntersState::WAITING_ORDER;

void Hunters::loop(int size, int rank){
	//wątek oczekujący na nowe zlecenia
	pthread_t incomingMissionThread;
	pthread_create(&incomingMissionThread,NULL,&incomingMissionMonitor,NULL);
	
	while(1){
		pthread_mutex_lock(&Monitor::incomingMissionMutex);
		pthread_mutex_lock(&Monitor::messageQMutex);
		if(!Monitor::messageQ.empty()){
			sleep(2);
			packet_t packet = Monitor::messageQ.front();
			Monitor::messageQ.pop();
			if(Hunters::state == HuntersState::WAITING_ORDER){
				sleep(rand()%3+1);
				std::cout << GREEN << Monitor::getLamport() << ": Łowca " << rank << " otrzymał wiadomość o dostępnym zleceniu nr " << packet.orderNumber << RESET << std::endl;
			} else if(Hunters::state == HuntersState::TRYING_ORDER){
				std::cout << WHITE << Monitor::getLamport() << ": Łowca " << rank << " otrzymał request o przydzielenie zlecenia " << packet.orderNumber << " dla łowcy " << packet.from << RESET << std::endl;	
			}
			Hunters::handleNewMessage(packet);
		}
                        
		pthread_mutex_unlock(&Monitor::messageQMutex);
		pthread_mutex_unlock(&Monitor::incomingMissionMutex);

	}
	pthread_join(incomingMissionThread,NULL);
}

void *incomingMissionMonitor (void* x) {
	Monitor::listen();
}	

void Hunters::handleNewMessage(packet_t packet){
	if(packet.tag == NEW_MISSION){
		Hunters::state = HuntersState::TRYING_ORDER;
		
		Monitor::mission_q.push_back(std::make_pair(Monitor::newMissionTimestamp,Monitor::rank));
	       	Monitor::missions_queues.insert(std::make_pair(packet.orderNumber,Monitor::mission_q));	
		//Monitor::print_map(Monitor::missions_queues);
		//sleep(rand()%2+1);
		std::cout << YELLOW << packet.lamport+1 << ": Łowca " << Monitor::rank << " pyta innych czy może wyruszyć na misję"  << RESET << std::endl;
		Hunters::sendOrderReq(packet);
	} else if(packet.tag == ORDER_REQ){
		auto firstInQueue = Monitor::missions_queues.find(packet.orderNumber);
		unsigned int anotherTimestamp = Monitor::incomingMissReqTimestamp;
		//std::cout << BLUE << Monitor::missions_queues.find(packet.orderNumber)->second.size() << RESET << std::endl;
		std::cout << RED << "Time:" << packet.lamport << " Lowca:" << packet.from << RESET << std::endl;
		if(anotherTimestamp > firstInQueue->second.at(0).first){
			Monitor::missions_queues.find(packet.orderNumber)->second.push_back(std::make_pair(anotherTimestamp,packet.from));
		} else if(anotherTimestamp < firstInQueue->second.at(0).first){
			Monitor::missions_queues.find(packet.orderNumber)->second.push_front(std::make_pair(anotherTimestamp,packet.from));
		} else if(packet.from > firstInQueue->second.at(0).first){
			Monitor::missions_queues.find(packet.orderNumber)->second.push_back(std::make_pair(anotherTimestamp,packet.from));
		} else if(packet.from < firstInQueue->second.at(0).first){
			Monitor::missions_queues.find(packet.orderNumber)->second.push_front(std::make_pair(anotherTimestamp,packet.from));
		}
		//std::cout << BLUE << Monitor::missions_queues.find(packet.orderNumber)->second.size() << RESET << std::endl;
		if(Monitor::missions_queues.find(packet.orderNumber)->second.size() == HUNTERS_COUNT){
			for (int i=0; i<Monitor::missions_queues.find(packet.orderNumber)->second.size(); ++i) {
        			cout << Monitor::missions_queues.find(packet.orderNumber)->second.at(i).first << ":" << Monitor::missions_queues.find(packet.orderNumber)->second.at(i).second << ' ';
    			}
			std::cout << BLUE << Monitor::missions_queues.find(packet.orderNumber)->second.at(0).second << RESET << std::endl;	
		}
	}
}

void Hunters::sendOrderReq(packet_t packet){
	int siz;
        MPI_Comm_size(MPI_COMM_WORLD,&siz);
	packet.lamport = Monitor::newMissionTimestamp;
        for(int i = 0; i <= siz; i++){
        	if(i%4!=0 && i != Monitor::rank){
                	Monitor::sendMessage(&packet,i,ORDER_REQ);
                }
	}
	Monitor::incrementLamport();
}
