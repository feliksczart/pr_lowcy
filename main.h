#ifndef MAINH
#define MAINH

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <queue>
#include <map>
#include <deque>
#include <iostream>
using namespace std;

//message tags
#define NEW_MISSION 100
#define ORDER_REQ 101
#define MISSION_FINISHED 200
#define WAIT_HUNTERS 2

extern int HUNTERS_COUNT;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

typedef struct {
    int from;
    int tag;
    int orderNumber;
    unsigned int lamport;
    //MPI_Status status;
} packet_t;
extern MPI_Datatype MPI_PAKIET_T;

#endif
