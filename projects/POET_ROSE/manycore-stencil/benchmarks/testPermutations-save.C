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
// The code in this function is what we expect users to want to write. However, 
// while this code uses the same data as the transformed code, but it less efficient.
// Higher level could be:
//    array(I) = (array(I-1) + array(I+1)) / 2.0;
// but our current abstraction does not support this
// (use array class (e.g. A++) for this level).
//printf("(%d,%d,%d)=%f\n",k,j,i,old_array(i,j,k));
template < typename T >
void
high_level_relax_orig ( MulticoreArray < T > & array, MulticoreArray < T > & old_array )
   {
     const int arraySizeX = array . get_arraySize ( 0 );
     const int arraySizeY = array . get_arraySize ( 1 );
     const int arraySizeZ = array . get_arraySize ( 2 );
     for ( int k = 1; k < arraySizeZ - 1; k ++ )
        {
          for ( int j = 1; j < arraySizeY - 1; j ++ )
             {
               for ( int i = 1; i < arraySizeX - 1; i ++ )
                  {
                    old_array ( i, j, k ) = ( array ( i - 1, j - 1, k - 1 ) + array ( i + 1, j - 1, k - 1 ) + array ( i - 1, j + 1, k - 1 ) + array ( i + 1, j + 1, k - 1 ) + array ( i - 1, j - 1, k + 1 ) + array ( i + 1, j - 1, k + 1 ) + array ( i - 1, j + 1, k + 1 ) + array ( i + 1, j + 1, k + 1 ) ) / (8.0);
                  }
             }
        }
   }// *********************************************************************
// *********************************************************************
// Examples of the use of the Core<T> and MulticoreArray<T> abstractions
// *********************************************************************
// *********************************************************************
// The code in this function is what we expect users to want to write. However, 
// while this code uses the same data as the transformed code, but it less efficient.
// Higher level could be:
//    array(I) = (array(I-1) + array(I+1)) / 2.0;
// but our current abstraction does not support this
// (use array class (e.g. A++) for this level).
// Use pragma to trigger compiler transformations, such as: 
//printf("(%d,%d,%d)=%f\n",k,j,i,old_array(i,j,k));
template < typename T >
void
high_level_relax ( MulticoreArray < T > & array, MulticoreArray < T > & old_array )
{
const int arraySizeX = array . get_arraySize ( 0 );
const int arraySizeY = array . get_arraySize ( 1 );
const int arraySizeZ = array . get_arraySize ( 2 );
#pragma stencil-manycore(T, old_array, array) D(k,0,arraySizeZ,1) D(j,0,arraySizeY,1) D(i,0,arraySizeX,1)
#pragma omp for
     for ( int k = 1; k < arraySizeZ - 1; k ++ )
        {
          for ( int j = 1; j < arraySizeY - 1; j ++ )
             {
               for ( int i = 1; i < arraySizeX - 1; i ++ )
                  {
                    old_array ( i, j, k ) = ( array ( i - 1, j - 1, k - 1 ) + array ( i + 1, j - 1, k - 1 ) + array ( i - 1, j + 1, k - 1 ) + array ( i + 1, j + 1, k - 1 ) + array ( i - 1, j - 1, k + 1 ) + array ( i + 1, j - 1, k + 1 ) + array ( i - 1, j + 1, k + 1 ) + array ( i + 1, j + 1, k + 1 ) ) / (8.0);
                  }
             }
        }
}
template<> void high_level_relax(class MulticoreArray< float  > &array,class MulticoreArray< float  > &old_array)
{
    const int arraySizeX = array . get_arraySize(0);
    const int arraySizeY = array . get_arraySize(1);
    const int arraySizeZ = array . get_arraySize(2);
    #pragma stencil-manycore(float, old_array, array) D(k,0,arraySizeZ,1) D(j,0,arraySizeY,1) D(i,0,arraySizeX,1) 
    int i;
    int j;
    int k;
    
    #define local_index3D(i,j,k,arraySizeX,arraySizeY) (k)*arraySizeY*arraySizeX+(j)*arraySizeX+(i)
    #define otherCore_index3D(i,j,k,arraySizeX,arraySizeY) (k)*arraySizeY*arraySizeX+(j)*arraySizeX+(i)
    
        int numberOfCores = array.get_numberOfCores();
        float** arraySectionPointers = array.get_arraySectionPointers();
        float** old_arraySectionPointers = old_array.get_arraySectionPointers();
    
    #pragma omp parallel for private(i,j,k)
        for (int core = 0; core < numberOfCores; core++)
           {
             float* arraySection     = arraySectionPointers[core];
             float* old_arraySection = old_arraySectionPointers[core];
    
             Core<float>* cur_core =  array.get_coreArray()[core];
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
    
             for (k=1; k<locallocallocal_size2-1; k=k+1)
             {
               for (j=1; j<locallocallocal_size1-1; j=j+1)
               {
                 for (i=1; i<locallocallocal_size0-1; i=i+1)
                 {
                   old_arraySection[local_index3D(i,j,k,locallocallocal_size0,locallocallocal_size1)] = (arraySection[local_index3D(i-1,j-1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,j-1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i-1,j+1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,j+1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i-1,j-1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,j-1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i-1,j+1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,j+1,k+1,locallocallocal_size0,locallocallocal_size1)])/8.0;
                 }
               }
             }
    
             //***************************************
             // Now process the edges and corners
             // ***************************************
               if (locallocallocal_size0>1) 
                 {
                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                     for (k=1; k<locallocallocal_size2-1; k=k+1)
                     {
                       for (j=1; j<locallocallocal_size1-1; j=j+1)
                       {
                         old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j-1,k-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j-1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j+1,k-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j+1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j-1,k+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j-1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j+1,k+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j+1,k+1,locallocallocal_size0,locallocallocal_size1)])/8.0;
                       }
                     }
                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                     {
                       for (k=1; k<locallocallocal_size2-1; k=k+1)
                       {
                         for (j=1; j<locallocallocal_size1-1; j=j+1)
                         {
                           old_arraySection[local_index3D(-1+locallocallocal_size0,j,k,locallocallocal_size0,locallocallocal_size1)] = (arraySection[local_index3D(-1+locallocallocal_size0-1,j-1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j-1,k-1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j+1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j+1,k-1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j-1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j-1,k+1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j+1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j+1,k+1,locallocalright_size0,locallocalright_size1)])/8.0;
                         }
                       }
                     }
                 }
               else 
                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                   {
                     for (k=1; k<locallocallocal_size2-1; k=k+1)
                     {
                       for (j=1; j<locallocallocal_size1-1; j=j+1)
                       {
                         old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j-1,k-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j-1,k-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j+1,k-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j+1,k-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j-1,k+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j-1,k+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j+1,k+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j+1,k+1,locallocalright_size0,locallocalright_size1)])/8.0;
                       }
                     }
                   }
               if (locallocallocal_size1>1) 
                 {
                   if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                     {
                       for (k=1; k<locallocallocal_size2-1; k=k+1)
                       {
                         for (i=1; i<locallocallocal_size0-1; i=i+1)
                         {
                           old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i-1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i+1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i-1,0+1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,0+1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i-1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i+1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i-1,0+1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,0+1,k+1,locallocallocal_size0,locallocallocal_size1)])/8.0;
                         }
                       }
                       if (locallocallocal_size0>1) 
                         {
                           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                             {
                               for (k=1; k<locallocallocal_size2-1; k=k+1)
                               {
                                 old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][0]][otherCore_index3D(-1+localleftleft_size0,-1+localleftleft_size1,k-1,localleftleft_size0,localleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0+1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,k-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,0+1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][0]][otherCore_index3D(-1+localleftleft_size0,-1+localleftleft_size1,k+1,localleftleft_size0,localleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0+1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,k+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,0+1,k+1,locallocallocal_size0,locallocallocal_size1)])/8.0;
                               }
                             }
                           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                             {
                               for (k=1; k<locallocallocal_size2-1; k=k+1)
                               {
                                 old_arraySection[local_index3D(-1+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][2]][otherCore_index3D(0,-1+localleftright_size1,k-1,localleftright_size0,localleftright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,0+1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,k-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][2]][otherCore_index3D(0,-1+localleftright_size1,k+1,localleftright_size0,localleftright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,0+1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,k+1,locallocalright_size0,locallocalright_size1)])/8.0;
                               }
                             }
                         }
                       else 
                         if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                           {
                             for (k=1; k<locallocallocal_size2-1; k=k+1)
                             {
                               old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][0]][otherCore_index3D(-1+localleftleft_size0,-1+localleftleft_size1,k-1,localleftleft_size0,localleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][2]][otherCore_index3D(0,-1+localleftright_size1,k-1,localleftright_size0,localleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,k-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,k-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][0]][otherCore_index3D(-1+localleftleft_size0,-1+localleftleft_size1,k+1,localleftleft_size0,localleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][2]][otherCore_index3D(0,-1+localleftright_size1,k+1,localleftright_size0,localleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,k+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,k+1,locallocalright_size0,locallocalright_size1)])/8.0;
                             }
                           }
                     }
                   if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                     {
                       for (k=1; k<locallocallocal_size2-1; k=k+1)
                       {
                         for (i=1; i<locallocallocal_size0-1; i=i+1)
                         {
                           old_arraySection[local_index3D(i,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = (arraySection[local_index3D(i-1,-1+locallocallocal_size1-1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,-1+locallocallocal_size1-1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i-1,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i+1,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i-1,-1+locallocallocal_size1-1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,-1+locallocallocal_size1-1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i-1,0,k+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i+1,0,k+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                         }
                       }
                       if (locallocallocal_size0>1) 
                         {
                           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                             {
                               for (k=1; k<locallocallocal_size2-1; k=k+1)
                               {
                                 old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,k-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,-1+locallocallocal_size1-1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][0]][otherCore_index3D(-1+localrightleft_size0,0,k-1,localrightleft_size0,localrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0+1,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,k+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,-1+locallocallocal_size1-1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][0]][otherCore_index3D(-1+localrightleft_size0,0,k+1,localrightleft_size0,localrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0+1,0,k+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                               }
                             }
                           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                             {
                               for (k=1; k<locallocallocal_size2-1; k=k+1)
                               {
                                 old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = (arraySection[local_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,k-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,k-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][2]][otherCore_index3D(0,0,k-1,localrightright_size0,localrightright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,k+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,k+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,k+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][2]][otherCore_index3D(0,0,k+1,localrightright_size0,localrightright_size1)])/8.0;
                               }
                             }
                         }
                       else 
                         if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                           {
                             for (k=1; k<locallocallocal_size2-1; k=k+1)
                             {
                               old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,k-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,k-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][0]][otherCore_index3D(-1+localrightleft_size0,0,k-1,localrightleft_size0,localrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][2]][otherCore_index3D(0,0,k-1,localrightright_size0,localrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,k+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,k+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][0]][otherCore_index3D(-1+localrightleft_size0,0,k+1,localrightleft_size0,localrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][2]][otherCore_index3D(0,0,k+1,localrightright_size0,localrightright_size1)])/8.0;
                             }
                           }
                     }
                 }
               else 
                 if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                   {
                     for (k=1; k<locallocallocal_size2-1; k=k+1)
                     {
                       for (i=1; i<locallocallocal_size0-1; i=i+1)
                       {
                         old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i-1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i+1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i-1,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i+1,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i-1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i+1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i-1,0,k+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i+1,0,k+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                       }
                     }
                     if (locallocallocal_size1>1) 
                       {
                         if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                           {
                             for (k=1; k<locallocallocal_size2-1; k=k+1)
                             {
                               old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0+1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,0,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0+1,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0+1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,0,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0+1,0,k+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                             }
                           }
                         if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                           {
                             for (k=1; k<locallocallocal_size2-1; k=k+1)
                             {
                               old_arraySection[local_index3D(-1+locallocallocal_size1,0,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+localrightlocal_size1,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size1-1,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+localrightlocal_size1,k+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size1-1,0,k+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,0,k+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                             }
                           }
                       }
                     else 
                       if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                         {
                           for (k=1; k<locallocallocal_size2-1; k=k+1)
                           {
                             old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,-1+localleftlocal_size1,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+localrightlocal_size1,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,0,k-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,0,k-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,-1+localleftlocal_size1,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+localrightlocal_size1,k+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,0,k+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,0,k+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                           }
                         }
                   }
               if (locallocallocal_size2>1) 
                 {
                   if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                     {
                       for (j=1; j<locallocallocal_size1-1; j=j+1)
                       {
                         for (i=1; i<locallocallocal_size0-1; i=i+1)
                         {
                           old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,j-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,j-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,j+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,j+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(i-1,j-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,j-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i-1,j+1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,j+1,0+1,locallocallocal_size0,locallocallocal_size1)])/8.0;
                         }
                       }
                       if (locallocallocal_size0>1) 
                         {
                           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                             {
                               for (j=1; j<locallocallocal_size1-1; j=j+1)
                               {
                                 old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][0]][otherCore_index3D(-1+leftlocalleft_size0,j-1,-1+leftlocalleft_size2,leftlocalleft_size0,leftlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,j-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][0]][otherCore_index3D(-1+leftlocalleft_size0,j+1,-1+leftlocalleft_size2,leftlocalleft_size0,leftlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,j+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j-1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j+1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j+1,0+1,locallocallocal_size0,locallocallocal_size1)])/8.0;
                               }
                             }
                           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                             {
                               for (j=1; j<locallocallocal_size1-1; j=j+1)
                               {
                                 old_arraySection[local_index3D(-1+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][2]][otherCore_index3D(0,j-1,-1+leftlocalright_size2,leftlocalright_size0,leftlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][2]][otherCore_index3D(0,j+1,-1+leftlocalright_size2,leftlocalright_size0,leftlocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j-1,0+1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j+1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j+1,0+1,locallocalright_size0,locallocalright_size1)])/8.0;
                               }
                             }
                         }
                       else 
                         if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                           {
                             for (j=1; j<locallocallocal_size1-1; j=j+1)
                             {
                               old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][0]][otherCore_index3D(-1+leftlocalleft_size0,j-1,-1+leftlocalleft_size2,leftlocalleft_size0,leftlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][2]][otherCore_index3D(0,j-1,-1+leftlocalright_size2,leftlocalright_size0,leftlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][0]][otherCore_index3D(-1+leftlocalleft_size0,j+1,-1+leftlocalleft_size2,leftlocalleft_size0,leftlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][2]][otherCore_index3D(0,j+1,-1+leftlocalright_size2,leftlocalright_size0,leftlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j-1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j-1,0+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j+1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j+1,0+1,locallocalright_size0,locallocalright_size1)])/8.0;
                             }
                           }
                       if (locallocallocal_size1>1) 
                         {
                           if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                             {
                               for (i=1; i<locallocallocal_size0-1; i=i+1)
                               {
                                 old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i-1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i+1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i-1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i+1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i-1,0+1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,0+1,0+1,locallocallocal_size0,locallocallocal_size1)])/8.0;
                               }
                               if (locallocallocal_size0>1) 
                                 {
                                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                                     {
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][0]][otherCore_index3D(-1+leftlocalleft_size0,0+1,-1+leftlocalleft_size2,leftlocalleft_size0,leftlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][0]][otherCore_index3D(-1+localleftleft_size0,-1+localleftleft_size1,0+1,localleftleft_size0,localleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0+1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,0+1,0+1,locallocallocal_size0,locallocallocal_size1)])/8.0;
                                     }
                                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                                     {
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][2]][otherCore_index3D(0,0+1,-1+leftlocalright_size2,leftlocalright_size0,leftlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][2]][otherCore_index3D(0,-1+localleftright_size1,0+1,localleftright_size0,localleftright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,0+1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,0+1,locallocalright_size0,locallocalright_size1)])/8.0;
                                     }
                                 }
                               else 
                                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                                   {
                                     old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][0]][otherCore_index3D(-1+leftleftleft_size0,-1+leftleftleft_size1,-1+leftleftleft_size2,leftleftleft_size0,leftleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][2]][otherCore_index3D(0,-1+leftleftright_size1,-1+leftleftright_size2,leftleftright_size0,leftleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][0]][otherCore_index3D(-1+leftlocalleft_size0,0+1,-1+leftlocalleft_size2,leftlocalleft_size0,leftlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][2]][otherCore_index3D(0,0+1,-1+leftlocalright_size2,leftlocalright_size0,leftlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][0]][otherCore_index3D(-1+localleftleft_size0,-1+localleftleft_size1,0+1,localleftleft_size0,localleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][2]][otherCore_index3D(0,-1+localleftright_size1,0+1,localleftright_size0,localleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,0+1,locallocalright_size0,locallocalright_size1)])/8.0;
                                   }
                             }
                           if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                             {
                               for (i=1; i<locallocallocal_size0-1; i=i+1)
                               {
                                 old_arraySection[local_index3D(i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i-1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i+1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySection[local_index3D(i-1,-1+locallocallocal_size1-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,-1+locallocallocal_size1-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i-1,0,0+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i+1,0,0+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                               }
                               if (locallocallocal_size0>1) 
                                 {
                                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                                     {
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][0]][otherCore_index3D(-1+leftlocalleft_size0,-1+locallocallocal_size1-1,-1+leftlocalleft_size2,leftlocalleft_size0,leftlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,-1+locallocallocal_size1-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][0]][otherCore_index3D(-1+localrightleft_size0,0,0+1,localrightleft_size0,localrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0+1,0,0+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                                     }
                                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                                     {
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+leftlocalright_size2,leftlocalright_size0,leftlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,0+1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,0+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,0+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][2]][otherCore_index3D(0,0,0+1,localrightright_size0,localrightright_size1)])/8.0;
                                     }
                                 }
                               else 
                                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                                   {
                                     old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][0]][otherCore_index3D(-1+leftlocalleft_size0,-1+locallocallocal_size1-1,-1+leftlocalleft_size2,leftlocalleft_size0,leftlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+leftlocalright_size2,leftlocalright_size0,leftlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][0]][otherCore_index3D(-1+leftrightleft_size0,0,-1+leftrightleft_size2,leftrightleft_size0,leftrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][2]][otherCore_index3D(0,0,-1+leftrightright_size2,leftrightright_size0,leftrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,0+1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,0+1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][0]][otherCore_index3D(-1+localrightleft_size0,0,0+1,localrightleft_size0,localrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][2]][otherCore_index3D(0,0,0+1,localrightright_size0,localrightright_size1)])/8.0;
                                   }
                             }
                         }
                       else 
                         if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                           {
                             for (i=1; i<locallocallocal_size0-1; i=i+1)
                             {
                               old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i-1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(i+1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i-1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(i+1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i-1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i+1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i-1,0,0+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i+1,0,0+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                             }
                             if (locallocallocal_size1>1) 
                               {
                                 if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                                   {
                                     old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(0+1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,0,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0+1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0+1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,0,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0+1,0,0+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                                   }
                                 if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                                   {
                                     old_arraySection[local_index3D(-1+locallocallocal_size1,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,-1+leftrightlocal_size1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(-1+locallocallocal_size1-1,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+localrightlocal_size1,0+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size1-1,0,0+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,0,0+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                                   }
                               }
                             else 
                               if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                                 {
                                   old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,-1+leftleftlocal_size1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,-1+leftrightlocal_size1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,0,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,-1+localleftlocal_size1,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+localrightlocal_size1,0+1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,0,0+1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,0,0+1,localrightlocal_size0,localrightlocal_size1)])/8.0;
                                 }
                           }
                     }
                   if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                     {
                       for (j=1; j<locallocallocal_size1-1; j=j+1)
                       {
                         for (i=1; i<locallocallocal_size0-1; i=i+1)
                         {
                           old_arraySection[local_index3D(i,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySection[local_index3D(i-1,j-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,j-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i-1,j+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,j+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,j-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,j-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,j+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,j+1,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                         }
                       }
                       if (locallocallocal_size0>1) 
                         {
                           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                             {
                               for (j=1; j<locallocallocal_size1-1; j=j+1)
                               {
                                 old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j-1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j+1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,j+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][0]][otherCore_index3D(-1+rightlocalleft_size0,j-1,0,rightlocalleft_size0,rightlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,j-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][0]][otherCore_index3D(-1+rightlocalleft_size0,j+1,0,rightlocalleft_size0,rightlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,j+1,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                               }
                             }
                           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                             {
                               for (j=1; j<locallocallocal_size1-1; j=j+1)
                               {
                                 old_arraySection[local_index3D(-1+locallocallocal_size0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySection[local_index3D(-1+locallocallocal_size0-1,j-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j-1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,j+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j+1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][2]][otherCore_index3D(0,j-1,0,rightlocalright_size0,rightlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,j+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][2]][otherCore_index3D(0,j+1,0,rightlocalright_size0,rightlocalright_size1)])/8.0;
                               }
                             }
                         }
                       else 
                         if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                           {
                             for (j=1; j<locallocallocal_size1-1; j=j+1)
                             {
                               old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j-1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j-1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,j+1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,j+1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][0]][otherCore_index3D(-1+rightlocalleft_size0,j-1,0,rightlocalleft_size0,rightlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][2]][otherCore_index3D(0,j-1,0,rightlocalright_size0,rightlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][0]][otherCore_index3D(-1+rightlocalleft_size0,j+1,0,rightlocalleft_size0,rightlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][2]][otherCore_index3D(0,j+1,0,rightlocalright_size0,rightlocalright_size1)])/8.0;
                             }
                           }
                       if (locallocallocal_size1>1) 
                         {
                           if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                             {
                               for (i=1; i<locallocallocal_size0-1; i=i+1)
                               {
                                 old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i-1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i+1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i-1,0+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,0+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i-1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i+1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                               }
                               if (locallocallocal_size0>1) 
                                 {
                                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                                     {
                                       old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][0]][otherCore_index3D(-1+localleftleft_size0,-1+localleftleft_size1,-1+locallocallocal_size2-1,localleftleft_size0,localleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0+1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,0+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0+1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][0]][otherCore_index3D(-1+rightlocalleft_size0,0+1,0,rightlocalleft_size0,rightlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                                     }
                                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                                     {
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][2]][otherCore_index3D(0,-1+localleftright_size1,-1+locallocallocal_size2-1,localleftright_size0,localleftright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0-1,0+1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][2]][otherCore_index3D(0,0+1,0,rightlocalright_size0,rightlocalright_size1)])/8.0;
                                     }
                                 }
                               else 
                                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                                   {
                                     old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][0]][otherCore_index3D(-1+localleftleft_size0,-1+localleftleft_size1,-1+locallocallocal_size2-1,localleftleft_size0,localleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][2]][otherCore_index3D(0,-1+localleftright_size1,-1+locallocallocal_size2-1,localleftright_size0,localleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,0+1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,0+1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][0]][otherCore_index3D(-1+rightleftleft_size0,-1+rightleftleft_size1,0,rightleftleft_size0,rightleftleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][2]][otherCore_index3D(0,-1+rightleftright_size1,0,rightleftright_size0,rightleftright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][0]][otherCore_index3D(-1+rightlocalleft_size0,0+1,0,rightlocalleft_size0,rightlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][2]][otherCore_index3D(0,0+1,0,rightlocalright_size0,rightlocalright_size1)])/8.0;
                                   }
                             }
                           if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                             {
                               for (i=1; i<locallocallocal_size0-1; i=i+1)
                               {
                                 old_arraySection[local_index3D(i,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySection[local_index3D(i-1,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+1,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i-1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i+1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i-1,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i+1,0,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                               }
                               if (locallocallocal_size0>1) 
                                 {
                                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                                     {
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+1,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][0]][otherCore_index3D(-1+localrightleft_size0,0,-1+locallocallocal_size2-1,localrightleft_size0,localrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0+1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][0]][otherCore_index3D(-1+rightlocalleft_size0,-1+locallocallocal_size1-1,0,rightlocalleft_size0,rightlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0+1,0,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                                     }
                                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                                     {
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySection[local_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][2]][otherCore_index3D(0,0,-1+locallocallocal_size2-1,localrightright_size0,localrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0-1,-1+locallocallocal_size1-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,0,rightlocalright_size0,rightlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size0-1,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)])/8.0;
                                     }
                                 }
                               else 
                                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                                   {
                                     old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(-1+locallocalleft_size0,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][0]][otherCore_index3D(-1+localrightleft_size0,0,-1+locallocallocal_size2-1,localrightleft_size0,localrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][2]][otherCore_index3D(0,0,-1+locallocallocal_size2-1,localrightright_size0,localrightright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][0]][otherCore_index3D(-1+rightlocalleft_size0,-1+locallocallocal_size1-1,0,rightlocalleft_size0,rightlocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][2]][otherCore_index3D(0,-1+locallocallocal_size1-1,0,rightlocalright_size0,rightlocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][0]][otherCore_index3D(-1+rightrightleft_size0,0,0,rightrightleft_size0,rightrightleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][2]][otherCore_index3D(0,0,0,rightrightright_size0,rightrightright_size1)])/8.0;
                                   }
                             }
                         }
                       else 
                         if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                           {
                             for (i=1; i<locallocallocal_size0-1; i=i+1)
                             {
                               old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i-1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i+1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i-1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i+1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i-1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(i+1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i-1,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(i+1,0,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                             }
                             if (locallocallocal_size1>1) 
                               {
                                 if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                                   {
                                     old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0+1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,0,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0+1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(0+1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,0,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0+1,0,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                                   }
                                 if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                                   {
                                     old_arraySection[local_index3D(-1+locallocallocal_size1,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+localrightlocal_size1,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size1-1,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,-1+rightrightlocal_size1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(-1+locallocallocal_size1-1,0,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                                   }
                               }
                             else 
                               if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                                 {
                                   old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,-1+localleftlocal_size1,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+localrightlocal_size1,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+localleftlocal_size1,0,-1+locallocallocal_size2-1,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,0,-1+locallocallocal_size2-1,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,-1+rightleftlocal_size1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,-1+rightrightlocal_size1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,0,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                                 }
                           }
                     }
                 }
               else 
                 if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                   {
                     for (j=1; j<locallocallocal_size1-1; j=j+1)
                     {
                       for (i=1; i<locallocallocal_size0-1; i=i+1)
                       {
                         old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,j-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,j-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,j+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,j+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,j-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,j-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,j+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,j+1,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                       }
                     }
                     if (locallocallocal_size1>1) 
                       {
                         if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                           {
                             for (j=1; j<locallocallocal_size1-1; j=j+1)
                             {
                               old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,j-1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,j-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,j+1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,j+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,j-1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,j-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,j+1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,j+1,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                             }
                           }
                         if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                           {
                             for (j=1; j<locallocallocal_size1-1; j=j+1)
                             {
                               old_arraySection[local_index3D(-1+locallocallocal_size1,j,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,j-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,j-1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,j+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,j+1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,j-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,j-1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,j+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,j+1,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                             }
                           }
                       }
                     else 
                       if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                         {
                           for (j=1; j<locallocallocal_size1-1; j=j+1)
                           {
                             old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,j-1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,j-1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,j+1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,j+1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,j-1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,j-1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,j+1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,j+1,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                           }
                         }
                     if (locallocallocal_size2>1) 
                       {
                         if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                           {
                             for (i=1; i<locallocallocal_size0-1; i=i+1)
                             {
                               old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                             }
                             if (locallocallocal_size1>1) 
                               {
                                 if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                                   {
                                     old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,-1+leftleftlocal_size2,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,0+1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,-1+leftleftlocal_size2,0,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,0+1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                                   }
                                 if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                                   {
                                     old_arraySection[local_index3D(-1+locallocallocal_size1,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,-1+leftrightlocal_size2,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,0+1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0+1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,-1+leftrightlocal_size2,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,0+1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0+1,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                                   }
                               }
                             else 
                               if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                                 {
                                   old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,-1+leftleftlocal_size2,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,-1+leftrightlocal_size2,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,0+1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,0+1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,-1+leftleftlocal_size2,0,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,-1+leftrightlocal_size2,0,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,0+1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0+1,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                                 }
                           }
                         if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                           {
                             for (i=1; i<locallocallocal_size0-1; i=i+1)
                             {
                               old_arraySection[local_index3D(i,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,-1+locallocallocal_size2-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,-1+locallocallocal_size2-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,-1+locallocallocal_size2-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,-1+locallocallocal_size2-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,0,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,0,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                             }
                             if (locallocallocal_size1>1) 
                               {
                                 if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                                   {
                                     old_arraySection[local_index3D(0,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,-1+locallocallocal_size2-1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,-1+locallocallocal_size2-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,0,-1+rightleftlocal_size2,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,-1+locallocallocal_size2-1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,-1+locallocallocal_size2-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,0,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,0,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                                   }
                                 if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                                   {
                                     old_arraySection[local_index3D(-1+locallocallocal_size1,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,-1+locallocallocal_size2-1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,-1+rightrightlocal_size2,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,-1+locallocallocal_size2-1,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,-1+locallocallocal_size2-1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1-1,0,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                                   }
                               }
                             else 
                               if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                                 {
                                   old_arraySection[local_index3D(0,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][0][1]][otherCore_index3D(-1+leftleftlocal_size1,-1+locallocallocal_size2-1,-1+leftleftlocal_size2,leftleftlocal_size0,leftleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][2][1]][otherCore_index3D(0,-1+locallocallocal_size2-1,-1+leftrightlocal_size2,leftrightlocal_size0,leftrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,0,-1+rightleftlocal_size2,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,-1+rightrightlocal_size2,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,-1+locallocallocal_size2-1,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,-1+locallocallocal_size2-1,0,rightrightlocal_size0,rightrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][0][1]][otherCore_index3D(-1+rightleftlocal_size1,0,0,rightleftlocal_size0,rightleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][2][1]][otherCore_index3D(0,0,0,rightrightlocal_size0,rightrightlocal_size1)])/8.0;
                                 }
                           }
                       }
                     else 
                       if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                         {
                           for (i=1; i<locallocallocal_size0-1; i=i+1)
                           {
                             old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i-1,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i+1,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i-1,0,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i+1,0,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                           }
                           if (locallocallocal_size2>1) 
                             {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                   old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size2,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size2,0,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size2,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0+1,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size2,0,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0+1,0,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                   old_arraySection[local_index3D(-1+locallocallocal_size2,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size2-1,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+rightlocallocal_size2,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size2-1,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size2-1,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+rightlocallocal_size2,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size2-1,0,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                                 }
                             }
                           else 
                             if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                               {
                                 old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = (arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size2,-1+leftlocallocal_size2,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+rightlocallocal_size2,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size2,0,-1+leftlocallocal_size2,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+rightlocallocal_size2,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size2,-1+leftlocallocal_size2,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+rightlocallocal_size2,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+leftlocallocal_size2,0,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,0,rightlocallocal_size0,rightlocallocal_size1)])/8.0;
                               }
                         }
                   }
             // **************************************************
             // End of processing the corners of the X and Y axis.
             // **************************************************
    
       }
    
  };
// ****************************************************
// ****************************************************
// Main test function for MulticoreArray<T> abstraction
// ****************************************************
// ****************************************************

int main()
{
// A multidimensional array support would have to include inter-array padding (to allow the seperate rows to work well with the cache).
// If multiple arrays are used then intra-array padding will separate them from each other and permit them to work well with cache.
  int numaSupport = - 1;
// #ifdef NUMA_NUM_NODES
#if HAVE_NUMA_H
// A return value of -1 means that all other functions in this library are undefined.
#endif
  if (numaSupport != - 1) {
    printf("NUMA support is AVAILABLE \n");
  }
  else {
    printf("NUMA support is NOT available numaSupport = %d \n",numaSupport);
  }
  int arraySize[DIM] = {(0), (0), (0)};
  int numberOfCoresArraySize[DIM] = {(0), (0), (0)};
// Page size to avoid cache conflicts with memory allocted for use by each core
  int interArraySubsectionPadding = 0;
// Tests for array abstraction constructor an initialization.
  int maxArraySizePerAxis = 8;
  for (int size_Z = 4; size_Z <= maxArraySizePerAxis; size_Z++) 
    for (int size_Y = 4; size_Y <= maxArraySizePerAxis; size_Y++) 
      for (int size_X = 4; size_X <= maxArraySizePerAxis; size_X++) {
        arraySize[0] = size_X;
        arraySize[1] = size_Y;
        arraySize[2] = size_Z;
        int numberOfCoresPerAxis = 2;
        for (int numberOfCores_Z = 1; numberOfCores_Z <= numberOfCoresPerAxis; numberOfCores_Z++) 
          for (int numberOfCores_Y = 1; numberOfCores_Y <= numberOfCoresPerAxis; numberOfCores_Y++) 
            for (int numberOfCores_X = 2; numberOfCores_X <= numberOfCoresPerAxis; numberOfCores_X++) {
              printf("***** Testing MulticoreArray construction using size_X = %2d size_Y = %2d size_Z = %2d numberOfCores = (%d,%d,%d) \n",size_X,size_Y,size_Z,numberOfCores_X,numberOfCores_Y,numberOfCores_Z);
              numberOfCoresArraySize[0] = numberOfCores_X;
              numberOfCoresArraySize[1] = numberOfCores_Y;
              numberOfCoresArraySize[2] = numberOfCores_Z;
              class MulticoreArray< float  > tmp_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
              class MulticoreArray< float  > tmp_array2(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,true);
              class MulticoreArray< float  > old_tmp_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
              class MulticoreArray< float  > old_tmp_array2(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,true);
// Test initialization
              for (int k = 0; k < size_Z; k++) 
                for (int j = 0; j < size_Y; j++) 
                  for (int i = 0; i < size_X; i++) {
                    tmp_array1(i,j,k) = (- 1.0);
                    tmp_array2(i,j,k) = (- 1.0);
                  }
              tmp_array1 . initializeDataWithCoreArrayIndex();
              tmp_array2 . initializeDataWithCoreArrayIndex();
              tmp_array1 . initializeBoundary((- 1));
              tmp_array2 . initializeBoundary((- 1));
              high_level_relax_orig(tmp_array1,old_tmp_array1);
              printf("==============\n");
              high_level_relax(tmp_array2,old_tmp_array2);
#if 0
// Output the result
#endif
// Make sure that the results are the same.
              tmp_array1 == tmp_array2?((void )0) : ((__assert_fail("tmp_array1 == tmp_array2","testPermutations.C",162,__PRETTY_FUNCTION__) , ((void )0)));
              old_tmp_array1 == old_tmp_array2?((void )0) : ((__assert_fail("old_tmp_array1 == old_tmp_array2","testPermutations.C",163,__PRETTY_FUNCTION__) , ((void )0)));
            }
      }
}
