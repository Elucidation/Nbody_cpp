#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <ctime>
using namespace std;
#define SETVEC(a,xx,yy,zz) a.x = xx; a.y = yy; a.z = zz
#define ADDVEC(c,a,b) c.x = a.x + b.x; c.y = a.y + b.y; c.z = a.z + b.z
#define SUBVEC(c,a,b) c.x = a.x - b.x; c.y = a.y - b.y; c.z = a.z - b.z
#define ADDVECMULT(c,a,b,s) c.x = a.x + b.x*(s); c.y = a.y + b.y*(s); c.z = a.z + b.z*(s)
#define MAGN2(a) (a.x*a.x+a.y*a.y+a.z*a.z)
#define MAGN(a) sqrt(MAGN2(a))
#define MULTVECSCALAR(c,a,s) c.x = a.x*(s); c.y = a.y*(s); c.z = a.z*(s)

#define G 1.0
#define ETA 0.1 // Ignore distances less than this

struct v3 {
  double x,y,z;
};

struct body {
  v3 pos,vel;
};

void printv3(v3 vec)
{
  cerr << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
}


void printBody (body b)
{
  cerr << "body < P";
  printv3(b.pos);
  cerr << " V";
  printv3(b.vel);
  cerr << " >\n";
}

void printBodies(body* bodies,int n)
{
  for (int i=0;i<n;i++) {
    cerr << i << ' ';
    printBody(bodies[i]);
  }
}

void outputBodies(body* bodies,int n) {
  for (int i=0;i<n;i++) {
    cout << bodies[i].pos.x << ' ' << bodies[i].pos.y << ' ';
    cout << bodies[i].pos.z << ' ' << bodies[i].vel.x << ' ';
    cout << bodies[i].vel.y << ' ' << bodies[i].vel.z << '\n';
  }
}

int inputCorrupt()
{
  cerr << "Corrupt input file\n";
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
    double r2 = MAGN2(d) + ETA*ETA; // ETA softening here.
    double r = sqrt(r2); 
    //if (r < ETA) continue; // Skip acceleration if distance is less than ETA
    MULTVECSCALAR(d,d,1.0/r);
    
    // ^a = ^d/r * G * m * m / (r*r);
    // accels = accels + ^a
    ADDVECMULT(a,a, d, G*1*1 / (r2*r) ) ;
    
  }
  return a;
}

void step(body* bodies,int n, double dt) {
  //~ for (int i=0; i < n; i++) {
    //~ // For each body
    
    //~ // Calculate accelerations on body from other bodies
    //~ v3 a = getForces(i,bodies,n);
    
    //~ // pos = pos + vel*dt
    //~ ADDVECMULT(bodies[i].pos,bodies[i].pos, bodies[i].vel,dt);
    
    //~ // vel = vel + acceleration*dt // After position update so vel doesn't change first
    //~ ADDVECMULT(bodies[i].vel,bodies[i].vel, a, dt);
  //~ }
  double dx,dy,r,f,fx,fy;
  for (int i=0; i < n; i++) {
    for (int j=i+1; j<n;j++) {
      dx = bodies[j].pos.x - bodies[i].pos.x;
      dy = bodies[j].pos.y - bodies[i].pos.y;
      r = sqrt(dx*dx+dy*dy) + ETA;
      f = double(G*1*1)/(r*r);
      fx = dx / r * f;
      fy = dy / r * f;
      bodies[i].vel.x += fx*dt;
      bodies[i].vel.y += fy*dt;
      bodies[j].vel.x -= fx*dt;
      bodies[j].vel.y -= fy*dt;
    }
  }
  for (int i=0; i < n; i++) {
    bodies[i].pos.x += bodies[i].vel.x*dt;
    bodies[i].pos.y += bodies[i].vel.y*dt;
  }
}

void simulate(body* bodies,int n, int steps,double dt, bool verbose) {
  cout << "SIMULATING " << n << " BODIES, ";
  cout << steps << " STEPS, " << dt << " DT\n";
  cerr << "Simulating " << n << " bodies for " << steps << " steps, using dt = " << dt << '\n';
  clock_t t = clock();
  for (int i=1;i<=steps;i++){
    // For each step
    step(bodies,n,dt);
    outputBodies(bodies,n);
    
    if (verbose) {
      cerr << "Step " << i << ", Time " << i*dt << '\n';
      printBodies(bodies,n);
    }
    if (double(clock()-t)/CLOCKS_PER_SEC > 5.0) { // Every 5 seconds
      cerr << "On Step " << i << '/' << steps << '\n';
      t = clock();
    }
  }
}

int main(int argc, char *argv[])
{
  int i; // iterator
  float k;
  int n; // Number of bodies, passed in as first digit in input
  body* bods = NULL; // Contains all the nbodies
  
  cerr << "Reading input...\n";
  cin >> n;
  cerr << "Generating " << n << " bodies...\n";
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
    if (verbose) printBodies(bods,n);
    clock_t t1 = clock();
    cerr << "SIMULATION BEGIN\n";
    simulate(bods,n,steps,dt,verbose);
    cerr << "SIMULATION END\n";
    clock_t t2 = clock();
    cerr << "Simulation completed in " << double(t2-t1)/CLOCKS_PER_SEC << " seconds.\n";
    if (verbose) printBodies(bods,n);
  } else {
    cerr << "You can pass in an integer and a double "
                "number of steps and dt";
  }
  
  // free memory used for bodies
  delete [] bods; 
  bods = NULL;
  
  cerr << "\nDone reading file.";
  return 0;
}