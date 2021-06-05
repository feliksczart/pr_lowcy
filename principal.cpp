#include "principal.h"
#include "monitor.h"

void Principal::loop(int size, int rank){
	
	//wątek oczekujący na wykonanie zlecenia
	pthread_t principalThread;
	pthread_create(&principalThread,NULL,&principalMonitor,NULL);
	
	packet_t packet;
	int orderId = 1;
	
	//wysyłanie zleceń
	while(1){
		if (rank == 0)
			sleep(10);//rand()%40);
		else
			sleep(rand()%40+100);	
		pthread_mutex_lock(&Monitor::newMissionMutex);

        	pthread_mutex_lock(&Monitor::missionsMutex);
       		Monitor::currentMissions+=1;
        	pthread_mutex_unlock(&Monitor::missionsMutex);
        	
        	if(Monitor::currentMissions <= Monitor::HM){
            		pthread_mutex_unlock(&Monitor::newMissionMutex);
		} else{
			while(Monitor::currentMissions > Monitor::LM){
				std::cout << RED << Monitor::getLamport() << ": Zleceniodawca " << rank << " nie może wysłać zlecenia, bo za dużo niewykonanych!" << RESET << std::endl;
				sleep(rand()%20);
				Monitor::currentMissions-=1;
				orderId--;
			}
			pthread_mutex_unlock(&Monitor::newMissionMutex);
		} 
        
        	packet.lamport = Monitor::getLamport();
		packet.orderNumber = Monitor::rank*10 + orderId;
		std::cout << RED << Monitor::getLamport() << ": U zleceniodawcy " << rank << " pojawiło się zlecenie nr: " << packet.orderNumber << RESET << std::endl;
                sleep(2);
		Monitor::incrementLamport();
		std::cout << RED << Monitor::getLamport() << ": Zleceniodawca " << rank << " wysyła zlecenie nr: " << packet.orderNumber << RESET << std::endl;
		int siz;
		MPI_Comm_size(MPI_COMM_WORLD,&siz);
                
		packet.lamport = Monitor::getLamport();
        	packet.from = Monitor::rank;
		for(int i = 0; i < siz; i++){
			if(i%4!=0){
                		Monitor::sendMessage(&packet,i,NEW_MISSION);
			}
		}
		if (rank == 0 && orderId > 3)
                        sleep(rand()%40);
                else if(rank!=0)
                        sleep(rand()%100+100);

        	orderId++;
		Monitor::incrementLamport();
	}
	pthread_join(principalThread,NULL);	
}

void *principalMonitor (void* x) {

     	packet_t packet;
     	pthread_t handleMission;
     	while(1) {

	     	sleep(2);
             	packet = Monitor::receiveMessage();
             	if(packet.tag == MISSION_FINISHED){
			std::cout << RED << Monitor::getLamport() << ": Zleceniodawca " << Monitor::rank << " przyjął informację o skończonej misji drużyny " << packet.from << RESET << std::endl;
			Monitor::currentMissions-=1;
             }
     }
}
