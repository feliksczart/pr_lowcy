#ifndef HUNTERS
#define HUNTERS
#include "main.h"

void *incomingMissionMonitor (void* x);

enum class HuntersState{
	WAITING_ORDER,
	TRYING_ORDER,
	GOT_ORDER,
	WAITING_SHOP,
	IN_SHOP,
	ON_MISSION
};

class Hunters {
	private:
		static void handleNewMessage(packet_t packet);
		static void sendOrderReq(packet_t packet);
		static int getMyLamportInQueue(int orderNum);
		static void sendAckToWinner(packet_t packet);
		static bool checkWinner(int winner);
		static bool canGoMission(int rank);
		static void goToShop(packet_t packet);
		static void askHowMuchInShop(packet_t packet);
		static bool canGoShop(int rank);
		static void sendAckToQueue(packet_t packet);
		static void sendAckInShop(packet_t packet);
		static void sendAckIGo(packet_t packet);
		static bool checkShopWinner(int winner);
		static bool waitPrinted;
		static void goMission(packet_t packet);
		static void sendMissionDone(packet_t packet);
		static int getMissionPrincipal(int mission);
		static int wonMissionNum;
		static void resetHunters();
	public:
        	static void loop(int size, int rank);
		static HuntersState state;
		static bool listenPrincipal;
};


#endif

