#include <iostream>
#include <stdlib.h>
using namespace std;

struct body {
  double x,y,z,vx,vy,vz;
};

struct v3 {
  double x,y,z;
};

void printBody (body b)
{
  cout << "body < P(" << b.x << "," << b.y << "," << b.z << ") ";
  cout << "V(" << b.vx << "," << b.vy << "," << b.  vz << ") >\n";
}

void printBodies(body* bodies,int n)
{
  for (int i=0;i<n;i++) {
    cout << i << " ";
    printBody(bodies[i]);
  }
}

int inputCorrupt()
{
  cout << "Corrupt input file\n";
  return -1;
}

void step(body* bodies,int n, double dt) {
  for (int i=0; i < n; i++) {
    // For each body
    bodies[i].x = bodies[i].x+bodies[i].vx;
  }
}

void simulate(body* bodies,int n, int steps,double dt) {
  for (int i=1;i<=steps;i++){
    // For each step
    step(bodies,n,dt);
    cout << "Step " << i << ", Time " << i*dt << '\n';
  }
}

int main(int argc, char *argv[])
{
  int i; // iterator
  float k;
  int n; // Number of bodies, passed in as first digit in input
  body* bods = NULL; // Contains all the nbodies
  
  cout << "Reading input...\n";
  cin >> n;
  cout << "Generating " << n << " bodies...\n";
  bods = new body[n];
  
  i = 0;
  while (cin >> k)
  {
    if (i > n-1) return inputCorrupt();
    bods[i].x = k;
    cin >> bods[i].y;
    cin >> bods[i].z;
    cin >> bods[i].vx;
    cin >> bods[i].vy;
    cin >> bods[i].vz;
    i++;
  }
  if (i != n) return inputCorrupt();
  
  printBodies(bods,n);
  
  if (argc == 3) {
    int steps = atoi(argv[1]);
    double dt = atof(argv[2]);
    cout << "Steps to iterate: " << steps << ", dt: " << dt << '\n';
    simulate(bods,n,steps,dt);
    printBodies(bods,n);
  } else {
    cout << "You can pass in an integer and a double "
                "number of steps and dt";
  }
  
  // free memory used for bodies
  delete [] bods; 
  bods = NULL;
  
  cout << "\nDone reading file.";
  return 0;
}