#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <utility>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <ctime>
using namespace cl;

#define SKIPFRAME 1 // Only output every nth step

void print_state(cl_float4 cl_pos[], cl_float4 cl_vel[],int n)
{
  for (int i=0;i<n;i++) {
    std::cerr << i << ' ';
    std::cerr << "body < P";
    std::cerr << "(" << cl_pos[i].s[0] << "," << cl_pos[i].s[1] << "," << cl_pos[i].s[2] << ")";
    std::cerr << " V";
    std::cerr << "(" << cl_vel[i].s[0] << "," << cl_vel[i].s[1] << "," << cl_vel[i].s[2] << ")";
    std::cerr << " >\n";
  }
}

void outputBodies(cl_float4 cl_pos[], cl_float4 cl_vel[],int n) {
  std::cout.precision(15);
  std::cout << std::scientific;
  for (int i=0;i<n;i++) {
    std::cout << cl_pos[i].s[0] << ' ' << cl_pos[i].s[1] << ' ';
    std::cout << cl_pos[i].s[2] << ' ' << cl_vel[i].s[0] << ' ';
    std::cout << cl_vel[i].s[1] << ' ' << cl_vel[i].s[2] << '\n';
  }
  std::cout.unsetf(std::ios::floatfield);
}

int inputCorrupt()
{
  std::cerr << "Corrupt input file\n";
  return -1;
}

void simulate(cl_float4 cl_pos[], cl_float4 cl_vel[], int n, int steps,double dt, bool verbose) {
  std::cout << "SIMULATING " << n << " BODIES, ";
  std::cout << steps << " STEPS, " << dt << " DT\n";
  std::cerr << "Simulating " << n << " bodies for " << steps << " steps, using dt = " << dt << '\n';

  clock_t tStart = clock();
  clock_t t = clock();
  Program program;
  std::vector<Device> devices;
  try{
    // Set up opencl gpu stuff
    // Get all platforms
    std::vector<Platform> platforms;
    Platform::get(&platforms);
    // printPlatformListInfo(platforms);

    // Select the default platform and create a context using this platform and the GPU
    cl_context_properties cps[3] = { 
        CL_CONTEXT_PLATFORM, 
        (cl_context_properties)(platforms[0])(), 
        0 
    };
    Context context( CL_DEVICE_TYPE_GPU, cps);

     // Get a list of devices on this platform
    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    // printDeviceListInfo(devices);

    // Create a command queue and use the first device
    CommandQueue queue = CommandQueue(context, devices[0]);

    // Read source file
    std::ifstream sourceFile("evolve_kernel.cl");
    std::string sourceCode(
        std::istreambuf_iterator<char>(sourceFile),
        (std::istreambuf_iterator<char>()));
    Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));

    // Make program from source code in the context
    program = Program(context, source);

    // Build program for these specific devices
    program.build(devices);

    // Make kernel
    Kernel kernel(program, "evolve_kernel");

    // Create memory buffers
    Buffer buffer_pos = Buffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float4));
    Buffer buffer_vel = Buffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float4));
    Buffer buffer_forces = Buffer(context, CL_MEM_READ_WRITE, n * sizeof(cl_float4));
    // Buffer buffer_dt = Buffer(context, CL_MEM_READ_ONLY, sizeof(float));

    // Copy cl_float4 arrays a & b into memory buffers
    queue.enqueueWriteBuffer(buffer_pos, CL_TRUE, 0, n * sizeof(cl_float4), cl_pos);
    queue.enqueueWriteBuffer(buffer_vel, CL_TRUE, 0, n * sizeof(cl_float4), cl_vel);
    // queue.enqueueWriteBuffer(buffer_dt, CL_TRUE, 0, sizeof(float), f_dt);

    // Set arguments to kernel
    kernel.setArg(0, buffer_pos);
    kernel.setArg(1, buffer_vel);
    kernel.setArg(2, buffer_forces);
    kernel.setArg(3, (float)dt);

    NDRange global(n);
    NDRange local(1);


    for (int i=1;i<=steps;i++){
      // For each step
      // Do CL magic
      // evolve(cl_pos, cl_vel, forces, n, dt);
      queue.enqueueNDRangeKernel(kernel, NullRange, global, local);

      if ((i-1) % SKIPFRAME == 0) {
        // read out of buffers
        queue.enqueueReadBuffer(buffer_pos, CL_TRUE, 0, n * sizeof(cl_float4), cl_pos);
        queue.enqueueReadBuffer(buffer_vel, CL_TRUE, 0, n * sizeof(cl_float4), cl_vel);
        outputBodies(cl_pos, cl_vel, n);
      }
      
      if (verbose) {
        std::cerr << "Step " << i << ", Time " << i*dt << '\n';
        print_state(cl_pos, cl_vel, n);
      }
      if (double(clock()-t)/CLOCKS_PER_SEC > 5.0) { // Every 5 seconds
        std::cerr << "On Step " << i << '/' << steps << " - Time Spent: " << double(clock()-tStart)/CLOCKS_PER_SEC;
        std::cerr << "s, Time left: ~" << int( (double(clock()-tStart)/CLOCKS_PER_SEC)/(double(i)/steps) - (double(clock()-tStart)/CLOCKS_PER_SEC) ) << "s \n";
        t = clock();
      }
    }
  }
  catch (Error error)
  {
      std::cout << "CL ERROR: " << error.what() << "(" << error.err() << ")" << std::endl;
      if(error.err() == CL_BUILD_PROGRAM_FAILURE) {
        std::cout << "Build log:" << std::endl << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
      }   
      throw error;
  }
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
  cl_float4* cl_pos = new cl_float4[n];
  cl_float4* cl_vel = new cl_float4[n];
  
  int i = 0; // iterator over n bodies while reading from file
  float k;
  while (std::cin >> k)
  {
    if (i > n-1) return inputCorrupt();

    cl_pos[i].s[0] = k; // x
    std::cin >> cl_pos[i].s[1]; // y
    std::cin >> cl_pos[i].s[2]; // z
    cl_pos[i].s[3] = 1.0; // mass is assumed unity, [not yet used]

    std::cin >> cl_vel[i].s[0];
    std::cin >> cl_vel[i].s[1];
    std::cin >> cl_vel[i].s[2];
    cl_vel[i].s[3] = 0.0; // unused and zeroed for vector operations

    i++;
  }
  if (i != n) return inputCorrupt();
  std::cerr << "\nDone reading file.";
  
  if (verbose) print_state(cl_pos, cl_vel, n);
  clock_t t1 = clock();
  std::cerr << "SIMULATION BEGIN\n";
  simulate(cl_pos, cl_vel, n, steps, dt, verbose);
  std::cerr << "SIMULATION END\n";
  clock_t t2 = clock();
  std::cerr << "Simulation completed in " << double(t2-t1)/CLOCKS_PER_SEC << " seconds.\n";
  if (verbose) print_state(cl_pos, cl_vel, n);

  // free memory used for bodies
  // delete [] mass; 
  delete [] cl_pos; 
  delete [] cl_vel; 
  
  return 0;
}