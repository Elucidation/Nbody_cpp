#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <ctime>
using namespace std;

#define G 1.0
#define ETA 0.1 // Ignore distances less than this
#define SKIPFRAME 1 // Only output every nth step

#define NDIM 3 // Number of dimensions (currently hardcoded to 3)

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

void evolve(double pos[][NDIM], double vel[][NDIM],int n, double dt) {  
  // LEAPFROG 
  double dx,dy,dz,r,r2,r3,f,fx,fy,fz;
  // half-step positions
  for (int i=0; i < n; i++) {
    pos[i][0] += 0.5*vel[i][0]*dt;
    pos[i][1] += 0.5*vel[i][1]*dt;
    pos[i][2] += 0.5*vel[i][2]*dt;
  }
  for (int i=0; i < n; i++) {
    for (int j=i+1; j<n;j++) {
      dx = pos[j][0] - pos[i][0];
      dy = pos[j][1] - pos[i][1];
      dz = pos[j][2] - pos[i][2];
      r2 = dx*dx+dy*dy+dz*dz;
      r = sqrt(r2);
      r3 = r*r2;
      
      f = double(G*1*1)/(r3 + ETA) * dt;
      fx = dx * f;
      fy = dy * f;
      fz = dz * f;
      
      vel[i][0] += fx;
      vel[i][1] += fy;
      vel[i][2] += fz;
      
      vel[j][0] -= fx;
      vel[j][1] -= fy;
      vel[j][2] -= fz;
    }
  }
  // half-step positions
  for (int i=0; i < n; i++) {
    pos[i][0] += 0.5*vel[i][0]*dt;
    pos[i][1] += 0.5*vel[i][1]*dt;
    pos[i][2] += 0.5*vel[i][2]*dt;
  }
}

void simulate(double pos[][NDIM], double vel[][NDIM],int n, int steps,double dt, bool verbose) {
  cout << "SIMULATING " << n << " BODIES, ";
  cout << steps << " STEPS, " << dt << " DT\n";
  cerr << "Simulating " << n << " bodies for " << steps << " steps, using dt = " << dt << '\n';
  clock_t tStart = clock();
  clock_t t = clock();
  for (int i=1;i<=steps;i++){
    // For each step
    evolve(pos, vel, n,dt);
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
    }
  }
}

int main(int argc, char *argv[])
{
  int n; // Number of bodies, passed in as first digit in input
  
  cerr << "Reading input...\n";
  cin >> n;
  cerr << "Generating " << n << " bodies...\n";

  // Arrays of variables on all n bodies
  // double * mass[n]= new double[n];
  double (* pos)[NDIM]= new double[n][NDIM];
  double (* vel)[NDIM]= new double[n][NDIM];
  
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
  cerr << "\nDone reading file.";
  
  if (argc >= 3) {
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
          cout << "WARNING: VERBOSE argument " << argv[3] << " not understood (should be 1 or 0)";
      }
    }
    int steps = atoi(argv[1]);
    double dt = atof(argv[2]);
    cerr << "Steps to iterate: " << steps << ", dt: " << dt << '\n';
    if (verbose) print_state(pos, vel, n);
    clock_t t1 = clock();
    cerr << "SIMULATION BEGIN\n";
    simulate(pos, vel, n, steps, dt, verbose);
    cerr << "SIMULATION END\n";
    clock_t t2 = clock();
    cerr << "Simulation completed in " << double(t2-t1)/CLOCKS_PER_SEC << " seconds.\n";
    if (verbose) print_state(pos, vel, n);
  } else {
    cerr << "You can pass in an integer and a double "
                "number of steps and dt";
  }
  
  // free memory used for bodies
  // delete [] mass; 
  delete [] pos; 
  delete [] vel; 
  
  return 0;
}