CFLAGS = -std=c++0x -Wall
LIBS = -lOpenCL

all: nbody nbody_openmpi nbody_opencl

nbody: nbody.cpp
	g++ $(CFLAGS) nbody.cpp -o nbody $(LIBS)

nbody_openmpi: nbody_openmpi.cpp
	mpiCC $(CFLAGS) nbody_openmpi.cpp -o nbody_openmpi $(LIBS)

nbody_opencl: nbody_opencl.cpp
	g++ $(CFLAGS) nbody_opencl.cpp -o nbody_opencl $(LIBS)


clean:
	rm -rf *.o nbody nbody_openmpi nbody_opencl
