#ifndef MAINH
#define MAINH

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//message tags
#define NEW_MISSION 100
#define MISSION_FINISHED 200
#define WAIT_HUNTERS 2

typedef struct {
    int data;
    int completedMissions;
    unsigned int lamport;
    MPI_Status status;
} packet_t;

#endif
