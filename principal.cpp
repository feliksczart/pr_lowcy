#include "principal.h"
#include "monitor.h"

void Principal::loop(int size){
	
	pthread_t principalThread;
	//pthread_create(&principalThread,NULL,&principalMonitor,NULL);
	//printf("%li\n", (unsigned long int) principalThread);
	
	packet_t packet;
	packet.data = 0;
	int orderId = 1;

	while(1){
		sleep(rand()%20);	
		pthread_mutex_lock(&Monitor::newMissionMutex);

        	pthread_mutex_lock(&Monitor::missionsMutex);
       		Monitor::currentMissions-=1;
        	pthread_mutex_unlock(&Monitor::missionsMutex);
        	
        	if(Monitor::currentMissions > 0)
            		pthread_mutex_unlock(&Monitor::newMissionMutex);
        
        	packet.lamport = Monitor::getLamport();
        	printf("%u: Pojawiło się zlecenie nr: %d!\n",Monitor::getLamport() ,orderId);
        	for(int i = 1;i < size; i++){ // broadcast NEW_ORDER
            		if(i%3 + 1 == 1) {
                		//sendMessage(NEW_ORDER,i);
				
            		}
        	}
        	orderId++;
	}	
}

//void *principalMonitor (void* x) {
//    
//	packet_t packet;
//   	pthread_t handleMission;
//    	bool end = false;
//    	while(!end) {
//        	packet = Monitor::receiveMessage();
//        	if(packet.status.MPI_TAG == MISSION_FINISHED){
//            		//pthread_create( &handleMission, NULL, &handleMissionFinished,NULL);
//		} else {
//            		end = true;
//            		printf("%u: principal ends listening\n",Monitor::getLamport());
//        	}
//    	}
//    	pthread_exit(NULL);
//}

//void *handleMissionFinished(void* x) {
//    	
//	int t = rand()%2+1;
//    	sleep(t);
//    	pthread_mutex_lock(&Monitor::missionsMutex);
//    	Monitor::currentMissions+=1;
//    	pthread_mutex_unlock(&Monitor::missionsMutex);
//    	pthread_mutex_unlock(&Monitor::newMissionMutex);
//    	printf("%u: X %ds\n",Monitor::getLamport(),t);
//    	pthread_exit(NULL);
//}
