GCC_DEBUG=gcc -Wall -g -pg 
GCC_RELEASE=gcc -Wall
GCC=$(GCC_DEBUG)

all: tsp vp pb ru

clean:
	rm *.o

tsp: graf.o heap.o hasharray.o set.o btree.o problem-tsp.o
		$(GCC) -DPROBLEM_TSP graf.o heap.o hasharray.o set.o btree.o problem-tsp.o main.c -o ../bin/tsp

vp: graf.o heap.o hasharray.o set.o btree.o problem-vp.o
		$(GCC) -DPROBLEM_VP graf.o heap.o hasharray.o set.o btree.o problem-vp.o main.c -o ../bin/vp

pb: graf.o heap.o hasharray.o set.o btree.o problem-pb.o
		$(GCC) -DPROBLEM_PB graf.o heap.o hasharray.o set.o btree.o problem-pb.o main.c -o ../bin/pb

ru: graf.o heap.o hasharray.o set.o btree.o problem-ru.o
		$(GCC) -DPROBLEM_RU graf.o heap.o hasharray.o set.o btree.o problem-ru.o main.c -o ../bin/ru

graf.o: graf.c graf.h
	$(GCC) -c graf.c

heap.o: heap.c heap.h
	$(GCC) -c heap.c

hasharray.o: hasharray.c hasharray.h
	$(GCC) -c hasharray.c

set.o: set.c set.h
	$(GCC) -c set.c

btree.o: btree.c btree.h
	$(GCC) -c btree.c

problem-tsp.o: problem-tsp.c problem-tsp.h
	$(GCC) -c problem-tsp.c

problem-vp.o: problem-vp.c problem-vp.h
	$(GCC) -c problem-vp.c

problem-pb.o: problem-pb.c problem-pb.h
	$(GCC) -c problem-pb.c

problem-ru.o: problem-ru.c problem-ru.h
	$(GCC) -c problem-ru.c
