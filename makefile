CFLAGS = -std=c++0x -Wall

nbody: nbody.o
	g++ $(CFLAGS) nbody.o -o nbody

nbody.o: nbody.cpp
	g++ $(CFLAGS) -c nbody.cpp

clean:
	rm -rf nbody.exe nbody.o # * has issues with windows path with spaces
