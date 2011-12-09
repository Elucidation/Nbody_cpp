#include <iostream>
#include <stdlib.h>
using namespace std;
#define ADDVEC(a,b) a.x = a.x + b.x; a.y = a.y + b.y; a.z = a.z + b.z
#define ADDVECMULT(a,b,s) a.x = a.x + b.x*s; a.y = a.y + b.y*s; a.z = a.z + b.z*s

struct v3 {
  double x,y,z;
};

struct body {
  v3 pos,vel;
};

void printv3(v3 vec)
{
  cout << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
}


void printBody (body b)
{
  cout << "body < P";
  printv3(b.pos);
  cout << " V";
  printv3(b.vel);
  cout << " >\n";
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
    // pos = pos + vel*dt
    ADDVECMULT(bodies[i].pos, bodies[i].vel,dt);
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
    bods[i].pos.x = k;
    cin >> bods[i].pos.y;
    cin >> bods[i].pos.z;
    cin >> bods[i].vel.x;
    cin >> bods[i].vel.y;
    cin >> bods[i].vel.z;
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