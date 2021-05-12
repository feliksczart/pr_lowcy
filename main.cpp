#include "main.h"

bool check_thread_support(int provided) {
    return provided >= MPI_THREAD_MULTIPLE;
}

bool init(int *argc, char ***argv){
	
	int provided;
	MPI_Init_thread(argc, argv,MPI_THREAD_MULTIPLE, &provided);
	return check_thread_support(provided);
}

int main(int argc, char **argv){
	
	bool thread_support = init(&argc,&argv);
	if(thread_support){
		printf("Support Granted!\n");
	} else{
		printf("Support Not Granted!\n");
	}
	MPI_Finalize();
	return 0;
}
