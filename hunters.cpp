#include "hunters.h"
#include "monitor.h"

void Hunters::loop(int size, int rank){

	pthread_t incomingMissionThread;
        //https://thispointer.com/posix-how-to-create-a-thread-pthread_create-example-tutorial/
	pthread_create(&incomingMissionThread,NULL,&incomingMissionMonitor,NULL);
	//printf("%li\n", (unsigned long int) incomingMissionThread);
	//pthread_join(incomingMissionThread,NULL);
}

void *incomingMissionMonitor (void* x) {
	
     	packet_t packet;
     	pthread_t handleMission;
     	while(1) {
		printf("Siema\n");
             	packet = Monitor::receiveMessage();
             	if(packet.status.MPI_TAG == NEW_MISSION){
                	printf("See mission");
            	}
	     	sleep(2);
     	}
}
