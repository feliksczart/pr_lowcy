#include "main.h"

bool check_thread_support(int provided) {
    return provided >= MPI_THREAD_MULTIPLE;
}

void init(int *argc, char ***argv){
	
	int provided;
	MPI_Init_thread(argc, argv,MPI_THREAD_MULTIPLE, &provided);
	bool support = check_thread_support(provided);
	printf("Support Granted: %s\n", support? "Yes" : "No");
}

int main(int argc, char **argv){
	
	init(&argc,&argv);
}
