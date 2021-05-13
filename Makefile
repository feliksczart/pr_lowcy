SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.cpp=.h)

all: main

main: $(SOURCES) $(HEADERS) 
	mpicc $(SOURCES) -o main

clear: 
	clean

clean: 
	rm main a.out

run: main 
	mpirun --hostfile ./hostfile -np 8 ./main

