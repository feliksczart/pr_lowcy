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
			sleep(1);
			packet_t packet = Monitor::messageQ.front();
			Monitor::messageQ.pop();
			if(packet.tag == NEW_MISSION){
				sleep(rand()%3+1);
				std::cout << GREEN << Monitor::getLamport() << ": Łowca " << rank << " otrzymał wiadomość o dostępnym zleceniu nr " << packet.orderNumber << RESET << std::endl;
			} else if(packet.tag == ORDER_REQ){
				//std::cout << WHITE << Monitor::getLamport() << ": Łowca " << rank << " otrzymał request o przydzielenie zlecenia " << packet.orderNumber << " dla łowcy " << packet.from << RESET << std::endl;	
			} else if(packet.tag == YOU_CAN_GO){
                        	Monitor::ackCount += 1;
				std::cout << YELLOW << Monitor::ackCount << RESET << std::endl;
			}
			Hunters::handleNewMessage(packet);
		}

		if(Monitor::ackCount + Monitor::onMission.size() == HUNTERS_COUNT - 1){
			std::cout << YELLOW << "Chłop na misji KEKW" << RESET << std::endl;
			sleep(200);
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
		
		Monitor::mission_q.push_back(std::make_pair(Monitor::getLamport(),Monitor::rank));
	       	Monitor::missions_queues.insert(std::make_pair(packet.orderNumber,Monitor::mission_q));	
		//Monitor::print_map(Monitor::missions_queues);
		//sleep(rand()%2+1);
		//std::cout << YELLOW << packet.lamport+1 << ": Łowca " << Monitor::rank << " pyta innych czy może wyruszyć na misję"  << RESET << std::endl;
		Hunters::sendOrderReq(packet);
	} else if(packet.tag == ORDER_REQ){
		auto orderQueue = Monitor::missions_queues.find(packet.orderNumber);
		//std::cout << BLUE << Monitor::missions_queues.find(packet.orderNumber)->second.size() << RESET << std::endl;
		if(packet.lamport > orderQueue->second.at(0).first){
			Monitor::missions_queues.find(packet.orderNumber)->second.push_back(std::make_pair(packet.lamport,packet.from));
		} else if(packet.lamport < orderQueue->second.at(0).first){
			Monitor::missions_queues.find(packet.orderNumber)->second.push_front(std::make_pair(packet.lamport,packet.from));
		} else if(packet.from > orderQueue->second.at(0).second){
			Monitor::missions_queues.find(packet.orderNumber)->second.push_back(std::make_pair(packet.lamport,packet.from));
		} else if(packet.from < orderQueue->second.at(0).second){
			Monitor::missions_queues.find(packet.orderNumber)->second.push_front(std::make_pair(packet.lamport,packet.from));
		}
		//std::cout << BLUE << Monitor::missions_queues.find(packet.orderNumber)->second.size() << RESET << std::endl;
		if(Monitor::missions_queues.find(packet.orderNumber)->second.size() == HUNTERS_COUNT){
		//if(Monitor::rank == 7){	
			//cout << packet.orderNumber << ": ";
			//for (int i=0; i<Monitor::missions_queues.find(packet.orderNumber)->second.size(); ++i) {
        			//cout << Monitor::missions_queues.find(packet.orderNumber)->second.at(i).first << ":" << Monitor::missions_queues.find(packet.orderNumber)->second.at(i).second << ' ';
    			//}
			//std::cout << BLUE << Monitor::missions_queues.find(packet.orderNumber)->second.at(0).second << RESET << std::endl;
			std::sort(Monitor::missions_queues.find(packet.orderNumber)->second.begin(),Monitor::missions_queues.find(packet.orderNumber)->second.end(),Monitor::myComparison);
			
			Hunters::sendAckToWinner(packet);
			Monitor::deleteQueue(packet.orderNumber);
		}
	}
}

void Hunters::sendOrderReq(packet_t packet){
	int siz;
        MPI_Comm_size(MPI_COMM_WORLD,&siz);
	packet.lamport = Hunters::getMyLamportInQueue(packet.orderNumber);
	packet.from = Monitor::rank;
        for(int i = 0; i <= siz; i++){
        	if(i%4!=0 && i != Monitor::rank){
                	Monitor::sendMessage(&packet,i,ORDER_REQ);
                }
	}
	Monitor::incrementLamport();
}

int Hunters::getMyLamportInQueue(int orderNum){
	auto orderQueue = Monitor::missions_queues.find(orderNum);
	for (int i = 0; i < orderQueue->second.size(); i++){
		if(orderQueue->second.at(i).second == Monitor::rank){
			return orderQueue->second.at(i).first;
		}
	}
	return 0;
}

void Hunters::sendAckToWinner(packet_t packet){
	packet.lamport = Monitor::getLamport();
	int winner;
	for (int i = 0; i < HUNTERS_COUNT; i++){
		winner = Monitor::missions_queues.find(packet.orderNumber)->second.at(i).second;
		if(checkWinner(winner)) break;
	}
	std::cout << GREEN << winner << RESET << std::endl;
	if(Monitor::rank != winner)
		Monitor::sendMessage(&packet,winner,YOU_CAN_GO);
	Monitor::onMission.push_back(winner);
	Monitor::incrementLamport();	
}

bool Hunters::checkWinner(int winner){
	if(Monitor::onMission.size() == 0)
		return true;
	for(int i = 0; i < Monitor::onMission.size(); i++)
		if(Monitor::onMission.at(i) == winner)
			return false;
	return true;
}

