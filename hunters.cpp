#include "hunters.h"
#include "monitor.h"

void Hunters::loop(int size, int rank){
	//wątek oczekujący na nowe zlecenia
	pthread_t incomingMissionThread;
	pthread_create(&incomingMissionThread,NULL,&incomingMissionMonitor,NULL);
	
	while(1){
		sleep(2);
		pthread_mutex_lock(&Monitor::incomingMissionMutex);
		pthread_mutex_lock(&Monitor::messageQMutex);
		if(!Monitor::messageQ.empty()){
			packet_t packet = Monitor::messageQ.front();
			Monitor::messageQ.pop();
			packet.lamport = Monitor::getLamport();
			printf("%d: Łowca %d otrzymał wiadomość o dostępnym zleceniu od zleceniodawcy %d\n",packet.lamport,Monitor::rank,packet.from);
		}

		pthread_mutex_unlock(&Monitor::messageQMutex);
		pthread_mutex_unlock(&Monitor::incomingMissionMutex);

	}
	pthread_join(incomingMissionThread,NULL);
}

void *incomingMissionMonitor (void* x) {
	Monitor::listen();
}	
