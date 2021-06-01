#include "main.h"
#include "monitor.h"
#include "principal.h"
#include "hunters.h"

bool thread_support = false;
int principal = 0;
MPI_Datatype MPI_PAKIET_T;
int HUNTERS_COUNT = 0;

bool check_thread_support(int provided) {
    return provided >= MPI_THREAD_MULTIPLE;
}

void init(int *argc, char ***argv){

	int provided;
	MPI_Init_thread(argc, argv,MPI_THREAD_MULTIPLE, &provided);
	thread_support = check_thread_support(provided);

	const int nitems=4;
    	int blocklengths[4] = {1,1,1,1};
	MPI_Datatype typy[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_UNSIGNED};

    	MPI_Aint offsets[4];
    	offsets[0] = offsetof(packet_t, from);
    	offsets[1] = offsetof(packet_t, tag);
	offsets[2] = offsetof(packet_t, orderNumber);
    	offsets[3] = offsetof(packet_t, lamport);

    	MPI_Type_create_struct(nitems, blocklengths, offsets, typy, &MPI_PAKIET_T);
    	MPI_Type_commit(&MPI_PAKIET_T);
}

void finalize(){
	
	MPI_Type_free(&MPI_PAKIET_T);
	MPI_Finalize();
}

void countHunters(){
	int siz;
        MPI_Comm_size(MPI_COMM_WORLD,&siz);
        for(int i = 0; i < siz; i++){
        	if(i%4!=0){
                	HUNTERS_COUNT++;	
        	}
        }
}

int main(int argc, char **argv){
	
	init(&argc,&argv);
	if(thread_support){
		//printf("Support Granted!\n");
		Monitor::initialize();
		
		countHunters();

		int type = Monitor::rank%4;
		if(type == principal){
			Principal::loop(Monitor::size,Monitor::rank);
		} else {
			Hunters::loop(Monitor::size,Monitor::rank);
		}
	} else{
		printf("Support Not Granted!\n");
	}
	finalize();
	return 0;
}
