// Example relaxation code to experiment with TMR transformations

// Things to do in this code:
// 1) Simplify the algorithm-based distribution.
// 2) Add support for ghost boundary halos (arbritrary width).
// 3) Hierarchy of use (nesting of MulticoreArray)
// 4) Dynamic data
// 5) Better use of OpenMP (specify parallel region "#pragma parallel")

// Use "ManyCoreArrayRuntimeSupport" as the name of this class/project...

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
high_level_relax_orig ( MulticoreArray<T> & array, MulticoreArray<T> & old_array, int dist)
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
#pragma omp parallel for
     for (int k = dist; k < arraySizeZ-dist; k++)
        {
          for (int j = dist; j < arraySizeY-dist; j++)
             {
               for (int i = dist; i < arraySizeX-dist; i++)
                  {
#if 0
if(i==7 && j == 8 && k == 9)
{
printf("high %d %d %d: %f %f %f %f %f %f\n",i,j,k,array(i-6,j,k) , array(i-5,j,k) , array(i-4,j,k) , array(i-3,j,k) , array(i-2,j,k) , array(i-1,j,k));
printf("high %d %d %d: %f %f %f %f %f %f\n",i,j,k,array(i+6,j,k) , array(i+5,j,k) , array(i+4,j,k) , array(i+3,j,k) , array(i+2,j,k) , array(i+1,j,k));
printf("high %d %d %d: %f %f %f %f %f %f\n",i,j,k,array(i,j-6,k) , array(i,j-5,k) , array(i,j-4,k) , array(i,j-3,k) , array(i,j-2,k) , array(i,j-1,k));
printf("high %d %d %d: %f %f %f %f %f %f\n",i,j,k,array(i,j+6,k) , array(i,j+5,k) , array(i,j+4,k) , array(i,j+3,k) , array(i,j+2,k) , array(i,j+1,k));
printf("high %d %d %d: %f %f %f %f %f %f\n",i,j,k,array(i,j,k-6) , array(i,j,k-5) , array(i,j,k-4) , array(i,j,k-3) , array(i,j,k-2) , array(i,j,k-1));
printf("high %d %d %d: %f %f %f %f %f %f\n",i,j,k,array(i,j,k+6) , array(i,j,k+5) , array(i,j,k+4) , array(i,j,k+3) , array(i,j,k+2) , array(i,j,k+1));
}
#endif
                 // 3D simple stencil
                 T tmp=0.0;
                 for(int d=1; d<=dist; d++)
                    tmp += (array(i-d,j,k) + array(i+d,j,k) + array(i,j-d,k) + array(i,j+d,k) + array(i,j,k-d) + array(i,j,k+d) );
                 old_array(i,j,k) = tmp / (6.0 * dist);
/*
                    old_array(i,j,k) = (
        array(i-6,j,k) + array(i-5,j,k) + array(i-4,j,k) + array(i-3,j,k) + array(i-2,j,k) + array(i-1,j,k) + 
        array(i+6,j,k) + array(i+5,j,k) + array(i+4,j,k) + array(i+3,j,k) + array(i+2,j,k) + array(i+1,j,k) + 
        array(i,j-6,k) + array(i,j-5,k) + array(i,j-4,k) + array(i,j-3,k) + array(i,j-2,k) + array(i,j-1,k) + 
        array(i,j+6,k) + array(i,j+5,k) + array(i,j+4,k) + array(i,j+3,k) + array(i,j+2,k) + array(i,j+1,k) + 
        array(i,j,k-6) + array(i,j,k-5) + array(i,j,k-4) + array(i,j,k-3) + array(i,j,k-2) + array(i,j,k-1) + 
        array(i,j,k+6) + array(i,j,k+5) + array(i,j,k+4) + array(i,j,k+3) + array(i,j,k+2) + array(i,j,k+1)  
        ) / 36.0;
*/
                  }
             }
        }
   }


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

     int interArraySubsectionPadding =  0; // Page size to avoid cache conflicts with memory allocted for use by each core

     arraySize[0] = global_arraySize;
     arraySize[1] = global_arraySize;
     arraySize[2] = global_arraySize;

     numberOfCoresArraySize[0] = coreX;
     numberOfCoresArraySize[1] = coreY;
     numberOfCoresArraySize[2] = coreZ;

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

     printf ("Initialize the array1 \n");
     array1.sequenceFill3D(100.0);
     old_array1.sequenceFill3D(100.0);
#if isComparedHighLevel
     printf ("Initialize the array2 \n");
     array2.sequenceFill3D(100.0);
     old_array2.sequenceFill3D(100.0);
#endif

//     array1.display    ("after initialization: array1");
//     old_array1.display("after initialization: old_array1");

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
//     array1.display    ("after initialization of boundary: array1");
//     old_array1.display("after initialization of boundary: old_array1");

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
          high_level_relax_orig(old_array2,array2,stencilSize);
          high_level_relax_orig(array2,old_array2,stencilSize);
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
     cout << "overall high-level time:" << diff(time1,time2).tv_sec << "." << diff(time1,time2).tv_nsec << endl;
#endif
#endif

#endif //isComparedHighLevel

#if 0
  // Output the result
//     array2.display("result: array2");
//     old_array2.display("result: array2");
#endif

#if isCompareHighLevel
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
       high_level_relax(old_array1,array1, stencilSize);
       high_level_relax(array1,old_array1, stencilSize);
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
     cout << "overall high-level time: " << diff(time1,time2).tv_sec +  diff(time1,time2).tv_nsec/1e9 << endl;
#endif
#endif
printf("time: %llu\n", b - a);
#if isComparedHighLevel
     assert(array1 == array2);
#endif
#endif

#if 0
  // Output the result
//     old_array1.display("After relax2D_highlevel(): old_array1");
     array1.display("Result: array1");
     array2.display("After relax2D_highlevel(): array2");
#endif

     return 0;
   }

