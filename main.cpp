#include "main.h"

bool thread_support = FALSE;

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
		printf("Support Granted!\n");
	} else{
		printf("Support Not Granted!\n");
	}
	finalize();
	return 0;
}
