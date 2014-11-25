#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <ctime>

#define G 1.0
#define ETA 0.1 // Ignore distances less than this
#define SKIPFRAME 1 // Only output every nth step

#define NDIM 3 // Number of dimensions (currently hardcoded to 3)

void print_state(double pos[][NDIM], double vel[][NDIM],int n)
{
  for (int i=0;i<n;i++) {
    std::cerr << i << ' ';
    std::cerr << "body < P";
    std::cerr << "(" << pos[i][0] << "," << pos[i][1] << "," << pos[i][2] << ")";
    std::cerr << " V";
    std::cerr << "(" << vel[i][0] << "," << vel[i][1] << "," << vel[i][2] << ")";
    std::cerr << " >\n";
  }
}

void outputBodies(double pos[][NDIM], double vel[][NDIM],int n) {
  std::cout.precision(15);
  std::cout << std::scientific;
  for (int i=0;i<n;i++) {
    std::cout << pos[i][0] << ' ' << pos[i][1] << ' ';
    std::cout << pos[i][2] << ' ' << vel[i][0] << ' ';
    std::cout << vel[i][1] << ' ' << vel[i][2] << '\n';
  }
  std::cout.unsetf(std::ios::floatfield);
}

int inputCorrupt()
{
  std::cerr << "Corrupt input file\n";
  return -1;
}

void zeroVec(double a[][NDIM], int n)
{
  for (int i=0; i < n; i++) {
    a[i][0] = 0;
    a[i][1] = 0;
    a[i][2] = 0;
  }
}

// updates forces array for each body based on current position
void calculateForces(double forces[][NDIM], double pos[][NDIM], int n)
{
  double dx,dy,dz,r,r2,r3,f,fx,fy,fz;

  // Initialize forces to zero
  zeroVec(forces,n);

  // Aggregate all forces for each body
  for (int i=0; i < n; i++) {
    for (int j=i+1; j<n;j++) {
      dx = pos[j][0] - pos[i][0];
      dy = pos[j][1] - pos[i][1];
      dz = pos[j][2] - pos[i][2];
      r2 = dx*dx+dy*dy+dz*dz;
      r = sqrt(r2);
      r3 = r*r2;
      
      f = double(G*1*1)/(r3 + ETA);  // extra divide by r in there needs to multiply
      fx = dx * f;
      fy = dy * f;
      fz = dz * f;
      
      forces[i][0] += fx;
      forces[i][1] += fy;
      forces[i][2] += fz;
      
      forces[j][0] -= fx;
      forces[j][1] -= fy;
      forces[j][2] -= fz;
    }
  }
}

void stepVec(double a[][NDIM], double b[][NDIM], int n, double dt)
{
  for (int i=0; i < n; i++) {
    a[i][0] += b[i][0]*dt;
    a[i][1] += b[i][1]*dt;
    a[i][2] += b[i][2]*dt;
  }
}

void evolve(double pos[][NDIM], double vel[][NDIM], double forces[][NDIM], int n, double dt) {  
  // LEAPFROG (half step position, full step vel, half step pos)
  
  // Half-step positions
  // pos += vel * dt/2
  stepVec(pos, vel, n, 0.5*dt);

  // Calculate forces applied to each body
  calculateForces(forces, pos, n);

  // Full-step velocity
  // vel += forces * dt
  stepVec(vel, forces, n, dt);

  // Half-step positions
  // pos += vel * dt/2
  stepVec(pos, vel, n, 0.5*dt);
}

void simulate(double pos[][NDIM], double vel[][NDIM], int n, int steps,double dt, bool verbose) {
  std::cout << "SIMULATING " << n << " BODIES, ";
  std::cout << steps << " STEPS, " << dt << " DT\n";
  std::cerr << "Simulating " << n << " bodies for " << steps << " steps, using dt = " << dt << '\n';

  // Set up forces array ( prep for openmpi )
  double (* forces)[NDIM] = new double[n][NDIM];

  clock_t tStart = clock();
  clock_t t = clock();
  for (int i=1;i<=steps;i++){
    // For each step
    evolve(pos, vel, forces, n, dt);
    if ((i-1) % SKIPFRAME == 0) {
        outputBodies(pos, vel, n);
    }
    
    if (verbose) {
      std::cerr << "Step " << i << ", Time " << i*dt << '\n';
      print_state(pos, vel, n);
    }
    if (double(clock()-t)/CLOCKS_PER_SEC > 5.0) { // Every 5 seconds
      std::cerr << "On Step " << i << '/' << steps << " - Time Spent: " << double(clock()-tStart)/CLOCKS_PER_SEC;
      std::cerr << "s, Time left: ~" << int( (double(clock()-tStart)/CLOCKS_PER_SEC)/(double(i)/steps) - (double(clock()-tStart)/CLOCKS_PER_SEC) ) << "s \n";
      t = clock();
    }
  }

  delete[] forces;
}

int main(int argc, char *argv[])
{
  int n; // Number of bodies, passed in as first digit in input
  bool verbose = false;

  if (argc < 3)
  {
    std::cerr << "Need to pass in number of steps and dt" << std::endl 
         << "  'nbody <steps> <dt> [verbose] < input_file > output_file'" << std::endl
         << "  ex. ./nbody 1000 0.1 < InputFiles/i1000.in > outA1000.out" << std::endl;
         return -1;
  }

  if (argc == 4) { 
    switch (atoi(argv[3])) {
      case 1: // true         
        verbose = true; 
        break;
      case 0: // false
        verbose = false;
        break;
      default:
        std::cout << "WARNING: VERBOSE argument " << argv[3] << " not understood (should be 1 or 0)";
    }
  }

  int steps = atoi(argv[1]);
  double dt = atof(argv[2]);
  std::cerr << "Steps to iterate: " << steps << ", dt: " << dt << '\n';
  
  std::cerr << "Reading input...\n";
  std::cin >> n;
  std::cerr << "Generating " << n << " bodies...\n";

  // Arrays of variables on all n bodies
  // double * mass[n]= new double[n];
  double (* pos)[NDIM]= new double[n][NDIM];
  double (* vel)[NDIM]= new double[n][NDIM];
  
  int i = 0; // iterator over n bodies while reading from file
  float k;
  while (std::cin >> k)
  {
    if (i > n-1) return inputCorrupt();
    pos[i][0] = k;
    std::cin >> pos[i][1];
    std::cin >> pos[i][2];

    std::cin >> vel[i][0];
    std::cin >> vel[i][1];
    std::cin >> vel[i][2];
    i++;
  }
  if (i != n) return inputCorrupt();
  std::cerr << "\nDone reading file.";
  
  if (verbose) print_state(pos, vel, n);
  clock_t t1 = clock();
  std::cerr << "SIMULATION BEGIN\n";
  simulate(pos, vel, n, steps, dt, verbose);
  std::cerr << "SIMULATION END\n";
  clock_t t2 = clock();
  std::cerr << "Simulation completed in " << double(t2-t1)/CLOCKS_PER_SEC << " seconds.\n";
  if (verbose) print_state(pos, vel, n);

  // free memory used for bodies
  // delete [] mass; 
  delete [] pos; 
  delete [] vel; 
  
  return 0;
}