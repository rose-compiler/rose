#define DISPLAY 0
#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#include<assert.h>

// Support for timing
#include <iostream>
#include <time.h>

// Local support for a timer better than clock().
#include "highresTimer.h"

using namespace std;
typedef double REAL;

void display(int const* arraySize, REAL*** globalArray);

int main()
{
  const int global_arraySizeX = 1024;
  const int global_arraySizeY = 1024;
  const int global_arraySizeZ = 1024;
  const int arraySize[3] = {global_arraySizeX,global_arraySizeY,global_arraySizeZ};

// allocate arrays
  REAL ***globalArray;
  REAL ***newArray;
  globalArray = (REAL***)malloc(arraySize[2] * sizeof(REAL **));
  newArray = (REAL***)malloc(arraySize[2] * sizeof(REAL **));
 
// sequneceFill
  REAL factor = 120.;
//  int count = 1;
#pragma omp parallel for
  for(int k=0; k < arraySize[2]; ++k)
  {
  globalArray[k] = (REAL**)malloc(arraySize[1] * sizeof(REAL*));
  newArray[k] = (REAL**)malloc(arraySize[1] * sizeof(REAL*));
  for(int j=0; j < arraySize[1]; ++j)
  {
  globalArray[k][j] = (REAL*)malloc(arraySize[0] * sizeof(REAL));
  newArray[k][j] = (REAL*)malloc(arraySize[0] * sizeof(REAL));
  for(int i=0; i < arraySize[0]; ++i)
  {
    globalArray[k][j][i] = factor * (i + j * arraySize[0]);
  }
  }
  }

  REAL boundaryValue = 0.;
// initializeBoundary
     if (arraySize[2] > 2)
        {
       // This is is a 3D array.
#if 0
          printf ("==== 3D: Z axis size = %2d \n",arraySize[2]);
#endif
          if (arraySize[1] > 2)
             {
               if (arraySize[0] > 2)
                  {
                    for (int j = 0; j < arraySize[1]; j++)
                       {
                         for (int i = 0; i < arraySize[0]; i++)
                            {
#if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
                              globalArray[0][j][i]              = boundaryValue;
                              globalArray[arraySize[2]-1][j][i] = boundaryValue;
#else
                              globalArray[0][j][i]              = -globalArray[0][j][i];
                              globalArray[arraySize[2]-1][j][i] = -globalArray[arraySize[2]-1][j][i];
#endif
                            }
                       }

                    for (int i = 0; i < arraySize[0]; i++)
                       {
                         for (int k = 0; k < arraySize[2]; k++)
                            {
#if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
                              globalArray[k][0][i]              = boundaryValue;
                              globalArray[k][arraySize[1]-1][i]= boundaryValue;
#else
                              globalArray[k][0][i]            = -globalArray[k][0][i];
                              globalArrayi[k][arraySize[1]-1][i] = -globalArrayi[k][arraySize[1]-1][i];
#endif
                            }
                       }

                    for (int k = 0; k < arraySize[2]; k++)
                       {
                         for (int j = 0; j < arraySize[1]; j++)
                            {
#if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
                              globalArray[k][j][0]              = boundaryValue;
                              globalArray[k][j][arraySize[0]-1] = boundaryValue;
#else
                              globalArray[k][j][0]            = -globalArray[k][j][0];
                              globalArray[k][j][arraySize[0]-1] = -globalArray[k][j][arraySize[0]-1];
#endif
                            }
                       }
                  }
                 else
                  {
#if 0
                    printf ("Axis X has no interior (there is no boundary). \n");
#endif
                 // assert(false);
                  }
             }
            else
             {
#if 0
               printf ("Axis Y has no interior (there is no boundary). \n");
#endif
            // assert(false);
             }
	     }
	    else
	     {
     #if 0
	       printf ("Axis Z has no interior (this is a 1D or 2D array). \n");
     #endif
	       if (arraySize[1] >= 2)
		  {
		    if (arraySize[0] >= 2)
		       {
			 for (int i = 0; i < arraySize[0]; i++)
			    {
     #if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
			      globalArray[0][0][i]              = boundaryValue;
			      globalArray[0][arraySize[1]-1][i] = boundaryValue;
     #else
			      globalArray[0][0][i]              = -globalArray[0][0][i];
			      globalArray[0][arraySize[1]-1][i] = -globalArray[0][arraySize[1]-1][i];
     #endif
			    }

			 for (int j = 0; j < arraySize[1]; j++)
			    {
     #if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
			      globalArray[0][j][0]              = boundaryValue;
			      globalArray[0][j][arraySize[0]-1] = boundaryValue;
     #else
			      globalArray[0][j][0]              = -globalArra[0][j][0];
			      globalArray[0][j][arraySize[0]-1] = -globalArray[0][j][arraySize[0]-1];
     #endif
			    }
		       }
		      else
		       {
     #if 0
			 printf ("Axis X has no interior (there is no boundary). \n");
     #endif
		      // assert(false);
		       }
		  }
		 else
		  {
     #if 0
		    printf ("Axis Y has no interior (this is a 1D array). \n");
     #endif
     #if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
		    globalArray[0][0][0]              = boundaryValue;
		    globalArray[0][0][arraySize[0]-1] = boundaryValue;
     #else
		    globalArray[0][0][0]              = -globalArray[0][0][0];
		    globalArray[0][0][arraySize[0]-1] = -globalArray[0][0][arraySize[0]-1];
     #endif
		  }
	     }

// display
#if DISPLAY
     printf ("===== After initialization ===== size(%2d,%2d,%2d)\n",arraySize[0],arraySize[1],arraySize[2]);
     for (int k = 0; k < arraySize[2]; k++)
        {
          if (arraySize[2] > 1)
               printf ("==== 3D: k = %2d \n",k);

          for (int j = 0; j < arraySize[1]; j++)
             {
               if (arraySize[2] > 1)
                  {
                    if (arraySize[1] > 1)
                         printf ("==== 3D: j = %2d: ",j);
                  }
                 else
                  {
                    if (arraySize[1] > 1)
                         printf ("==== 2D: j = %2d: ",j);
                  }

               for (int i = 0; i < arraySize[0]; i++)
                  {
                 // printf ("%1.2e ",operator()(i));
                    printf ("%5.2f ",globalArray[k][j][i]);
                  }
               printf ("\n");
             }
          printf ("\n");
        }
     printf ("\n");
#endif 

// 2D relaxation 
     printf ("Call relaxation \n");

#ifndef __APPLE__
  // Setup timer
     timespec time1, time2;
     clock_gettime(CLOCK_REALTIME, &time1);
#endif

int numberOfIterations = 10;
  for (int i = 0; i < numberOfIterations; i++)
     {
#pragma omp parallel for
     for(int k=1; k < arraySize[2]-1; ++k)
     for(int j=1; j < arraySize[1]-1; ++j)
     for(int i=1; i < arraySize[0]-1; ++i)
     {
       newArray[k][j][i] = (globalArray[k][j][i-1] + globalArray[k][j][i+1] + globalArray[k][j-1][i] + globalArray[k][j+1][i] + globalArray[k-1][j][i] + globalArray[k+1][j][i]) /6.0;
     }
     }
  
#ifndef __APPLE__
  // Get and report the time.
     clock_gettime(CLOCK_REALTIME, &time2);
     cout << diff(time1,time2).tv_sec << ":" << diff(time1,time2).tv_nsec << endl;
#endif

// display
#if DISPLAY
     printf ("===== After 2D relaxation ===== size(%2d,%2d,%2d)\n",arraySize[0],arraySize[1],arraySize[2]);
     for (int k = 0; k < arraySize[2]; k++)
        {
          if (arraySize[2] > 1)
               printf ("==== 3D: k = %2d \n",k);

          for (int j = 0; j < arraySize[1]; j++)
             {
               if (arraySize[2] > 1)
                  {
                    if (arraySize[1] > 1)
                         printf ("==== 3D: j = %2d: ",j);
                  }
                 else
                  {
                    if (arraySize[1] > 1)
                         printf ("==== 2D: j = %2d: ",j);
                  }

               for (int i = 0; i < arraySize[0]; i++)
                  {
                 // printf ("%1.2e ",operator()(i));
                    printf ("%5.2f ",newArray[k][j][i]);
                  }
               printf ("\n");
             }
          printf ("\n");
        }
     printf ("\n");
#endif

    for(int i=0;i<arraySize[2];i++)
    {
    	for(int j=0;j<arraySize[1];j++)
    	{
    		free(globalArray[i][j]);
    		free(newArray[i][j]);
    	}
    	free(globalArray[i]);
    	free(newArray[i]);
    }
    free(globalArray);
    free(newArray);
}
