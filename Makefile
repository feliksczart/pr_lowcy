SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.cpp=.h)

all: main

main: $(SOURCES) $(HEADERS) 
	mpic++ $(SOURCES) -pthread -o main

clear: 
	clean

clean: 
	rm main a.out

run: main 
	mpirun --hostfile ./hostfile -np 8 ./main

