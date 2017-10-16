
#define hasHaloCopy 1
#define hasDetachHalo 1
#define usePAPI 0
#define isComparedHighLevel 1
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <string>
#include "rdtsc.h"
#include <iostream>
#include <time.h>
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
template <typename T>
void
high_level_relax ( MulticoreArray<T> & array, MulticoreArray<T> & old_array, int dist)
{
const int arraySizeX = array.get_arraySize(0);
const int arraySizeY = array.get_arraySize(1);
const int arraySizeZ = array.get_arraySize(2);
#pragma stencil-manycore(T, old_array, array) D(k,0,arraySizeZ,dist) D(j,0,arraySizeY,dist) D(i,0,arraySizeX,dist) H(d,1,dist,1)
int d;
int i;
int j;
int k;


#define local_index3D(i,j,k,arraySizeX,arraySizeY) (k)*arraySizeY*arraySizeX+(j)*arraySizeX+(i)
#define otherCore_index3D(i,j,k,arraySizeX,arraySizeY) (k)*arraySizeY*arraySizeX+(j)*arraySizeX+(i)

    int numberOfCores = array.get_numberOfCores();
    T** arraySectionPointers = array.get_arraySectionPointers();
    T** old_arraySectionPointers = old_array.get_arraySectionPointers();

#ifdef TIMING
    timespec time1[numberOfCores], time2[numberOfCores];
    timespec bdyExecTime[numberOfCores];
    timespec ctrExecTime[numberOfCores];
#endif

#pragma omp parallel for private(d,i,j,k)
    for (int core = 0; core < numberOfCores; core++)
       {
         T* arraySection     = arraySectionPointers[core];
         T* old_arraySection = old_arraySectionPointers[core];

         Core<T>* cur_core =  array.get_coreArray()[core];
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

#ifdef TIMING
        int tid = omp_get_thread_num();
        clock_gettime(CLOCK_REALTIME, &time1[tid]);
#endif

         for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
         {
           for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
           {
             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
             {
               T tmp=0.0;
               for (int d=1; d<1+dist; d+=1) 
                 {
                    tmp = tmp+(arraySection[local_index3D(i-d,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j,k+d,locallocallocal_size0,locallocallocal_size1)]);
                 }
               old_arraySection[local_index3D(i,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
             }
           }
         }

#ifdef TIMING
     clock_gettime(CLOCK_REALTIME, &time2[tid]);
     ctrExecTime[tid] = diff(time1[tid],time2[tid]);
     clock_gettime(CLOCK_REALTIME, &time1[tid]);
#endif

         
         
         
         
         if (locallocallocal_size0>1)  
           {
              if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                {
                   for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                   {
                     for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                     {
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,j,k,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k+d,locallocallocal_size0,locallocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
                   }
                }
              if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                {
                   for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                   {
                     for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                     {
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,j,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,j,k,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,k+d,locallocallocal_size0,locallocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(-1+locallocallocal_size0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
                   }
                }
           }
         else  
           {
              if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                {
                   for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                   {
                     for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                     {
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,j,k,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,j,k,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(0,j-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,k+d,locallocallocal_size0,locallocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(0,j,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
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
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySection[local_index3D(i-d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,localleftlocal_size1-d,k,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i,0+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,k+d,locallocallocal_size0,locallocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
                   }
                   if (locallocallocal_size0>1)  
                     {
                        if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,0,k,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,k,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0,0+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                        if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,0,k,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0,localleftlocal_size1-d,k,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(-1+locallocallocal_size0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   else  
                     {
                        if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,0,k,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,0,k,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,k,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0,0+d,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                }
              if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                {
                   for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                   {
                     for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                     {
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySection[local_index3D(i-d,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,-1+d,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1,k+d,locallocallocal_size0,locallocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(i,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
                   }
                   if (locallocallocal_size0>1)  
                     {
                        if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,-1+locallocallocal_size1,k,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                        if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,-1+locallocallocal_size1,k,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size0,-1+d,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   else  
                     {
                        if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,-1+locallocallocal_size1,k,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,-1+locallocallocal_size1,k,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,-1+locallocallocal_size1,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                }
           }
         else  
           {
              if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                {
                   for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                   {
                     for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                     {
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySection[local_index3D(i-d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,localleftlocal_size1-d,k,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,-1+d,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,k+d,locallocallocal_size0,locallocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(i,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
                   }
                   if (locallocallocal_size1>1)  
                     {
                        if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,0,k,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0+d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,k,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                        if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size1-d,0,k,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,0,k,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size1,localleftlocal_size1-d,k,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size1,-1+d,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,0,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(-1+locallocallocal_size1,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   else  
                     {
                        if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                          {
                             for (k=dist; k<locallocallocal_size2-dist; k=k+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,0,k,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,0,k,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,k,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,k,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,0,k-d,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,k+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,0,k,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
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
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySection[local_index3D(i-d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,j,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(i,j,0+d,locallocallocal_size0,locallocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
                   }
                   if (locallocallocal_size0>1)  
                     {
                        if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,j,0,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,j,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                        if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,j,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0,j,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(-1+locallocallocal_size0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   else  
                     {
                        if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,j,0,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,j,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,j,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   if (locallocallocal_size1>1)  
                     {
                        if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,localleftlocal_size1-d,0,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i,0+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(i,0,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size0>1)  
                               {
                                  if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,0,0,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,0,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,0,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,0,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0,localleftlocal_size1-d,0,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,0,0,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,0,0,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,0,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,0,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                        if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,-1+d,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,-1+locallocallocal_size1,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size0>1)  
                               {
                                  if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,-1+locallocallocal_size1,0,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,-1+locallocallocal_size1,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,-1+locallocallocal_size1,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size0,-1+d,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,-1+locallocallocal_size1,0,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,-1+locallocallocal_size1,0,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,-1+locallocallocal_size1,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size1,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                     }
                   else  
                     {
                        if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,localleftlocal_size1-d,0,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,-1+d,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(i,0,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size1>1)  
                               {
                                  if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,0,0,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0+d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,0,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,0,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size1-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,0,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size1,localleftlocal_size1-d,0,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size1,-1+d,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,0,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size1,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,0,0,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,0,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,0,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,0,0+d,locallocallocal_size0,locallocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                     }
                }
              if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])  
                {
                   for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                   {
                     for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                     {
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySection[local_index3D(i-d,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,j,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(i,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
                   }
                   if (locallocallocal_size0>1)  
                     {
                        if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,j,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                        if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,j,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,j,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0,j,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(-1+locallocallocal_size0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   else  
                     {
                        if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,j,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,j,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(0,j-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,j,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   if (locallocallocal_size1>1)  
                     {
                        if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,localleftlocal_size1-d,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(i,0+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size0>1)  
                               {
                                  if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,0,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0,0+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,0,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size0,localleftlocal_size1-d,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,0,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,0,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0,0+d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                        if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,-1+d,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size1,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,-1+locallocallocal_size1,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size0>1)  
                               {
                                  if (locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]+arraySection[local_index3D(0+d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+locallocallocal_size1,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size0-d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size0,-1+d,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((locallocalleft_size0>0&&!cur_core->boundaryCore_3D[0][0])&&(locallocalright_size0>0&&!cur_core->boundaryCore_3D[0][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(locallocalleft_size0-d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocalleft_size0,locallocalleft_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D(-1+d,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocalright_size0,locallocalright_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1-d,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+locallocallocal_size1,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size1,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                     }
                   else  
                     {
                        if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,localleftlocal_size1-d,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,-1+d,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(i,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size1>1)  
                               {
                                  if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,0,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0+d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size1-d,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,0,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(-1+locallocallocal_size1,localleftlocal_size1-d,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+locallocallocal_size1,-1+d,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size1,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,0,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,0,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(0,localleftlocal_size1-d,-1+locallocallocal_size2,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(0,-1+d,-1+locallocallocal_size2,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,0,-1+locallocallocal_size2-d,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,-1+locallocallocal_size2,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                     }
                }
           }
         else  
           {
              if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]))  
                {
                   for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                   {
                     for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                     {
                       T tmp=0.0;
                       for (int d=1; d<1+dist; d+=1) 
                         {
                            tmp = tmp+(arraySection[local_index3D(i-d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,j+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,j,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,j,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                         }
                       old_arraySection[local_index3D(i,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                     }
                   }
                   if (locallocallocal_size1>1)  
                     {
                        if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,j,0,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0+d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                        if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size1-d,j,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,j,0,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,j+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1,j,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1,j,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(-1+locallocallocal_size1,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   else  
                     {
                        if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                          {
                             for (j=dist; j<locallocallocal_size1-dist; j=j+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,j,0,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,j,0,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,j-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,j+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,j,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,j,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(0,j,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                          }
                     }
                   if (locallocallocal_size2>1)  
                     {
                        if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,leftlocallocal_size2-d,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(i,0+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size1>1)  
                               {
                                  if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,0,0,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0+d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,leftlocallocal_size2-d,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size1-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,0,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1,leftlocallocal_size2-d,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,0+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size1,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,0,0,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,0,0,localrightlocal_size0,localrightlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,leftlocallocal_size2-d,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0,0+d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                        if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i,-1+locallocallocal_size2-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,-1+d,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,-1+locallocallocal_size2,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,-1+locallocallocal_size2,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size1>1)  
                               {
                                  if (localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,-1+locallocallocal_size2,0,localleftlocal_size0,localleftlocal_size1)]+arraySection[local_index3D(0+d,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size2-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+d,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,-1+locallocallocal_size2,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+locallocallocal_size2,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size1-d,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,-1+locallocallocal_size2,0,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(-1+locallocallocal_size1,-1+locallocallocal_size2-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1,-1+d,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size1,-1+locallocallocal_size2,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size1,-1+locallocallocal_size2,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size1,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((localleftlocal_size1>0&&!cur_core->boundaryCore_3D[1][0])&&(localrightlocal_size1>0&&!cur_core->boundaryCore_3D[1][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(localleftlocal_size1-d,-1+locallocallocal_size2,0,localleftlocal_size0,localleftlocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(-1+d,-1+locallocallocal_size2,0,localrightlocal_size0,localrightlocal_size1)]+arraySection[local_index3D(0,-1+locallocallocal_size2-d,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+d,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,-1+locallocallocal_size2,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+locallocallocal_size2,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,-1+locallocallocal_size2,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                     }
                   else  
                     {
                        if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]))  
                          {
                             for (i=dist; i<locallocallocal_size0-dist; i=i+dist) 
                             {
                               T tmp=0.0;
                               for (int d=1; d<1+dist; d+=1) 
                                 {
                                    tmp = tmp+(arraySection[local_index3D(i-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySection[local_index3D(i+d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,leftlocallocal_size2-d,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,-1+d,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                 }
                               old_arraySection[local_index3D(i,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                             }
                             if (locallocallocal_size2>1)  
                               {
                                  if (leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(leftlocallocal_size2-d,0,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySection[local_index3D(0+d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,leftlocallocal_size2-d,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+d,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                                  if (rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1])  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySection[local_index3D(-1+locallocallocal_size2-d,0,0,locallocallocal_size0,locallocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+d,0,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size2,leftlocallocal_size2-d,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size2,-1+d,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(-1+locallocallocal_size2,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+locallocallocal_size2,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(-1+locallocallocal_size2,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                             else  
                               {
                                  if ((leftlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][0])&&(rightlocallocal_size2>0&&!cur_core->boundaryCore_3D[2][1]))  
                                    {
                                       T tmp=0.0;
                                       for (int d=1; d<1+dist; d+=1) 
                                         {
                                            tmp = tmp+(arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(leftlocallocal_size2-d,0,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(-1+d,0,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,leftlocallocal_size2-d,0,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,-1+d,0,rightlocallocal_size0,rightlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(0,0,leftlocallocal_size2-d,leftlocallocal_size0,leftlocallocal_size1)]+arraySectionPointers[cur_core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(0,0,-1+d,rightlocallocal_size0,rightlocallocal_size1)]);
                                         }
                                       old_arraySection[local_index3D(0,0,0,locallocallocal_size0,locallocallocal_size1)] = tmp/(6.0*dist);
                                    }
                               }
                          }
                     }
                }
           }
	 
	 
	 
         

#ifdef TIMING
  
     clock_gettime(CLOCK_REALTIME, &time2[tid]);
     bdyExecTime[tid] = diff(time1[tid],time2[tid]);
#endif

   }                              
#ifdef TIMING
double ctrTime=0.0, bdyTime=0.0;
for (int core = 0; core < numberOfCores; core++)
{
ctrTime += (ctrExecTime[core].tv_sec) + (ctrExecTime[core].tv_nsec) / 1e9;
bdyTime += (bdyExecTime[core].tv_sec) + (bdyExecTime[core].tv_nsec) / 1e9;
}
cout << "center time:" << ctrTime/numberOfCores << endl;
cout << "boundry time:" << bdyTime/numberOfCores << endl;
#endif
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
#else
#pragma omp parallel for
for (int core = 0; core < numberCores; core++)
{
int tid = omp_get_thread_num();
int core_Z = core / (numberOfCores_X * numberOfCores_Y);
int core_Y = (core - core_Z * (numberOfCores_X * numberOfCores_Y)) / numberOfCores_X;
int core_X = core % numberOfCores_X;
#endif
Core<T> & coreMemory = array.getCore(core_X,core_Y,core_Z);
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
clock_gettime(CLOCK_REALTIME, &time1[tid]);
#endif
if(isMidLevel)
{
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
clock_gettime(CLOCK_REALTIME, &time2[tid]);
ctrExecTime[tid] = diff(time1[tid],time2[tid]);
clock_gettime(CLOCK_REALTIME, &time1[tid]);
#endif
if(isMidLevel)
{
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
}
#else
relax_on_boundary<T>(core,old_array,array,dist);
#endif
}
#ifndef __APPLE__
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
#if usePAPI
init_papi();
#define NUM_EVENTS 4
int Events[NUM_EVENTS] = {PAPI_L1_DCM, PAPI_L1_DCA, PAPI_L2_DCM, PAPI_L2_DCA};
long_long values[NUM_EVENTS];
long_long start_cycles, end_cycles, start_usec, end_usec;
int retval;
#endif
int numaSupport = -1;
#if HAVE_NUMA_H
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
int interArraySubsectionPadding =  0; 
haloSize[0] = stencilSize;
haloSize[1] = stencilSize;
haloSize[2] = stencilSize;
#if 1
arraySize[0] = global_arraySize;
arraySize[1] = global_arraySize;
arraySize[2] = global_arraySize;
#if 0
arraySize[1] = 1024;
arraySize[2] = 1;
#endif
numberOfCoresArraySize[0] = coreX;
numberOfCoresArraySize[1] = coreY;
numberOfCoresArraySize[2] = coreZ;
#if 0
numberOfCoresArraySize[0] = 8;
numberOfCoresArraySize[1] = 8;
numberOfCoresArraySize[2] = 1;
#endif
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
#endif
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
clock_gettime(CLOCK_REALTIME, &time1);
#endif
#endif
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
printf("\t Level 1 data cache misses: %lld\n",values[0]);
printf("\t Level 1 data cache accesses: %lld\n",values[1]);
printf("\t Level 1 miss rate: %.3f\n",((float)values[0])/((float)values[1]));
printf("\t Level 2 data cache misses: %lld\n",values[2]);
printf("\t Level 2 data cache accesses: %lld\n",values[3]);
printf("\t Level 2 miss rate: %.3f\n",((float)values[2])/((float)values[3]));
#else
#ifndef __APPLE__
clock_gettime(CLOCK_REALTIME, &time2);
cout << "overall high-level time: " << diff(time1,time2).tv_sec +  diff(time1,time2).tv_nsec/1e9 << endl;
#endif
#endif
#endif 
#if 0
#endif
#endif
#if 0
relax2D(array1,old_array1);
#endif
#if 1
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
printf("\t Level 1 data cache misses: %lld\n",values[0]);
printf("\t Level 1 data cache accesses: %lld\n",values[1]);
printf("\t Level 1 miss rate: %.3f\n",((float)values[0])/((float)values[1]));
printf("\t Level 2 data cache misses: %lld\n",values[2]);
printf("\t Level 2 data cache accesses: %lld\n",values[3]);
printf("\t Level 2 miss rate: %.3f\n",((float)values[2])/((float)values[3]));
#else
#ifndef __APPLE__
clock_gettime(CLOCK_REALTIME, &time2);
cout << "overall low-level time: " << diff(time1,time2).tv_sec + diff(time1,time2).tv_nsec/1e9 << endl;
#endif
#endif
printf("time: %llu\n", b - a);
assert(array1 == array2);
#endif
#if 0
array1.display("Result: array1");
array2.display("After relax2D_highlevel(): array2");
#endif
return 0;
}
