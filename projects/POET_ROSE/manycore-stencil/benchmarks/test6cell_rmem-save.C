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
#endif
// *********************************************************************
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
// #pragma manycore
#if 0
#endif
// 3D simple stencil
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
template < typename T >
void
high_level_relax_orig ( MulticoreArray < T > & array, MulticoreArray < T > & old_array, int dist )
   {
     const int arraySizeX = array . get_arraySize ( 0 );
     const int arraySizeY = array . get_arraySize ( 1 );
     const int arraySizeZ = array . get_arraySize ( 2 );
#pragma omp parallel for
     for ( int k = dist; k < arraySizeZ - dist; k ++ )
        {
          for ( int j = dist; j < arraySizeY - dist; j ++ )
             {
               for ( int i = dist; i < arraySizeX - dist; i ++ )
                  {
                 T tmp = (0.0);
                 for ( int d = 1; d <= dist; d ++ )
                    tmp += ( array ( i - d, j, k ) + array ( i + d, j, k ) + array ( i, j - d, k ) + array ( i, j + d, k ) + array ( i, j, k - d ) + array ( i, j, k + d ) );
                 old_array ( i, j, k ) = tmp / ( (6.0) * dist );
                  }
             }
        }
   }// The code in this function is what we expect users to want to write. However,
// while this code uses the same data as the transformed code, but it less efficient.
// Higher level could be:
//    array(I) = (array(I-1) + array(I+1)) / 2.0;
// but our current abstraction does not support this
// (use array class (e.g. A++) for this level).
// Use pragma to trigger compiler transformations, such as:
// #pragma manycore
// 3D simple stencil
template < typename T >
void
high_level_relax ( MulticoreArray < T > & array, MulticoreArray < T > & old_array, int dist )
   {
     const int arraySizeX = array . get_arraySize ( 0 );
     const int arraySizeY = array . get_arraySize ( 1 );
     const int arraySizeZ = array . get_arraySize ( 2 );
#pragma stencil-manycore(T, old_array, array) D(k,0,arraySizeZ,dist) D(j,0,arraySizeY,dist) D(i,0,arraySizeX,dist) H(d,1,dist,1)
     for ( int k = dist; k < arraySizeZ - dist; k ++ )
        {
          for ( int j = dist; j < arraySizeY - dist; j ++ )
             {
               for ( int i = dist; i < arraySizeX - dist; i ++ )
                  {
                  T tmp = (0.0);
                  for ( int d = 1; d <= dist; d ++ )
                     tmp += ( array ( i - d, j, k ) + array ( i + d, j, k ) + array ( i, j - d, k ) + array ( i, j + d, k ) + array ( i, j, k - d ) + array ( i, j, k + d ) );
                 old_array ( i, j, k ) = tmp / ( (6.0) * dist );
                  }
             }
        }
   }int global_arraySize;int stencilSize;int numberOfIterations;int coreX;int coreY;int coreZ;
template<> void high_level_relax(class MulticoreArray< double  > &array,class MulticoreArray< double  > &old_array,int dist)
{
    const int arraySizeX = array . get_arraySize(0);
    const int arraySizeY = array . get_arraySize(1);
    const int arraySizeZ = array . get_arraySize(2);
    #pragma stencil-manycore(double, old_array, array) D(k,0,arraySizeZ,dist) D(j,0,arraySizeY,dist) D(i,0,arraySizeX,dist) H(d,1,dist,1) 
    int d;
    int i;
    int j;
    int k;
    
    #define local_index3D(i,j,k,arraySizeX,arraySizeY) (k)*arraySizeY*arraySizeX+(j)*arraySizeX+(i)
    #define otherCore_index3D(i,j,k,arraySizeX,arraySizeY) (k)*arraySizeY*arraySizeX+(j)*arraySizeX+(i)
    
        int numberOfCores = array.get_numberOfCores();
        double** arraySectionPointers = array.get_arraySectionPointers();
        double** old_arraySectionPointers = old_array.get_arraySectionPointers();
    
    #pragma omp parallel for private(d,i,j,k)
        for (int core = 0; core < numberOfCores; core++)
           {
             double* arraySection     = arraySectionPointers[core];
             double* old_arraySection = old_arraySectionPointers[core];
    
             Core<double>* cur_core =  array.get_coreArray()[core];
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
    
             int locallocalleft_cp_index;
             double locallocalleft_cp[locallocalleft_size2*locallocalleft_size1*dist] ;
             locallocalleft_cp_index = 0;
             for (k=0; k<locallocalleft_size2; k+=1) 
               for (j=0; j<locallocalleft_size1; j+=1) 
                 for (d=0; d<dist; d+=1) 
                   locallocalleft_cp[locallocalleft_cp_index++]  = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-(1+d),j,k,locallocalleft_size0,locallocalleft_size1)];
             int locallocalright_cp_index;
             double locallocalright_cp[locallocalright_size2*locallocalright_size1*dist] ;
             locallocalright_cp_index = 0;
             for (k=0; k<locallocalright_size2; k+=1) 
               for (j=0; j<locallocalright_size1; j+=1) 
                 for (d=0; d<dist; d+=1) 
                   locallocalright_cp[locallocalright_cp_index++]  = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+(1+d),j,k,locallocalright_size0,locallocalright_size1)];
             int localleftlocal_cp_index;
             double localleftlocal_cp[localleftlocal_size2*dist*localleftlocal_size0] ;
             localleftlocal_cp_index = 0;
             for (k=0; k<localleftlocal_size2; k+=1) 
               for (d=0; d<dist; d+=1) 
                 for (i=0; i<localleftlocal_size0; i+=1) 
                   localleftlocal_cp[localleftlocal_cp_index++]  = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,localleftlocal_size1-(1+d),k,localleftlocal_size0,localleftlocal_size1)];
             int localrightlocal_cp_index;
             double localrightlocal_cp[localrightlocal_size2*dist*localrightlocal_size0] ;
             localrightlocal_cp_index = 0;
             for (k=0; k<localrightlocal_size2; k+=1) 
               for (d=0; d<dist; d+=1) 
                 for (i=0; i<localrightlocal_size0; i+=1) 
                   localrightlocal_cp[localrightlocal_cp_index++]  = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,-1+(1+d),k,localrightlocal_size0,localrightlocal_size1)];
             int leftlocallocal_cp_index;
             double leftlocallocal_cp[dist*leftlocallocal_size1*leftlocallocal_size0] ;
             leftlocallocal_cp_index = 0;
             for (d=0; d<dist; d+=1) 
               for (j=0; j<leftlocallocal_size1; j+=1) 
                 for (i=0; i<leftlocallocal_size0; i+=1) 
                   leftlocallocal_cp[leftlocallocal_cp_index++]  = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,j,leftlocallocal_size2-(1+d),leftlocallocal_size0,leftlocallocal_size1)];
             int rightlocallocal_cp_index;
             double rightlocallocal_cp[dist*rightlocallocal_size1*rightlocallocal_size0] ;
             rightlocallocal_cp_index = 0;
             for (d=0; d<dist; d+=1) 
               for (j=0; j<rightlocallocal_size1; j+=1) 
                 for (i=0; i<rightlocallocal_size0; i+=1) 
                   rightlocallocal_cp[rightlocallocal_cp_index++]  = arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,j,-1+(1+d),rightlocallocal_size0,rightlocallocal_size1)];for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                   {
                     for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                     {
                       for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                       {
                         double tmp = 0.0;
                         for (int d = 1;d<=dist;d++) 
                           tmp+=arraySection[local_index3D(i-d,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j,k+d,locallocallocal_size0,locallocallocal_size1)];
                         old_arraySection[local_index3D(i,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                       }
                     }
                   }
    
             //***************************************
             // Now process the edges and corners
             // ***************************************
               if (locallocallocal_size0>1) 
                 {
                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                     for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                     {
                       for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                       {
                         double tmp = 0.0;
                         for (int d = 1;d<=dist;d++) 
                           tmp+=locallocalleft_cp[-d+(1+(k*(dist*locallocalleft_size1)+j*dist))] +arraySection[local_index3D(0+d,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k+d,locallocallocal_size0,locallocallocal_size1)];
                         old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                       }
                     }
                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                     {
                       for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                       {
                         for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                         {
                           double tmp = 0.0;
                           for (int d = 1;d<=dist;d++) 
                             tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,j,k,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[d+(-1+(k*(dist*locallocalright_size1)+j*dist))] +arraySection[local_index3D(-1+locallocallocal_size0,j-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,k+d,locallocallocal_size0,locallocallocal_size1)];
                           old_arraySection[local_index3D(-1+locallocallocal_size0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                         }
                       }
                     }
                 }
               else 
                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                   {
                     for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                     {
                       for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                       {
                         double tmp = 0.0;
                         for (int d = 1;d<=dist;d++) 
                           tmp+=locallocalleft_cp[-d+(1+(k*(dist*locallocalleft_size1)+j*dist))] +locallocalright_cp[d+(-1+(k*(dist*locallocalright_size1)+j*dist))] +arraySection[local_index3D(0,j-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k+d,locallocallocal_size0,locallocallocal_size1)];
                         old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                       }
                     }
                   }
               if (locallocallocal_size1>1) 
                 {
                   if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                     {
                       for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                       {
                         for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                         {
                           double tmp = 0.0;
                           for (int d = 1;d<=dist;d++) 
                             tmp+=arraySection[local_index3D(i-d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,k,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[i+(k*(localleftlocal_size0*dist)+-(1-d)*localleftlocal_size0)] +arraySection[local_index3D(i,0+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,k+d,locallocallocal_size0,locallocallocal_size1)];
                           old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                         }
                       }
                       if (locallocallocal_size0>1) 
                         {
                           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                             {
                               for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=locallocalleft_cp[k*(dist*locallocalleft_size1)+(1-d)] +arraySection[local_index3D(0+d,0,k,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[k*(localleftlocal_size0*dist)+-(1-d)*localleftlocal_size0] +arraySection[local_index3D(0,0+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k+d,locallocallocal_size0,locallocallocal_size1)];
                                 old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                             }
                           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                             {
                               for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,0,k,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[k*(dist*locallocalright_size1)+(-1+d)] +localleftlocal_cp[locallocallocal_size0+(-1+(k*(localleftlocal_size0*dist)+-(1-d)*localleftlocal_size0))] +arraySection[local_index3D(-1+locallocallocal_size0,0+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,k+d,locallocallocal_size0,locallocallocal_size1)];
                                 old_arraySection[local_index3D(-1+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                             }
                         }
                       else 
                         if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                           {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=locallocalleft_cp[k*(dist*locallocalleft_size1)+(1-d)] +locallocalright_cp[k*(dist*locallocalright_size1)+(-1+d)] +localleftlocal_cp[k*(localleftlocal_size0*dist)+-(1-d)*localleftlocal_size0] +arraySection[local_index3D(0,0+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k+d,locallocallocal_size0,locallocallocal_size1)];
                               old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                           }
                     }
                   if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                     {
                       for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                       {
                         for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                         {
                           double tmp = 0.0;
                           for (int d = 1;d<=dist;d++) 
                             tmp+=arraySection[local_index3D(i-d,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1-d,k,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[i+(k*(localrightlocal_size0*dist)+(-localrightlocal_size0+d*localrightlocal_size0))] +arraySection[local_index3D(i,-1+locallocallocal_size1,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1,k+d,locallocallocal_size0,locallocallocal_size1)];
                           old_arraySection[local_index3D(i,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                         }
                       }
                       if (locallocallocal_size0>1) 
                         {
                           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                             {
                               for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=locallocalleft_cp[-d+(1+(k*(dist*locallocalleft_size1)+(-dist+locallocallocal_size1*dist)))] +arraySection[local_index3D(0+d,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,k,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[k*(localrightlocal_size0*dist)+(-localrightlocal_size0+d*localrightlocal_size0)] +arraySection[local_index3D(0,-1+locallocallocal_size1,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,k+d,locallocallocal_size0,locallocallocal_size1)];
                                 old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                             }
                           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                             {
                               for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[d+(-1+(k*(dist*locallocalright_size1)+(-dist+locallocallocal_size1*dist)))] +arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1-d,k,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[locallocallocal_size0+(-1+(k*(localrightlocal_size0*dist)+(-localrightlocal_size0+d*localrightlocal_size0)))] +arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k+d,locallocallocal_size0,locallocallocal_size1)];
                                 old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                             }
                         }
                       else 
                         if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                           {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=locallocalleft_cp[-d+(1+(k*(dist*locallocalleft_size1)+(-dist+locallocallocal_size1*dist)))] +locallocalright_cp[d+(-1+(k*(dist*locallocalright_size1)+(-dist+locallocallocal_size1*dist)))] +arraySection[local_index3D(0,-1+locallocallocal_size1-d,k,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[k*(localrightlocal_size0*dist)+(-localrightlocal_size0+d*localrightlocal_size0)] +arraySection[local_index3D(0,-1+locallocallocal_size1,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,k+d,locallocallocal_size0,locallocallocal_size1)];
                               old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                           }
                     }
                 }
               else 
                 if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                   {
                     for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                     {
                       for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                       {
                         double tmp = 0.0;
                         for (int d = 1;d<=dist;d++) 
                           tmp+=arraySection[local_index3D(i-d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,k,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[i+(k*(localleftlocal_size0*dist)+-(1-d)*localleftlocal_size0)] +localrightlocal_cp[i+(k*(localrightlocal_size0*dist)+(-localrightlocal_size0+d*localrightlocal_size0))] +arraySection[local_index3D(i,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,k+d,locallocallocal_size0,locallocallocal_size1)];
                         old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                       }
                     }
                     if (locallocallocal_size1>1) 
                       {
                         if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                           {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=localleftlocal_cp[-d+(localleftlocal_size1+(k*(localleftlocal_size0*dist)+-(1-localleftlocal_size1)*localleftlocal_size0))] +arraySection[local_index3D(0+d,0,k,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[k*(localleftlocal_size0*dist)+-(1-d)*localleftlocal_size0] +localrightlocal_cp[k*(localrightlocal_size0*dist)+(-localrightlocal_size0+d*localrightlocal_size0)] +arraySection[local_index3D(0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k+d,locallocallocal_size0,locallocallocal_size1)];
                               old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                           }
                         if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                           {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=arraySection[local_index3D(-1+locallocallocal_size1-d,0,k,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[k*(localrightlocal_size0*dist)+(-1+d)] +localleftlocal_cp[locallocallocal_size1+(-1+(k*(localleftlocal_size0*dist)+-(1-d)*localleftlocal_size0))] +localrightlocal_cp[locallocallocal_size1+(-1+(k*(localrightlocal_size0*dist)+(-localrightlocal_size0+d*localrightlocal_size0)))] +arraySection[local_index3D(-1+locallocallocal_size1,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,0,k+d,locallocallocal_size0,locallocallocal_size1)];
                               old_arraySection[local_index3D(-1+locallocallocal_size1,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                           }
                       }
                     else 
                       if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                         {
                           for (k=dist; k<locallocallocal_size2-dist; k=k+dist)
                           {
                             double tmp = 0.0;
                             for (int d = 1;d<=dist;d++) 
                               tmp+=localleftlocal_cp[-d+(localleftlocal_size1+(k*(localleftlocal_size0*dist)+-(1-localleftlocal_size1)*localleftlocal_size0))] +localrightlocal_cp[k*(localrightlocal_size0*dist)+(-1+d)] +localleftlocal_cp[k*(localleftlocal_size0*dist)+-(1-d)*localleftlocal_size0] +localrightlocal_cp[k*(localrightlocal_size0*dist)+(-localrightlocal_size0+d*localrightlocal_size0)] +arraySection[local_index3D(0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k+d,locallocallocal_size0,locallocallocal_size1)];
                             old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                           }
                         }
                   }
               if (locallocallocal_size2>1) 
                 {
                   if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                     {
                       for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                       {
                         for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                         {
                           double tmp = 0.0;
                           for (int d = 1;d<=dist;d++) 
                             tmp+=arraySection[local_index3D(i-d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[i+(j*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +arraySection[local_index3D(i,j,0+d,locallocallocal_size0,locallocallocal_size1)];
                           old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                         }
                       }
                       if (locallocallocal_size0>1) 
                         {
                           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                             {
                               for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=locallocalleft_cp[j*dist+(1-d)] +arraySection[local_index3D(0+d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[j*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))] +arraySection[local_index3D(0,j,0+d,locallocallocal_size0,locallocallocal_size1)];
                                 old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                             }
                           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                             {
                               for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,j,0,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[j*dist+(-1+d)] +arraySection[local_index3D(-1+locallocallocal_size0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[locallocallocal_size0+(-1+(j*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))))] +arraySection[local_index3D(-1+locallocallocal_size0,j,0+d,locallocallocal_size0,locallocallocal_size1)];
                                 old_arraySection[local_index3D(-1+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                             }
                         }
                       else 
                         if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                           {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=locallocalleft_cp[j*dist+(1-d)] +locallocalright_cp[j*dist+(-1+d)] +arraySection[local_index3D(0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[j*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))] +arraySection[local_index3D(0,j,0+d,locallocallocal_size0,locallocallocal_size1)];
                               old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                           }
                       if (locallocallocal_size1>1) 
                         {
                           if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                             {
                               for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=arraySection[local_index3D(i-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,0,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[-(1-d)*localleftlocal_size0+i] +arraySection[local_index3D(i,0+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[i+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))] +arraySection[local_index3D(i,0,0+d,locallocallocal_size0,locallocallocal_size1)];
                                 old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                               if (locallocallocal_size0>1) 
                                 {
                                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                                     {
                                       double tmp = 0.0;
                                       for (int d = 1;d<=dist;d++) 
                                         tmp+=locallocalleft_cp[1-d] +arraySection[local_index3D(0+d,0,0,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[-(1-d)*localleftlocal_size0] +arraySection[local_index3D(0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)] +arraySection[local_index3D(0,0,0+d,locallocallocal_size0,locallocallocal_size1)];
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                     }
                                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                                     {
                                       double tmp = 0.0;
                                       for (int d = 1;d<=dist;d++) 
                                         tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,0,0,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[-1+d] +localleftlocal_cp[-(1-d)*localleftlocal_size0+(-1+locallocallocal_size0)] +arraySection[local_index3D(-1+locallocallocal_size0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[locallocallocal_size0+(-1+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +arraySection[local_index3D(-1+locallocallocal_size0,0,0+d,locallocallocal_size0,locallocallocal_size1)];
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                     }
                                 }
                               else 
                                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=locallocalleft_cp[1-d] +locallocalright_cp[-1+d] +localleftlocal_cp[-(1-d)*localleftlocal_size0] +arraySection[local_index3D(0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)] +arraySection[local_index3D(0,0,0+d,locallocallocal_size0,locallocallocal_size1)];
                                     old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                             }
                           if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                             {
                               for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=arraySection[local_index3D(i-d,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1-d,0,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[i+(-localrightlocal_size0+d*localrightlocal_size0)] +leftlocallocal_cp[i+(locallocallocal_size1*leftlocallocal_size0+(-leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))))] +arraySection[local_index3D(i,-1+locallocallocal_size1,0+d,locallocallocal_size0,locallocallocal_size1)];
                                 old_arraySection[local_index3D(i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                               if (locallocallocal_size0>1) 
                                 {
                                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                                     {
                                       double tmp = 0.0;
                                       for (int d = 1;d<=dist;d++) 
                                         tmp+=locallocalleft_cp[-d+(1+(-dist+locallocallocal_size1*dist))] +arraySection[local_index3D(0+d,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,0,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[-localrightlocal_size0+d*localrightlocal_size0] +leftlocallocal_cp[locallocallocal_size1*leftlocallocal_size0+(-leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +arraySection[local_index3D(0,-1+locallocallocal_size1,0+d,locallocallocal_size0,locallocallocal_size1)];
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                     }
                                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                                     {
                                       double tmp = 0.0;
                                       for (int d = 1;d<=dist;d++) 
                                         tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[d+(-1+(-dist+locallocallocal_size1*dist))] +arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1-d,0,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[locallocallocal_size0+(-1+(-localrightlocal_size0+d*localrightlocal_size0))] +leftlocallocal_cp[locallocallocal_size0+(-1+(locallocallocal_size1*leftlocallocal_size0+(-leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))))] +arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0+d,locallocallocal_size0,locallocallocal_size1)];
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                     }
                                 }
                               else 
                                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=locallocalleft_cp[-d+(1+(-dist+locallocallocal_size1*dist))] +locallocalright_cp[d+(-1+(-dist+locallocallocal_size1*dist))] +arraySection[local_index3D(0,-1+locallocallocal_size1-d,0,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[-localrightlocal_size0+d*localrightlocal_size0] +leftlocallocal_cp[locallocallocal_size1*leftlocallocal_size0+(-leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +arraySection[local_index3D(0,-1+locallocallocal_size1,0+d,locallocallocal_size0,locallocallocal_size1)];
                                     old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                             }
                         }
                       else 
                         if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                           {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=arraySection[local_index3D(i-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,0,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[-(1-d)*localleftlocal_size0+i] +localrightlocal_cp[i+(-localrightlocal_size0+d*localrightlocal_size0)] +leftlocallocal_cp[i+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))] +arraySection[local_index3D(i,0,0+d,locallocallocal_size0,locallocallocal_size1)];
                               old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size1>1) 
                               {
                                 if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=localleftlocal_cp[-(1-localleftlocal_size1)*localleftlocal_size0+(localleftlocal_size1-d)] +arraySection[local_index3D(0+d,0,0,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[-(1-d)*localleftlocal_size0] +localrightlocal_cp[-localrightlocal_size0+d*localrightlocal_size0] +leftlocallocal_cp[leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)] +arraySection[local_index3D(0,0,0+d,locallocallocal_size0,locallocallocal_size1)];
                                     old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                                 if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=arraySection[local_index3D(-1+locallocallocal_size1-d,0,0,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[-1+d] +localleftlocal_cp[-(1-d)*localleftlocal_size0+(-1+locallocallocal_size1)] +localrightlocal_cp[locallocallocal_size1+(-1+(-localrightlocal_size0+d*localrightlocal_size0))] +leftlocallocal_cp[locallocallocal_size1+(-1+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +arraySection[local_index3D(-1+locallocallocal_size1,0,0+d,locallocallocal_size0,locallocallocal_size1)];
                                     old_arraySection[local_index3D(-1+locallocallocal_size1,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                               }
                             else 
                               if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                                 {
                                   double tmp = 0.0;
                                   for (int d = 1;d<=dist;d++) 
                                     tmp+=localleftlocal_cp[-(1-localleftlocal_size1)*localleftlocal_size0+(localleftlocal_size1-d)] +localrightlocal_cp[-1+d] +localleftlocal_cp[-(1-d)*localleftlocal_size0] +localrightlocal_cp[-localrightlocal_size0+d*localrightlocal_size0] +leftlocallocal_cp[leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)] +arraySection[local_index3D(0,0,0+d,locallocallocal_size0,locallocallocal_size1)];
                                   old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                 }
                           }
                     }
                   if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                     {
                       for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                       {
                         for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                         {
                           double tmp = 0.0;
                           for (int d = 1;d<=dist;d++) 
                             tmp+=arraySection[local_index3D(i-d,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[i+(j*rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                           old_arraySection[local_index3D(i,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                         }
                       }
                       if (locallocallocal_size0>1) 
                         {
                           if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                             {
                               for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=locallocalleft_cp[-d+(1+(j*dist+(-(dist*locallocalleft_size1)+locallocallocal_size2*(dist*locallocalleft_size1))))] +arraySection[local_index3D(0+d,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[j*rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))] ;
                                 old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                             }
                           if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                             {
                               for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[d+(-1+(j*dist+(-(dist*locallocalright_size1)+locallocallocal_size2*(dist*locallocalright_size1))))] +arraySection[local_index3D(-1+locallocallocal_size0,j-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[locallocallocal_size0+(-1+(j*rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))))] ;
                                 old_arraySection[local_index3D(-1+locallocallocal_size0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                             }
                         }
                       else 
                         if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                           {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=locallocalleft_cp[-d+(1+(j*dist+(-(dist*locallocalleft_size1)+locallocallocal_size2*(dist*locallocalleft_size1))))] +locallocalright_cp[d+(-1+(j*dist+(-(dist*locallocalright_size1)+locallocallocal_size2*(dist*locallocalright_size1))))] +arraySection[local_index3D(0,j-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[j*rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))] ;
                               old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                           }
                       if (locallocallocal_size1>1) 
                         {
                           if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                             {
                               for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=arraySection[local_index3D(i-d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[i+(-(1-d)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist)))] +arraySection[local_index3D(i,0+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[i+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))] ;
                                 old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                               if (locallocallocal_size0>1) 
                                 {
                                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                                     {
                                       double tmp = 0.0;
                                       for (int d = 1;d<=dist;d++) 
                                         tmp+=locallocalleft_cp[-d+(1+(-(dist*locallocalleft_size1)+locallocallocal_size2*(dist*locallocalleft_size1)))] +arraySection[local_index3D(0+d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[-(1-d)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist))] +arraySection[local_index3D(0,0+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)] ;
                                       old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                     }
                                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                                     {
                                       double tmp = 0.0;
                                       for (int d = 1;d<=dist;d++) 
                                         tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[d+(-1+(-(dist*locallocalright_size1)+locallocallocal_size2*(dist*locallocalright_size1)))] +localleftlocal_cp[locallocallocal_size0+(-1+(-(1-d)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist))))] +arraySection[local_index3D(-1+locallocallocal_size0,0+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[locallocallocal_size0+(-1+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                     }
                                 }
                               else 
                                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=locallocalleft_cp[-d+(1+(-(dist*locallocalleft_size1)+locallocallocal_size2*(dist*locallocalleft_size1)))] +locallocalright_cp[d+(-1+(-(dist*locallocalright_size1)+locallocallocal_size2*(dist*locallocalright_size1)))] +localleftlocal_cp[-(1-d)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist))] +arraySection[local_index3D(0,0+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)] ;
                                     old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                             }
                           if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                             {
                               for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=arraySection[local_index3D(i-d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[i+(d*localrightlocal_size0+(-localrightlocal_size0+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist))))] +arraySection[local_index3D(i,-1+locallocallocal_size1,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[i+(locallocallocal_size1*rightlocallocal_size0+(-rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))))] ;
                                 old_arraySection[local_index3D(i,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                               if (locallocallocal_size0>1) 
                                 {
                                   if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0]) 
                                     {
                                       double tmp = 0.0;
                                       for (int d = 1;d<=dist;d++) 
                                         tmp+=locallocalleft_cp[-d+(1+(locallocallocal_size1*dist+(-dist+(-(dist*locallocalleft_size1)+locallocallocal_size2*(dist*locallocalleft_size1)))))] +arraySection[local_index3D(0+d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[d*localrightlocal_size0+(-localrightlocal_size0+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist)))] +arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[locallocallocal_size1*rightlocallocal_size0+(-rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                     }
                                   if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]) 
                                     {
                                       double tmp = 0.0;
                                       for (int d = 1;d<=dist;d++) 
                                         tmp+=arraySection[local_index3D(-1+locallocallocal_size0-d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+locallocalright_cp[d+(-1+(locallocallocal_size1*dist+(-dist+(-(dist*locallocalright_size1)+locallocallocal_size2*(dist*locallocalright_size1)))))] +arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[locallocallocal_size0+(-1+(d*localrightlocal_size0+(-localrightlocal_size0+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist)))))] +arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[locallocallocal_size0+(-1+(locallocallocal_size1*rightlocallocal_size0+(-rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))))] ;
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                     }
                                 }
                               else 
                                 if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=locallocalleft_cp[-d+(1+(locallocallocal_size1*dist+(-dist+(-(dist*locallocalleft_size1)+locallocallocal_size2*(dist*locallocalleft_size1)))))] +locallocalright_cp[d+(-1+(locallocallocal_size1*dist+(-dist+(-(dist*locallocalright_size1)+locallocallocal_size2*(dist*locallocalright_size1)))))] +arraySection[local_index3D(0,-1+locallocallocal_size1-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[d*localrightlocal_size0+(-localrightlocal_size0+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist)))] +arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[locallocallocal_size1*rightlocallocal_size0+(-rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                                     old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                             }
                         }
                       else 
                         if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                           {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=arraySection[local_index3D(i-d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[i+(-(1-d)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist)))] +localrightlocal_cp[i+(d*localrightlocal_size0+(-localrightlocal_size0+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist))))] +arraySection[local_index3D(i,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[i+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))] ;
                               old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size1>1) 
                               {
                                 if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=localleftlocal_cp[-d+(localleftlocal_size1+(-(1-localleftlocal_size1)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist))))] +arraySection[local_index3D(0+d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localleftlocal_cp[-(1-d)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist))] +localrightlocal_cp[d*localrightlocal_size0+(-localrightlocal_size0+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist)))] +arraySection[local_index3D(0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)] ;
                                     old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                                 if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=arraySection[local_index3D(-1+locallocallocal_size1-d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[d+(-1+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist)))] +localleftlocal_cp[locallocallocal_size1+(-1+(-(1-d)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist))))] +localrightlocal_cp[locallocallocal_size1+(-1+(d*localrightlocal_size0+(-localrightlocal_size0+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist)))))] +arraySection[local_index3D(-1+locallocallocal_size1,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[locallocallocal_size1+(-1+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                                     old_arraySection[local_index3D(-1+locallocallocal_size1,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                               }
                             else 
                               if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                                 {
                                   double tmp = 0.0;
                                   for (int d = 1;d<=dist;d++) 
                                     tmp+=localleftlocal_cp[-d+(localleftlocal_size1+(-(1-localleftlocal_size1)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist))))] +localrightlocal_cp[d+(-1+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist)))] +localleftlocal_cp[-(1-d)*localleftlocal_size0+(-(localleftlocal_size0*dist)+locallocallocal_size2*(localleftlocal_size0*dist))] +localrightlocal_cp[d*localrightlocal_size0+(-localrightlocal_size0+(-(localrightlocal_size0*dist)+locallocallocal_size2*(localrightlocal_size0*dist)))] +arraySection[local_index3D(0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)] ;
                                   old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                 }
                           }
                     }
                 }
               else 
                 if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                   {
                     for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                     {
                       for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                       {
                         double tmp = 0.0;
                         for (int d = 1;d<=dist;d++) 
                           tmp+=arraySection[local_index3D(i-d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[i+(j*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +rightlocallocal_cp[i+(j*rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                         old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                       }
                     }
                     if (locallocallocal_size1>1) 
                       {
                         if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                           {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=localleftlocal_cp[-(j+(1-localleftlocal_size1))*localleftlocal_size0+(localleftlocal_size1-d)] +arraySection[local_index3D(0+d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[j*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))] +rightlocallocal_cp[j*rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))] ;
                               old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                           }
                         if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                           {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=arraySection[local_index3D(-1+locallocallocal_size1-d,j,0,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[j*localrightlocal_size0+(-1+d)] +arraySection[local_index3D(-1+locallocallocal_size1,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,j+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[locallocallocal_size1+(-1+(j*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))))] +rightlocallocal_cp[locallocallocal_size1+(-1+(j*rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))))] ;
                               old_arraySection[local_index3D(-1+locallocallocal_size1,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                           }
                       }
                     else 
                       if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                         {
                           for (j=dist; j<locallocallocal_size1-dist; j=j+dist)
                           {
                             double tmp = 0.0;
                             for (int d = 1;d<=dist;d++) 
                               tmp+=localleftlocal_cp[-(j+(1-localleftlocal_size1))*localleftlocal_size0+(localleftlocal_size1-d)] +localrightlocal_cp[j*localrightlocal_size0+(-1+d)] +arraySection[local_index3D(0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[j*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))] +rightlocallocal_cp[j*rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))] ;
                             old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                           }
                         }
                     if (locallocallocal_size2>1) 
                       {
                         if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                           {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=arraySection[local_index3D(i-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[i+(-d*leftlocallocal_size0+(leftlocallocal_size2*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1))))] +arraySection[local_index3D(i,0+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[i+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))] +rightlocallocal_cp[i+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))] ;
                               old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size1>1) 
                               {
                                 if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=localleftlocal_cp[-(1-localleftlocal_size1)*localleftlocal_size0+(localleftlocal_size1-d)] +arraySection[local_index3D(0+d,0,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[-d*leftlocallocal_size0+(leftlocallocal_size2*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1)))] +arraySection[local_index3D(0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)] +rightlocallocal_cp[-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)] ;
                                     old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                                 if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=arraySection[local_index3D(-1+locallocallocal_size1-d,0,0,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[-1+d] +leftlocallocal_cp[locallocallocal_size1+(-1+(-d*leftlocallocal_size0+(leftlocallocal_size2*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1)))))] +arraySection[local_index3D(-1+locallocallocal_size1,0+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[locallocallocal_size1+(-1+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +rightlocallocal_cp[locallocallocal_size1+(-1+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                                     old_arraySection[local_index3D(-1+locallocallocal_size1,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                               }
                             else 
                               if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                                 {
                                   double tmp = 0.0;
                                   for (int d = 1;d<=dist;d++) 
                                     tmp+=localleftlocal_cp[-(1-localleftlocal_size1)*localleftlocal_size0+(localleftlocal_size1-d)] +localrightlocal_cp[-1+d] +leftlocallocal_cp[-d*leftlocallocal_size0+(leftlocallocal_size2*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1)))] +arraySection[local_index3D(0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)] +rightlocallocal_cp[-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)] ;
                                   old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                 }
                           }
                         if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                           {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                             {
                               double tmp = 0.0;
                               for (int d = 1;d<=dist;d++) 
                                 tmp+=arraySection[local_index3D(i-d,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size2-d,0,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[i+(-rightlocallocal_size0+d*rightlocallocal_size0)] +leftlocallocal_cp[i+(locallocallocal_size2*leftlocallocal_size0+(-leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))))] +rightlocallocal_cp[i+(locallocallocal_size2*rightlocallocal_size0+(-rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))))] ;
                               old_arraySection[local_index3D(i,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size1>1) 
                               {
                                 if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0]) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=localleftlocal_cp[-(locallocallocal_size2-localleftlocal_size1)*localleftlocal_size0+(localleftlocal_size1-d)] +arraySection[local_index3D(0+d,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size2-d,0,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[-rightlocallocal_size0+d*rightlocallocal_size0] +leftlocallocal_cp[locallocallocal_size2*leftlocallocal_size0+(-leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +rightlocallocal_cp[locallocallocal_size2*rightlocallocal_size0+(-rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                                     old_arraySection[local_index3D(0,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                                 if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]) 
                                   {
                                     double tmp = 0.0;
                                     for (int d = 1;d<=dist;d++) 
                                       tmp+=arraySection[local_index3D(-1+locallocallocal_size1-d,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)]+localrightlocal_cp[d+(-1+(-localrightlocal_size0+locallocallocal_size2*localrightlocal_size0))] +arraySection[local_index3D(-1+locallocallocal_size1,-1+locallocallocal_size2-d,0,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[locallocallocal_size1+(-1+(-rightlocallocal_size0+d*rightlocallocal_size0))] +leftlocallocal_cp[locallocallocal_size1+(-1+(locallocallocal_size2*leftlocallocal_size0+(-leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))))] +rightlocallocal_cp[locallocallocal_size1+(-1+(locallocallocal_size2*rightlocallocal_size0+(-rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))))] ;
                                     old_arraySection[local_index3D(-1+locallocallocal_size1,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                   }
                               }
                             else 
                               if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])) 
                                 {
                                   double tmp = 0.0;
                                   for (int d = 1;d<=dist;d++) 
                                     tmp+=localleftlocal_cp[-(locallocallocal_size2-localleftlocal_size1)*localleftlocal_size0+(localleftlocal_size1-d)] +localrightlocal_cp[d+(-1+(-localrightlocal_size0+locallocallocal_size2*localrightlocal_size0))] +arraySection[local_index3D(0,-1+locallocallocal_size2-d,0,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[-rightlocallocal_size0+d*rightlocallocal_size0] +leftlocallocal_cp[locallocallocal_size2*leftlocallocal_size0+(-leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +rightlocallocal_cp[locallocallocal_size2*rightlocallocal_size0+(-rightlocallocal_size0+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                                   old_arraySection[local_index3D(0,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                 }
                           }
                       }
                     else 
                       if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                         {
                           for (i=dist; i<locallocallocal_size0-dist; i=i+dist)
                           {
                             double tmp = 0.0;
                             for (int d = 1;d<=dist;d++) 
                               tmp+=arraySection[local_index3D(i-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[i+(-d*leftlocallocal_size0+(leftlocallocal_size2*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1))))] +rightlocallocal_cp[i+(-rightlocallocal_size0+d*rightlocallocal_size0)] +leftlocallocal_cp[i+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1))] +rightlocallocal_cp[i+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1))] ;
                             old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                           }
                           if (locallocallocal_size2>1) 
                             {
                               if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0]) 
                                 {
                                   double tmp = 0.0;
                                   for (int d = 1;d<=dist;d++) 
                                     tmp+=leftlocallocal_cp[-d+(leftlocallocal_size2+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1)))] +arraySection[local_index3D(0+d,0,0,locallocallocal_size0,locallocallocal_size1)]+leftlocallocal_cp[-d*leftlocallocal_size0+(leftlocallocal_size2*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1)))] +rightlocallocal_cp[-rightlocallocal_size0+d*rightlocallocal_size0] +leftlocallocal_cp[leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)] +rightlocallocal_cp[-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)] ;
                                   old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                 }
                               if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]) 
                                 {
                                   double tmp = 0.0;
                                   for (int d = 1;d<=dist;d++) 
                                     tmp+=arraySection[local_index3D(-1+locallocallocal_size2-d,0,0,locallocallocal_size0,locallocallocal_size1)]+rightlocallocal_cp[-1+d] +leftlocallocal_cp[locallocallocal_size2+(-1+(-d*leftlocallocal_size0+(leftlocallocal_size2*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1)))))] +rightlocallocal_cp[locallocallocal_size2+(-1+(-rightlocallocal_size0+d*rightlocallocal_size0))] +leftlocallocal_cp[locallocallocal_size2+(-1+(leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)))] +rightlocallocal_cp[locallocallocal_size2+(-1+(-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)))] ;
                                   old_arraySection[local_index3D(-1+locallocallocal_size2,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                 }
                             }
                           else 
                             if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])) 
                               {
                                 double tmp = 0.0;
                                 for (int d = 1;d<=dist;d++) 
                                   tmp+=leftlocallocal_cp[-d+(leftlocallocal_size2+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1)))] +rightlocallocal_cp[-1+d] +leftlocallocal_cp[-d*leftlocallocal_size0+(leftlocallocal_size2*leftlocallocal_size0+(leftlocallocal_size0*leftlocallocal_size1+-leftlocallocal_size2*(leftlocallocal_size0*leftlocallocal_size1)))] +rightlocallocal_cp[-rightlocallocal_size0+d*rightlocallocal_size0] +leftlocallocal_cp[leftlocallocal_size0*leftlocallocal_size1+-d*(leftlocallocal_size0*leftlocallocal_size1)] +rightlocallocal_cp[-(rightlocallocal_size0*rightlocallocal_size1)+d*(rightlocallocal_size0*rightlocallocal_size1)] ;
                                 old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                               }
                         }
                   }
             // **************************************************
             // End of processing the corners of the X and Y axis.
             // **************************************************
    
       }
    
  };

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

int main(int argc,char *argv[])
{
  unsigned long long a;
  unsigned long long b;
  if (argc < 7) {
    print_usage();
    return - 1;
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
#define NUM_EVENTS 4
#endif
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
  arraySize[0] = global_arraySize;
  arraySize[1] = global_arraySize;
  arraySize[2] = global_arraySize;
  numberOfCoresArraySize[0] = coreX;
  numberOfCoresArraySize[1] = coreY;
  numberOfCoresArraySize[2] = coreZ;
  printf("Allocate array1 \n");
  class MulticoreArray< double  > array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
  printf("Allocate array1 \n");
  class MulticoreArray< double  > old_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
#if isComparedHighLevel
  printf("Allocate array2 \n");
  class MulticoreArray< double  > array2(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
  printf("Allocate old_array2 \n");
  class MulticoreArray< double  > old_array2(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
#endif
  printf("Initialize the array1 \n");
  array1 . sequenceFill3D(100.0);
  old_array1 . sequenceFill3D(100.0);
#if isComparedHighLevel
  printf("Initialize the array2 \n");
  array2 . sequenceFill3D(100.0);
  old_array2 . sequenceFill3D(100.0);
#endif
//     array1.display    ("after initialization: array1");
//     old_array1.display("after initialization: old_array1");
// Initialize the boundary
#if 0
#if isComparedHighLevel
#endif
#endif
//     array1.display    ("after initialization of boundary: array1");
//     old_array1.display("after initialization of boundary: old_array1");
  printf("Call relaxation \n");
#if usePAPI == 0
  struct timespec time1;
  struct timespec time2;
#endif
#if isComparedHighLevel
#if usePAPI
#else
#ifndef __APPLE__
// Setup timer
  clock_gettime(0,&time1);
#endif
#endif
// Call relaxation on array
  for (int i = 0; i < numberOfIterations / 2; i++) {
#if 0
#endif
    high_level_relax_orig(old_array2,array2,stencilSize);
    high_level_relax_orig(array2,old_array2,stencilSize);
  }
#if usePAPI
//           printf("\t Data TLB miss : %lld\n",values[0]);
#else
#ifndef __APPLE__
// Get and report the time.
  clock_gettime(0,&time2);
  (((cout<<"overall high-level time:") << diff(time1,time2) . tv_sec<<".") << diff(time1,time2) . tv_nsec) << endl;
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
#endif
#if 1
// Test refactored version of operator.
//     array1 = 0.0;
#if usePAPI
#else
#ifndef __APPLE__
// Setup timer
  clock_gettime(0,&time1);
#endif
#endif
  a = rdtsc();
  for (int i = 0; i < numberOfIterations / 2; i++) {
    high_level_relax(old_array1,array1,stencilSize);
    high_level_relax(array1,old_array1,stencilSize);
  }
  b = rdtsc();
#if usePAPI
//           printf("\t Data TLB miss : %lld\n",values[0]);
#else
#ifndef __APPLE__
// Get and report the time.
  clock_gettime(0,&time2);
  ((cout<<"overall high-level time: ") << (diff(time1,time2) . tv_sec + diff(time1,time2) . tv_nsec / 1e9)) << endl;
#endif
#endif
  printf("time: %llu\n",b - a);
#if isComparedHighLevel
  array1 == array2?((void )0) : ((__assert_fail("array1 == array2","test6cell.C",404,__PRETTY_FUNCTION__) , ((void )0)));
#endif
#endif
#if 0
// Output the result
//     old_array1.display("After relax2D_highlevel(): old_array1");
#endif
  return 0;
}
