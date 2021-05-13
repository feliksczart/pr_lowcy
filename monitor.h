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
    		static int HM;
		static int LM;
		static int currentMissions;
		static pthread_mutex_t missionsMutex;
    		static pthread_mutex_t newMissionMutex;
    
    		static void initialize();
		static packet_t sendMessage(int target, int tag);
    		static packet_t receiveMessage();
		static unsigned int incrementLamportOnSend();
		static void incrementLamportOnReceive(packet_t packet);
		static unsigned int getLamport();
    		static void finalize();
};

#endif
