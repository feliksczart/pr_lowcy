#include "hunters.h"
#include "monitor.h"

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
				printf("%d: Łowca %d otrzymał wiadomość o dostępnym zleceniu nr %d\n",packet.lamport,Monitor::rank,packet.orderNumber);
				
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
		//Hunters::state = HuntersState::TRYING_ORDER;
	       	Monitor::missions_queues.insert(std::make_pair(packet.orderNumber,Monitor::mission_q));	
		Monitor::print_map(Monitor::missions_queues);
		//Hunters::sendOrderReq(packet);
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
