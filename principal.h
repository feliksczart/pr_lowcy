#ifndef PRINCIPAL
#define PRINCIPAL
#include "main.h"

void *principalMonitor (void* x);
void *handleMissionFinished(void* x);

class Principal {

public:
	static void loop(int size,int rank);
};


#endif
