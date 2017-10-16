// Example relaxation code to experiment with TMR transformations

// Things to do in this code:
// 1) Simplify the algorithm-based distribution.
// 2) Add support for ghost boundary halos (arbritrary width).
// 3) Hierarchy of use (nesting of MulticoreArray)
// 4) Dynamic data
// 5) Better use of OpenMP (specify parallel region "#pragma parallel")

// Use "ManyCoreArrayRuntimeSupport" as the name of this class/project...

#define hasHaloCopy 1
#define hasDetachHalo 1
#define usePAPI 0
#define isComparedHighLevel 1

#include<stdio.h>
#include<stdlib.h>

#include<assert.h>

#include<vector>
#include<string>
#include "rdtsc.h"
// This header file can't be found (unless John Van Zant installs the libnuma development version or the RPM).
// Note that libnuma will return a NULL pointer when ask to allocate zero bytes (C++ allocation will alternatively
// allocated memory returning a valid pointer; so the two are semantically different on minor points). We now
// handle this case to preserve the same semantics; when using libnuma we check for this case of zero bytes and 
// use the C++ allocation to get a valid pointer.

// Support for timing
#include <iostream>
#include <time.h>

// Local support for a timer better than clock().
#include "highresTimer.h"

using namespace std;

#ifdef PREFETCH
#include "stencil-prefetch.h"
#else
#include "stencil.h"
#endif
#include <omp.h>

#if usePAPI
#include <papi.h>
void handle_error (int retval, char* errormsg)
{
  printf("PAPI error %d: %s at %s\n", retval, PAPI_strerror(retval),errormsg);
  exit(1);
}
void init_papi() {
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT && retval < 0) {
    printf("PAPI library version mismatch!\n");
    exit(1);
  }
  if (retval < 0) handle_error(retval,"init_papi");

  std::cout << "PAPI Version Number: MAJOR: " << PAPI_VERSION_MAJOR(retval)
            << " MINOR: " << PAPI_VERSION_MINOR(retval)
            << " REVISION: " << PAPI_VERSION_REVISION(retval) << "\n";
}
#endif
// *********************************************************************
// *********************************************************************
// Examples of the use of the Core<T> and MulticoreArray<T> abstractions
// *********************************************************************
// *********************************************************************

template <typename T>
void
high_level_relax ( MulticoreArray<T> & array, MulticoreArray<T> & old_array, int dist)
   {
  // The code in this function is what we expect users to want to write. However, 
  // while this code uses the same data as the transformed code, but it less efficient.

     const int arraySizeX = array.get_arraySize(0);
     const int arraySizeY = array.get_arraySize(1);
     const int arraySizeZ = array.get_arraySize(2);

  // Higher level could be:
  //    array(I) = (array(I-1) + array(I+1)) / 2.0;
  // but our current abstraction does not support this
  // (use array class (e.g. A++) for this level).

  // Use pragma to trigger compiler transformations, such as:
  // #pragma manycore
#pragma stencil-manycore(T, old_array, array)  D(k,0,arraySizeZ,dist) D(j,0,arraySizeY,dist) D(i,0,arraySizeX,dist) H(d,1,dist,1)
     for (int k = dist; k < arraySizeZ-dist; k++)
        {
          for (int j = dist; j < arraySizeY-dist; j++)
             {
               for (int i = dist; i < arraySizeX-dist; i++)
                  {
                 // 3D simple stencil
                  T tmp=0.0;
                  for(int d=1; d<=dist; d++)
                     tmp += (array(i-d,j,k) + array(i+d,j,k) + array(i,j-d,k) + array(i,j+d,k) + array(i,j,k-d) + array(i,j,k+d) );
                 old_array(i,j,k) = tmp / (6.0 * dist);
                  }
             }
        }
   }

template <typename T>
void
test_relax( MulticoreArray<T> & array,  MulticoreArray<T> & old_array,int dist, bool isMidLevel )
   {
     int numberOfCores_X = array.get_coreArraySize(0);
     int numberOfCores_Y = array.get_coreArraySize(1);
     int numberOfCores_Z = array.get_coreArraySize(2);
     int numberCores = numberOfCores_X*numberOfCores_Y*numberOfCores_Z;
#ifndef __APPLE__
  // Setup timer
     timespec time1[numberCores], time2[numberCores];
     timespec bdyExecTime[numberCores];
     timespec ctrExecTime[numberCores];
#endif
     if(array.isHaloExist())
     {
        array.haloExchange();
     }

#if HAVE_NUMA_H
     int numberOfNUMACores_X = array.get_NUMAcoreArraySize(0);
     int numberOfNUMACores_Y = array.get_NUMAcoreArraySize(1);
     int numberOfNUMACores_Z = array.get_NUMAcoreArraySize(2);
     int numberNUMACores = numberOfNUMACores_X*numberOfNUMACores_Y*numberOfNUMACores_Z;
     omp_set_num_threads(numberNUMACores);
#pragma omp parallel 
     {
       cpu_set_t mask;
       CPU_ZERO(&mask);
       int tid = omp_get_thread_num();
       CPU_SET(tid, &mask);
       if(sched_setaffinity(0, sizeof(mask),&mask) == -1)
       {
         printf("Error: sched_setaffinity(%d, sizeof(mask), &mask)\n",tid);
       }
       int physicalCore = sched_getcpu();
       int idz = physicalCore / (numberOfNUMACores_X * numberOfNUMACores_Y);
       int idy = (physicalCore - idz * (numberOfNUMACores_X * numberOfNUMACores_Y)) / numberOfNUMACores_X;
       int idx = physicalCore % numberOfNUMACores_X;
       int XcorePerNUMAcore = numberOfCores_X/numberOfNUMACores_X;
       int YcorePerNUMAcore = numberOfCores_Y/numberOfNUMACores_Y;
       int ZcorePerNUMAcore = numberOfCores_Z/numberOfNUMACores_Z;
       int baseX = idx * (XcorePerNUMAcore);
       int baseY = idy * (YcorePerNUMAcore);
       int baseZ = idz * (ZcorePerNUMAcore);
       for(int ix = 0 ; ix < XcorePerNUMAcore; ++ix) 
       for(int iy = 0 ; iy < XcorePerNUMAcore; ++iy) 
       for(int iz = 0 ; iz < XcorePerNUMAcore; ++iz)
       {
         int core_Z = baseZ + iz; 
         int core_Y = baseY + iy; 
         int core_X = baseX + ix; 
         int core = array.coreArrayElement(core_X,core_Y,core_Z);
//         printf("HW core ID: %d thread ID: %d is updating logical core %d: %d %d %d\n",sched_getcpu(), tid, core,core_X,core_Y,core_Z); 
#else

  // Use OpenMP to support the threading...
#pragma omp parallel for
     for (int core = 0; core < numberCores; core++)
     {
       int tid = omp_get_thread_num();

       int core_Z = core / (numberOfCores_X * numberOfCores_Y);
       int core_Y = (core - core_Z * (numberOfCores_X * numberOfCores_Y)) / numberOfCores_X;
       int core_X = core % numberOfCores_X;
#endif
       //int core_X = (core >> 0) & (0x00000007);
       //int core_Y = (core >> 3) & (0x00000007);
       //int core_Z = (core >> 6) & (0x00000007);
            // This lifts out loop invariant portions of the code.
               Core<T> & coreMemory = array.getCore(core_X,core_Y,core_Z);

            // Lift out loop invariant local array size values.
               int sizeX = array.getCore(core_X,core_Y,core_Z).coreArrayNeighborhoodSizes_3D[1][1][1][0];
               int sizeY = array.getCore(core_X,core_Y,core_Z).coreArrayNeighborhoodSizes_3D[1][1][1][1];
               int sizeZ = array.getCore(core_X,core_Y,core_Z).coreArrayNeighborhoodSizes_3D[1][1][1][2];
#if 0
               printf ("\nsizeX = %d sizeY = %d sizeZ = %d \n",sizeX,sizeY,sizeZ);
#endif
               int base_X  = (coreMemory.boundaryCore_3D[0][0] == true) ? dist : 0;
               int bound_X = (coreMemory.boundaryCore_3D[0][1] == true) ? sizeX - (dist+1): sizeX - 1;
               int base_Y  = (coreMemory.boundaryCore_3D[1][0] == true) ? dist : 0;
               int bound_Y = (coreMemory.boundaryCore_3D[1][1] == true) ? sizeY - (dist+1): sizeY - 1;
               int base_Z  = (coreMemory.boundaryCore_3D[2][0] == true) ? dist : 0;
               int bound_Z = (coreMemory.boundaryCore_3D[2][1] == true) ? sizeZ - (dist+1): sizeZ - 1;

#ifndef __APPLE__
  // Setup timer
     clock_gettime(CLOCK_REALTIME, &time1[tid]);
#endif

if(isMidLevel)
{

// Updating the center
               for (int k = dist; k <= sizeZ-(dist+1); k++)
                  {
               for (int j = dist; j <= sizeY-(dist+1); j++)
                  {
                    for (int i = dist; i <= sizeX-(dist+1); i++)
                       {
                 T tmp(0.0);
                 for(int d=1; d<=dist; d++)
                    tmp += (array.getCore(core_X,core_Y,core_Z)(i-d,j,k) + array.getCore(core_X,core_Y,core_Z)(i+d,j,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j-d,k) + array.getCore(core_X,core_Y,core_Z)(i,j+d,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j,k-d) + array.getCore(core_X,core_Y,core_Z)(i,j,k+d) );
                 old_array.getCore(core_X,core_Y,core_Z)(i,j,k) = tmp / (6.0 * dist);
                       }
                  }
                  }
}
else
{
        relax<T>(core,old_array,array,dist);
}

#ifndef __APPLE__
  // Get and report the time.
     clock_gettime(CLOCK_REALTIME, &time2[tid]);
     ctrExecTime[tid] = diff(time1[tid],time2[tid]);
     clock_gettime(CLOCK_REALTIME, &time1[tid]);
#endif

if(isMidLevel)
{
// Updating the boundary
               for (int k = base_Z; k <= bound_Z; k++)
                  {
               for (int j = base_Y; j <= bound_Y; j++)
                  {
                    for (int i = base_X; i < dist; i++)
                       {
                 T tmp(0.0);
                 for(int d=1; d<=dist; d++)
                    tmp += (array.getCore(core_X,core_Y,core_Z)(i-d,j,k) + array.getCore(core_X,core_Y,core_Z)(i+d,j,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j-d,k) + array.getCore(core_X,core_Y,core_Z)(i,j+d,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j,k-d) + array.getCore(core_X,core_Y,core_Z)(i,j,k+d) );
                 old_array.getCore(core_X,core_Y,core_Z)(i,j,k) = tmp / (6.0 * dist);
                       }
                  }
                  }

               for (int k = base_Z; k <= bound_Z; k++)
                  {
               for (int j = base_Y; j <= bound_Y; j++)
                  {
                    for (int i = sizeZ - dist; i <= bound_X; i++)
                       {
                 T tmp(0.0);
                 for(int d=1; d<=dist; d++)
                    tmp += (array.getCore(core_X,core_Y,core_Z)(i-d,j,k) + array.getCore(core_X,core_Y,core_Z)(i+d,j,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j-d,k) + array.getCore(core_X,core_Y,core_Z)(i,j+d,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j,k-d) + array.getCore(core_X,core_Y,core_Z)(i,j,k+d) );
                 old_array.getCore(core_X,core_Y,core_Z)(i,j,k) = tmp / (6.0 * dist);
                       }
                  }
                  }

               for (int k = base_Z; k <= bound_Z; k++)
                  {
               for (int j = base_Y; j < dist; j++)
                  {
                    for (int i = base_X; i <= bound_X; i++)
                       {
                 T tmp(0.0);
                 for(int d=1; d<=dist; d++)
                    tmp += (array.getCore(core_X,core_Y,core_Z)(i-d,j,k) + array.getCore(core_X,core_Y,core_Z)(i+d,j,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j-d,k) + array.getCore(core_X,core_Y,core_Z)(i,j+d,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j,k-d) + array.getCore(core_X,core_Y,core_Z)(i,j,k+d) );
                 old_array.getCore(core_X,core_Y,core_Z)(i,j,k) = tmp / (6.0 * dist);
                       }
                  }
                  }


               for (int k = base_Z; k <= bound_Z; k++)
                  {
               for (int j = sizeY - dist ; j <= bound_Y; j++)
                  {
                    for (int i = base_X; i <= bound_X; i++)
                       {
                 T tmp(0.0);
                 for(int d=1; d<=dist; d++)
                    tmp += (array.getCore(core_X,core_Y,core_Z)(i-d,j,k) + array.getCore(core_X,core_Y,core_Z)(i+d,j,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j-d,k) + array.getCore(core_X,core_Y,core_Z)(i,j+d,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j,k-d) + array.getCore(core_X,core_Y,core_Z)(i,j,k+d) );
                 old_array.getCore(core_X,core_Y,core_Z)(i,j,k) = tmp / (6.0 * dist);
                       }
                  }
                  }

               for (int k = base_Z; k < dist; k++)
                  {
               for (int j = base_Y; j <= bound_Y; j++)
                  {
                    for (int i = base_X; i <= bound_X; i++)
                       {
                 T tmp(0.0);
                 for(int d=1; d<=dist; d++)
                    tmp += (array.getCore(core_X,core_Y,core_Z)(i-d,j,k) + array.getCore(core_X,core_Y,core_Z)(i+d,j,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j-d,k) + array.getCore(core_X,core_Y,core_Z)(i,j+d,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j,k-d) + array.getCore(core_X,core_Y,core_Z)(i,j,k+d) );
                 old_array.getCore(core_X,core_Y,core_Z)(i,j,k) = tmp / (6.0 * dist);
                       }
                  }
                  }

               for (int k = sizeZ - dist; k <= bound_Z; k++)
                  {
               for (int j = base_Y; j <= bound_Y; j++)
                  {
                    for (int i = base_X; i <= bound_X; i++)
                       {
                 T tmp(0.0);
                 for(int d=1; d<=dist; d++)
                    tmp += (array.getCore(core_X,core_Y,core_Z)(i-d,j,k) + array.getCore(core_X,core_Y,core_Z)(i+d,j,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j-d,k) + array.getCore(core_X,core_Y,core_Z)(i,j+d,k) + 
                         array.getCore(core_X,core_Y,core_Z)(i,j,k-d) + array.getCore(core_X,core_Y,core_Z)(i,j,k+d) );
                 old_array.getCore(core_X,core_Y,core_Z)(i,j,k) = tmp / (6.0 * dist);
                       }
                  }
                  }
}
else
{
#if hasHaloCopy == 1
   if(array.hasDetachedHalo())
   {
     relax_on_detachedhalo_boundary<T>(core,old_array,array, dist);
     //array.coreArray[core]->relax_on_detachedhalo_boundary_simplified(core,old_array,array, dist);
   }
#else
     relax_on_boundary<T>(core,old_array,array,dist);
     //array.coreArray[core]->relax_on_boundary_simplified(core,old_array,array,dist);
#endif
}

#ifndef __APPLE__
  // Get and report the time.
     clock_gettime(CLOCK_REALTIME, &time2[tid]);
     bdyExecTime[tid] = diff(time1[tid],time2[tid]);
#endif
#if HAVE_NUMA_H
          }
#endif
        }
#ifndef __APPLE__
     double ctrTime=0.0, bdyTime=0.0;
     for (int core = 0; core < numberCores; core++)
     {
       ctrTime += (ctrExecTime[core].tv_sec) + (ctrExecTime[core].tv_nsec) / 1e9;
       bdyTime += (bdyExecTime[core].tv_sec) + (bdyExecTime[core].tv_nsec) / 1e9;
     }
     cout << "center time:" << ctrTime/numberCores << endl;
     cout << "boundry time:" << bdyTime/numberCores << endl;
#endif
   }


void print_usage()
{
	printf("Usage of Manycore Runtime:\n");
	printf("$> stencil <iteration> <num_of_elem in each direction> <stencil size> <cores in X> <cores in Y> <cores in Z>\n\n");
}
int global_arraySize;
int stencilSize;
int numberOfIterations;
int coreX, coreY, coreZ;
// ****************************************************
// ****************************************************
// Main test function for MulticoreArray<T> abstraction
// ****************************************************
// ****************************************************

int main(int argc, char *argv[])
   {
     unsigned long long a, b;
	if (argc < 7) {
		print_usage();
		return -1;
	}
	else {
		numberOfIterations = atoi(argv[1]);
		global_arraySize = atoi(argv[2]);
		stencilSize = atoi(argv[3]);
		coreX = atoi(argv[4]);
		coreY = atoi(argv[5]);
		coreZ = atoi(argv[6]);
	}
  // A multidimensional array support would have to include inter-array padding (to allow the seperate rows to work well with the cache).
  // If multiple arrays are used then intra-array padding will separate them from each other and permit them to work well with cache.
#if usePAPI
          init_papi();
#define NUM_EVENTS 4

          int Events[NUM_EVENTS] = {PAPI_L1_DCM, PAPI_L1_DCA, PAPI_L2_DCM, PAPI_L2_DCA};
          long_long values[NUM_EVENTS];
          long_long start_cycles, end_cycles, start_usec, end_usec;
	  int retval;
#endif

     int numaSupport = -1;
// #ifdef NUMA_NUM_NODES
#if HAVE_NUMA_H
  // A return value of -1 means that all other functions in this library are undefined.
     numaSupport = numa_available();
#endif

     if (numaSupport != -1)
        {
          printf ("NUMA support is AVAILABLE \n");
        }
       else
        {
          printf ("NUMA support is NOT available numaSupport = %d \n",numaSupport);
        }

     int arraySize[DIM]              = {0,0,0};
     int numberOfCoresArraySize[DIM] = {0,0,0};
     int haloSize[DIM]               = {0,0,0};

     int interArraySubsectionPadding =  0; // Page size to avoid cache conflicts with memory allocted for use by each core

     haloSize[0] = stencilSize;
     haloSize[1] = stencilSize;
     haloSize[2] = stencilSize;

#if 1
  // 10x10x10 is 1 million elements

     arraySize[0] = global_arraySize;
     arraySize[1] = global_arraySize;
     arraySize[2] = global_arraySize;

#if 0
     arraySize[1] = 1024;
     arraySize[2] = 1;
#endif

//     int numberOfCores =  4; // Number of cores to use (each axis)
     numberOfCoresArraySize[0] = coreX;
     numberOfCoresArraySize[1] = coreY;
     numberOfCoresArraySize[2] = coreZ;

#if 0
     numberOfCoresArraySize[0] = 8;
     numberOfCoresArraySize[1] = 8;
     numberOfCoresArraySize[2] = 1;
#endif

  // Array constructor call
#if hasHaloCopy
#if hasDetachHalo
     MulticoreArray<double>::haloType type = MulticoreArray<double>::detached;
#else
     MulticoreArray<double>::haloType type = MulticoreArray<double>::attached;
#endif
     printf ("Allocate array1 \n");
     MulticoreArray<double> array1    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],haloSize[0],haloSize[1],haloSize[2],type,interArraySubsectionPadding,false);
     printf ("Allocate array1 \n");
     MulticoreArray<double> old_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],haloSize[0],haloSize[1],haloSize[2],type,interArraySubsectionPadding,false);
#if isComparedHighLevel
     printf ("Allocate array2 \n");
     MulticoreArray<double> array2    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],haloSize[0],haloSize[1],haloSize[2],type,interArraySubsectionPadding,false);
     printf ("Allocate array2 \n");
     MulticoreArray<double> old_array2(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],haloSize[0],haloSize[1],haloSize[2],type,interArraySubsectionPadding,false);
     printf ("DONE: Allocate array2 \n");
#endif
#else
     printf ("Allocate array1 \n");
     MulticoreArray<double> array1    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
     printf ("Allocate array1 \n");
     MulticoreArray<double> old_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
#if isComparedHighLevel
     printf ("Allocate array2 \n");
     MulticoreArray<double> array2    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
     printf ("Allocate old_array2 \n");
     MulticoreArray<double> old_array2(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
#endif
#endif

#if 0
     array1.display    ("initial values: array1");
     old_array1.display("initial values: old_array1");
#endif

#if 0
  // Initialize to non-zero value.
     array1 = 1.0;
     array2 = 1.0;
#endif

#if 1
     printf ("Initialize the array1 \n");
     array1.sequenceFill3D(100.0);
     old_array1.sequenceFill3D(100.0);
#if isComparedHighLevel
     printf ("Initialize the array2 \n");
     array2.sequenceFill3D(100.0);
     old_array2.sequenceFill3D(100.0);
#endif
#endif

#if 1
//     array1.display    ("after initialization: array1");
//     old_array1.display("after initialization: old_array1");
#endif

  // Initialize the boundary
#if 0
     int boundaryValue = 0;
     printf ("Initialize the boundary of array1 \n");
     array1.initializeBoundary(boundaryValue);
     old_array1.initializeBoundary(boundaryValue);
#if isComparedHighLevel
     printf ("Initialize the boundary of array2 \n");
     array2.initializeBoundary(boundaryValue);
     old_array2.initializeBoundary(boundaryValue);
#endif
#endif
#if 0
//     array1.display    ("after initialization of boundary: array1");
     old_array1.display("after initialization of boundary: old_array1");
#endif

     printf ("Call relaxation \n");

#if usePAPI == 0
     timespec time1, time2;
#endif
#if isComparedHighLevel
#if usePAPI
          start_cycles = PAPI_get_real_cyc();
          start_usec = PAPI_get_real_usec();
          if((retval=PAPI_start_counters(Events, NUM_EVENTS)) < PAPI_OK)
           {
            printf("\n\t Error : Could not initialise PAPI_flops.");
            printf("\n\t\t Your platform may not support floating point operation event.");
            printf("\n\t Error string : %s  :: Error code : %d \n",PAPI_strerror(retval),retval);
            return(-1);
           }
#else
#ifndef __APPLE__
  // Setup timer
     clock_gettime(CLOCK_REALTIME, &time1);
#endif
#endif
  // Call relaxation on array
     for (int i = 0; i < numberOfIterations/2; i++)
        {
#if 0
          printf ("Calling relaxation on large array \n");
#endif
          high_level_relax(old_array2,array2,stencilSize);
          high_level_relax(array2,old_array2,stencilSize);
        }

#if usePAPI
          if((retval=PAPI_stop_counters(values, NUM_EVENTS)) < PAPI_OK)
           {
            printf("\n\t Error : Could not get PAPI_flops.");
            printf("\n\t Error string : %s  :: Error code : %d \n",PAPI_strerror(retval),retval);
            return(-1);
           }
           end_cycles = PAPI_get_real_cyc();
           end_usec = PAPI_get_real_usec();
	   printf("\t High Level Summary : \n");
           printf("\t Wall clock cycles: %lld\n", end_cycles - start_cycles);
           printf("\t Wall clock time in microseconds: %lld\n", end_usec - start_usec); 
//           printf("\t Data TLB miss : %lld\n",values[0]);
           printf("\t Level 1 data cache misses: %lld\n",values[0]);
           printf("\t Level 1 data cache accesses: %lld\n",values[1]);
           printf("\t Level 1 miss rate: %.3f\n",((float)values[0])/((float)values[1]));
           printf("\t Level 2 data cache misses: %lld\n",values[2]);
           printf("\t Level 2 data cache accesses: %lld\n",values[3]);
           printf("\t Level 2 miss rate: %.3f\n",((float)values[2])/((float)values[3]));
#else
#ifndef __APPLE__
  // Get and report the time.
     clock_gettime(CLOCK_REALTIME, &time2);
     cout << "overall high-level time: " << diff(time1,time2).tv_sec +  diff(time1,time2).tv_nsec/1e9 << endl;
#endif
#endif

#endif //isComparedHighLevel

#if 0
  // Output the result
//     array2.display("result: array2");
//     old_array2.display("result: array2");
#endif

  // Make sure that the results are the same.
//     assert(array1 == array2);
#endif

#if 0
  // Test refactored version of operator.
     relax2D(array1,old_array1);
#endif

#if 1
  // Test refactored version of operator.
//     array1 = 0.0;
#if usePAPI
          start_cycles = PAPI_get_real_cyc();
          start_usec = PAPI_get_real_usec();
          if((retval=PAPI_start_counters(Events, NUM_EVENTS)) < PAPI_OK)
           {
            printf("\n\t Error : Could not initialise PAPI_flops.");
            printf("\n\t\t Your platform may not support floating point operation event.");
            printf("\n\t Error string : %s  :: Error code : %d \n",PAPI_strerror(retval),retval);
            return(-1);
           }
#else
#ifndef __APPLE__
  // Setup timer
     clock_gettime(CLOCK_REALTIME, &time1);
#endif
#endif
a = rdtsc();
     for (int i = 0; i < numberOfIterations/2; i++)
     {
       test_relax<double>(old_array1,array1, stencilSize, false);
       test_relax<double>(array1,old_array1, stencilSize, false);
     }
b = rdtsc();
#if usePAPI
          if((retval=PAPI_stop_counters(values, NUM_EVENTS)) < PAPI_OK)
           {
            printf("\n\t Error : Could not get PAPI_flops.");
            printf("\n\t Error string : %s  :: Error code : %d \n",PAPI_strerror(retval),retval);
            return(-1);
           }
           end_cycles = PAPI_get_real_cyc();
           end_usec = PAPI_get_real_usec();
	   printf("\t Low Level Summary : \n");	
           printf("\t Wall clock cycles: %lld\n", end_cycles - start_cycles);
           printf("\t Wall clock time in microseconds: %lld\n", end_usec - start_usec); 
//           printf("\t Data TLB miss : %lld\n",values[0]);
           printf("\t Level 1 data cache misses: %lld\n",values[0]);
           printf("\t Level 1 data cache accesses: %lld\n",values[1]);
           printf("\t Level 1 miss rate: %.3f\n",((float)values[0])/((float)values[1]));
           printf("\t Level 2 data cache misses: %lld\n",values[2]);
           printf("\t Level 2 data cache accesses: %lld\n",values[3]);
           printf("\t Level 2 miss rate: %.3f\n",((float)values[2])/((float)values[3]));
#else
#ifndef __APPLE__
  // Get and report the time.
     clock_gettime(CLOCK_REALTIME, &time2);
     cout << "overall low-level time: " << diff(time1,time2).tv_sec + diff(time1,time2).tv_nsec/1e9 << endl;
#endif
#endif
printf("time: %llu\n", b - a);
     assert(array1 == array2);
#endif

#if 0
  // Output the result
//     old_array1.display("After relax2D_highlevel(): old_array1");
     array1.display("Result: array1");
     array2.display("After relax2D_highlevel(): array2");
#endif

     return 0;
   }

