// Example relaxation code to experiment with TMR transformations

// Things to do in this code:
// 1) Simplify the algorithm-based distribution.
// 2) Add support for ghost boundary halos (arbritrary width).
// 3) Hierarchy of use (nesting of MulticoreArray)
// 4) Dynamic data
// 5) Better use of OpenMP (specify parallel region "#pragma parallel")

// Use "ManyCoreArrayRuntimeSupport" as the name of this class/project...

#define usePAPI 0

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

template <typename T>
void
sequenceFill3D(T* array, T x, int arraySize)
   {

#pragma omp parallel for
     for (int k = 0; k < arraySize; k++)
        {
          for (int j = 0; j < arraySize; j++)
             {
               for (int i = 0; i < arraySize; i++)
                  {
                      array[i + j * arraySize + k * arraySize*arraySize] = x * (i + j * arraySize + k * arraySize*arraySize);
                  }
             }
        }
   }
// *********************************************************************
// *********************************************************************
// Examples of the use of the Core<T> and MulticoreArray<T> abstractions
// *********************************************************************
// *********************************************************************

template <typename T>
void
high_level_relax (T* array, T* old_array, int arraySize,int dist)
   {
  // The code in this function is what we expect users to want to write. However, 
  // while this code uses the same data as the transformed code, but it less efficient.

     const int arraySizeX = arraySize;
     const int arraySizeY = arraySize;
     const int arraySizeZ = arraySize;

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
                 // 3D simple stencil
                 T tmp = 0.0;
                 for(int d=1; d<=dist; d++)
                    tmp += (array[(i + d) + j * arraySizeX + k * arraySizeX*arraySizeY] + array[(i - d) + j * arraySizeX + k * arraySizeX*arraySizeY] + array[i + (j + d) * arraySizeX + k * arraySizeX*arraySizeY] + array[i + (j-d) * arraySizeX + k * arraySizeX*arraySizeY] + array[i + j * arraySizeX + (k+d) * arraySizeX*arraySizeY] + array[i + j * arraySizeX + (k-d) * arraySizeX*arraySizeY] );
                 old_array[i + j * arraySizeX + k * arraySizeX*arraySizeY] = tmp / (6.0 * dist);
                  }
             }
        }
   }


void print_usage()
{
	printf("Usage of Manycore Runtime:\n");
	printf("$> stencil <iteration> <num_of_elem in each direction> <stencil size>\n\n");
}


int global_arraySize;
int stencilSize;
int numberOfIterations;
int coreX, coreY, coreZ;

int main(int argc, char *argv[])
   {
     unsigned long long a, b;
	if (argc < 4) {
		print_usage();
		return -1;
	}
	else {
		numberOfIterations = atoi(argv[1]);
		global_arraySize = atoi(argv[2]);
		stencilSize = atoi(argv[3]);
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
     int arraySize[3]              = {0,0,0};


     arraySize[0] = global_arraySize;
     arraySize[1] = global_arraySize;
     arraySize[2] = global_arraySize;

     double* array = (double*) malloc(sizeof(double)*arraySize[0]*arraySize[1]*arraySize[2]);
     double* old_array = (double*) malloc(sizeof(double)*arraySize[0]*arraySize[1]*arraySize[2]);

     printf ("Call relaxation \n");

#if usePAPI == 0
     timespec time1, time2;
#endif
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
          high_level_relax(old_array,array,global_arraySize,stencilSize);
          high_level_relax(array,old_array,global_arraySize,stencilSize);
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
     cout << "overall high-level time:" << diff(time1,time2).tv_sec + diff(time1,time2).tv_nsec/1e9 << endl;
#endif
#endif

     return 0;
   }

