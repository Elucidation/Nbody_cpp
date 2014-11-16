Basic nbody graviational simulation program in c++
Comparison of nbody vs nbody_openmpi which uses open-mpi to run force calculations in parallel

![10k bodies](http://i.imgur.com/0JFPQqM.gif)

Speed comparison
---

Running base version:

```
./nbody 1000 0.01 < InputFiles/i1000.in > outA1000.outSteps to iterate: 1000, dt: 0.01
Reading input...
Generating 1000 bodies...

Done reading file.SIMULATION BEGIN
Simulating 1000 bodies for 1000 steps, using dt = 0.01
On Step 147/1000 - Time Spent: 5.01023s, Time left: ~29s 
On Step 292/1000 - Time Spent: 10.0281s, Time left: ~24s 
On Step 437/1000 - Time Spent: 15.0409s, Time left: ~19s 
On Step 580/1000 - Time Spent: 20.0654s, Time left: ~14s 
On Step 724/1000 - Time Spent: 25.0883s, Time left: ~9s 
On Step 868/1000 - Time Spent: 30.1039s, Time left: ~4s 
SIMULATION END
Simulation completed in 34.6856 seconds.
```

Running with open-mpi:

```
sudo mpirun -np 8 nbody_openmpi 1000 0.01 < InputFiles/i1000.in > outB1000.out
Steps to iterate: 1000, dt: 0.01
Reading input...
Generating 1000 bodies...

Done reading file.
SIMULATION BEGIN
Simulating 1000 bodies for 1000 steps, using dt = 0.01
On Step 236/1000 - Time Spent: 5.00299s, Time left: ~16s 
On Step 460/1000 - Time Spent: 10.0161s, Time left: ~11s 
On Step 654/1000 - Time Spent: 15.0183s, Time left: ~7s 
On Step 827/1000 - Time Spent: 20.0226s, Time left: ~4s 
SIMULATION END
Simulation completed in 24.7905 seconds.
```

Usage without Open-MPI
---
To compile normal version:
	make

To run:
	nbody.exe STEPS DT < INPUTFILE > OUTPUTFILE

Example:
	nbody.exe 1000 0.01 < InputFiles/testInput > testOutput.out



Outdated pictures
---

Nov 16, 2014:
![1k bodies](http://i.imgur.com/TTmS4t2.gif)
![10k bodies pic](http://i.imgur.com/Ocvs1ng.png)

Matlab:

![5 Bodies](http://i.imgur.com/A4ctD.jpg)
![10k Bodies in cube](http://i.imgur.com/ZXPdF.jpg)
![10k Bodies drawn trajectories](http://i.imgur.com/eann1.jpg)
![3D view](http://i.imgur.com/T4m6W.jpg)

