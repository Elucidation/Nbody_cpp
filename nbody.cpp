#include <iostream>
using namespace std;

#define NUM_BODS 2

struct body {
  double x,y,z,vx,vy,vz;
} bods [NUM_BODS];

void printBody (body b)
{
  cout << "Body : P(" << b.x << "," << b.y << "," << b.z << ") ";
  cout << "V(" << b.vx << "," << b.vy << "," << b.  vz << ")\n";
}

int main(int argc, char *argv[])
{
  int i;
  float k;
  
  cout << "Hello World\n";
  
  i = 0;
  while (cin >> k)
  {
    bods[i].x = k;
    cin >> bods[i].y;
    cin >> bods[i].z;
    cin >> bods[i].vx;
    cin >> bods[i].vy;
    cin >> bods[i].vz;
    i++;
  }
  
  for (i=0;i<NUM_BODS;i++) {
    printBody(bods[i]);
  }
  
  cout << "\nDone reading file.";
  return 0;
}