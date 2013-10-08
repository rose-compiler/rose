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
#pragma 
#define local_index3D(i,j,k,arraySizeX,arraySizeY) (i)+((j)*arraySizeX+(k)*(arraySizeY*arraySizeX))

    int numberOfCores = array.get_numberOfCores();
    T** arraySectionPointers = array.get_arraySectionPointers();
    T** old_arraySectionPointers = old_array.get_arraySectionPointers();

    for (int core = 0; core < numberOfCores; core++)
       {
         T* arraySection     = arraySectionPointers[core];
         T* old_arraySection = old_arraySectionPointers[core];

         Core<T>* cur_core =  array.get_coreArray()[core];
         typename Core<T>::index_set_type (*coreArrayNeighborhoodSizes)[3] = cur_core->coreArrayNeighborhoodSizes_2D;
         bool (*bounaryCore)[2]= cur_core->bounaryCore_2D;
         int (*coreArrayNeighborhoodLinearized)[3] = cur_core->coreArrayNeighborhoodLinearized_2D; 
         
         int local_size2 = coreArrayNeighborhoodSizes[1][1][2]; 
         int upper_size2 = coreArrayNeighborhoodSizes[0][1][2];
         int lower_size2 = coreArrayNeighborhoodSizes[2][1][2];
         int left_size2 = coreArrayNeighborhoodSizes[1][0][2];
         int right_size2 = coreArrayNeighborhoodSizes[1][2][2];
         int upper_left_size2 = coreArrayNeighborhoodSizes[0][0][2];
         int upper_right_size2 = coreArrayNeighborhoodSizes[0][2][2];
         int lower_left_size2 = coreArrayNeighborhoodSizes[2][0][2];
         int lower_right_size2 = coreArrayNeighborhoodSizes[2][2][2];
         
         int local_size1 = coreArrayNeighborhoodSizes[1][1][1]; 
         int upper_size1 = coreArrayNeighborhoodSizes[0][1][1];
         int lower_size1 = coreArrayNeighborhoodSizes[2][1][1];
         int left_size1 = coreArrayNeighborhoodSizes[1][0][1];
         int right_size1 = coreArrayNeighborhoodSizes[1][2][1];
         int upper_left_size1 = coreArrayNeighborhoodSizes[0][0][1];
         int upper_right_size1 = coreArrayNeighborhoodSizes[0][2][1];
         int lower_left_size1 = coreArrayNeighborhoodSizes[2][0][1];
         int lower_right_size1 = coreArrayNeighborhoodSizes[2][2][1];
         
         int local_size0 = coreArrayNeighborhoodSizes[1][1][0]; 
         int upper_size0 = coreArrayNeighborhoodSizes[0][1][0];
         int lower_size0 = coreArrayNeighborhoodSizes[2][1][0];
         int left_size0 = coreArrayNeighborhoodSizes[1][0][0];
         int right_size0 = coreArrayNeighborhoodSizes[1][2][0];
         int upper_left_size0 = coreArrayNeighborhoodSizes[0][0][0];
         int upper_right_size0 = coreArrayNeighborhoodSizes[0][2][0];
         int lower_left_size0 = coreArrayNeighborhoodSizes[2][0][0];
         int lower_right_size0 = coreArrayNeighborhoodSizes[2][2][0];
         

         for (int  k=1; k<arraySizeZ-1; k++) 
           for (int  j=1; j<local_size1-1; j++) 
             for (int  i=1; i<local_size0-1; i++) 
               old_arraySection[local_index3D(i,j,k,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1+j,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1+j,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1+j,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,-1+k,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1+j,-1+k,local_size0,local_size1)])))))))/8.0;

         // ***************************************
         // Now process the edges 
         // ***************************************
         if (local_size0 > 1)
           {
             if (left_size0>0 && 
                 bounaryCore[0][0] == false)
             { 
               // ***** | ****** | *****
               // ----------------------
               // ***** | *XXXX* | *****
               // ***** | ****** | *****
               // ***** | ****** | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  k=1; k<-1+arraySizeZ; k++) 
                 for (int  j=1; j<-1+local_size1; j++) 
                   old_arraySection[local_index3D(0,j,k,local_size0,local_size1)] = (arraySection[local_index3D(1,1+j,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,1+j,1+k,left_size0,left_size1)]+(arraySection[local_index3D(1,-1+j,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,-1+j,1+k,left_size0,left_size1)]+(arraySection[local_index3D(1,1+j,-1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,1+j,-1+k,left_size0,left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,-1+j,-1+k,left_size0,left_size1)]+arraySection[local_index3D(1,-1+j,-1+k,local_size0,local_size1)])))))))/8.0; 
             }
             if (right_size0>0 &&
                 bounaryCore[0][1] == false)
             {
               // ***** | ****** | *****
               // ----------------------
               // ***** | ****** | *****
               // ***** | ****** | *****
               // ***** | *XXXX* | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  k=1; k<-1+arraySizeZ; k++) 
                 for (int  j=1; j<-1+local_size1; j++) 
                   old_arraySection[local_index3D(-1+local_size0,j,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,1+j,1+k,right_size0,right_size1)]+(arraySection[local_index3D(-2+local_size0,1+j,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,-1+j,1+k,right_size0,right_size1)]+(arraySection[local_index3D(-2+local_size0,-1+j,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,1+j,-1+k,right_size0,right_size1)]+(arraySection[local_index3D(-2+local_size0,1+j,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-2+local_size0,-1+j,-1+k,local_size0,local_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,-1+j,-1+k,right_size0,right_size1)])))))))/8.0; 
             }
           }
         else if (local_size0 == 1 &&
                  bounaryCore[0][0] == false && 
                  bounaryCore[0][1] == false &&
                  left_size0>0 &&
                  right_size0>0)
            {
               // ***** | ****** | *****
               // ----------------------
               // ***** | *XXXX* | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  k=1; k<-1+arraySizeZ; k++) 
                 for (int  j=1; j<-1+local_size1; j++) 
                   old_arraySection[local_index3D(0,j,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,1+j,1+k,right_size0,right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,1+j,1+k,left_size0,left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,-1+j,1+k,right_size0,right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,-1+j,1+k,left_size0,left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,1+j,-1+k,right_size0,right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,1+j,-1+k,left_size0,left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,-1+j,-1+k,left_size0,left_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,-1+j,-1+k,right_size0,right_size1)])))))))/8.0;
            }
         if (local_size1 > 1)
           {
             if (upper_size1>0 && 
                 bounaryCore[1][0] == false)
             { 
               // ***** | ****** | *****
               // ----------------------
               // ***** | *XXXX* | *****
               // ***** | ****** | *****
               // ***** | ****** | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  k=1; k<-1+arraySizeZ; k++) 
                 for (int  i=1; i<-1+local_size0; i++) 
                   old_arraySection[local_index3D(i,0,k,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(1+i,-1+upper_size1,1+k,upper_size0,upper_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(-1+i,-1+upper_size1,1+k,upper_size0,upper_size1)]+(arraySection[local_index3D(1+i,1,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,-1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(-1+i,-1+upper_size1,-1+k,upper_size0,upper_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(1+i,-1+upper_size1,-1+k,upper_size0,upper_size1)])))))))/8.0; 
             }
             if (lower_size1>0 &&
                 bounaryCore[1][1] == false)
             {
               // ***** | ****** | *****
               // ----------------------
               // ***** | ****** | *****
               // ***** | ****** | *****
               // ***** | *XXXX* | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  k=1; k<-1+arraySizeZ; k++) 
                 for (int  i=1; i<-1+local_size0; i++) 
                   old_arraySection[local_index3D(i,-1+local_size1,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(1+i,0,1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(-1+i,0,1+k,lower_size0,lower_size1)]+(arraySection[local_index3D(1+i,-2+local_size1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size1,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(1+i,0,-1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(-1+i,0,-1+k,lower_size0,lower_size1)]+(arraySection[local_index3D(-1+i,-2+local_size1,-1+k,local_size0,local_size1)]+arraySection[local_index3D(1+i,-2+local_size1,-1+k,local_size0,local_size1)])))))))/8.0; 
             }
           }
         else if (local_size1 == 1 &&
                  bounaryCore[1][0] == false && 
                  bounaryCore[1][1] == false &&
                  upper_size1>0 &&
                  lower_size1>0)
            {
               // ***** | ****** | *****
               // ----------------------
               // ***** | *XXXX* | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  k=1; k<-1+arraySizeZ; k++) 
                 for (int  i=1; i<-1+local_size0; i++) 
                   old_arraySection[local_index3D(i,0,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(1+i,0,1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(-1+i,0,1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(1+i,-1+upper_size1,1+k,upper_size0,upper_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(-1+i,-1+upper_size1,1+k,upper_size0,upper_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(1+i,0,-1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(-1+i,0,-1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(-1+i,-1+upper_size1,-1+k,upper_size0,upper_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(1+i,-1+upper_size1,-1+k,upper_size0,upper_size1)])))))))/8.0;
            }
         

         // ********************************************
	 // Now process the corners of the X and Y axis.
	 // ********************************************
         if (local_size0 > 1)
	     {
               if (bounaryCore[0][0] == false)
                {
                  if (left_size0 > 0)
                   {
                      if (local_size1 > 1)
               	 {
               	   // Upper corner
               	   if (bounaryCore[1][0] == false)
                            // Upper left corner
                            // ***** | ****** | *****
                            // ----------------------
                            // ***** | X***** | *****
                            // ***** | ****** | *****
                            // ***** | ****** | *****
                            // ----------------------
                            // ***** | ****** | *****
                            for (int  k=1; k<-1+arraySizeZ; k++) 
                              old_arraySection[local_index3D(0,0,k,local_size0,local_size1)] = (arraySection[local_index3D(1,1,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,1,1+k,left_size0,left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(1,-1+upper_size1,1+k,upper_size0,upper_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][0]][otherCore_index3D(-1+left_size0,-1+upper_size1,1+k,upper_left_size0,upper_left_size1)]+(arraySection[local_index3D(1,1,-1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,1,-1+k,left_size0,left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][0]][otherCore_index3D(-1+left_size0,-1+upper_size1,-1+k,upper_left_size0,upper_left_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(1,-1+upper_size1,-1+k,upper_size0,upper_size1)])))))))/8.0; 
               	   // Lower corner
               	   if (bounaryCore[1][1] == false)
                            // Lower left corner
                            // ***** | ****** | *****
                            // ----------------------
                            // ***** | ****** | *****
                            // ***** | ****** | *****
                            // ***** | X***** | *****
                            // ----------------------
                            // ***** | ****** | *****
                            for (int  k=1; k<-1+arraySizeZ; k++) 
                              old_arraySection[local_index3D(0,-1+local_size1,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(1,0,1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][0]][otherCore_index3D(-1+left_size0,0,1+k,lower_left_size0,lower_left_size1)]+(arraySection[local_index3D(1,-2+local_size1,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,-2+local_size1,1+k,left_size0,left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(1,0,-1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][0]][otherCore_index3D(-1+left_size0,0,-1+k,lower_left_size0,lower_left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][0]][otherCore_index3D(-1+left_size0,-2+local_size1,-1+k,left_size0,left_size1)]+arraySection[local_index3D(1,-2+local_size1,-1+k,local_size0,local_size1)])))))))/8.0; 
                          }
                      else if (local_size1 == 1)
                         // Case of upper and lower corners are the same point
                         // ***** | ****** | *****
                         // ----------------------
                         // ***** | X***** | *****
                         // ----------------------
                         // ***** | ****** | *****
                         for (int  k=1; k<-1+arraySizeZ; k++) 
                           old_arraySection[local_index3D(0,0,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(1,0,1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][0]][otherCore_index3D(-1+left_size0,0,1+k,lower_left_size0,lower_left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(1,-1+upper_size1,1+k,upper_size0,upper_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][0]][otherCore_index3D(-1+left_size0,-1+upper_size1,1+k,upper_left_size0,upper_left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(1,0,-1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][0]][otherCore_index3D(-1+left_size0,0,-1+k,lower_left_size0,lower_left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][0]][otherCore_index3D(-1+left_size0,-1+upper_size1,-1+k,upper_left_size0,upper_left_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(1,-1+upper_size1,-1+k,upper_size0,upper_size1)])))))))/8.0; 
                    }
                  else
                    {
                      printf ("We don't support the size on the adjacent being zero! \n");
                      assert(false);
                    }
                } 
               if (bounaryCore[0][1] == false)
                {
                  if (right_size0 > 0)
                   {
                      if (local_size1 > 1)
               	 {
               	   // Upper corner
               	   if (bounaryCore[1][0] == false)
                            // Upper right corner
                            // ***** | ****** | *****
                            // ----------------------
                            // ***** | X***** | *****
                            // ***** | ****** | *****
                            // ***** | ****** | *****
                            // ----------------------
                            // ***** | ****** | *****
                            for (int  k=1; k<-1+arraySizeZ; k++) 
                              old_arraySection[local_index3D(-1+local_size0,0,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,1,1+k,right_size0,right_size1)]+(arraySection[local_index3D(-2+local_size0,1,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][2]][otherCore_index3D(0,-1+upper_size1,1+k,upper_right_size0,upper_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(-2+local_size0,-1+upper_size1,1+k,upper_size0,upper_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,1,-1+k,right_size0,right_size1)]+(arraySection[local_index3D(-2+local_size0,1,-1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(-2+local_size0,-1+upper_size1,-1+k,upper_size0,upper_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[0][2]][otherCore_index3D(0,-1+upper_size1,-1+k,upper_right_size0,upper_right_size1)])))))))/8.0; 
               	   // Lower corner
               	   if (bounaryCore[1][1] == false)
                            // Lower right corner
                            // ***** | ****** | *****
                            // ----------------------
                            // ***** | ****** | *****
                            // ***** | ****** | *****
                            // ***** | X***** | *****
                            // ----------------------
                            // ***** | ****** | *****
                            for (int  k=1; k<-1+arraySizeZ; k++) 
                              old_arraySection[local_index3D(-1+local_size0,-1+local_size1,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[2][2]][otherCore_index3D(0,0,1+k,lower_right_size0,lower_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(-2+local_size0,0,1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,-2+local_size1,1+k,right_size0,right_size1)]+(arraySection[local_index3D(-2+local_size0,-2+local_size1,1+k,local_size0,local_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][2]][otherCore_index3D(0,0,-1+k,lower_right_size0,lower_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(-2+local_size0,0,-1+k,lower_size0,lower_size1)]+(arraySection[local_index3D(-2+local_size0,-2+local_size1,-1+k,local_size0,local_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[1][2]][otherCore_index3D(0,-2+local_size1,-1+k,right_size0,right_size1)])))))))/8.0; 
                          }
                      else if (local_size1 == 1)
                         // Case of upper and lower corners are the same point
                         // ***** | ****** | *****
                         // ----------------------
                         // ***** | X***** | *****
                         // ----------------------
                         // ***** | ****** | *****
                         for (int  k=1; k<-1+arraySizeZ; k++) 
                           old_arraySection[local_index3D(-1+local_size0,0,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[2][2]][otherCore_index3D(0,0,1+k,lower_right_size0,lower_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(-2+local_size0,0,1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][2]][otherCore_index3D(0,-1+upper_size1,1+k,upper_right_size0,upper_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(-2+local_size0,-1+upper_size1,1+k,upper_size0,upper_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][2]][otherCore_index3D(0,0,-1+k,lower_right_size0,lower_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][1]][otherCore_index3D(-2+local_size0,0,-1+k,lower_size0,lower_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][1]][otherCore_index3D(-2+local_size0,-1+upper_size1,-1+k,upper_size0,upper_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[0][2]][otherCore_index3D(0,-1+upper_size1,-1+k,upper_right_size0,upper_right_size1)])))))))/8.0; 
                    }
                  else
                    {
                      printf ("We don't support the size on the adjacent being zero! \n");
                      assert(false);
                    }
                } 
	     }
	   else if (local_size0 == 1 && local_size1 == 1)
		 {

		   if (bounaryCore[0][0] == false && bounaryCore[0][1] == false && bounaryCore[1][0] == false && bounaryCore[1][1] == false)
		     {
		       if (left_size0 > 0 && right_size0 > 0)
                          // ***** | * | *****
                          // -----------------
                          // ***** | X | *****
                          // -----------------
                          // ***** | * | *****
                          for (int  k=1; k<-1+arraySizeZ; k++) 
                            old_arraySection[local_index3D(0,-1+local_size0,k,local_size0,local_size1)] = (arraySectionPointers[coreArrayNeighborhoodLinearized[2][2]][otherCore_index3D(0,0,1+k,lower_right_size0,lower_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][2]][otherCore_index3D(-1+upper_size1,0,1+k,upper_right_size0,upper_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][0]][otherCore_index3D(0,-1+left_size0,1+k,lower_left_size0,lower_left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][0]][otherCore_index3D(-1+upper_size1,-1+left_size0,1+k,upper_left_size0,upper_left_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[2][2]][otherCore_index3D(0,0,-1+k,lower_right_size0,lower_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][2]][otherCore_index3D(-1+upper_size1,0,-1+k,upper_right_size0,upper_right_size1)]+(arraySectionPointers[coreArrayNeighborhoodLinearized[0][0]][otherCore_index3D(-1+upper_size1,-1+left_size0,-1+k,upper_left_size0,upper_left_size1)]+arraySectionPointers[coreArrayNeighborhoodLinearized[2][0]][otherCore_index3D(0,-1+left_size0,-1+k,lower_left_size0,lower_left_size1)])))))))/8.0; 
		     }
	     }

	   // **************************************************
	   // End of processing the corners of the X and Y axis.
	   // **************************************************
   }                              
}
template void high_level_relax ( MulticoreArray<float> & array, MulticoreArray<float> & old_array );
