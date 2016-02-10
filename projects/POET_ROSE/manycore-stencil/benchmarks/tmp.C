#include<stdio.h>
#include<stdlib.h>

#include<assert.h>

#include<vector>
#include<string>

// This header file can't be found (unless John Van Zant installs the libnuma development version or the RPM).
// Note that libnuma will return a NULL pointer when ask to allocate zero bytes (C++ allocation will alternatively
// allocated memory returning a valid pointer; so the two are semantically different on minor points). We now
// handle this case to preserve the same semantics; when using libnuma we check for this case of zero bytes and 
// use the C++ allocation to get a valid pointer.
#if HAVE_NUMA_H
   #include<numa.h>
#endif

// Support for timing
// #include <iostream>
// #include <time.h>

// Local support for a timer better than clock().
// #include "highresTimer.h"

using namespace std;

#include "core.h"
#include "multicore.h"

void
high_level_relax ( MulticoreArray<int> & array, MulticoreArray<int> & old_array )
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
#pragma stencil-manycore(int, old_array, array) D(i,0,arraySizeX,1) D(j,0,arraySizeY,1) S(k,0,arraySizeZ,1) 
#pragma omp for
     for (int k = 1; k < arraySizeZ-1; k++)
        {
          for (int j = 1; j < arraySizeY-1; j++)
             {
               for (int i = 1; i < arraySizeX-1; i++)
                  {
                    old_array(i,j,k) = (array(i-1,j-1,k-1) + array(i+1,j-1,k-1) + array(i-1,j+1,k-1) + array(i+1,j+1,k-1) + array(i-1,j-1,k+1) + array(i+1,j-1,k+1) + array(i-1,j+1,k+1) + array(i+1,j+1,k+1)) / 8.0;
                  }
             }
        }
   }

