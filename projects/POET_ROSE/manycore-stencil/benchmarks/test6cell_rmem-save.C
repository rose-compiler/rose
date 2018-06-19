
#include <vector>
#include <string>
using namespace std;
#include "core.h"
#include "multicore.h"

#ifndef CORE_NUM
#define CORE_NUM 1,1,2
#endif

#ifdef CHECK_TIME
#include <time.h>
#include "highresTimer.h"
#endif

#define local_index3D(__2,__1,__0,__2_s,__1_s) (__0)*__2_s*__1_s+(__1)*__2_s+(__2)
#define otherCore_index3D(__2,__1,__0,__2_s,__1_s) (__0)*__2_s*__1_s+(__1)*__2_s+(__2)
// Example relaxation code to experiment with TMR transformations
// Things to do in this code:
// 1) Simplify the algorithm-based distribution.
// 2) Add support for ghost boundary halos (arbritrary width).
// 3) Hierarchy of use (nesting of MulticoreArray)
// 4) Dynamic data
// 5) Better use of OpenMP (specify parallel region "#pragma parallel")
// Use "ManyCoreArrayRuntimeSupport" as the name of this class/project...
#define usePAPI 0
#ifndef TEST_OMP
#define TEST_OMP 1
#endif
#ifndef TEST_OPT
#define TEST_OPT 1
#endif
#ifndef TIME
#define TIME 2
#endif
#ifndef SIZE
#define SIZE 16
#endif
#define STENCIL_SIZE 1
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
#endif
int global_arraySize;int stencilSize;int numberOfIterations;
// *********************************************************************
// *********************************************************************
// Examples of the use of the Core<T> and MulticoreArray<T> abstractions
// *********************************************************************
// *********************************************************************
// The code in this function is what we expect users to want to write. However, 
// while this code uses the same data as the transformed code, but it less efficient.
// 3D simple stencil
template < typename T >
void
high_level_relax_orig ( T * array, T * old_array, int dist )
   {
     const int arraySizeX = global_arraySize;
     const int arraySizeY = global_arraySize;
     const int arraySizeZ = global_arraySize;
#pragma omp parallel for
     for ( int k = dist; k < arraySizeZ - dist; k ++ )
        {
          for ( int j = dist; j < arraySizeY - dist; j ++ )
             {
               for ( int i = dist; i < arraySizeX - dist; i ++ )
                 {
                 T tmp = ( (0.0) );
                 for ( int d = 1; d <= dist; d ++ )
                 {
                    tmp += ( array [ ( i + d ) + j * arraySizeX + k * arraySizeX * arraySizeY ] + array [ ( i - d ) + j * arraySizeX + k * arraySizeX * arraySizeY ] );
                    tmp += ( array [ i + ( j + d ) * arraySizeX + k * arraySizeX * arraySizeY ] + array [ i + ( j - d ) * arraySizeX + k * arraySizeX * arraySizeY ] );
                    tmp += ( array [ i + j * arraySizeX + ( k + d ) * arraySizeX * arraySizeY ] + array [ i + j * arraySizeX + ( k - d ) * arraySizeX * arraySizeY ] );
                 }
                 old_array [ i + j * arraySizeX + k * arraySizeX * arraySizeY ] = tmp / ( (6.0) * dist );
                 }
             }
        }
   }
// The code in this function is what we expect users to want to write. However, 
// while this code uses the same data as the transformed code, but it less efficient.
// 3D simple stencil
template < typename T >
void
high_level_relax ( T * array, T * old_array, int dist )
   {
     const int arraySizeX = global_arraySize;
     const int arraySizeY = global_arraySize;
     const int arraySizeZ = global_arraySize;
#pragma stencil T data <i,j,k> array [arraySizeX*arraySizeY*arraySizeZ]<old_array, array>
     for ( int k = 1; k < arraySizeZ - 1; k ++ )
        {
          for ( int j = 1; j < arraySizeY - 1; j ++ )
             {
               for ( int i = 1; i < arraySizeX - 1; i ++ )
                 {
                    T tmp = ( (0.0) );
                    tmp += ( array [ ( i + 1 ) + j * arraySizeX + k * arraySizeX * arraySizeY ] + array [ ( i - 1 ) + j * arraySizeX + k * arraySizeX * arraySizeY ] );
                    tmp += ( array [ i + ( j + 1 ) * arraySizeX + k * arraySizeX * arraySizeY ] + array [ i + ( j - 1 ) * arraySizeX + k * arraySizeX * arraySizeY ] );
                    tmp += ( array [ i + j * arraySizeX + ( k + 1 ) * arraySizeX * arraySizeY ] + array [ i + j * arraySizeX + ( k - 1 ) * arraySizeX * arraySizeY ] );
                 old_array [ i + j * arraySizeX + k * arraySizeX * arraySizeY ] = tmp / ( (6.0) * dist );
                 }
             }
        }
   }void print_usage()
{printf("Usage of Manycore Runtime:\n");printf("$> stencil <iteration> <num_of_elem in each direction> <stencil size> <cores in X> <cores in Y> <cores in Z>\n\n");
}
void high_level_relax(MulticoreArray<double>& array,MulticoreArray<double>& old_array,int dist);// ****************************************************
// ****************************************************
// Main test function for MulticoreArray<T> abstraction
// ****************************************************
// ****************************************************
int main(int argc,char *argv[]){
   unsigned long long a;
   unsigned long long b;
   
/*
     //Change to use macro CORE_NUM, TIME and SIZE to pass argumens
     char core_str[12];
     sprintf(core_str, "%d,%d,%d", CORE_NUM);
     sscanf(core_str, "%d,%d,%d", &coreX, &coreY, &coreZ);
*/
numberOfIterations = 2;
   global_arraySize = 16;
   stencilSize = 1;
   
#ifdef CHECK_TIME
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
#define NUM_EVENTS 4
#endif
int numaSupport = - 1;
   
// #ifdef NUMA_NUM_NODES
#if HAVE_NUMA_H
// A return value of -1 means that all other functions in this library are undefined.
#endif
if (numaSupport != - 1) 
{printf("NUMA support is AVAILABLE \n");
} else 
{printf("NUMA support is NOT available numaSupport = %d \n",numaSupport);
}
   int arraySize[DIM] = {(0), (0), (0)};
   int numberOfCoresArraySize[DIM] = {(0), (0), (0)};
   
// Page size to avoid cache conflicts with memory allocted for use by each core
int interArraySubsectionPadding = 0;
   arraySize[0] = global_arraySize;
   arraySize[1] = global_arraySize;
   arraySize[2] = global_arraySize;
   double *array1 = (double *)(malloc(sizeof(double ) * arraySize[0] * arraySize[1] * arraySize[2]));
   double *array2 = (double *)(malloc(sizeof(double ) * arraySize[0] * arraySize[1] * arraySize[2]));
   double *old_array1 = (double *)(malloc(sizeof(double ) * arraySize[0] * arraySize[1] * arraySize[2]));
   double *old_array2 = (double *)(malloc(sizeof(double ) * arraySize[0] * arraySize[1] * arraySize[2]));
   
//init array here
for (int i = 0; i < arraySize[0] * arraySize[1] * arraySize[2]; i++) 
{array1[i] = (rand());array2[i] = array1[i];
}
   printf("Call relaxation \n");
   
#if usePAPI == 0
struct timespec time1;
   struct timespec time2;
   
#endif
#if TEST_OMP
#ifdef CHECK_TIME
#endif
// Call relaxation on array
omp_set_num_threads(2);
   for (int i = 0; i < numberOfIterations / 2; i++) 
{::high_level_relax_orig(old_array2,array2,stencilSize);::high_level_relax_orig(array2,old_array2,stencilSize);
}
   
#ifdef CHECK_TIME
#endif
#endif //TEST_OMP
#if TEST_OPT
#ifdef CHECK_TIME
#endif
#pragma stencil T time <i> array [global_arraySize*global_arraySize*global_arraySize] <old_array1, array1>

   MulticoreArray<double> _old_array1(global_arraySize,global_arraySize,global_arraySize,CORE_NUM,0,true);
   MulticoreArray<double> _array1(global_arraySize,global_arraySize,global_arraySize,CORE_NUM,0,true);
   int numberOfCores = (_array1).get_numberOfCores();
   
   MulticoreArray<double>* _old_array;
   MulticoreArray<double>* _array;
   {
       double** old_array1SectionPointers = (_old_array1).get_arraySectionPointers();
       #pragma omp parallel for 
       for (int core = 0; core < numberOfCores; core++)
       {
         int __2,__1,__0;
         double* old_array1Section     = old_array1SectionPointers[core];
         Core<double>* cur_core =  (_old_array1).get_coreArray()[core];
         int locallocallocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][2];
         int locallocallocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][1];
         int locallocallocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][0];
         int cpstart = ((_old_array1.get_arraySectionRanges(2))[core].first)*global_arraySize*global_arraySize+((_old_array1.get_arraySectionRanges(1))[core].first)*global_arraySize+((_old_array1.get_arraySectionRanges(0))[core].first);
         for (int __0=0; __0<locallocallocal_size2; __0+=1)
           {
              for (int __1=0; __1<locallocallocal_size1; __1+=1)
                {
                   for (int __2=0; __2<locallocallocal_size0; __2+=1)
                     {
                        old_array1Section[local_index3D(__2,__1,__0,locallocallocal_size0,locallocallocal_size1)]=old_array1[cpstart+(__0)*global_arraySize*global_arraySize+(__1)*global_arraySize+(__2)];
                     }
                }
           }
       }
   }
   {
       double** array1SectionPointers = (_array1).get_arraySectionPointers();
       #pragma omp parallel for 
       for (int core = 0; core < numberOfCores; core++)
       {
         int __2,__1,__0;
         double* array1Section     = array1SectionPointers[core];
         Core<double>* cur_core =  (_array1).get_coreArray()[core];
         int locallocallocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][2];
         int locallocallocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][1];
         int locallocallocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][0];
         int cpstart = ((_array1.get_arraySectionRanges(2))[core].first)*global_arraySize*global_arraySize+((_array1.get_arraySectionRanges(1))[core].first)*global_arraySize+((_array1.get_arraySectionRanges(0))[core].first);
         for (int __0=0; __0<locallocallocal_size2; __0+=1)
           {
              for (int __1=0; __1<locallocallocal_size1; __1+=1)
                {
                   for (int __2=0; __2<locallocallocal_size0; __2+=1)
                     {
                        array1Section[local_index3D(__2,__1,__0,locallocallocal_size0,locallocallocal_size1)]=array1[cpstart+(__0)*global_arraySize*global_arraySize+(__1)*global_arraySize+(__2)];
                     }
                }
           }
       }
   }
   #ifdef CHECK_TIME
       clock_gettime(CLOCK_REALTIME, &ctime2);
       printf("pre-copy time: %f\n", diff(ctime1,ctime2).tv_sec +  diff(ctime1,ctime2).tv_nsec/1e9);
       clock_gettime(CLOCK_REALTIME, &ctime1);
   #endif 
   for (int i = 0; i < numberOfIterations / 2; i++)
     {
        ::high_level_relax(_old_array1,_array1,stencilSize);
        ::high_level_relax(_array1,_old_array1,stencilSize);
     }
   #ifdef CHECK_TIME
       clock_gettime(CLOCK_REALTIME, &ctime2);
       printf("process time: %f\n", diff(ctime1,ctime2).tv_sec +  diff(ctime1,ctime2).tv_nsec/1e9);
       clock_gettime(CLOCK_REALTIME, &ctime1);
   #endif
   {
       double** old_array1SectionPointers = (_old_array1).get_arraySectionPointers();
       #pragma omp parallel for 
       for (int core = 0; core < numberOfCores; core++)
       {
         int __2,__1,__0;
         double* old_array1Section     = old_array1SectionPointers[core];
         Core<double>* cur_core =  (_old_array1).get_coreArray()[core];
         int locallocallocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][2];
         int locallocallocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][1];
         int locallocallocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][0];
         int cpstart = ((_old_array1.get_arraySectionRanges(2))[core].first)*global_arraySize*global_arraySize+((_old_array1.get_arraySectionRanges(1))[core].first)*global_arraySize+((_old_array1.get_arraySectionRanges(0))[core].first);
         for (int __0=0; __0<locallocallocal_size2; __0+=1)
           {
              for (int __1=0; __1<locallocallocal_size1; __1+=1)
                {
                   for (int __2=0; __2<locallocallocal_size0; __2+=1)
                     {
                        old_array1[cpstart+(__0)*global_arraySize*global_arraySize+(__1)*global_arraySize+(__2)] = old_array1Section[local_index3D(__2,__1,__0,locallocallocal_size0,locallocallocal_size1)];
                     }
                }
           }
       }
   }
   {
       double** array1SectionPointers = (_array1).get_arraySectionPointers();
       #pragma omp parallel for 
       for (int core = 0; core < numberOfCores; core++)
       {
         int __2,__1,__0;
         double* array1Section     = array1SectionPointers[core];
         Core<double>* cur_core =  (_array1).get_coreArray()[core];
         int locallocallocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][2];
         int locallocallocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][1];
         int locallocallocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][0];
         int cpstart = ((_array1.get_arraySectionRanges(2))[core].first)*global_arraySize*global_arraySize+((_array1.get_arraySectionRanges(1))[core].first)*global_arraySize+((_array1.get_arraySectionRanges(0))[core].first);
         for (int __0=0; __0<locallocallocal_size2; __0+=1)
           {
              for (int __1=0; __1<locallocallocal_size1; __1+=1)
                {
                   for (int __2=0; __2<locallocallocal_size0; __2+=1)
                     {
                        array1[cpstart+(__0)*global_arraySize*global_arraySize+(__1)*global_arraySize+(__2)] = array1Section[local_index3D(__2,__1,__0,locallocallocal_size0,locallocallocal_size1)];
                     }
                }
           }
       }
   }
   #ifdef CHECK_TIME
       clock_gettime(CLOCK_REALTIME, &ctime2);
       printf("post-copy time: %f\n", diff(ctime1,ctime2).tv_sec +  diff(ctime1,ctime2).tv_nsec/1e9);
   #endif
   
   
#ifdef CHECK_TIME
#endif
#endif //TEST_OPT
#if TEST_OMP && TEST_OPT
for (int i = 0; i < arraySize[0] * arraySize[1] * arraySize[2]; i++) if (array1[i] != array2[i]) 
{printf("Failed test at %d: %d vs %d\n",i,array1[i],array2[i]);return 1;
  }
   
#endif
return 0;
}
void high_level_relax(MulticoreArray<double>& array,MulticoreArray<double>& old_array,int dist){
   const int arraySizeX = global_arraySize;
   const int arraySizeY = global_arraySize;
   const int arraySizeZ = global_arraySize;
   
#pragma stencil T data <i,j,k> array [arraySizeX*arraySizeY*arraySizeZ]<old_array, array>

   
   int numberOfCores = (array).get_numberOfCores();
   
   
   
   double** arraySectionPointers = (array).get_arraySectionPointers();
   
   
   double** old_arraySectionPointers = (old_array).get_arraySectionPointers();
   #pragma omp parallel for 
   for (int core = 0; core < numberOfCores; core++)
   {
      double* arraySection     = arraySectionPointers[core];
      double* old_arraySection = old_arraySectionPointers[core];
   
      Core<double>* cur_core =  (array).get_coreArray()[core];
      int rightrightright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][2][2];
      int rightrightright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][2][1];
      int rightrightright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][2][0];
      int rightrightleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][0][2];
      int rightrightleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][0][1];
      int rightrightleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][0][0];
      int rightrightlocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][1][2];
      int rightrightlocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][1][1];
      int rightrightlocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][2][1][0];
      int rightleftright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][2][2];
      int rightleftright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][2][1];
      int rightleftright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][2][0];
      int rightleftleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][0][2];
      int rightleftleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][0][1];
      int rightleftleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][0][0];
      int rightleftlocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][1][2];
      int rightleftlocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][1][1];
      int rightleftlocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][0][1][0];
      int rightlocalright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][2][2];
      int rightlocalright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][2][1];
      int rightlocalright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][2][0];
      int rightlocalleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][0][2];
      int rightlocalleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][0][1];
      int rightlocalleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][0][0];
      int rightlocallocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][1][2];
      int rightlocallocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][1][1];
      int rightlocallocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[2][1][1][0];
      int leftrightright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][2][2];
      int leftrightright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][2][1];
      int leftrightright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][2][0];
      int leftrightleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][0][2];
      int leftrightleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][0][1];
      int leftrightleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][0][0];
      int leftrightlocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][1][2];
      int leftrightlocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][1][1];
      int leftrightlocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][2][1][0];
      int leftleftright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][2][2];
      int leftleftright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][2][1];
      int leftleftright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][2][0];
      int leftleftleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][0][2];
      int leftleftleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][0][1];
      int leftleftleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][0][0];
      int leftleftlocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][1][2];
      int leftleftlocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][1][1];
      int leftleftlocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][0][1][0];
      int leftlocalright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][2][2];
      int leftlocalright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][2][1];
      int leftlocalright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][2][0];
      int leftlocalleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][0][2];
      int leftlocalleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][0][1];
      int leftlocalleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][0][0];
      int leftlocallocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][1][2];
      int leftlocallocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][1][1];
      int leftlocallocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[0][1][1][0];
      int localrightright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][2][2];
      int localrightright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][2][1];
      int localrightright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][2][0];
      int localrightleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][0][2];
      int localrightleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][0][1];
      int localrightleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][0][0];
      int localrightlocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][1][2];
      int localrightlocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][1][1];
      int localrightlocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][2][1][0];
      int localleftright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][2][2];
      int localleftright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][2][1];
      int localleftright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][2][0];
      int localleftleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][0][2];
      int localleftleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][0][1];
      int localleftleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][0][0];
      int localleftlocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][1][2];
      int localleftlocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][1][1];
      int localleftlocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][0][1][0];
      int locallocalright_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][2][2];
      int locallocalright_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][2][1];
      int locallocalright_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][2][0];
      int locallocalleft_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][0][2];
      int locallocalleft_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][0][1];
      int locallocalleft_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][0][0];
      int locallocallocal_size2 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][2];
      int locallocallocal_size1 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][1];
      int locallocallocal_size0 = cur_core->coreArrayNeighborhoodSizes_3D[1][1][1][0];
   
      int leftleftleft_cp_index;
      double leftleftleft_cp[1];
      leftleftleft_cp_index = 0;
      leftleftleft_cp[leftleftleft_cp_index++] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(0,0,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)];
      int rightleftleft_cp_index;
      double rightleftleft_cp[1];
      rightleftleft_cp_index = 0;
      rightleftleft_cp[rightleftleft_cp_index++] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(0,0,0,rightleftleft_size0,rightleftleft_size1)];
      int leftrightleft_cp_index;
      double leftrightleft_cp[1];
      leftrightleft_cp_index = 0;
      leftrightleft_cp[leftrightleft_cp_index++] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(0,-1+locallocallocal_size1,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)];
      int rightrightleft_cp_index;
      double rightrightleft_cp[1];
      rightrightleft_cp_index = 0;
      rightrightleft_cp[rightrightleft_cp_index++] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(0,-1+locallocallocal_size1,0,rightrightleft_size0,rightrightleft_size1)];
      int leftleftright_cp_index;
      double leftleftright_cp[1];
      leftleftright_cp_index = 0;
      leftleftright_cp[leftleftright_cp_index++] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(-1+locallocallocal_size0,0,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)];
      int rightleftright_cp_index;
      double rightleftright_cp[1];
      rightleftright_cp_index = 0;
      rightleftright_cp[rightleftright_cp_index++] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(-1+locallocallocal_size0,0,0,rightleftright_size0,rightleftright_size1)];
      int leftrightright_cp_index;
      double leftrightright_cp[1];
      leftrightright_cp_index = 0;
      leftrightright_cp[leftrightright_cp_index++] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)];
      int rightrightright_cp_index;
      double rightrightright_cp[1];
      rightrightright_cp_index = 0;
      rightrightright_cp[rightrightright_cp_index++] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0,rightrightright_size0,rightrightright_size1)];for (int k=1; k<locallocallocal_size2-1; k++) 
      {
        for (int j=1; j<locallocallocal_size1-1; j++) 
        {
          for (int i=1; i<locallocallocal_size0-1; i++) 
          {
            double tmp = 0.0;
            tmp+=(arraySection[local_index3D(1+i,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j,k,locallocallocal_size0,locallocallocal_size1)]);
            tmp+=(arraySection[local_index3D(i,1+j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j - 1,k,locallocallocal_size0,locallocallocal_size1)]);
            tmp+=(arraySection[local_index3D(i,j,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j,k - 1,locallocallocal_size0,locallocallocal_size1)]);
            old_arraySection[local_index3D(i,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
          }
        }
      }
   
      //***************************************
      // Now process the edges and corners
      // ***************************************
      if (locallocallocal_size0>1) 
        {
           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
             {
                for (int k=1; k<locallocallocal_size2-1; k++) 
                {
                  for (int j=1; j<locallocallocal_size1-1; j++) 
                  {
                    {
                       double tmp = 0.0;
                       tmp+=(arraySection[local_index3D(1+0,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j,k,locallocalleft_size0,locallocalleft_size1)]);
                       tmp+=(arraySection[local_index3D(0,1+j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j - 1,k,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(0,j,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                       old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                    }
                  }
                }
                if (locallocallocal_size1>1) 
                  {
                     if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+0,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,k,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySection[local_index3D(0,1+0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,k,leftleftlocal_size0,leftleftlocal_size1)]);
                                  tmp+=(arraySection[local_index3D(0,0,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySection[local_index3D(0,1+0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,0,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(arraySection[local_index3D(0,0,1+0,locallocallocal_size0,locallocallocal_size1)]+leftleftleft_cp[0]);
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySection[local_index3D(0,1+0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,-1+locallocallocal_size2,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(rightleftleft_cp[0]+arraySection[local_index3D(0,0,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySection[local_index3D(0,1+0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,0,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(rightleftleft_cp[0]+leftleftleft_cp[0]);
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                     if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1,k,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,k,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(0,-2+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(0,-1+locallocallocal_size1,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(0,-2+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySection[local_index3D(0,-1+locallocallocal_size1,1+0,locallocallocal_size0,locallocallocal_size1)]+leftrightleft_cp[0]);
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(0,-2+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(rightrightleft_cp[0]+arraySection[local_index3D(0,-1+locallocallocal_size1,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(0,-2+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(rightrightleft_cp[0]+leftrightleft_cp[0]);
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                  }
                else 
                  {
                     if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+0,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,k,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,k,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,k,leftleftlocal_size0,leftleftlocal_size1)]);
                                  tmp+=(arraySection[local_index3D(0,0,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,0,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(arraySection[local_index3D(0,0,1+0,locallocallocal_size0,locallocallocal_size1)]+leftleftleft_cp[0-0]);
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,-1+locallocallocal_size2,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(rightleftleft_cp[0-0]+arraySection[local_index3D(0,0,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySection[local_index3D(1+0,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,0,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(rightleftleft_cp[0-0]+leftleftleft_cp[0-0]);
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                  }
                if (locallocallocal_size2>1) 
                  {
                     if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+0,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j,0,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySection[local_index3D(0,1+j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j - 1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(0,j,1+0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]);
                                  old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                     if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySection[local_index3D(0,1+j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j - 1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySection[local_index3D(0,j,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                  }
                else 
                  {
                     if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+0,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j,0,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySection[local_index3D(0,1+j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j - 1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]);
                                  old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                  }
             }
           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
             {
                for (int k=1; k<locallocallocal_size2-1; k++) 
                {
                  for (int j=1; j<locallocallocal_size1-1; j++) 
                  {
                    {
                       double tmp = 0.0;
                       tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j,k,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,j,k,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,1+j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j - 1,k,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,j,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                       old_arraySection[local_index3D(-1+locallocallocal_size0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                    }
                  }
                }
                if (locallocallocal_size1>1) 
                  {
                     if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,k,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,1+0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0,-1+rightleftlocal_size1,k,rightleftlocal_size0,rightleftlocal_size1)]);
                                  tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,0,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,1+0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]);
                                             tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,0,1+0,locallocallocal_size0,locallocallocal_size1)]+leftleftright_cp[0]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,1+0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0,-1+rightleftlocal_size1,-1+locallocallocal_size2,rightleftlocal_size0,rightleftlocal_size1)]);
                                             tmp+=(rightleftright_cp[0]+arraySection[local_index3D(-1+locallocallocal_size0,0,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,1+0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]);
                                             tmp+=(rightleftright_cp[0]+leftleftright_cp[0]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                     if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1,k,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0,0,k,rightrightlocal_size0,rightrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-2+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-2+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,1+0,locallocallocal_size0,locallocallocal_size1)]+leftrightright_cp[0]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,rightrightlocal_size0,rightrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-2+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(rightrightright_cp[0]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-2+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(rightrightright_cp[0]+leftrightright_cp[0]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                  }
                else 
                  {
                     if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,k,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0,0,k,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0,-1+rightleftlocal_size1,k,rightleftlocal_size0,rightleftlocal_size1)]);
                                  tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,0,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]);
                                             tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,0,1+0,locallocallocal_size0,locallocallocal_size1)]+leftleftright_cp[0-0]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0,-1+rightleftlocal_size1,-1+locallocallocal_size2,rightleftlocal_size0,rightleftlocal_size1)]);
                                             tmp+=(rightleftright_cp[0-0]+arraySection[local_index3D(-1+locallocallocal_size0,0,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]);
                                             tmp+=(rightleftright_cp[0-0]+leftleftright_cp[0-0]);
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                  }
                if (locallocallocal_size2>1) 
                  {
                     if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,1+j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j - 1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,j,1+0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0,j,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]);
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                     if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,1+j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j - 1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0,j,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                  }
                else 
                  {
                     if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-2+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(-1+locallocallocal_size0,1+j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j - 1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0,j,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0,j,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]);
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                  }
             }
        }
      else 
        {
           if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
             {
                for (int k=1; k<locallocallocal_size2-1; k++) 
                {
                  for (int j=1; j<locallocallocal_size1-1; j++) 
                  {
                    {
                       double tmp = 0.0;
                       tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j,k,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j,k,locallocalleft_size0,locallocalleft_size1)]);
                       tmp+=(arraySection[local_index3D(0,1+j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j - 1,k,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(0,j,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                       old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                    }
                  }
                }
                if (locallocallocal_size1>1) 
                  {
                     if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,k,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,k,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySection[local_index3D(0,1+0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,k,leftleftlocal_size0,leftleftlocal_size1)]);
                                  tmp+=(arraySection[local_index3D(0,0,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySection[local_index3D(0,1+0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,0,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(arraySection[local_index3D(0,0,1+0,locallocallocal_size0,locallocallocal_size1)]+leftleftleft_cp[0-0]);
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySection[local_index3D(0,1+0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,-1+locallocallocal_size2,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(rightleftleft_cp[0-0]+arraySection[local_index3D(0,0,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySection[local_index3D(0,1+0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,0,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(rightleftleft_cp[0-0]+leftleftleft_cp[0-0]);
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                     if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1,k,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1,k,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,k,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(0,-2+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(0,-1+locallocallocal_size1,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(0,-2+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(arraySection[local_index3D(0,-1+locallocallocal_size1,1+0,locallocallocal_size0,locallocallocal_size1)]+leftrightleft_cp[0-0]);
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(0,-2+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(rightrightleft_cp[0-0]+arraySection[local_index3D(0,-1+locallocallocal_size1,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(0,-2+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                             tmp+=(rightrightleft_cp[0-0]+leftrightleft_cp[0-0]);
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                  }
                else 
                  {
                     if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                       {
                          for (int k=1; k<locallocallocal_size2-1; k++) 
                          {
                            {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,k,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,k,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,k,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,k,leftleftlocal_size0,leftleftlocal_size1)]);
                                  tmp+=(arraySection[local_index3D(0,0,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                            }
                          }
                          if (locallocallocal_size2>1) 
                            {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,0,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(arraySection[local_index3D(0,0,1+0,locallocallocal_size0,locallocallocal_size1)]+leftleftleft_cp[-0+(0+(0-0))]);
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,-1+locallocallocal_size2,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(rightleftleft_cp[-0+(0+(0-0))]+arraySection[local_index3D(0,0,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                             old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                          else 
                            {
                               if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                                 {
                                    {
                                       {
                                          {
                                             double tmp = 0.0;
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0,0,locallocalleft_size0,locallocalleft_size1)]);
                                             tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0,-1+leftleftlocal_size1,0,leftleftlocal_size0,leftleftlocal_size1)]);
                                             tmp+=(rightleftleft_cp[-0+(0+(0-0))]+leftleftleft_cp[-0+(0+(0-0))]);
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                          }
                                       }
                                    }
                                 }
                            }
                       }
                  }
                if (locallocallocal_size2>1) 
                  {
                     if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j,0,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySection[local_index3D(0,1+j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j - 1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(0,j,1+0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]);
                                  old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                     if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySection[local_index3D(0,1+j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j - 1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySection[local_index3D(0,j,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                  }
                else 
                  {
                     if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                       {
                          {
                             for (int j=1; j<locallocallocal_size1-1; j++) 
                             {
                               {
                                  double tmp = 0.0;
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j,0,locallocalleft_size0,locallocalleft_size1)]);
                                  tmp+=(arraySection[local_index3D(0,1+j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j - 1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]);
                                  old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                               }
                             }
                          }
                       }
                  }
             }
        }
      if (locallocallocal_size1>1) 
        {
           if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
             {
                for (int k=1; k<locallocallocal_size2-1; k++) 
                {
                  {
                     for (int i=1; i<locallocallocal_size0-1; i++) 
                     {
                       double tmp = 0.0;
                       tmp+=(arraySection[local_index3D(1+i,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,0,k,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(i,1+0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,-1+localleftlocal_size1,k,localleftlocal_size0,localleftlocal_size1)]);
                       tmp+=(arraySection[local_index3D(i,0,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                       old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                     }
                  }
                }
                if (locallocallocal_size2>1) 
                  {
                     if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,0,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(i,1+0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,-1+localleftlocal_size1,0,localleftlocal_size0,localleftlocal_size1)]);
                                  tmp+=(arraySection[local_index3D(i,0,1+0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i,0,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]);
                                  old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                     if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(i,1+0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,-1+localleftlocal_size1,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i,0,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySection[local_index3D(i,0,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                  }
                else 
                  {
                     if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,0,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(i,1+0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,-1+localleftlocal_size1,0,localleftlocal_size0,localleftlocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i,0,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i,0,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]);
                                  old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                  }
             }
           if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
             {
                for (int k=1; k<locallocallocal_size2-1; k++) 
                {
                  {
                     for (int i=1; i<locallocallocal_size0-1; i++) 
                     {
                       double tmp = 0.0;
                       tmp+=(arraySection[local_index3D(1+i,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,0,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i,-2+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(i,-1+locallocallocal_size1,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                       old_arraySection[local_index3D(i,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                     }
                  }
                }
                if (locallocallocal_size2>1) 
                  {
                     if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,0,0,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i,-2+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySection[local_index3D(i,-1+locallocallocal_size1,1+0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i,-1+locallocallocal_size1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]);
                                  old_arraySection[local_index3D(i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                     if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,0,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i,-2+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i,-1+locallocallocal_size1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(i,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                  }
                else 
                  {
                     if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,0,0,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i,-2+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i,-1+locallocallocal_size1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i,-1+locallocallocal_size1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]);
                                  old_arraySection[local_index3D(i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                  }
             }
        }
      else 
        {
           if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
             {
                for (int k=1; k<locallocallocal_size2-1; k++) 
                {
                  {
                     for (int i=1; i<locallocallocal_size0-1; i++) 
                     {
                       double tmp = 0.0;
                       tmp+=(arraySection[local_index3D(1+i,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,0,k,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,0,k,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,-1+localleftlocal_size1,k,localleftlocal_size0,localleftlocal_size1)]);
                       tmp+=(arraySection[local_index3D(i,0,1+k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,k - 1,locallocallocal_size0,locallocallocal_size1)]);
                       old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                     }
                  }
                }
                if (locallocallocal_size2>1) 
                  {
                     if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,0,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,0,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,-1+localleftlocal_size1,0,localleftlocal_size0,localleftlocal_size1)]);
                                  tmp+=(arraySection[local_index3D(i,0,1+0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i,0,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]);
                                  old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                     if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,0,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,-1+localleftlocal_size1,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i,0,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySection[local_index3D(i,0,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                                  old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                  }
                else 
                  {
                     if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                       {
                          {
                             {
                                for (int i=1; i<locallocallocal_size0-1; i++) 
                                {
                                  double tmp = 0.0;
                                  tmp+=(arraySection[local_index3D(1+i,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,0,0,locallocallocal_size0,locallocallocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,0,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,-1+localleftlocal_size1,0,localleftlocal_size0,localleftlocal_size1)]);
                                  tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i,0,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i,0,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]);
                                  old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                                }
                             }
                          }
                       }
                  }
             }
        }
      if (locallocallocal_size2>1) 
        {
           if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
             {
                {
                   for (int j=1; j<locallocallocal_size1-1; j++) 
                   {
                     for (int i=1; i<locallocallocal_size0-1; i++) 
                     {
                       double tmp = 0.0;
                       tmp+=(arraySection[local_index3D(1+i,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j,0,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(i,1+j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j - 1,0,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(i,j,1+0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,j,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]);
                       old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                     }
                   }
                }
             }
           if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
             {
                {
                   for (int j=1; j<locallocallocal_size1-1; j++) 
                   {
                     for (int i=1; i<locallocallocal_size0-1; i++) 
                     {
                       double tmp = 0.0;
                       tmp+=(arraySection[local_index3D(1+i,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(i,1+j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j - 1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,j,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySection[local_index3D(i,j,-2+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]);
                       old_arraySection[local_index3D(i,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                     }
                   }
                }
             }
        }
      else 
        {
           if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
             {
                {
                   for (int j=1; j<locallocallocal_size1-1; j++) 
                   {
                     for (int i=1; i<locallocallocal_size0-1; i++) 
                     {
                       double tmp = 0.0;
                       tmp+=(arraySection[local_index3D(1+i,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j,0,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySection[local_index3D(i,1+j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j - 1,0,locallocallocal_size0,locallocallocal_size1)]);
                       tmp+=(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,j,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,j,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]);
                       old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp / (6.0 * dist);
                     }
                   }
                }
             }
        }
      // **************************************************
      // End of processing the corners of the X and Y axis.
      // **************************************************
   
   }                              
   
   
}