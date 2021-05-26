#include "principal.h"
#include "monitor.h"

void Principal::loop(int size, int rank){
	
	//wątek oczekujący na wykonanie zlecenia
	pthread_t principalThread;
	pthread_create(&principalThread,NULL,&principalMonitor,NULL);
	
	packet_t packet;
	packet.data = 0;
	packet.from = Monitor::rank;
	int orderId = 1;
	
	//wysyłanie zleceń
	while(1){
		sleep(rand()%40);	
		pthread_mutex_lock(&Monitor::newMissionMutex);

        	pthread_mutex_lock(&Monitor::missionsMutex);
       		Monitor::currentMissions+=1;
        	pthread_mutex_unlock(&Monitor::missionsMutex);
        	
        	if(Monitor::currentMissions <= Monitor::HM){
            		pthread_mutex_unlock(&Monitor::newMissionMutex);
		} else{
			while(Monitor::currentMissions > Monitor::LM){
				printf("%u: Zleceniodawca %d nie może wysłać zlecenia, bo za dużo niewykonanych!\n",Monitor::getLamport(),rank);
				sleep(rand()%20);
				Monitor::currentMissions-=1;
				orderId--;
			}
			pthread_mutex_unlock(&Monitor::newMissionMutex);
		} 
        
        	packet.lamport = Monitor::getLamport();
        	printf("%u: U zleceniodawcy %d pojawiło się zlecenie nr: %d!\n",Monitor::getLamport() ,rank, orderId);
                sleep(2);
		Monitor::incrementLamport();	
		printf("%u: Zleceniodawca %d wysyła zlecenie nr: %d do oczekujących łowców!\n",Monitor::getLamport() ,rank, orderId);
		int siz;
		MPI_Comm_size(MPI_COMM_WORLD,&siz);
		for(int i = 0; i <= siz; i++){
			if(i%4!=0){	
                		Monitor::sendMessage(&packet,i,NEW_MISSION);
			}
		}

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
	     	printf("Siema id %d\n",Monitor::rank);
             	packet = Monitor::receiveMessage();
             	if(packet.tag == MISSION_FINISHED){
                	pthread_create( &handleMission, NULL, &handleMissionFinished,NULL);

             }
     }
}

void *handleMissionFinished(void* x) {

     int t = rand()%2+1;
     sleep(t);
     pthread_mutex_lock(&Monitor::missionsMutex);
     Monitor::currentMissions-=1;
     pthread_mutex_unlock(&Monitor::missionsMutex);
     pthread_mutex_unlock(&Monitor::newMissionMutex);
     printf("%u: X %ds\n",Monitor::getLamport(),t);
     pthread_exit(NULL);
}
