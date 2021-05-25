#include <algorithm>
#include "monitor.h"

int Monitor::rank;
int Monitor::size;

int Monitor::HM = 5;
int Monitor::LM = 3;
int Monitor::currentMissions = 0;
unsigned int Monitor::lamport = 0;
bool Monitor::listening = false;
std::queue<packet_t> Monitor::messageQ;

pthread_mutex_t Monitor::lamportMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::missionsMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::newMissionMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::waitHuntersMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::messageQMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Monitor::incomingMissionMutex = PTHREAD_MUTEX_INITIALIZER;

void Monitor::initialize(){
	MPI_Comm_rank(MPI_COMM_WORLD, &Monitor::rank);
    	MPI_Comm_size(MPI_COMM_WORLD, &Monitor::size);
}

packet_t Monitor::sendMessage(int target, int tag) {
    packet_t packet;
    packet.lamport = Monitor::incrementLamportOnSend();
    MPI_Send(&packet, 1, MPI_PAKIET_T, target, tag, MPI_COMM_WORLD);
    return packet;
}

packet_t Monitor::receiveMessage() {
	packet_t packet;
    	MPI_Status status;
	printf("22222222222\n");
    	MPI_Recv( &packet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    	MPI_Barrier(MPI_COMM_WORLD);
	printf("33333333333\n");
	packet.tag = status.MPI_TAG;
    	Monitor::incrementLamportOnReceive(packet);
   	return packet;
}

void Monitor::listen(){
	Monitor::listening = true;
	packet_t packet;
	while(Monitor::listening){
		packet = Monitor::receiveMessage();
		pthread_mutex_lock(&Monitor::messageQMutex);
		Monitor::messageQ.push(packet);
		pthread_mutex_unlock(&Monitor::messageQMutex);
        	pthread_mutex_unlock(&Monitor::incomingMissionMutex);
	}	
}

unsigned int Monitor::incrementLamportOnSend() {
    pthread_mutex_lock(&Monitor::lamportMutex);
    Monitor::lamport += 1;
    unsigned int newLamport = Monitor::lamport;
    pthread_mutex_unlock(&Monitor::lamportMutex);
    return newLamport;
}

void Monitor::incrementLamportOnReceive(packet_t packet) {    
	pthread_mutex_lock(&Monitor::lamportMutex);
   	Monitor::lamport = std::max(packet.lamport, Monitor::lamport) + 1;
    	pthread_mutex_unlock(&Monitor::lamportMutex);
}

unsigned int Monitor::getLamport() {
    return Monitor::lamport;
}
