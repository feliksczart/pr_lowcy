#ifndef HUNTERS
#define HUNTERS
#include "main.h"

void *incomingMissionMonitor (void* x);

class Hunters {

public:
        static void loop(int size, int rank);
};


#endif

