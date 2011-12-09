#include <iostream>
using namespace std;

struct body {
  double x,y,z,vx,vy,vz;
};

void printBody (body b)
{
  cout << "Body : P(" << b.x << "," << b.y << "," << b.z << ") ";
  cout << "V(" << b.vx << "," << b.vy << "," << b.  vz << ")\n";
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
    cout << i << endl;
    if (i > n-1) {
      cout << "Corrupt input file\n";
      return -1;
    }
    bods[i].x = k;
    cin >> bods[i].y;
    cin >> bods[i].z;
    cin >> bods[i].vx;
    cin >> bods[i].vy;
    cin >> bods[i].vz;
    i++;
  }
  if (i != n) {
    cout << "Corrupt input file\n";
    return -1;
  }
  
  for (i=0;i<n;i++) {
    printBody(bods[i]);
  }
  
  cout << "\nDone reading file.";
  return 0;
}