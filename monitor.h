#ifndef MONITOR
#define MONITOR
#include "main.h"

class Monitor {
	private:
		static unsigned int lamport;
		static pthread_mutex_t lamportMutex;
	public:
    		static int rank;
    		static int size;
    		static int currentMissions;
		static pthread_mutex_t missionsMutex;
    		static pthread_mutex_t newMissionMutex;
    
    		static void initialize();
    		static packet_t receiveMessage();
		static void incrementLamportOnReceive(packet_t packet);
		static unsigned int getLamport();
    		static void finalize();
};

#endif
