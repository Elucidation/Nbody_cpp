// Uses openmpi to parallelize calculation of forces
// N-body simulation using leapfrog scheme, brute force pair-wise matching O(n^2)

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <ctime>
using namespace std;

#include "mpi.h"

#define G 1.0
#define ETA 0.1 // Ignore distances less than this
#define SKIPFRAME 1 // Only output every nth step

#define NDIM 3 // Number of dimensions (currently hardcoded to 3)

//////
void timestamp();
//////

void print_state(double pos[][NDIM], double vel[][NDIM],int n)
{
  for (int i=0;i<n;i++) {
    cerr << i << ' ';
    cerr << "body < P";
    cerr << "(" << pos[i][0] << "," << pos[i][1] << "," << pos[i][2] << ")";
    cerr << " V";
    cerr << "(" << vel[i][0] << "," << vel[i][1] << "," << vel[i][2] << ")";
    cerr << " >\n";
  }
}

void outputBodies(double pos[][NDIM], double vel[][NDIM],int n) {
  cout.precision(15);
  cout << scientific;
  for (int i=0;i<n;i++) {
    cout << pos[i][0] << ' ' << pos[i][1] << ' ';
    cout << pos[i][2] << ' ' << vel[i][0] << ' ';
    cout << vel[i][1] << ' ' << vel[i][2] << '\n';
  }
  cout.unsetf(ios::floatfield);
}

int inputCorrupt()
{
  cerr << "Corrupt input file\n";
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
void calculateForces(double forces[][NDIM], double pos[][NDIM], int n, int id, int p)
{
  double dx,dy,dz,r,r2,r3,f,fx,fy,fz;

  // Initialize forces to zero
  zeroVec(forces,n);

  // for (int i=0; i < n; i++) {
  // Aggregate all forces for each body
  // stride based on id
  for (int i=id; i < n; i+=p) { // split up n bodies into p processes
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

  // All reduce the forces together
  // MPI::COMM_WORLD.Allreduce(&forces, &forces, n*NDIM, MPI::DOUBLE, MPI::SUM);

  // sum up forces to id 0
  if (id == 0)
  {
    MPI::COMM_WORLD.Reduce(MPI::IN_PLACE, forces, n*NDIM, MPI::DOUBLE, MPI::SUM, 0);
  }
  else
  {
    MPI::COMM_WORLD.Reduce(forces, forces, n*NDIM, MPI::DOUBLE, MPI::SUM, 0);
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

// TODO : parallelize the stepVec calls!
void evolve(double pos[][NDIM], double vel[][NDIM], double forces[][NDIM], int n, 
            double dt, int id, int p) {  
  // LEAPFROG (half step position, full step vel, half step pos)
  
  // Half-step positions
  // pos += vel * dt/2
  if (id == 0)
  {
    stepVec(pos, vel, n, 0.5*dt);
  }
  // Broadcast updated position to all
  MPI::COMM_WORLD.Bcast(pos, n*NDIM, MPI::DOUBLE, 0);

  // Calculate forces applied to each body
  calculateForces(forces, pos, n, id, p); // Note, Allreduce vs Reduce

  // Full-step velocity
  // vel += forces * dt
  if (id == 0) // currently forces is only updated in id 0, 
  {
    stepVec(vel, forces, n, dt);
  }
  // Broadcast updated position to all
  MPI::COMM_WORLD.Bcast(vel, n*NDIM, MPI::DOUBLE, 0);

  // Half-step positions
  // pos += vel * dt/2
  if (id == 0)
  {
    stepVec(pos, vel, n, 0.5*dt);
  }
  // Broadcast updated position to all
  MPI::COMM_WORLD.Bcast(pos, n*NDIM, MPI::DOUBLE, 0);
}

void simulate(double pos[][NDIM], double vel[][NDIM], int n, 
              int steps,double dt, bool verbose, int id, int p) {
  if (id == 0)
  {
    cout << "SIMULATING " << n << " BODIES, ";
    cout << steps << " STEPS, " << dt << " DT\n";
    cerr << "Simulating " << n << " bodies for " << steps << " steps, using dt = " << dt << '\n';
  }

  // Set up forces array ( prep for openmpi )
  double (* forces)[NDIM] = new double[n][NDIM];
  
  clock_t tStart;
  clock_t t;

  if (id == 0)
  {
    tStart = clock();
    t = clock();
  }
  for (int i=1;i<=steps;i++){
    // For each step
    evolve(pos, vel, forces, n, dt, id, p);

    if (id == 0)
    {
      if ((i-1) % SKIPFRAME == 0) {
          outputBodies(pos, vel, n);
      }
      
      if (verbose) {
        cerr << "Step " << i << ", Time " << i*dt << '\n';
        print_state(pos, vel, n);
      }
      if (double(clock()-t)/CLOCKS_PER_SEC > 5.0) { // Every 5 seconds
        cerr << "On Step " << i << '/' << steps << " - Time Spent: " << double(clock()-tStart)/CLOCKS_PER_SEC;
        cerr << "s, Time left: ~" << int( (double(clock()-tStart)/CLOCKS_PER_SEC)/(double(i)/steps) - (double(clock()-tStart)/CLOCKS_PER_SEC) ) << "s \n";
        t = clock();
        timestamp();
      }
    }
  }

  delete[] forces;
}

int main(int argc, char *argv[])
{

  // MPI stuff
  /////////////////////////////////////////////////////////////////
  MPI::Init(argc, argv);
  int p = MPI::COMM_WORLD.Get_size();
  int id = MPI::COMM_WORLD.Get_rank();

  // Initialize from passed in input arguments
  /////////////////////////////////////////
  if (argc < 3)
  {
    if (id == 0) {
      cerr << "Need to pass in number of steps and dt" << endl 
           << "  'nbody <steps> <dt> [verbose] < input_file > output_file'" << endl
           << "  ex. ./nbody 1000 0.1 < InputFiles/i1000.in > outA1000.out" << endl;
    }
    MPI::Finalize();
    return -1;
  }

  bool verbose = false;
  if (argc == 4) { 
    switch (atoi(argv[3])) {
      case 1: // true         
        verbose = true; 
        break;
      case 0: // false
        verbose = false;
        break;
      default:
        if (id == 0) {
          cout << "WARNING: VERBOSE argument " << argv[3] << " not understood (should be 1 or 0)";
        }
    }
  }

  int steps = atoi(argv[1]);
  double dt = atof(argv[2]);
  if (id == 0)
  {
    cerr << "Steps to iterate: " << steps << ", dt: " << dt << '\n';
  }

  /////////////////////////////////////////

  int n; // Number of bodies, passed in as first digit/line in input

  if (id == 0)
  {
    cerr << "Reading input...\n";
    cin >> n;
  }
  // Broadcast n to all processes
  /////////////////////////////////////////////////////////////////
  MPI::COMM_WORLD.Bcast(&n, 1, MPI::INT, 0);
  
  

  // Arrays of variables on all n bodies
  // double * mass[n]= new double[n];
  double (* pos)[NDIM]= new double[n][NDIM];
  double (* vel)[NDIM]= new double[n][NDIM];
  
  if (id == 0)
  {
    cerr << "Generating " << n << " bodies...\n";
    int i = 0; // iterator over n bodies while reading from file
    float k;
    while (cin >> k)
    {
      if (i > n-1) return inputCorrupt();
      pos[i][0] = k;
      cin >> pos[i][1];
      cin >> pos[i][2];

      cin >> vel[i][0];
      cin >> vel[i][1];
      cin >> vel[i][2];
      i++;
    }
    if (i != n) return inputCorrupt();
    cerr << endl << "Done reading file." << endl;
  }

  // Broadcast body information to all processes (in future scatter this?)
  /////////////////////////////////////////////////////////////////
  MPI::COMM_WORLD.Bcast(pos, n*NDIM, MPI::DOUBLE, 0);
  MPI::COMM_WORLD.Bcast(vel, n*NDIM, MPI::DOUBLE, 0);
  
  // 1 - PRE-SIMULATION
  if (verbose && id == 0) print_state(pos, vel, n);

  clock_t t1;
  if (id == 0)
  {
   t1 = clock();
   timestamp();
   cerr << "SIMULATION BEGIN\n";
  }

  // 2 - RUN SIMULATION
  simulate(pos, vel, n, steps, dt, verbose, id, p);

  // 3 - POST-SIMULATION
  if (id == 0)
  {
    cerr << "SIMULATION END\n";
    clock_t t2 = clock();
    cerr << "Simulation completed in " << double(t2-t1)/CLOCKS_PER_SEC << " seconds.\n";
    timestamp();
  }
  if (verbose && id == 0) print_state(pos, vel, n);

  // free memory used for bodies
  // delete [] mass; 
  delete [] pos; 
  delete [] vel;

  MPI::Finalize();
  
  return 0;
}

void timestamp()
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct std::tm *tm_ptr;
  // size_t len;
  std::time_t now;

  now = std::time ( NULL );
  tm_ptr = std::localtime ( &now );

  std::strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm_ptr );

  cerr << time_buffer << endl;

  return;
# undef TIME_SIZE
}
