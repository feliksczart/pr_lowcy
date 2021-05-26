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
		static HuntersState state;
		static void handleNewMessage(packet_t packet);
		static void sendOrderReq(packet_t packet);

	public:
        	static void loop(int size, int rank);
};


#endif

