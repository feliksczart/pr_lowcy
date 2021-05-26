#ifndef MONITOR
#define MONITOR
#include "main.h"

class Monitor {
	private:
		static unsigned int lamport;
		static pthread_mutex_t lamportMutex;
		static bool listening;
	public:
    		static int rank;
    		static int size;
    		static int HM;
		static int LM;
		static int currentMissions;
		static pthread_mutex_t missionsMutex;
    		static pthread_mutex_t newMissionMutex;
    		static pthread_mutex_t waitHuntersMutex;
		static pthread_mutex_t messageQMutex;
		static pthread_mutex_t incomingMissionMutex;
		static std::queue<packet_t> messageQ;	

		static void initialize();
		static void sendMessage(packet_t *packet, int target, int tag);
    		static packet_t receiveMessage();
		static void incrementLamport();
		static void incrementLamportOnReceive(packet_t packet);
		static unsigned int getLamport();
    		static void listen();
		static void finalize();
};

#endif
