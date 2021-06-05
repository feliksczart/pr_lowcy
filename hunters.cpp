#include "hunters.h"
#include "monitor.h"
#include <iostream>

HuntersState Hunters::state = HuntersState::WAITING_ORDER;

void Hunters::loop(int size, int rank){
	//wątek oczekujący na nowe zlecenia
	pthread_t incomingMissionThread;
	pthread_create(&incomingMissionThread,NULL,&incomingMissionMonitor,NULL);
	
	bool missRcv = false;
	bool goShopPrinted = false;
	packet_t packet;	

	while(1){
		pthread_mutex_lock(&Monitor::incomingMissionMutex);
		pthread_mutex_lock(&Monitor::messageQMutex);
		
		if(!Monitor::messageQ.empty()){
			sleep(1);
			packet = Monitor::messageQ.front();
			Monitor::messageQ.pop();
			//std::cout << GREEN << packet.tag << RESET << std::endl;
			if(packet.tag == NEW_MISSION && Hunters::state != HuntersState::WAITING_SHOP){
				sleep(rand()%3+1);
				missRcv = true;
				std::cout << GREEN << Monitor::getLamport() << ": Łowca " << rank << " otrzymał wiadomość o dostępnym zleceniu nr " << packet.orderNumber << RESET << std::endl;
			} else if(packet.tag == ORDER_REQ && Hunters::state != HuntersState::WAITING_SHOP){
				//std::cout << WHITE << Monitor::getLamport() << ": Łowca " << rank << " otrzymał request o przydzielenie zlecenia " << packet.orderNumber << " dla łowcy " << packet.from << RESET << std::endl;	
			} else if(packet.tag == YOU_CAN_GO && Hunters::state != HuntersState::WAITING_SHOP){
                        	Monitor::ackCount += 1;
			}
			if(Monitor::onMission.size() < HUNTERS_COUNT-1 && Hunters::checkWinner(packet.from))
				Hunters::handleNewMessage(packet);
		}

		if(Monitor::ackCount + Monitor::onMission.size() == HUNTERS_COUNT/*canGoMission(rank)*/ || ((Monitor::onMission.size() == HUNTERS_COUNT-1 || HUNTERS_COUNT == 1) && missRcv || Hunters::state == HuntersState::WAITING_SHOP)){
			Hunters::state = HuntersState::WAITING_SHOP;
			if(!goShopPrinted)	
				std::cout << BLUE << Monitor::getLamport() << ": Łowca " << rank << " rusza do sklepu" << RESET << std::endl;
			goShopPrinted = true;
			sleep(rand()%2);
			Hunters::goToShop(packet);
		}
		
		missRcv = false;
		
		pthread_mutex_unlock(&Monitor::messageQMutex);
		pthread_mutex_unlock(&Monitor::incomingMissionMutex);

	}
	pthread_join(incomingMissionThread,NULL);
}

void *incomingMissionMonitor (void* x) {
	Monitor::listen();
}	

void Hunters::handleNewMessage(packet_t packet){
	if(packet.tag == NEW_MISSION && !Hunters::canGoMission(Monitor::rank)){
		
		Monitor::mission_q.push_back(std::make_pair(Monitor::getLamport(),Monitor::rank));
	       	Monitor::missions_queues.insert(std::make_pair(packet.orderNumber,Monitor::mission_q));
		Monitor::mission_q.clear();
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
		if(Monitor::missions_queues.find(packet.orderNumber)->second.size() == HUNTERS_COUNT - Monitor::onMission.size()){
		//if(Monitor::rank == 7){	
			//cout << packet.orderNumber << ": ";
			//for (int i=0; i<Monitor::missions_queues.find(packet.orderNumber)->second.size(); ++i) {
        			//cout << Monitor::missions_queues.find(packet.orderNumber)->second.at(i).first << ":" << Monitor::missions_queues.find(packet.orderNumber)->second.at(i).second << ' ';
    			//}
			//std::cout << BLUE << Monitor::missions_queues.find(packet.orderNumber)->second.at(0).second << RESET << std::endl;
			std::sort(Monitor::missions_queues.find(packet.orderNumber)->second.begin(),Monitor::missions_queues.find(packet.orderNumber)->second.end(),Monitor::myComparison);
			if(!Hunters::canGoMission(Monitor::rank)){	
				Hunters::sendAckToWinner(packet);
				Monitor::deleteQueue(packet.orderNumber);
			}
		}
	}	
}

void Hunters::sendOrderReq(packet_t packet){
	int siz;
        MPI_Comm_size(MPI_COMM_WORLD,&siz);
	packet.lamport = Hunters::getMyLamportInQueue(packet.orderNumber);
	packet.from = Monitor::rank;
        for(int i = 0; i < siz; i++){
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
	//std::cout << GREEN << winner << RESET << std::endl;
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

bool Hunters::canGoMission(int rank){
	if(Monitor::onMission.size() == 0)
        	return false;
        for(int i = 0; i < Monitor::onMission.size(); i++){
		//std::cout << YELLOW << Monitor::onMission.at(i) << RESET << std::endl;
		if(Monitor::onMission.at(i) == rank)
                         return true;
	}
        return false;
}

void Hunters::goToShop(packet_t packet){
	if(!Monitor::shopAsked){
		Hunters::askHowMuchInShop(packet);
		Monitor::shop_q.push_back(std::make_pair(Monitor::getLamport(),Monitor::rank));
	}
	sleep(1);
	//std::cout << WHITE << Monitor::rank << "::" << Monitor::ackShop << RESET << std::endl;
	if(Monitor::shop_q.size()>1)
                std::sort(Monitor::shop_q.begin(),Monitor::shop_q.end(),Monitor::myComparison);
	int winner;
        for (int i = 0; i < Monitor::shop_q.size(); i++){
                winner = Monitor::shop_q.at(i).second;
                if(checkShopWinner(winner)) break;
        }
	if(Monitor::ackShop >= HUNTERS_COUNT - MAX_SHOP){
		//std::cout << YELLOW << winner << RESET << std::endl;
		if(Monitor::rank == winner){
			while(Monitor::inShop.size() >= MAX_SHOP){
				sleep(1);
			}
			Hunters::sendAckInShop(packet);
			Monitor::shop_q.erase(Monitor::shop_q.begin());
			Hunters::state = HuntersState::IN_SHOP;
			std::cout << BLUE << Monitor::getLamport() << ": Łowca " << Monitor::rank << " wszedł do sklepu" << RESET << std::endl;
			Monitor::incrementLamport();
			sleep(30);
			std::cout << WHITE << Monitor::getLamport() << ": Łowca " << Monitor::rank << " wyszedł ze sklepu" << RESET << std::endl;
			Hunters::sendAckToQueue(packet);
			Hunters::state = HuntersState::ON_MISSION;
			sleep(200);	
		} else {
			Monitor::ackShop--;
		}
	}
}

void Hunters::askHowMuchInShop(packet_t packet){
	int siz;
        MPI_Comm_size(MPI_COMM_WORLD,&siz);
        packet.lamport = Monitor::getLamport();
        packet.from = Monitor::rank;
        for(int i = 0; i < siz; i++){
                if(i%4!=0 && i != Monitor::rank){
                        Monitor::sendMessage(&packet,i,SHOP_REQ);
                }
        }
	Monitor::shopAsked = true;
        Monitor::incrementLamport();

}

void Hunters::sendAckInShop(packet_t packet){
	int siz;
        MPI_Comm_size(MPI_COMM_WORLD,&siz);
        packet.lamport = Monitor::getLamport();
        packet.from = Monitor::rank;
        for(int i = 0; i < siz; i++){
                if(i%4!=0 && i != Monitor::rank){
                        Monitor::sendMessage(&packet,i,IN);
                }
        }
        Monitor::incrementLamport();
}

void Hunters::sendAckToQueue(packet_t packet){
	int siz;
        MPI_Comm_size(MPI_COMM_WORLD,&siz);
        packet.lamport = Monitor::getLamport();
        packet.from = Monitor::rank;
        for(int i = 0; i < siz; i++){
                if(i%4!=0 && i != Monitor::rank){
                        Monitor::sendMessage(&packet,i,OUT);
                }
        }
        Monitor::incrementLamport();

}

bool Hunters::checkShopWinner(int winner){
        if(Monitor::inShop.size() == 0)
                return true;
        for(int i = 0; i < Monitor::inShop.size(); i++)
                if(Monitor::inShop.at(i) == winner)
                        return false;
        return true;
}
