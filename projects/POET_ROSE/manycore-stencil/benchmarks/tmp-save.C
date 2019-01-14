
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

#define local_index3D(i,j,k,i_s,j_s) (k)*i_s*j_s+(j)*i_s+(i)
#define otherCore_index3D(i,j,k,i_s,j_s) (k)*i_s*j_s+(j)*i_s+(i)
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
void high_level_relax(class MulticoreArray< int  > &array,class MulticoreArray< int  > &old_array){
   
// The code in this function is what we expect users to want to write. However, 
// while this code uses the same data as the transformed code, but it less efficient.
const int arraySizeX = array . get_arraySize(0);
   const int arraySizeY = array . get_arraySize(1);
   const int arraySizeZ = array . get_arraySize(2);
   
// Higher level could be:
//    array(I) = (array(I-1) + array(I+1)) / 2.0;
// but our current abstraction does not support this
// (use array class (e.g. A++) for this level).
// Use pragma to trigger compiler transformations, such as: 
#pragma stencil _ data <i,j,k> array [arraySizeX*arraySizeY*arraySizeZ] <old_array,array>

   
   int numberOfCores = (array).get_numberOfCores();
   
   
   
   int** arraySectionPointers = (array).get_arraySectionPointers();
   
   
   int** old_arraySectionPointers = (old_array).get_arraySectionPointers();
   #pragma omp parallel for 
   for (int core = 0; core < numberOfCores; core++)
   {
      int* arraySection     = arraySectionPointers[core];
      int* old_arraySection = old_arraySectionPointers[core];
   
      Core<int>* cur_core =  (array).get_coreArray()[core];
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
   
      for (int k=1; k<locallocallocal_size2-1; k++) 
      {
        for (int j=1; j<locallocallocal_size1-1; j++) 
        {
          for (int i=1; i<locallocallocal_size0-1; i++) 
          {
            old_arraySection[local_index3D(i,j,k,locallocallocal_size0,locallocallocal_size1)] = arraySection[local_index3D(i - 1,j - 1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,j - 1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j + 1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,j + 1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j - 1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,j - 1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j + 1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,j + 1,k + 1,locallocallocal_size0,locallocallocal_size1)]/8;
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
                       old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j - 1,k - 1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j - 1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j + 1,k - 1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j + 1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j - 1,k + 1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j - 1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j + 1,k + 1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j + 1,k + 1,locallocallocal_size0,locallocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,k - 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,k - 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,k - 1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,0+1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,k + 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,k + 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,k + 1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,0+1,k + 1,locallocallocal_size0,locallocallocal_size1)]/8;
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
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(0+1,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,0+1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,0+1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,0+1,0+1,locallocallocal_size0,locallocallocal_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,-1+locallocallocal_size2-1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,-1+locallocallocal_size2-1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,0+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(0+1,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(0+1,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(0+1,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,k - 1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,-1+locallocallocal_size1-1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,k - 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,k - 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,k + 1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,-1+locallocallocal_size1-1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,k + 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,k + 1,leftrightlocal_size0,leftrightlocal_size1)]/8;
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
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(0+1,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,-1+locallocallocal_size1-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,0+1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,0+1,leftrightlocal_size0,leftrightlocal_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,-1+locallocallocal_size2-1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,-1+locallocallocal_size2-1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(0+1,0,0,rightrightleft_size0,rightrightleft_size1)]/8;
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
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(0+1,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(0+1,0,0,rightrightleft_size0,rightrightleft_size1)]/8;
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
                                  old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,k - 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,k - 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,k - 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,k - 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,k + 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,k + 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,k + 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,k + 1,leftrightlocal_size0,leftrightlocal_size1)]/8;
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
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(0+1,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(0+1,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,0+1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,0+1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,0+1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,0+1,leftrightlocal_size0,leftrightlocal_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,-1+locallocallocal_size2-1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,-1+locallocallocal_size2-1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,-1+locallocallocal_size2-1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,-1+locallocallocal_size2-1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(0+1,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(0+1,0,0,rightrightleft_size0,rightrightleft_size1)]/8;
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
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(0+1,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(0+1,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(0+1,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(0+1,0,0,rightrightleft_size0,rightrightleft_size1)]/8;
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
                                  old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j - 1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j - 1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j + 1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j + 1,0+1,locallocallocal_size0,locallocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j - 1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j - 1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j + 1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j + 1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                       old_arraySection[local_index3D(-1+locallocallocal_size0,j,k,locallocallocal_size0,locallocallocal_size1)] = arraySection[local_index3D(-1+locallocallocal_size0-1,j - 1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j - 1,k - 1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j + 1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j + 1,k - 1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j - 1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j - 1,k + 1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j + 1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j + 1,k + 1,locallocalright_size0,locallocalright_size1)]/8;
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
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+localleftlocal_size1,k - 1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,k - 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,0+1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,k - 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftlocal_size1,k + 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,k + 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,0+1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,k + 1,locallocalright_size0,locallocalright_size1)]/8;
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
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(-1+locallocallocal_size0-1,0+1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftlocal_size1,0+1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,0+1,rightleftlocal_size0,rightleftlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,0+1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,0+1,locallocalright_size0,locallocalright_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,-1+locallocallocal_size2-1,rightleftlocal_size0,rightleftlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,0+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(-1+locallocallocal_size0-1,0+1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(-1+locallocallocal_size0-1,0+1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(-1+locallocallocal_size0-1,0+1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = arraySection[local_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,k - 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,k - 1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,k - 1,rightrightlocal_size0,rightrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,k + 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,k + 1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,k + 1,rightrightlocal_size0,rightrightlocal_size1)]/8;
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
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,0+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,0+1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,0+1,rightrightlocal_size0,rightrightlocal_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySection[local_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,-1+locallocallocal_size2-1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2-1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,0,rightrightright_size0,rightrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,0,0,rightrightright_size0,rightrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)]/8;
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
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,0,rightrightright_size0,rightrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,0,0,rightrightright_size0,rightrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)]/8;
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
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+localleftlocal_size1,k - 1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,k - 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,k - 1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,k - 1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftlocal_size1,k + 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,k + 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,k + 1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,k + 1,rightrightlocal_size0,rightrightlocal_size1)]/8;
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
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftlocal_size1,0+1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,0+1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,0+1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,0+1,rightrightlocal_size0,rightrightlocal_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,-1+locallocallocal_size2-1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,-1+locallocallocal_size2-1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2-1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,0,0,rightrightright_size0,rightrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)]/8;
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
                                             old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(-1+locallocallocal_size0-1,0,0,rightrightright_size0,rightrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)]/8;
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
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j - 1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j - 1,0+1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j + 1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j + 1,0+1,locallocalright_size0,locallocalright_size1)]/8;
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
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySection[local_index3D(-1+locallocallocal_size0-1,j - 1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j - 1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j + 1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j + 1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(-1+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                       old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j - 1,k - 1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j - 1,k - 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j + 1,k - 1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j + 1,k - 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j - 1,k + 1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j - 1,k + 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j + 1,k + 1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j + 1,k + 1,locallocalright_size0,locallocalright_size1)]/8;
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
                                  old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,k - 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,k - 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,k - 1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,k - 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,k + 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,k + 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,k + 1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,k + 1,locallocalright_size0,locallocalright_size1)]/8;
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
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,0+1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,0+1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,0+1,locallocalright_size0,locallocalright_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,-1+locallocallocal_size2-1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,-1+locallocallocal_size2-1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,k - 1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,k - 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,k - 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,k - 1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,k + 1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,k + 1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,k + 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,k + 1,rightrightlocal_size0,rightrightlocal_size1)]/8;
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
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,0+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,0+1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,0+1,rightrightlocal_size0,rightrightlocal_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,-1+locallocallocal_size2-1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2-1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)]/8;
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
                                             old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)]/8;
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
                                  old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,k - 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,k - 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,k - 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,k - 1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,k + 1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,k + 1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,k + 1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,k + 1,rightrightlocal_size0,rightrightlocal_size1)]/8;
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
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,0+1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,0+1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,0+1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,0+1,rightrightlocal_size0,rightrightlocal_size1)]/8;
                                          }
                                       }
                                    }
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                    {
                                       {
                                          {
                                             old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size0,-1+leftleftlocal_size1,-1+locallocallocal_size2-1,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0,-1+rightleftlocal_size1,-1+locallocallocal_size2-1,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+leftrightlocal_size0,0,-1+locallocallocal_size2-1,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2-1,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)]/8;
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
                                             old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)]/8;
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
                                  old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j - 1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j - 1,0+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j + 1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j + 1,0+1,locallocalright_size0,locallocalright_size1)]/8;
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
                                  old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j - 1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j - 1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j + 1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j + 1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size0,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+rightlocallocal_size0,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                       old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i - 1,-1+localleftlocal_size1,k - 1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i + 1,-1+localleftlocal_size1,k - 1,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i - 1,0+1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,0+1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i - 1,-1+localleftlocal_size1,k + 1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i + 1,-1+localleftlocal_size1,k + 1,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i - 1,0+1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,0+1,k + 1,locallocallocal_size0,locallocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i - 1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i + 1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i - 1,0+1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i + 1,0+1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i - 1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i + 1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i - 1,0+1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,0+1,0+1,locallocallocal_size0,locallocallocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i - 1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i + 1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i - 1,0+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,0+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i - 1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i + 1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i - 1,0+1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i + 1,0+1,0,rightleftlocal_size0,rightleftlocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i - 1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i + 1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i - 1,0+1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i + 1,0+1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i - 1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i + 1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i - 1,0+1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i + 1,0+1,0,rightleftlocal_size0,rightleftlocal_size1)]/8;
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
                       old_arraySection[local_index3D(i,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = arraySection[local_index3D(i - 1,-1+locallocallocal_size1-1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,-1+locallocallocal_size1-1,k - 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i - 1,0,k - 1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i + 1,0,k - 1,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i - 1,-1+locallocallocal_size1-1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,-1+locallocallocal_size1-1,k + 1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i - 1,0,k + 1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i + 1,0,k + 1,localrightlocal_size0,localrightlocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i - 1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i + 1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i - 1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i + 1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(i - 1,-1+locallocallocal_size1-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,-1+locallocallocal_size1-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i - 1,0,0+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i + 1,0,0+1,localrightlocal_size0,localrightlocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySection[local_index3D(i - 1,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i - 1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i + 1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i - 1,-1+locallocallocal_size1-1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i + 1,-1+locallocallocal_size1-1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i - 1,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i + 1,0,0,rightrightlocal_size0,rightrightlocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i - 1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i + 1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i - 1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i + 1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i - 1,-1+locallocallocal_size1-1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i + 1,-1+locallocallocal_size1-1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i - 1,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i + 1,0,0,rightrightlocal_size0,rightrightlocal_size1)]/8;
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
                       old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i - 1,-1+localleftlocal_size1,k - 1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i + 1,-1+localleftlocal_size1,k - 1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i - 1,0,k - 1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i + 1,0,k - 1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i - 1,-1+localleftlocal_size1,k + 1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i + 1,-1+localleftlocal_size1,k + 1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i - 1,0,k + 1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i + 1,0,k + 1,localrightlocal_size0,localrightlocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i - 1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i + 1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i - 1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i + 1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i - 1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i + 1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i - 1,0,0+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i + 1,0,0+1,localrightlocal_size0,localrightlocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i - 1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i + 1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i - 1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i + 1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i - 1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i + 1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i - 1,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i + 1,0,0,rightrightlocal_size0,rightrightlocal_size1)]/8;
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
                                  old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i - 1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i + 1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i - 1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i + 1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i - 1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i + 1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i - 1,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i + 1,0,0,rightrightlocal_size0,rightrightlocal_size1)]/8;
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
                       old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i - 1,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i + 1,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i - 1,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i + 1,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(i - 1,j - 1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,j - 1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j + 1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,j + 1,0+1,locallocallocal_size0,locallocallocal_size1)]/8;
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
                       old_arraySection[local_index3D(i,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = arraySection[local_index3D(i - 1,j - 1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,j - 1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i - 1,j + 1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i + 1,j + 1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i - 1,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i + 1,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i - 1,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i + 1,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
                       old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i - 1,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i + 1,j - 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i - 1,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i + 1,j + 1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i - 1,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i + 1,j - 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i - 1,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i + 1,j + 1,0,rightlocallocal_size0,rightlocallocal_size1)]/8;
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
