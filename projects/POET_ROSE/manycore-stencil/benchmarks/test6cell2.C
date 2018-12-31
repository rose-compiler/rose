// Example relaxation code to experiment with TMR transformations

// Things to do in this code:
// 1) Simplify the algorithm-based distribution.
// 2) Add support for ghost boundary halos (arbritrary width).
// 3) Hierarchy of use (nesting of MulticoreArray)
// 4) Dynamic data
// 5) Better use of OpenMP (specify parallel region "#pragma parallel")

// Use "ManyCoreArrayRuntimeSupport" as the name of this class/project...

#define usePAPI 0

#ifndef TIME
#define TIME 2
#endif

#ifndef SIZE
#define SIZE 16
#endif

#define STENCIL_SIZE 2

#ifndef OMP_THREAD_NUM 
#define OMP_THREAD_NUM   2
#endif

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

#ifndef CORE_NUM
#define CORE_NUM 2,2,2
#endif

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

int global_arraySize;
int stencilSize;
int numberOfIterations;

// *********************************************************************
// *********************************************************************
// Examples of the use of the Core<T> and MulticoreArray<T> abstractions
// *********************************************************************
// *********************************************************************

template <typename T>
void
high_level_relax_orig (T* array, T* old_array, int dist)
   {
  // The code in this function is what we expect users to want to write. However, 
  // while this code uses the same data as the transformed code, but it less efficient.

     const int arraySizeX = global_arraySize;
     const int arraySizeY = global_arraySize;
     const int arraySizeZ = global_arraySize;

#pragma omp parallel for
     for (int k = dist; k < arraySizeZ-dist; k++)
        {
          for (int j = dist; j < arraySizeY-dist; j++)
             {
               for (int i = dist; i < arraySizeX-dist; i++)
                 {
                 // 3D simple stencil
                 T tmp = (0.0);
                 for(int d=1; d<=dist; d++)
                 {
                    tmp += (array[(i + d) + j * arraySizeX + k * arraySizeX*arraySizeY] + array[(i - d) + j * arraySizeX + k * arraySizeX*arraySizeY]);
                    tmp += (array[i + (j + d) * arraySizeX + k * arraySizeX*arraySizeY] + array[i + (j - d) * arraySizeX + k * arraySizeX*arraySizeY]);
                    tmp += (array[i + j * arraySizeX + (k + d) * arraySizeX*arraySizeY] + array[i + j * arraySizeX + (k - d) * arraySizeX*arraySizeY]);
                 }
                 old_array[i + j * arraySizeX + k * arraySizeX*arraySizeY] = tmp / (6.0 * dist);
                 }
             }
        }
   }


template <typename T>
void
high_level_relax (T* array, T* old_array, int dist)
   {
  // The code in this function is what we expect users to want to write. However, 
  // while this code uses the same data as the transformed code, but it less efficient.

     const int arraySizeX = global_arraySize;
     const int arraySizeY = global_arraySize;
     const int arraySizeZ = global_arraySize;

#pragma stencil T data <i,j,k> array [arraySizeX*arraySizeY*arraySizeZ]<old_array, array>  
     for (int k = 2; k < arraySizeZ-2; k++)
        {
          for (int j = 2; j < arraySizeY-2; j++)
             {
               for (int i = 2; i < arraySizeX-2; i++)
                 {
                 // 3D simple stencil
                    T tmp = (0.0);
                    tmp += (array[(i + 1) + j * arraySizeX + k * arraySizeX*arraySizeY] + array[(i - 1) + j * arraySizeX + k * arraySizeX*arraySizeY]);
                    tmp += (array[i + (j + 1) * arraySizeX + k * arraySizeX*arraySizeY] + array[i + (j - 1) * arraySizeX + k * arraySizeX*arraySizeY]);
                    tmp += (array[i + j * arraySizeX + (k + 1) * arraySizeX*arraySizeY] + array[i + j * arraySizeX + (k - 1) * arraySizeX*arraySizeY]);
                    tmp += (array[(i + 2) + j * arraySizeX + k * arraySizeX*arraySizeY] + array[(i - 2) + j * arraySizeX + k * arraySizeX*arraySizeY]);
                    tmp += (array[i + (j + 2) * arraySizeX + k * arraySizeX*arraySizeY] + array[i + (j - 2) * arraySizeX + k * arraySizeX*arraySizeY]);
                    tmp += (array[i + j * arraySizeX + (k + 2) * arraySizeX*arraySizeY] + array[i + j * arraySizeX + (k - 2) * arraySizeX*arraySizeY]);
                 old_array[i + j * arraySizeX + k * arraySizeX*arraySizeY] = tmp / (6.0 * dist);
                 }
             }
        }
   }



void print_usage()
{
	printf("Usage of Manycore Runtime:\n");
	printf("$> stencil <iteration> <num_of_elem in each direction> <stencil size> <cores in X> <cores in Y> <cores in Z>\n\n");
}

// ****************************************************
// ****************************************************
// Main test function for MulticoreArray<T> abstraction
// ****************************************************
// ****************************************************

int main(int argc, char *argv[])
   {
     unsigned long long a, b;
/*
     //Change to use macro CORE_NUM, TIME and SIZE to pass argumens
     char core_str[12];
     sprintf(core_str, "%d,%d,%d", CORE_NUM);
     sscanf(core_str, "%d,%d,%d", &coreX, &coreY, &coreZ);
*/
     numberOfIterations = TIME;
     global_arraySize = SIZE;
     stencilSize = STENCIL_SIZE;
#ifdef CHECK_TIME
     timespec mctime1, mctime2;
#endif
/*
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
*/
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

     double* array1 = (double*) malloc(sizeof(double)*arraySize[0]*arraySize[1]*arraySize[2]);
     double* array2 = (double*) malloc(sizeof(double)*arraySize[0]*arraySize[1]*arraySize[2]);
     double* old_array1 = (double*) malloc(sizeof(double)*arraySize[0]*arraySize[1]*arraySize[2]);
     double* old_array2 = (double*) malloc(sizeof(double)*arraySize[0]*arraySize[1]*arraySize[2]);
//init array here
     for (int i = 0; i < arraySize[0]*arraySize[1]*arraySize[2]; i++)
     {
        array1[i] = rand();
        array2[i] = array1[i];
     }

     printf ("Call relaxation \n");

#if usePAPI == 0
     timespec time1, time2;
#endif
#ifdef CHECK_TIME
    clock_gettime(CLOCK_REALTIME, &mctime1);
#endif
  // Call relaxation on array
     omp_set_num_threads(OMP_THREAD_NUM);

     for (int i = 0; i < numberOfIterations/2; i++)
        {
          high_level_relax_orig(old_array2,array2,stencilSize);
          high_level_relax_orig(array2,old_array2,stencilSize);
        }

#ifdef CHECK_TIME
    clock_gettime(CLOCK_REALTIME, &mctime2);
    printf("omp code test time: %f\n", diff(mctime1,mctime2).tv_sec +  diff(mctime1,mctime2).tv_nsec / 1e9);
#endif


#ifdef CHECK_TIME
    clock_gettime(CLOCK_REALTIME, &mctime1);
#endif

#pragma stencil T time <i> array [global_arraySize*global_arraySize*global_arraySize] <old_array1, array1>
     for (int i = 0; i < numberOfIterations/2; i++)
     {
       high_level_relax(old_array1,array1, stencilSize);
       high_level_relax(array1,old_array1, stencilSize);
     }
#ifdef CHECK_TIME
    clock_gettime(CLOCK_REALTIME, &mctime2);
    printf("opt code test time: %f\n", diff(mctime1,mctime2).tv_sec +  (diff(mctime1,mctime2).tv_nsec/1e9));
#endif

     for (int i = 0; i < arraySize[0]*arraySize[1]*arraySize[2]; i++)
        if (array1[i] != array2[i]) { printf("Failed test at %d: %d vs %d\n", i, array1[i], array2[i]); return 1; }

     return 0;
   }

