// Example relaxation code to experiment with TMR transformations
// Things to do in this code:
// 1) Simplify the algorithm-based distribution.
// 2) Add support for ghost boundary halos (arbritrary width).
// 3) Hierarchy of use (nesting of MulticoreArray)
// 4) Dynamic data
// 5) Better use of OpenMP (specify parallel region "#pragma parallel")
// Use "ManyCoreArrayRuntimeSupport" as the name of this class/project...
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <string>
// This header file can't be found (unless John Van Zant installs the libnuma development version or the RPM).
// Note that libnuma will return a NULL pointer when ask to allocate zero bytes (C++ allocation will alternatively
// allocated memory returning a valid pointer; so the two are semantically different on minor points). We now
// handle this case to preserve the same semantics; when using libnuma we check for this case of zero bytes and 
// use the C++ allocation to get a valid pointer.
#if HAVE_NUMA_H
#include <numa.h>
#endif
// Support for timing
// #include <iostream>
// #include <time.h>
// Local support for a timer better than clock().
// #include "highresTimer.h"
using namespace std;
#include "core.h"
#include "multicore.h"

template <typename T>
void
high_level_relax_orig ( MulticoreArray<T> & array, MulticoreArray<T> & old_array )
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

     for (int k = 1; k < arraySizeZ-1; k++)
        {
          for (int j = 1; j < arraySizeY-1; j++)
             {
               for (int i = 1; i < arraySizeX-1; i++)
                  {
                    old_array(i,j,k) = (array(i-1,j-1,k-1) + array(i+1,j-1,k-1) + array(i-1,j+1,k-1) + array(i+1,j+1,k-1) + array(i-1,j-1,k+1) + array(i+1,j-1,k+1) + array(i-1,j+1,k+1) + array(i+1,j+1,k+1)) / 8.0;
//printf("(%d,%d,%d)=%f\n",k,j,i,old_array(i,j,k));
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
high_level_relax ( MulticoreArray<T> & array, MulticoreArray<T> & old_array )
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
#pragma stencil-manycore(T, old_array, array) D(k,0,arraySizeZ,1) D(j,0,arraySizeY,1) D(i,0,arraySizeX,1) 
#pragma omp for
     for (int k = 1; k < arraySizeZ-1; k++)
        {
          for (int j = 1; j < arraySizeY-1; j++)
             {
               for (int i = 1; i < arraySizeX-1; i++)
                  {
                    old_array(i,j,k) = (array(i-1,j-1,k-1) + array(i+1,j-1,k-1) + array(i-1,j+1,k-1) + array(i+1,j+1,k-1) + array(i-1,j-1,k+1) + array(i+1,j-1,k+1) + array(i-1,j+1,k+1) + array(i+1,j+1,k+1)) / 8.0;
//printf("(%d,%d,%d)=%f\n",k,j,i,old_array(i,j,k));
                  }
             }
        }
}
// ****************************************************
// ****************************************************
// Main test function for MulticoreArray<T> abstraction
// ****************************************************
// ****************************************************
int main ()
{
// A multidimensional array support would have to include inter-array padding (to allow the seperate rows to work well with the cache).
// If multiple arrays are used then intra-array padding will separate them from each other and permit them to work well with cache.
  int numaSupport = -1;
  // #ifdef NUMA_NUM_NODES
#if HAVE_NUMA_H
  // A return value of -1 means that all other functions in this library are undefined.
  numaSupport = numa_available();
#endif
  if (numaSupport != -1) { printf ("NUMA support is AVAILABLE \n"); }
  else { printf ("NUMA support is NOT available numaSupport = %d \n",numaSupport); }
  int arraySize[DIM]              = {0,0,0};
  int numberOfCoresArraySize[DIM] = {0,0,0};
  int interArraySubsectionPadding =  0; // Page size to avoid cache conflicts with memory allocted for use by each core

  // Tests for array abstraction constructor an initialization.
  int maxArraySizePerAxis = 8;
  for (int size_Z = 4; size_Z <= maxArraySizePerAxis; size_Z++)
  for (int size_Y = 4; size_Y <= maxArraySizePerAxis; size_Y++)
  for (int size_X = 4; size_X <= maxArraySizePerAxis; size_X++)
  {
    arraySize[0] = size_X;
    arraySize[1] = size_Y;
    arraySize[2] = size_Z;
    int numberOfCoresPerAxis = 2;
    for (int numberOfCores_Z = 1; numberOfCores_Z <= numberOfCoresPerAxis; numberOfCores_Z++)
    for (int numberOfCores_Y = 1; numberOfCores_Y <= numberOfCoresPerAxis; numberOfCores_Y++)
    for (int numberOfCores_X = 2; numberOfCores_X <= numberOfCoresPerAxis; numberOfCores_X++)
    {
      printf ("***** Testing MulticoreArray construction using size_X = %2d size_Y = %2d size_Z = %2d numberOfCores = (%d,%d,%d) \n",size_X,size_Y,size_Z,numberOfCores_X, numberOfCores_Y, numberOfCores_Z);
      numberOfCoresArraySize[0] = numberOfCores_X;
      numberOfCoresArraySize[1] = numberOfCores_Y;
      numberOfCoresArraySize[2] = numberOfCores_Z;
      MulticoreArray<float> tmp_array1    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
      MulticoreArray<float> tmp_array2    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,true);
      MulticoreArray<float> old_tmp_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
      MulticoreArray<float> old_tmp_array2(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,true);

      // Test initialization
      for (int k = 0; k < size_Z; k++)
      for (int j = 0; j < size_Y; j++)
      for (int i = 0; i < size_X; i++)
      {
        tmp_array1(i,j,k) = -1.0;
        tmp_array2(i,j,k) = -1.0;
      }
      tmp_array1.initializeDataWithCoreArrayIndex();
      tmp_array2.initializeDataWithCoreArrayIndex();
      tmp_array1.initializeBoundary(-1);
      tmp_array2.initializeBoundary(-1);


     high_level_relax_orig(tmp_array1,old_tmp_array1);
printf("==============\n");
     high_level_relax(tmp_array2,old_tmp_array2);

#if 0
// Output the result
tmp_array1.display("result: tmp_array1");
tmp_array2.display("result: tmp_array2");
#endif

    // Make sure that the results are the same.
    assert(tmp_array1 == tmp_array2);
    assert(old_tmp_array1 == old_tmp_array2);
   }
  }
}
