#ifndef MONITOR
#define MONITOR
#include "main.h"

class Monitor {
public:
    static int rank;
    static int size;
    
    static void initialize();
    static void finalize();
};

#endif
