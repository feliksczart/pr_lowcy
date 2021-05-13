#include "hunters.h"
#include "monitor.h"

void Hunters::loop(int size, int rank){

	pthread_t incomingMissionThread;
        pthread_create(&incomingMissionThread,NULL,&incomingMissionMonitor,NULL);
	//printf("%li\n", (unsigned long int) incomingMissionThread);
	//pthread_join(incomingMissionThread,NULL);
}

void *incomingMissionMonitor (void* x) {
	
	printf("Siema");
     	packet_t packet;
     	pthread_t handleMission;
     	while(1) {
		printf("Siema");
             	packet = Monitor::receiveMessage();
             	if(packet.status.MPI_TAG == NEW_MISSION){
                	printf("See mission");
            	}
	     	sleep(2);
     	}
}
