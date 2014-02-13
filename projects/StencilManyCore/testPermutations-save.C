// Example relaxation code to experiment with TMR transformations
// Things to do in this code:
// 1) Simplify the algorithm-based distribution.
// 2) Add support for ghost boundary halos (arbritrary width).
// 3) Hierarchy of use (nesting of MulticoreArray)
// 4) Dynamic data
// 5) Better use of OpenMP (specify parallel region "#pragma parallel")
// Use "ManyCoreArrayRuntimeSupport" as the name of this class/project...
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
#pragma 
#define local_index3D(i,j,k,arraySizeX,arraySizeY) (i)+((j)*arraySizeX+(k)*(arraySizeY*arraySizeX))

    int numberOfCores = old_array.get_numberOfCores();
    T** arraySectionPointers = old_array.get_arraySectionPointers();
    T** old_arraySectionPointers = array.get_arraySectionPointers();

    for (int core = 0; core < numberOfCores; core++)
       {
         T* arraySection     = arraySectionPointers[core];
         T* old_arraySection = old_arraySectionPointers[core];

         Core<T>* cur_core =  old_array.get_coreArray()[core];
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
             for (int  i=1; i<local_size2-1; i++) 
               old_arraySection[local_index3D(i,j,k,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1+j,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1+j,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1+j,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,-1+k,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1+j,-1+k,local_size0,local_size1)])))))))/8.0;

         // ***************************************
         // Now process the edges 
         // ***************************************
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
                 for (int  i=1; i<-1+local_size2; i++) 
                   old_arraySection[local_index3D(i,0,k,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,-1+k,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1,-1+k,local_size0,local_size1)])))))))/8.0; 
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
                 for (int  i=1; i<-1+local_size2; i++) 
                   old_arraySection[local_index3D(i,-1+local_size1,k,local_size0,local_size1)] = (arraySection[local_index3D(1+i,local_size1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,local_size1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-2+local_size1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,local_size1,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,local_size1,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size1,-1+k,local_size0,local_size1)]+arraySection[local_index3D(1+i,-2+local_size1,-1+k,local_size0,local_size1)])))))))/8.0; 
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
                 for (int  i=1; i<-1+local_size2; i++) 
                   old_arraySection[local_index3D(i,0,k,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,1+k,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,-1+k,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,-1+k,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1,-1+k,local_size0,local_size1)])))))))/8.0;
            }
         if (local_size2 > 1)
           {
             if (left_size2>0 && 
                 bounaryCore[0][0] == false)
             { 
               // ***** | ****** | *****
               // ----------------------
               // ***** | *XXXX* | *****
               // ***** | ****** | *****
               // ***** | ****** | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  j=1; j<-1+local_size1; j++) 
                 for (int  i=1; i<-1+local_size2; i++) 
                   old_arraySection[local_index3D(i,j,0,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1+j,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1+j,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1+j,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,-1,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1+j,-1,local_size0,local_size1)])))))))/8.0; 
             }
             if (right_size2>0 &&
                 bounaryCore[0][1] == false)
             {
               // ***** | ****** | *****
               // ----------------------
               // ***** | ****** | *****
               // ***** | ****** | *****
               // ***** | *XXXX* | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  j=1; j<-1+local_size1; j++) 
                 for (int  i=1; i<-1+local_size2; i++) 
                   old_arraySection[local_index3D(i,j,-1+local_size2,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1+j,local_size2,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,local_size2,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1+j,local_size2,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,local_size2,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1+j,-2+local_size2,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,-2+local_size2,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,-2+local_size2,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1+j,-2+local_size2,local_size0,local_size1)])))))))/8.0; 
             }
           }
         else if (local_size2 == 1 &&
                  bounaryCore[0][0] == false && 
                  bounaryCore[0][1] == false &&
                  left_size2>0 &&
                  right_size2>0)
            {
               // ***** | ****** | *****
               // ----------------------
               // ***** | *XXXX* | *****
               // ----------------------
               // ***** | ****** | *****
               for (int  j=1; j<-1+local_size1; j++) 
                 for (int  i=1; i<-1+local_size2; i++) 
                   old_arraySection[local_index3D(i,j,0,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1+j,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1+j,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1+j,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1+j,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1+j,-1,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1+j,-1,local_size0,local_size1)])))))))/8.0;
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
                            for (int  i=1; i<-1+local_size2; i++) 
                              old_arraySection[local_index3D(i,0,0,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,-1,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1,-1,local_size0,local_size1)])))))))/8.0; 
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
                            for (int  i=1; i<-1+local_size2; i++) 
                              old_arraySection[local_index3D(i,0,-1+local_size1,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1,local_size1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,local_size1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1,local_size1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,local_size1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1,-2+local_size1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,-2+local_size1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,-2+local_size1,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1,-2+local_size1,local_size0,local_size1)])))))))/8.0; 
                          }
                      else if (local_size1 == 1)
                         // Case of upper and lower corners are the same point
                         // ***** | ****** | *****
                         // ----------------------
                         // ***** | X***** | *****
                         // ----------------------
                         // ***** | ****** | *****
                         for (int  i=1; i<-1+local_size2; i++) 
                           old_arraySection[local_index3D(i,0,0,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,-1,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1,-1,local_size0,local_size1)])))))))/8.0; 
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
                            for (int  i=1; i<-1+local_size2; i++) 
                              old_arraySection[local_index3D(i,-1+local_size0,0,local_size0,local_size1)] = (arraySection[local_index3D(1+i,local_size0,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,local_size0,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-2+local_size0,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size0,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,local_size0,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,local_size0,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size0,-1,local_size0,local_size1)]+arraySection[local_index3D(1+i,-2+local_size0,-1,local_size0,local_size1)])))))))/8.0; 
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
                            for (int  i=1; i<-1+local_size2; i++) 
                              old_arraySection[local_index3D(i,-1+local_size0,-1+local_size1,local_size0,local_size1)] = (arraySection[local_index3D(1+i,local_size0,local_size1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,local_size0,local_size1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-2+local_size0,local_size1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size0,local_size1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,local_size0,-2+local_size1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,local_size0,-2+local_size1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size0,-2+local_size1,local_size0,local_size1)]+arraySection[local_index3D(1+i,-2+local_size0,-2+local_size1,local_size0,local_size1)])))))))/8.0; 
                          }
                      else if (local_size1 == 1)
                         // Case of upper and lower corners are the same point
                         // ***** | ****** | *****
                         // ----------------------
                         // ***** | X***** | *****
                         // ----------------------
                         // ***** | ****** | *****
                         for (int  i=1; i<-1+local_size2; i++) 
                           old_arraySection[local_index3D(i,-1+local_size0,0,local_size0,local_size1)] = (arraySection[local_index3D(1+i,local_size0,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,local_size0,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-2+local_size0,1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size0,1,local_size0,local_size1)]+(arraySection[local_index3D(1+i,local_size0,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,local_size0,-1,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-2+local_size0,-1,local_size0,local_size1)]+arraySection[local_index3D(1+i,-2+local_size0,-1,local_size0,local_size1)])))))))/8.0; 
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
                          for (int  i=1; i<-1+local_size2; i++) 
                            old_arraySection[local_index3D(i,0,-1+local_size0,local_size0,local_size1)] = (arraySection[local_index3D(1+i,1,local_size0,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,local_size0,local_size0,local_size1)]+(arraySection[local_index3D(1+i,-1,local_size0,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,local_size0,local_size0,local_size1)]+(arraySection[local_index3D(1+i,1,-2+local_size0,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,1,-2+local_size0,local_size0,local_size1)]+(arraySection[local_index3D(-1+i,-1,-2+local_size0,local_size0,local_size1)]+arraySection[local_index3D(1+i,-1,-2+local_size0,local_size0,local_size1)])))))))/8.0; 
		     }
	     }

	   // **************************************************
	   // End of processing the corners of the X and Y axis.
	   // **************************************************
   }                              
}
template <typename T>
void
lower_level_relax ( MulticoreArray<T> & array, MulticoreArray<T> & old_array )
{
// The code in this function is what we would want to have ben generated by the compiler.
const int numberOfCores = array.get_numberOfCores();
// Make sure that these are distributed using the same approach (table-based or via an algorithmic approach).
assert(array.get_tableBasedDistribution() == old_array.get_tableBasedDistribution());
// Use OpenMP to support the parallel threads on each core.
#pragma omp parallel for
for (int p = 0; p < numberOfCores; p++)
{
// Refactored form of relaxation on the interior.
//          array.coreArray[p]->relax(p,array,old_array);
// **************************************************************
// Fixup internal bounaries of the memory allocated to each core.
// **************************************************************
// Refactored form of relaxation on the interior.
array.coreArray[p]->relax_on_boundary(p,array,old_array);
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
#if 1
// Tests for array abstraction constructor an initialization.
int maxArraySizePerAxis = 8;
for (int size_Z = 1; size_Z <= maxArraySizePerAxis; size_Z++)
for (int size_Y = 1; size_Y <= maxArraySizePerAxis; size_Y++)
for (int size_X = 1; size_X <= maxArraySizePerAxis; size_X++)
{
arraySize[0] = size_X;
arraySize[1] = size_Y;
arraySize[2] = size_Z;
int numberOfCoresPerAxis = 5;
printf ("***** Testing MulticoreArray construction using size_X = %2d size_Y = %2d size_Z = %2d numberOfCoresPerAxis = %d \n",size_X,size_Y,size_Z,numberOfCoresPerAxis);
for (int numberOfCores_Z = 1; numberOfCores_Z <= numberOfCoresPerAxis; numberOfCores_Z++)
for (int numberOfCores_Y = 1; numberOfCores_Y <= numberOfCoresPerAxis; numberOfCores_Y++)
for (int numberOfCores_X = 2; numberOfCores_X <= numberOfCoresPerAxis; numberOfCores_X++)
{
numberOfCoresArraySize[0] = numberOfCores_X;
numberOfCoresArraySize[1] = numberOfCores_Y;
numberOfCoresArraySize[2] = numberOfCores_Z;
#if 0
printf ("\n***** Testing MulticoreArray construction using size_X = %2d size_Y = %2d size_Z = %2d and numberOfCores_X = %2d numberOfCores_Y = %2d numberOfCores_Z = %2d \n",size_X,size_Y,size_Z,numberOfCores_X,numberOfCores_Y,numberOfCores_Z);
#endif
MulticoreArray<float> tmp_array1    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
MulticoreArray<float> old_tmp_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
// MulticoreArray<float> tmp_array1    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,true);
// MulticoreArray<float> old_tmp_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,true);
#if 1
// Test initialization
for (int k = 0; k < size_Z; k++)
for (int j = 0; j < size_Y; j++)
for (int i = 0; i < size_X; i++)
{
#if 0
printf ("\n\n*******************************************************************\n");
printf ("***** Indexing for tmp_array1(i=%2d,j=%2d,k=%2d) order (X,Y,Z) *****\n",i,j,k);
#endif
// Inner most (left most) index is the fastest moving index (as in Fortran).
tmp_array1(i,j,k) = -1.0;
}
#endif
#if 0
if (numberOfCores_X == 1 && numberOfCores_Y == 1 && numberOfCores_Z == 1)
{
tmp_array1.display("result: tmp_array1");
}
#else
tmp_array1.initializeDataWithCoreArrayIndex();
// Initialize the array boundary to a value.
tmp_array1.initializeBoundary(-1);
#if 0
tmp_array1.display("result: tmp_array1");
old_tmp_array1.display("result: old_tmp_array1");
#endif
#endif
#if 1
MulticoreArray<float> tmp_array2    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,true);
MulticoreArray<float> old_tmp_array2(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,true);
// Initialize array
#if 0
#if 0
tmp_array1 = 1.0;
tmp_array2 = 1.0;
#else
tmp_array1.sequenceFill(100.0);
tmp_array2.sequenceFill(100.0);
#endif
#endif
#if 1
// Comment out so we can generate different numbers for each index (debugging).
// Initialize the boundary
// Test initialization
for (int k = 0; k < size_Z; k++)
for (int j = 0; j < size_Y; j++)
for (int i = 0; i < size_X; i++)
{
#if 0
printf ("\n\n*******************************************************************\n");
printf ("***** Indexing for tmp_array2(i=%2d,j=%2d,k=%2d) order (X,Y,Z) *****\n",i,j,k);
#endif
// Inner most (left most) index is the fastest moving index (as in Fortran).
tmp_array2(i,j,k) = -1.0;
}
#endif
tmp_array2.initializeDataWithCoreArrayIndex();
// Initialize the array boundary to a value.
tmp_array2.initializeBoundary(-1);
#if 0
tmp_array2.display("result: tmp_array2");
#endif
#if 0
printf ("Testing relaxation using size_X = %2d size_Y = %2d size_Z = %2d and numberOfCores_X = %2d numberOfCores_Y = %2d numberOfCores_Z = %2d \n",arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2]);
#endif
#if 1
#if 0
// This is the higher level implementation attractive for users (or the even high level array notation, but not supported with this abstraction).
high_level_relax(tmp_array1,old_tmp_array1);
high_level_relax(tmp_array2,old_tmp_array2);
#else
lower_level_relax(tmp_array1,old_tmp_array1);
lower_level_relax(tmp_array2,old_tmp_array2);
#endif
#endif
#if 0
// Output the result
tmp_array1.display("result: tmp_array1");
tmp_array2.display("result: tmp_array2");
#endif
// Make sure that the results are the same.
assert(tmp_array1 == tmp_array2);
#endif
#if 0
// This does not yet pass all of the different permutation tests...
// Test refactored version of operator.
relax2D(tmp_array1,old_tmp_array1);
#endif
#if 0
// Test refactored version of operator.
tmp_array1 = 0.0;
relax2D_midlevel(tmp_array1,old_tmp_array1);
#endif
#if 0
// Test refactored version of operator.
tmp_array1 = 0.0;
relax2D_highlevel(tmp_array1,old_tmp_array1);
#endif
}
}
#endif
}
