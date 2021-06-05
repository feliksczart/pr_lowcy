#include <algorithm>
#include "monitor.h"
#include "hunters.h"

int Monitor::rank;
int Monitor::size;

int Monitor::HM = 5;
int Monitor::LM = 3;
int Monitor::currentMissions = 0;
unsigned int Monitor::lamport = 0;
bool Monitor::listening = false;
int Monitor::ackCount = 0;
bool Monitor::shopAsked = false;
int Monitor::ackShop = 0;

std::queue<packet_t> Monitor::messageQ;
std::deque<pair<unsigned int,int>> Monitor::mission_q;
std::pair<unsigned int,int> Monitor::hunter_p;
std::map<int, deque<pair<unsigned int,int>>> Monitor::missions_queues;
std::deque<int> Monitor::onMission;
std::deque<pair<unsigned int,int>> Monitor::shop_q;

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

void Monitor::sendMessage(packet_t *packet, int target, int tag) {
    	
	//std::cout << BLUE << packet->lamport << " id: " << Monitor::rank << RESET << std::endl;
    	MPI_Send(packet, 1, MPI_PAKIET_T, target, tag, MPI_COMM_WORLD);
}

packet_t Monitor::receiveMessage() {
	packet_t packet;
    	MPI_Status status;
    	MPI_Recv( &packet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	packet.tag = status.MPI_TAG;
    	Monitor::incrementLamportOnReceive(packet);
	return packet;
}

void Monitor::listen(){
	Monitor::listening = true;
	packet_t packet;
	while(Monitor::listening){
		packet = Monitor::receiveMessage();
		if(packet.tag == SHOP_REQ){
			int target = packet.from;
			Monitor::shop_q.push_back(std::make_pair(packet.lamport,target));
                        packet.from = Monitor::rank;
                        packet.lamport = Monitor::getMyLamportShopQueue();
			if(Hunters::state == HuntersState::WAITING_SHOP){
				Monitor::sendMessage(&packet,target,TRUE);
			} else {
				Monitor::sendMessage(&packet,target,FALSE);
			}
		} else if(packet.tag == TRUE){
                	Monitor::ackShop++;
			Monitor::shop_q.push_back(std::make_pair(packet.lamport,packet.from));
        	} else if(packet.tag == FALSE){
                	Monitor::ackShop++;
        	} else {
			pthread_mutex_lock(&Monitor::messageQMutex);
			Monitor::messageQ.push(packet);
			pthread_mutex_unlock(&Monitor::messageQMutex);
		}
	}	
}
void Monitor::incrementLamport() {
    pthread_mutex_lock(&Monitor::lamportMutex);
    Monitor::lamport += 1;
    pthread_mutex_unlock(&Monitor::lamportMutex);
}

void Monitor::incrementLamportOnReceive(packet_t packet) {    
	pthread_mutex_lock(&Monitor::lamportMutex);
   	Monitor::lamport = std::max(packet.lamport, Monitor::lamport) + 1;
    	pthread_mutex_unlock(&Monitor::lamportMutex);
}

unsigned int Monitor::getLamport() {
    return Monitor::lamport;
}

void Monitor::deleteQueue(int orderNum){
	auto orderQueue = Monitor::missions_queues.find(orderNum);
	//pthread_mutex_lock(&Monitor::messageQMutex);
	Monitor::missions_queues.erase(orderQueue);
	//pthread_mutex_unlock(&Monitor::messageQMutex);	
}

bool Monitor::myComparison(const pair<unsigned int,int> &a,const pair<unsigned int,int> &b){
	if(a.first < b.first)
		return true;
	if(a.first > b.first)
		return false;
	if(a.second < b.second)
		return true;
	if(a.second > b.second)
                return false;
}

void Monitor::print_map(map<int, deque<pair<unsigned int,int>>> const &m)
{
    for (auto const &pair: m) {
        std::cout << "{" << pair.first /*<< ": " << pair.second */<< "}\n";
    }
}

int Monitor::getMyLamportShopQueue(){
        for (int i = 0; i < Monitor::shop_q.size(); i++){
                if(Monitor::shop_q.at(i).second == Monitor::rank){
                        return Monitor::shop_q.at(i).first;
                }
        }
        return 0;
}

