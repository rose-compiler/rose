#include <stdio.h>
#include <stdlib.h>

// #include<vector>
#include<assert.h>
#include<math.h>

// Details on hypercube sides (faces, edges, points) are at: http://en.wikipedia.org/wiki/Hypercube

// This code computes some properties of hypercubes:
//   1) What are the number of sides (broke down by dimension).  This is important
//      in the organization of stencil operation on the hyper cube sides when the 
//      global hypercube is partitioned across cores (or processors).
//   2) What is the size (volumes) of the ghost boundaries required to support hypercubes
//   3) What is the ratio of interior volume to ghost boundary volume for different ghost 
//      boundary widths on different dimension hypercubes on different core array topologies.

int 
factorial( int n )
   {
  // This function is not available in the math library, so we have to generate it 
  // explicitly (here as a recursive function).
  // factorial as a function...note that (0! == 1).

     int returnValue = 0;

     if ( n <= 1 )
          returnValue = 1;
       else
          returnValue = n * factorial(n-1);

     return returnValue;
   }

int
numberOfSides( int d, int m )
   {
  // This computed the number of lower dimensional sides of a hypercube of dimension d.
  // E_{m,n} = 2^{n-m}{n \choose m} , where {n \choose m}=\frac{n!}{m!\,(n-m)!} and n! denotes the factorial of n.

     assert(d >= m);

     int numberOf_D_minus_M_sides = pow(2.0,d-m) * factorial(d) / (factorial(m) * factorial(d-m));

  // printf ("numberOf_D_minus_M_sides = %d \n",numberOf_D_minus_M_sides);

     return numberOf_D_minus_M_sides;
   }

const int maxDimension = 7;

                          // Dimention     Dimension of Side 
int numberOfHypercubeSides[maxDimension+1] [maxDimension+1];
int volumeOfHypercubeSides[maxDimension+1] [maxDimension+1];

int ghostBoundaryVolumeOfHypercubeSides[maxDimension+1] [maxDimension+1];

int main()
   {
  // Assume 0.1 byte per flop; assume 1 TeraFLOP processor; so 100 Gig per core

  // Assume the partitioning of the data across cores matches the dimensionality of the data.

  // Assume array size per processor of 100^3 (globally on a million processors, this is 100 times this per axis for 3D; or 10000^3)
     int N = 100;

  // Assume core array of 10^3 (1000 cores)
     int P = 10;

  // Ghost boundary widths are driven by the stencil width (2nd order stencil 
  // requires ghost bounary width of 1, 4th order stencil requires width 2, etc.)
  // However, ghost boundaries might only be required for spatial dimensions, 
  // so results for hypercubes beyond D=3 may not be realistic).
     int maxGhostBoundaryWidth = 2;

     for (int dimension = 0; dimension <= maxDimension; dimension++)
        {
       // Compute the number of sides of each lower order dimension, and associated volumes.

          float ratio                        = 0.0;
          int interiorVolume                 = 0;
          int ghostBoundaryVolumeAccumulated = 0;

          for (int lowerDimenionalSide = dimension; lowerDimenionalSide >= 0; lowerDimenionalSide--)
             {
               int numberOfLowerDimenionalSide = numberOfSides( dimension, lowerDimenionalSide );

               numberOfHypercubeSides[dimension][lowerDimenionalSide] = numberOfLowerDimenionalSide;

               int volume = numberOfLowerDimenionalSide*(int)pow(N/P,lowerDimenionalSide);
               for (int ghostBoundaryWidth = 1; ghostBoundaryWidth <= maxGhostBoundaryWidth; ghostBoundaryWidth++)
                  {
                    int ghostBoundaryVolume = 0;
                    if (lowerDimenionalSide == dimension)
                         interiorVolume = volume;
                      else
                         ghostBoundaryVolume = numberOfLowerDimenionalSide*(int)pow((N/P)+(ghostBoundaryWidth*2),lowerDimenionalSide);

                    ghostBoundaryVolumeAccumulated += ghostBoundaryVolume;

                 // printf ("--- Ghost boundary volume for ghostBoundaryWidth = %d ghostBoundaryVolume = %d ghostBoundaryVolumeAccumulated = %d \n",ghostBoundaryWidth,ghostBoundaryVolume,ghostBoundaryVolumeAccumulated);
                  }

               volumeOfHypercubeSides[dimension][lowerDimenionalSide] = volume;

               ghostBoundaryVolumeOfHypercubeSides[dimension][lowerDimenionalSide] = ghostBoundaryVolumeAccumulated;

               if (lowerDimenionalSide < dimension)
                  {
                 // ratio = ((float)interiorVolume/(float)ghostBoundaryVolumeAccumulated);
                    ratio = ((float)ghostBoundaryVolumeAccumulated/(float)interiorVolume);
                  }

            // printf ("For %dD Hypercube: number of %dD side = %4d %s volume = %10d ghostBoundaryVolumeAccumulated = %10d ratio of interior/ghostBounary volume = %f \n",
            //      dimension,lowerDimenionalSide,numberOfLowerDimenionalSide,lowerDimenionalSide == dimension ? "(interior)" : "(sides)   ",
            //         volume,ghostBoundaryVolumeAccumulated,ratio);
               printf ("For %dD Hypercube: number of %dD side = %4d %s volume = %10d ghostBoundaryVolumeAccumulated = %10d \n",
                    dimension,lowerDimenionalSide,numberOfLowerDimenionalSide,lowerDimenionalSide == dimension ? "(interior)" : "(sides)   ",
                       volume,ghostBoundaryVolumeAccumulated);
             }

          printf ("For %dD Hypercube: N (array size per axis) = %d P (core array size per axis) = %d GhostBoundaryWidth = %d interior volume = %10d ghostBoundaryVolume = %10d ratio of (ghostBounary volume)/(interior volume) = %f \n",
                  dimension,N,P,maxGhostBoundaryWidth,interiorVolume,ghostBoundaryVolumeAccumulated,ratio);

          printf ("\n");
        }

     return 0;
   }
