CFLAGS = -std=c++0x -Wall

nbody: nbody.o
	g++ $(CFLAGS) nbody.o -o nbody

nbody.o: nbody.cpp
	g++ $(CFLAGS) -c nbody.cpp

nbody_openmpi: nbody_openmpi.cpp
	mpiCC $(CFLAGS) nbody_openmpi.cpp -o nbody_openmpi

clean:
	rm -rf *.o nbody nbody_openmpi
