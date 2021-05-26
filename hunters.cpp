#include "hunters.h"
#include "monitor.h"
#include <iostream>

HuntersState Hunters::state = HuntersState::WAITING_ORDER;

void Hunters::loop(int size, int rank){
	//wątek oczekujący na nowe zlecenia
	pthread_t incomingMissionThread;
	pthread_create(&incomingMissionThread,NULL,&incomingMissionMonitor,NULL);
	
	while(1){
		sleep(2);
		pthread_mutex_lock(&Monitor::incomingMissionMutex);
		pthread_mutex_lock(&Monitor::messageQMutex);
		if(Hunters::state == HuntersState::WAITING_ORDER){
			if(!Monitor::messageQ.empty()){
				packet_t packet = Monitor::messageQ.front();
				Monitor::messageQ.pop();
				packet.lamport = Monitor::getLamport();
				sleep(rand()%15+1);
				std::cout << GREEN << packet.lamport << ": Łowca " << rank << " otrzymał wiadomość o dostępnym zleceniu nr " << packet.orderNumber << RESET << std::endl;
				Hunters::handleNewMessage(packet);
			}
		} else if(Hunters::state == HuntersState::TRYING_ORDER){
                        if(!Monitor::messageQ.empty()){
                                packet_t packet = Monitor::messageQ.front();
                                Monitor::messageQ.pop();
                                packet.lamport = Monitor::getLamport();
				sleep(rand()%15+1);
                                std::cout << GREEN << packet.lamport << ": Łowca " << rank << " otrzymał request o przydzielenie zlecenia dla łowcy: " << packet.from << RESET << std::endl;
                                Hunters::handleNewMessage(packet);
                        }
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

		if(!Monitor::mission_q.empty()){
			std::pair<unsigned int,int> front = Monitor::mission_q.front();
			if(packet.lamport <= front.first && Monitor::rank < front.second){
				Monitor::mission_q.push_front(std::make_pair(packet.lamport,Monitor::rank));
			} else {
			
			}
		} else {
			Monitor::mission_q.push_back(std::make_pair(packet.lamport,Monitor::rank));
		}
	       	Monitor::missions_queues.insert(std::make_pair(packet.orderNumber,Monitor::mission_q));	
		//Monitor::print_map(Monitor::missions_queues);
		Monitor::incrementLamport();
		packet.lamport = Monitor::getLamport();
		sleep(rand()%2+1);
		std::cout << YELLOW << packet.lamport << ": Łowca " << Monitor::rank << " pyta innych czy może wyruszyć na misję"  << RESET << std::endl;
		Hunters::sendOrderReq(packet);
	}
}

void Hunters::sendOrderReq(packet_t packet){
	int siz;
        MPI_Comm_size(MPI_COMM_WORLD,&siz);
        for(int i = 0; i <= siz; i++){
        	if(i%4!=0 && i != Monitor::rank){
                	Monitor::sendMessage(&packet,i,ORDER_REQ);
                }
	}
}
