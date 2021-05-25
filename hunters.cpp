#include "hunters.h"
#include "monitor.h"

void Hunters::loop(int size, int rank){
	pthread_t incomingMissionThread;
        //https://thispointer.com/posix-how-to-create-a-thread-pthread_create-example-tutorial/
	pthread_create(&incomingMissionThread,NULL,&incomingMissionMonitor,NULL);
	//printf("%li\n", (unsigned long int) incomingMissionThread);
	//pthread_join(incomingMissionThread,NULL);
	while(1){
		sleep(2);
		pthread_mutex_lock(&Monitor::incomingMissionMutex);
		pthread_mutex_lock(&Monitor::messageQMutex);
		if(!Monitor::messageQ.empty()){
			packet_t packet = Monitor::messageQ.front();
			Monitor::messageQ.pop();
			pthread_mutex_unlock(&Monitor::messageQMutex);
			printf("%d CCCCCC\n",packet.data);
		} else {
			pthread_mutex_unlock(&Monitor::messageQMutex);
		}
		pthread_mutex_unlock(&Monitor::incomingMissionMutex);

	}
}

void *incomingMissionMonitor (void* x) {
	Monitor::listen();
}	
