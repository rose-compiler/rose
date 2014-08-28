#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;
#include "./core.h"
#include "./multicore.h"
template <typename T>
void relax2D_now ( MulticoreArray<T> & array, MulticoreArray<T> &old_array )
{
const int arraySizeX = array.get_arraySize(0);
const int arraySizeY = array.get_arraySize(1);
#pragma 
#define local_index2D(i,j,arraySizeX) (i)+(j)*arraySizeX

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
         

         for (int  j=1; j<local_size1-1; j++) 
           for (int  i=1; i<local_size0-1; i++) 
             old_arraySection[local_index2D(i,j,local_size0)] = (arraySection[local_index2D(i,1+j,local_size0)]+(arraySection[local_index2D(i,-1+j,local_size0)]+(arraySection[local_index2D(-1+i,j,local_size0)]+arraySection[local_index2D(1+i,j,local_size0)])))/4.0;

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
               for (int  j=1; j<-1+local_size1; j++) 
                 old_arraySection[local_index2D(0,j,local_size0)] = (arraySection[local_index2D(0,1+j,local_size0)]+(arraySection[local_index2D(0,-1+j,local_size0)]+(arraySection[local_index2D(-1,j,local_size0)]+arraySection[local_index2D(1,j,local_size0)])))/4.0; 
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
               for (int  j=1; j<-1+local_size1; j++) 
                 old_arraySection[local_index2D(-1+local_size0,j,local_size0)] = (arraySection[local_index2D(-1+local_size0,1+j,local_size0)]+(arraySection[local_index2D(-1+local_size0,-1+j,local_size0)]+(arraySection[local_index2D(-2+local_size0,j,local_size0)]+arraySection[local_index2D(local_size0,j,local_size0)])))/4.0; 
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
               for (int  j=1; j<-1+local_size1; j++) 
                 old_arraySection[local_index2D(0,j,local_size0)] = (arraySection[local_index2D(0,1+j,local_size0)]+(arraySection[local_index2D(0,-1+j,local_size0)]+(arraySection[local_index2D(-1,j,local_size0)]+arraySection[local_index2D(1,j,local_size0)])))/4.0;
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
               for (int  i=1; i<-1+local_size0; i++) 
                 old_arraySection[local_index2D(i,0,local_size0)] = (arraySection[local_index2D(i,1,local_size0)]+(arraySection[local_index2D(i,-1,local_size0)]+(arraySection[local_index2D(-1+i,0,local_size0)]+arraySection[local_index2D(1+i,0,local_size0)])))/4.0; 
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
               for (int  i=1; i<-1+local_size0; i++) 
                 old_arraySection[local_index2D(i,-1+local_size1,local_size0)] = (arraySection[local_index2D(i,local_size1,local_size0)]+(arraySection[local_index2D(i,-2+local_size1,local_size0)]+(arraySection[local_index2D(-1+i,-1+local_size1,local_size0)]+arraySection[local_index2D(1+i,-1+local_size1,local_size0)])))/4.0; 
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
               for (int  i=1; i<-1+local_size0; i++) 
                 old_arraySection[local_index2D(i,0,local_size0)] = (arraySection[local_index2D(i,1,local_size0)]+(arraySection[local_index2D(i,-1,local_size0)]+(arraySection[local_index2D(-1+i,0,local_size0)]+arraySection[local_index2D(1+i,0,local_size0)])))/4.0;
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
                            old_arraySection[local_index2D(0,0,local_size0)] = (arraySection[local_index2D(0,1,local_size0)]+(arraySection[local_index2D(0,-1,local_size0)]+(arraySection[local_index2D(-1,0,local_size0)]+arraySection[local_index2D(1,0,local_size0)])))/4.0; 
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
                            old_arraySection[local_index2D(0,-1+local_size1,local_size0)] = (arraySection[local_index2D(0,local_size1,local_size0)]+(arraySection[local_index2D(0,-2+local_size1,local_size0)]+(arraySection[local_index2D(-1,-1+local_size1,local_size0)]+arraySection[local_index2D(1,-1+local_size1,local_size0)])))/4.0; 
                          }
                      else if (local_size1 == 1)
                         // Case of upper and lower corners are the same point
                         // ***** | ****** | *****
                         // ----------------------
                         // ***** | X***** | *****
                         // ----------------------
                         // ***** | ****** | *****
                         old_arraySection[local_index2D(0,0,local_size0)] = (arraySection[local_index2D(0,1,local_size0)]+(arraySection[local_index2D(0,-1,local_size0)]+(arraySection[local_index2D(-1,0,local_size0)]+arraySection[local_index2D(1,0,local_size0)])))/4.0; 
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
                            old_arraySection[local_index2D(-1+local_size0,0,local_size0)] = (arraySection[local_index2D(-1+local_size0,1,local_size0)]+(arraySection[local_index2D(-1+local_size0,-1,local_size0)]+(arraySection[local_index2D(-2+local_size0,0,local_size0)]+arraySection[local_index2D(local_size0,0,local_size0)])))/4.0; 
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
                            old_arraySection[local_index2D(-1+local_size0,-1+local_size1,local_size0)] = (arraySection[local_index2D(-1+local_size0,local_size1,local_size0)]+(arraySection[local_index2D(-1+local_size0,-2+local_size1,local_size0)]+(arraySection[local_index2D(-2+local_size0,-1+local_size1,local_size0)]+arraySection[local_index2D(local_size0,-1+local_size1,local_size0)])))/4.0; 
                          }
                      else if (local_size1 == 1)
                         // Case of upper and lower corners are the same point
                         // ***** | ****** | *****
                         // ----------------------
                         // ***** | X***** | *****
                         // ----------------------
                         // ***** | ****** | *****
                         old_arraySection[local_index2D(-1+local_size0,0,local_size0)] = (arraySection[local_index2D(-1+local_size0,1,local_size0)]+(arraySection[local_index2D(-1+local_size0,-1,local_size0)]+(arraySection[local_index2D(-2+local_size0,0,local_size0)]+arraySection[local_index2D(local_size0,0,local_size0)])))/4.0; 
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
                          old_arraySection[local_index2D(0,-1+local_size0,local_size0)] = (arraySection[local_index2D(0,local_size0,local_size0)]+(arraySection[local_index2D(0,-2+local_size0,local_size0)]+(arraySection[local_index2D(-1,-1+local_size0,local_size0)]+arraySection[local_index2D(1,-1+local_size0,local_size0)])))/4.0; 
		     }
	     }

	   // **************************************************
	   // End of processing the corners of the X and Y axis.
	   // **************************************************
   }                              
}
