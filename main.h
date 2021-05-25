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
using namespace std;

//message tags
#define NEW_MISSION 100
#define MISSION_FINISHED 200
#define WAIT_HUNTERS 2

typedef struct {
    int data;
    int tag;
    unsigned int lamport;
    //MPI_Status status;
} packet_t;
extern MPI_Datatype MPI_PAKIET_T;

#endif
