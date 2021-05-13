#include "main.h"
#include "monitor.h"
#include "principal.h"
#include "hunters.h"

bool thread_support = false;
int principal = 0;
int hunters = 1;

bool check_thread_support(int provided) {
    return provided >= MPI_THREAD_MULTIPLE;
}

void init(int *argc, char ***argv){

	int provided;
	MPI_Init_thread(argc, argv,MPI_THREAD_MULTIPLE, &provided);
	thread_support = check_thread_support(provided);
}

void finalize(){

	MPI_Finalize();
}

int main(int argc, char **argv){
	
	init(&argc,&argv);
	if(thread_support){
		//printf("Support Granted!\n");
		Monitor::initialize();
		//printf("%d\n",Monitor::rank);

		int type = Monitor::rank%2;
		if(type == principal){
			Principal::loop(Monitor::size,Monitor::rank);
		} else if(type == hunters){
			Hunters::loop(Monitor::size,Monitor::rank);
		}
	} else{
		printf("Support Not Granted!\n");
	}
	finalize();
	return 0;
}
