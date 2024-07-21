CFLAGS=-I. 
MPICC=mpic++
OBJ=activation_functions.o data.o individual.o log.o main.o memory.o monitor.o network.o network_test.o node.o other.o parallel.o population.o registry.o representation.o roulette.o sockets.o

%.o: %.cc
	$(MPICC) -c -Wno-write-strings -o $@ $< 

bp: $(OBJ)
	$(MPICC) -o ../bin/bp -lm -ldl $^

