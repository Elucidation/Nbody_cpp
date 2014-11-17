CFLAGS = -std=c++0x -Wall

all: nbody nbody_openmpi

nbody: nbody.cpp
	g++ $(CFLAGS) nbody.cpp -o nbody

nbody_openmpi: nbody_openmpi.cpp
	mpiCC $(CFLAGS) nbody_openmpi.cpp -o nbody_openmpi

clean:
	rm -rf *.o nbody nbody_openmpi
