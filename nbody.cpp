#include <iostream>
#include <stdlib.h>
#include <math.h>
using namespace std;
#define SETVEC(a,xx,yy,zz) a.x = xx; a.y = yy; a.z = zz
#define ADDVEC(c,a,b) c.x = a.x + b.x; c.y = a.y + b.y; c.z = a.z + b.z
#define SUBVEC(c,a,b) c.x = a.x - b.x; c.y = a.y - b.y; c.z = a.z - b.z
#define ADDVECMULT(c,a,b,s) c.x = a.x + b.x*(s); c.y = a.y + b.y*(s); c.z = a.z + b.z*(s)
#define MAGN2(a) (a.x*a.x+a.y*a.y+a.z*a.z)
#define MAGN(a) sqrt(MAGN2(a))
#define MULTVECSCALAR(c,a,s) c.x = a.x*(s); c.y = a.y*(s); c.z = a.z*(s)

#define G 1.0
#define ETA 0.01 // Ignore distances less than this

#define VERBOSE true

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

v3 getForces(int index, body* bodies, int n) {
  v3 a = {0,0,0}; // Initialize to zeros
  for (int i=0; i < n; i++) {
    if (i == index) continue; // Skip same one
    // For each body
    // Get direction vector between two bodies ^d
    v3 d;
    SUBVEC(d,bodies[i].pos,bodies[index].pos); // d = bodies[i].pos - b.pos
    
    // Get distance magnitude r
    double r2 = MAGN2(d);
    double r = sqrt(r2);
    if (r < ETA) continue; // Skip acceleration if distance is less than ETA
    MULTVECSCALAR(d,d,1.0/r);
    
    // ^a = ^d/r * G * m * m / (r*r);
    // accels = accels + ^a
    ADDVECMULT(a,a, d, G*1*1 / (r2*r) ) ;
    
  }
  return a;
}

void step(body* bodies,int n, double dt) {
  for (int i=0; i < n; i++) {
    // For each body
    
    // Calculate accelerations on body from other bodies
    v3 a = getForces(i,bodies,n);
    
    // pos = pos + vel*dt
    ADDVECMULT(bodies[i].pos,bodies[i].pos, bodies[i].vel,dt);
    
    // vel = vel + acceleration*dt // After position update so vel doesn't change first
    ADDVECMULT(bodies[i].vel,bodies[i].vel, a, dt);
    
  }
}

void simulate(body* bodies,int n, int steps,double dt) {
  for (int i=1;i<=steps;i++){
    // For each step
    step(bodies,n,dt);
    if (VERBOSE) {
      cout << "Step " << i << ", Time " << i*dt << '\n';
      printBodies(bodies,n);
    }
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
    cout << "SIMULATION BEGIN\n";
    simulate(bods,n,steps,dt);
    cout << "SIMULATION END\n";
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