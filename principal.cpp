#include "principal.h"
#include "monitor.h"

void Principal::loop(int size){
	
	pthread_t principalThread;
	pthread_create(&principalThread,NULL,&principalMonitor,NULL);
	printf("%li\n", (unsigned long int) principalThread);	
}

void *principalMonitor (void* x) {
    
	packet_t packet;
    	pthread_t handleMission;
    	bool end = false;
    	while(!end) {
        	packet = Monitor::receiveMessage();
        	if(packet.status.MPI_TAG == MISSION_FINISHED)
            		pthread_create( &handleMission, NULL, &handleMissionFinished,NULL);
        	else {
            		end = true;
            		printf("%u: principal ends listening\n",Monitor::getLamport());
        	}
    	}
    	pthread_exit(NULL);
}

void *handleMissionFinished(void* x) {
    	
	int t = rand()%2+1;
    	sleep(t);
    	pthread_mutex_lock(&Monitor::missionsMutex);
    	Monitor::currentMissions+=1;
    	pthread_mutex_unlock(&Monitor::missionsMutex);
    	pthread_mutex_unlock(&Monitor::newMissionMutex);
    	printf("%u: Dragon died after %ds\n",Monitor::getLamport(),t);
    	pthread_exit(NULL);
}
