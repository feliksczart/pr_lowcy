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
		static int ackCount;
		static int currentMissions;
		static bool shopAsked;
		static int inShopCount;
		static int ackShop;

		static pthread_mutex_t missionsMutex;
    		static pthread_mutex_t newMissionMutex;
    		static pthread_mutex_t waitHuntersMutex;
		static pthread_mutex_t messageQMutex;
		static pthread_mutex_t incomingMissionMutex;
		
		static std::deque<int> inShop;		
		static std::queue<packet_t> messageQ;
		static std::deque<int> onMission;
		//queue trzymający id i zegar lamporta łowcy	
		static std::deque<pair<unsigned int,int>> mission_q;
		static std::pair<unsigned int, int> hunter_p;
		static std::map<int, deque<pair<unsigned int,int>>> missions_queues;
		static std::deque<pair<unsigned int,int>> shop_q;
		static std::deque<pair<int,int>> principal_mission;	
	
		static void initialize();
		static void sendMessage(packet_t *packet, int target, int tag);
    		static packet_t receiveMessage();
		static void incrementLamport();
		static void incrementLamportOnReceive(packet_t packet);
		static unsigned int getLamport();
    		static void listen();
		static void deleteQueue(int orderNum);
		static bool myComparison(const pair<unsigned int,int> &a,const pair<unsigned int,int> &b);
		static int getMyLamportShopQueue();
		static void finalize();

		static void print_map(map<int, deque<pair<unsigned int,int>>> const &m);
};

#endif
