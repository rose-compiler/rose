#include "multicore.h"
#define index1D(i)     (i)
#define index2D(i,j)   (((j)*(core->coreArrayNeighborhoodSizes_2D[1][1][0]))+(i))

// I think this should be in terms of the size for X and Y, not X, Y, and Z!
// #define index3D(i,j,k) (((k)*core->coreArrayNeighborhoodSizes_3D[1][1][1][2]*core->coreArrayNeighborhoodSizes_3D[1][1][1][1])+((j)*core->coreArrayNeighborhoodSizes_3D[1][1][1][0])+(i))
#define index3D(i,j,k) (((k)*(core->coreArrayNeighborhoodSizes_3D[1][1][1][0])*(core->coreArrayNeighborhoodSizes_3D[1][1][1][1]))+((j)*(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]))+(i))

// And we need another macro for the general case where the memory segment is a different size than coreArrayNeighborhoodSizes_2D[1][1][0] in the X (and Y axis)
// since the lenght of the data in each axis can be different along the same axis of the core array.  These macros take the length of the array in the requires
// axis to properly referne the element on the associated "other core".
#define otherCore_index2D(i,j,sizeX)   (((j)*sizeX)+(i))
#define otherCore_index3D(i,j,k,sizeX,sizeY) (((k)*sizeX*sizeY)+((j)*sizeX)+(i))
template <typename T>
void
relax2D( MulticoreArray<T> & array,  MulticoreArray<T> & old_array )
   {
  // This is a working example of the relaxation associated with the a stencil on the array abstraction
  // mapped to the separate multi-dimensional memorys allocated per core and onto a multi-dimenional 
  // array of cores (core array).

     int numberOfCores = array.get_numberOfCores();

// Macro to support linearization of multi-dimensional 2D array index computation
#define local_index2D(i,j) (((j)*sizeX)+(i))

  // Use OpenMP to support the threading...
#pragma omp parallel for
     for (int core = 0; core < numberOfCores; core++)
        {
       // This lifts out loop invariant portions of the code.
          T* arraySection     = array.get_arraySectionPointers()[core];
          T* old_arraySection = old_array.get_arraySectionPointers()[core];

       // Lift out loop invariant local array size values.
          int sizeX = array.get_coreArray()[core]->coreArrayNeighborhoodSizes_2D[1][1][0];
          int sizeY = array.get_coreArray()[core]->coreArrayNeighborhoodSizes_2D[1][1][1];

          for (int j = 1; j < sizeY-1; j++)
             {
               for (int i = 1; i < sizeX-1; i++)
                  {
                 // This is the dominant computation for each array section per core. The compiler
                 // will use the user's code to derive the code that will be put here.
                    arraySection[local_index2D(i,j)] = 
                         (old_arraySection[local_index2D(i-1,j)] + old_arraySection[local_index2D(i+1,j)] + 
                          old_arraySection[local_index2D(i,j-1)] + old_arraySection[local_index2D(i,j+1)]) / 4.0;
                  }
             }

       // We could alternatively generate the call for relaxation for the internal 
       // boundaries in the same loop (reduces syncronization).
          array.get_coreArray()[core]->relax_on_boundary(core,array,old_array);
        }

  // Relax on the edges of the array sections on each core (use the alternative approach).
  // relax2D_on_boundary(array,old_array);

// undefine the local 2D index support macro
#undef local_index2D
   }


template <typename T>
void
relax2D_on_boundary( MulticoreArray<T> & array,  MulticoreArray<T> & old_array )
   {
  // This function supports the relaxation operator on the internal boundaries 
  // of the different arrays allocated on a per core basis.  We take advantage 
  // of shared memory to support the stencil operations.

     int numberOfCores = array.get_numberOfCores();

#pragma omp parallel for
     for (int core = 0; core < numberOfCores; core++)
        {
          array.get_coreArray()[core]->relax_on_boundary(core,array,old_array);
        }
   }



template <typename T>
void
relax3D_midlevel( MulticoreArray<T> & array,  MulticoreArray<T> & old_array )
   {
  // This is a working example of the relaxation associated with the a stencil on the array abstraction
  // mapped to the separate multi-dimensional memorys allocated per core and onto a multi-dimenional 
  // array of cores (core array).

  // Note: As an alternative to the specialized side handling for internal boundary updates 
  // consider a loop over all of the array while skipping the interior regions for better
  // performance.  This could use the general technique demonstrated above for general
  // internal core edge updates without any significant loss in parformance (maybe).
  // This might permit more general internal application of the stencil operator to the
  // edges of array sections on each core. Such code might be more easily generated then
  // the more complex form of edge code in the much larger functions (below).

     int numberOfCores_X = array.get_coreArraySize(0);
     int numberOfCores_Y = array.get_coreArraySize(1);
     int numberOfCores_Z = array.get_coreArraySize(2);


  // Use OpenMP to support the threading...
#pragma omp parallel for
     for (int core_X = 0; core_X < numberOfCores_X; core_X++)
        {
//#pragma omp for
          for (int core_Y = 0; core_Y < numberOfCores_Y; core_Y++)
             {
          for (int core_Z = 0; core_Z < numberOfCores_Z; core_Z++)
             {
            // This lifts out loop invariant portions of the code.
               Core<T> & coreMemory = array.getCore(core_X,core_Y,core_Z);

            // Lift out loop invariant local array size values.
               int sizeX = array.getCore(core_X,core_Y,core_Z).coreArrayNeighborhoodSizes_3D[1][1][1][0];
               int sizeY = array.getCore(core_X,core_Y,core_Z).coreArrayNeighborhoodSizes_3D[1][1][1][1];
               int sizeZ = array.getCore(core_X,core_Y,core_Z).coreArrayNeighborhoodSizes_3D[1][1][1][2];
#if 0
               printf ("\nsizeX = %d sizeY = %d sizeZ = %d \n",sizeX,sizeY,sizeZ);
#endif
               int base_X  = (coreMemory.boundaryCore_3D[0][0] == true) ? 1 : 0;
               int bound_X = (coreMemory.boundaryCore_3D[0][1] == true) ? sizeX - 2: sizeX - 1;
               int base_Y  = (coreMemory.boundaryCore_3D[1][0] == true) ? 1 : 0;
               int bound_Y = (coreMemory.boundaryCore_3D[1][1] == true) ? sizeY - 2: sizeY - 1;
               int base_Z  = (coreMemory.boundaryCore_3D[2][0] == true) ? 1 : 0;
               int bound_Z = (coreMemory.boundaryCore_3D[2][1] == true) ? sizeZ - 2: sizeZ - 1;
#if 0
               printf ("core_X = %d core_Y = %d core_Z = %d base_X = %d bound_X = %d base_Y = %d bound_Y = %d base_Z = %d bound_Z = %d\n",core_X,core_Y,core_Z,base_X,bound_X,base_Y,bound_Y,base_Z, bound_Z);
#endif
               for (int k = base_Z; k <= bound_Z; k++)
                  {
               for (int j = base_Y; j <= bound_Y; j++)
                  {
                    for (int i = base_X; i <= bound_X; i++)
                       {
#if 0
                         printf ("\ncore_X = %d core_Y = %d i = %d j = %d \n",core_X,core_Y,i,j);
                         printf ("    array.getCore(core_X,core_Y,core_Z)(i,j,k)   = %f \n",array.getCore(core_X,core_Y,core_Z)(i,j,k));
                         printf ("old_array.getCore(core_X,core_Y,core_Z)(i-1,j,k) = %f \n",old_array.getCore(core_X,core_Y,core_Z)(i-1,j,k));
                         printf ("old_array.getCore(core_X,core_Y,core_Z)(i+1,j,k) = %f \n",old_array.getCore(core_X,core_Y,core_Z)(i+1,j,k));
                         printf ("old_array.getCore(core_X,core_Y,core_Z)(i,j-1,k) = %f \n",old_array.getCore(core_X,core_Y,core_Z)(i,j-1,k));
                         printf ("old_array.getCore(core_X,core_Y,core_Z)(i,j+1,k) = %f \n",old_array.getCore(core_X,core_Y,core_Z)(i,j+1,k));
                         printf ("old_array.getCore(core_X,core_Y,core_Z)(i,j,k-1) = %f \n",old_array.getCore(core_X,core_Y,core_Z)(i,j-1,k-1));
                         printf ("old_array.getCore(core_X,core_Y,core_Z)(i,j,k+1) = %f \n",old_array.getCore(core_X,core_Y,core_Z)(i,j+1,k+1));
#endif
                         array.getCore(core_X,core_Y,core_Z)(i,j,k) = 
                            ( old_array.getCore(core_X,core_Y,core_Z)(i-1,j,k) +
                              old_array.getCore(core_X,core_Y,core_Z)(i+1,j,k) +
                              old_array.getCore(core_X,core_Y,core_Z)(i,j-1,k) +
                              old_array.getCore(core_X,core_Y,core_Z)(i,j+1,k) +
                              old_array.getCore(core_X,core_Y,core_Z)(i,j,k-1) +
                              old_array.getCore(core_X,core_Y,core_Z)(i,j,k+1)) / 6.0;
                       }
                  }
                  }
             }
             }
        }
   }



template <typename T>
void
relax2D_midlevel( MulticoreArray<T> & array,  MulticoreArray<T> & old_array )
   {
  // This is a working example of the relaxation associated with the a stencil on the array abstraction
  // mapped to the separate multi-dimensional memorys allocated per core and onto a multi-dimenional 
  // array of cores (core array).

  // Note: As an alternative to the specialized side handling for internal boundary updates 
  // consider a loop over all of the array while skipping the interior regions for better
  // performance.  This could use the general technique demonstrated above for general
  // internal core edge updates without any significant loss in parformance (maybe).
  // This might permit more general internal application of the stencil operator to the
  // edges of array sections on each core. Such code might be more easily generated then
  // the more complex form of edge code in the much larger functions (below).

     int numberOfCores_X = array.get_coreArraySize(0);
     int numberOfCores_Y = array.get_coreArraySize(1);

  // Use OpenMP to support the threading...
#pragma omp parallel for
     for (int core_X = 0; core_X < numberOfCores_X; core_X++)
        {
//#pragma omp for
          for (int core_Y = 0; core_Y < numberOfCores_Y; core_Y++)
             {
            // This lifts out loop invariant portions of the code.
               Core<T> & coreMemory = array.getCore(core_X,core_Y,0);

            // Lift out loop invariant local array size values.
               int sizeX = array.getCore(core_X,core_Y,0).coreArrayNeighborhoodSizes_2D[1][1][0];
               int sizeY = array.getCore(core_X,core_Y,0).coreArrayNeighborhoodSizes_2D[1][1][1];
#if 0
               printf ("\nsizeX = %d sizeY = %d \n",sizeX,sizeY);
#endif
               int base_X  = (coreMemory.boundaryCore_2D[0][0] == true) ? 1 : 0;
               int bound_X = (coreMemory.boundaryCore_2D[0][1] == true) ? sizeX - 2: sizeX - 1;
               int base_Y  = (coreMemory.boundaryCore_2D[1][0] == true) ? 1 : 0;
               int bound_Y = (coreMemory.boundaryCore_2D[1][1] == true) ? sizeY - 2: sizeY - 1;
#if 0
               printf ("core_X = %d core_Y = %d base_X = %d bound_X = %d base_Y = %d bound_Y = %d \n",core_X,core_Y,base_X,bound_X,base_Y,bound_Y);
#endif
               for (int j = base_Y; j <= bound_Y; j++)
                  {
                    for (int i = base_X; i <= bound_X; i++)
                       {
#if 0
                         printf ("\ncore_X = %d core_Y = %d i = %d j = %d \n",core_X,core_Y,i,j);
                         printf ("array.getCore(core_X,core_Y,0)(i,j,0)   = %f \n",array.getCore(core_X,core_Y,0)(i,j,0));
                         printf ("old_array.getCore(core_X,core_Y,0)(i-1,j,0) = %f \n",old_array.getCore(core_X,core_Y,0)(i-1,j,0));
                         printf ("old_array.getCore(core_X,core_Y,0)(i+1,j,0) = %f \n",old_array.getCore(core_X,core_Y,0)(i+1,j,0));
                         printf ("old_array.getCore(core_X,core_Y,0)(i,j-1,0) = %f \n",old_array.getCore(core_X,core_Y,0)(i,j-1,0));
                         printf ("old_array.getCore(core_X,core_Y,0)(i,j+1,0) = %f \n",old_array.getCore(core_X,core_Y,0)(i,j+1,0));
#endif
                         array.getCore(core_X,core_Y,0)(i,j,0) = 
                            ( old_array.getCore(core_X,core_Y,0)(i-1,j,0) +
                              old_array.getCore(core_X,core_Y,0)(i+1,j,0) +
                              old_array.getCore(core_X,core_Y,0)(i,j-1,0) +
                              old_array.getCore(core_X,core_Y,0)(i,j+1,0) ) / 4.0;
                       }
                  }
             }
        }
   }



template <typename T>
void
relax3D_highlevel( MulticoreArray<T> & array,  MulticoreArray<T> & old_array )
   {
  // This is a working example of a 3D stencil demonstrating a high level interface
  // suitable only as debugging support.

#pragma omp parallel for
      for (int k = 1; k < array.get_arraySize(2)-1; k++)
        {
           for (int j = 1; j < array.get_arraySize(1)-1; j++)
             {
                for (int i = 1; i < array.get_arraySize(0)-1; i++)
                  {
                    array(i,j,k) = ( old_array(i-1,j,k) + old_array(i+1,j,k) + old_array(i,j-1,k) + 
                                     old_array(i,j+1,k) + old_array(i,j,k-1) + old_array(i,j,k+1)) / 6.0;
                  }
             }
        }
   }


template <typename T>
void
relax2D_highlevel( MulticoreArray<T> & array,  MulticoreArray<T> & old_array )
   {
  // This is a working example of a 3D stencil demonstrating a high level interface
  // suitable only as debugging support.

//#pragma omp parallel for
//      for (int k = 1; k < array.get_arraySize(2)-1; k++)
//        {
#pragma omp parallel for
           for (int j = 1; j < array.get_arraySize(1)-1; j++)
             {
                for (int i = 1; i < array.get_arraySize(0)-1; i++)
                  {
                    array(i,j,0) = ( old_array(i-1,j,0) + old_array(i+1,j,0) + old_array(i,j-1,0) + 
                                     old_array(i,j+1,0)) / 4.0;
                  }
             }
//        }
   }


// **********************************************************************

template <typename T>
void
relax( int coreID, MulticoreArray<T> & array,  MulticoreArray<T> & old_array, int dist)
   {
//     assert(multicoreArray != NULL);

     const int arraySizeX    = array.get_arraySize(0);
     const int arraySizeY    = array.get_arraySize(1);
     const int arraySizeZ    = array.get_arraySize(2);

     int p = coreID;
     Core<T>* core = array.coreArray[coreID];

  // This lifts out loop invariant portions of the code.
     T* arraySection     = array.get_arraySectionPointers()[p];
     T* old_arraySection = old_array.get_arraySectionPointers()[p];

#if 0
     array.display("before relaxation on interior");
#endif

     assert(array.get_coreArraySize(0) == array.get_coreArraySize(0));
     assert(array.get_coreArraySize(1) == array.get_coreArraySize(1));
     assert(array.get_coreArraySize(2) == array.get_coreArraySize(2));

#if 0
     printf ("\nIterate over all cores: p = %d arraySection = %p old_arraySection = %p \n",p,arraySection,old_arraySection);
#endif
     assert(arraySection     != NULL);
     assert(old_arraySection != NULL);

#if 0
     printf ("array.get_tableBasedDistribution() = %s \n",array.get_tableBasedDistribution() ? "true" : "false");
#endif

#if 1
          if (arraySizeZ > 2)
             {
               if (arraySizeY > 2 && arraySizeX > 2) 
                  {
                 // This is the case of 3D relaxation
#if 0
                    printf ("This is the case of 3D relaxation \n");

                 // Iterate on the interior of the section (non-shared memory operation, local to the closest local memory declared for each core).
                    printf ("This needs to use sectionSize[0-2] to get the local size instead of the global size! \n");
#endif

                 // This is required to avoid valgrind reported errors on some blocks where the local (sectionSize[dim]) is zero.
                 // This is likely because of over flow from size_t type varaibles.
                    assert(core->coreArrayNeighborhoodSizes_3D[1][1][1][0] >= 0);
                    assert(core->coreArrayNeighborhoodSizes_3D[1][1][1][1] >= 0);
                    assert(core->coreArrayNeighborhoodSizes_3D[1][1][1][2] >= 0);
                    int LBX = (array.hasAttachedHalo() && core->boundaryCore_3D[0][0]) ? (dist+array.get_haloWidth(0)) : dist;
                    int LBY = (array.hasAttachedHalo() && core->boundaryCore_3D[1][0]) ? (dist+array.get_haloWidth(1)) : dist;
                    int LBZ = (array.hasAttachedHalo() && core->boundaryCore_3D[2][0]) ? (dist+array.get_haloWidth(2)) : dist;
                    int UBX = (array.hasAttachedHalo() && core->boundaryCore_3D[0][1]) ? (core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist-array.get_haloWidth(0)) : (core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist);
                    int UBY = (array.hasAttachedHalo() && core->boundaryCore_3D[1][1]) ? (core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist-array.get_haloWidth(1)) : (core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist);
                    int UBZ = (array.hasAttachedHalo() && core->boundaryCore_3D[2][1]) ? (core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist-array.get_haloWidth(2)) : (core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist);
                    if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > (2*dist) && core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > (2*dist) && core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > (2*dist))
                       {
                         for (int k = LBZ; k < UBZ; k++)
                            {
                              for (int j = LBY; j < UBY; j++)
                                 {
                                   for (int i = LBX; i < UBX; i++)
                                      {
                                     // This is the dominant computation for each array section per core. The compiler will use the 
                                     // user's code to derive the code that will be put here.
#if 0
                                        printf ("p= %d Indexing 3D array (i,j,k) = (%d,%d,%d) \n",p,i,j,k);
#endif
#if 0
                                        arraySection[index3D(i,j,k)] = 
                                           (old_arraySection[index3D(i-1,j,k)] + old_arraySection[index3D(i+1,j,k)] + 
                                            old_arraySection[index3D(i,j-1,k)] + old_arraySection[index3D(i,j+1,k)] + 
                                            old_arraySection[index3D(i,j,k-1)] + old_arraySection[index3D(i,j,k+1)]) / 6.0;
#else
                                        T tmp(0.0);
                                        for(int d=1; d <=dist; d++)
                                          {
                                           tmp += 
                                             (old_arraySection[index3D(i-d,j,k)] + old_arraySection[index3D(i+d,j,k)] + 
                                              old_arraySection[index3D(i,j-d,k)] + old_arraySection[index3D(i,j+d,k)] + 
                                              old_arraySection[index3D(i,j,k-d)] + old_arraySection[index3D(i,j,k+d)]);
                                          }
                                        arraySection[index3D(i,j,k)] = tmp / (6.0*dist);
#endif
                                      }
                                 }
                            }
                       }
                  }
                 else
                  {
#if 0
                    printf ("3D array too small (still no interior) \n");
#endif
                  }
             }
            else
             {
               if (arraySizeZ == 2)
                  {
#if 0
                    printf ("3D array (with size 2 in Z axis) too small (still no interior) \n");
#endif
                  }
                 else
                  {
                    if (arraySizeY > 2)
                       {
                         if (arraySizeX > 2) 
                            {
                           // This is the case of 2D relaxation
#if 0
                              printf ("This is the case of 2D relaxation (interior) p = %d \n",p);
                              printf ("core->coreArrayNeighborhoodSizes_2D[1][1][0] = %d core->coreArrayNeighborhoodSizes_2D[1][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][1][1]);
#endif
                           int LBX = (array.hasAttachedHalo() && core->boundaryCore_2D[0][0]) ? (dist+array.get_haloWidth(0)) : dist;
                           int LBY = (array.hasAttachedHalo() && core->boundaryCore_2D[1][0]) ? (dist+array.get_haloWidth(1)) : dist;
                           int UBX = (array.hasAttachedHalo() && core->boundaryCore_2D[0][1]) ? (core->coreArrayNeighborhoodSizes_2D[1][1][0]-dist-array.get_haloWidth(0)) : (core->coreArrayNeighborhoodSizes_2D[1][1][0]-dist);
                           int UBY = (array.hasAttachedHalo() && core->boundaryCore_2D[1][1]) ? (core->coreArrayNeighborhoodSizes_2D[1][1][1]-dist-array.get_haloWidth(1)) : (core->coreArrayNeighborhoodSizes_2D[1][1][1]-dist);
                           // The core array may higher dimensional then the array and if so then the local size along
                           // the Z axis may be zero.  If so, then we don't want to process the local array section.
                           // if (sectionSize[2] == 1)
                           // if (sectionSize[0] > 2 && sectionSize[1] > 2 && sectionSize[2] == 1)
                           // if (core->coreArrayNeighborhoodSizes_2D[1][1][0] > 2 && core->coreArrayNeighborhoodSizes_2D[1][1][1] > 2)
                              if (core->coreArrayNeighborhoodSizes_2D[1][1][0] > (dist*2) && core->coreArrayNeighborhoodSizes_2D[1][1][1] > (dist*2) && core->coreArrayNeighborhoodSizes_2D[1][1][2] == 1)
                                 {
                                   for (int j = LBY; j < UBY; j++)
                                      {
                                        for (int i = LBX; i < UBX; i++)
                                           {
                                          // This is the dominant computation for each array section per core. The compiler will use the 
                                          // user's code to derive the code that will be put here.
#if 1
                                        T tmp(0.0);
                                        for(int d=1; d <=dist; d++)
                                          {
                                           tmp += 
                                                (old_arraySection[index2D(i-1,j)] + old_arraySection[index2D(i+1,j)] + 
                                                 old_arraySection[index2D(i,j-1)] + old_arraySection[index2D(i,j+1)]);
                                          }
                                             arraySection[index2D(i,j)] = tmp / (4.0 * dist); 
#endif
                                           }
                                      }
                                 }
                            }
                           else
                            {
#if 0
                              printf ("2D array too small (still no interior) \n");
#endif
                            }
                       }
                      else
                       {
                         if (arraySizeY == 2)
                            {
#if 0
                              printf ("2D array (with size 2 in Y axis) too small (still no interior) \n");
#endif
                            }
                           else
                            {
                              if (arraySizeX > 2)
                                 {
                                // This is the case of 1D relaxation
#if 0
                                   printf ("This is the case of 1D relaxation sectionSize[0] = %d \n",sectionSize[0]);
#endif
                                // The core array may higher dimensional then the array and if so then the local size along either 
                                // the Y or Z axis may be zero.  If so, then we don't want to process the local array section.
                                // if (sectionSize[1] == 1 && sectionSize[2] == 1)
                                // if (sectionSize[0] > 2 && sectionSize[1] == 1 && sectionSize[2] == 1)
                                // if (sectionSize[0] > 0 && ((sectionSize[1] == 1 && sectionSize[2] == 1) || array.get_tableBasedDistribution() == false))
                                   if (core->coreArrayNeighborhoodSizes_1D[1][0] > 0 && (core->coreArrayNeighborhoodSizes_1D[1][1] == 1 && core->coreArrayNeighborhoodSizes_1D[1][2] == 1))
                                      {
                                        for (int i = 1; i < core->coreArrayNeighborhoodSizes_1D[1][0]-1; i++)
                                           {
                                          // This is the dominant computation for each array section per core. The compiler will use the 
                                          // user's code to derive the code that will be put here.
#if 0
                                             printf ("i = %d old_arraySection[index1D(i-1)=%d] = %f \n",i,index1D(i-1),arraySection[index1D(i-1)]);
                                             printf ("i = %d old_arraySection[index1D(i+1)=%d] = %f \n",i,index1D(i+1),arraySection[index1D(i+1)]);
#endif
#if 1
                                        T tmp(0.0);
                                        for(int d=1; d <=dist; d++)
                                          {
                                           tmp += 
                                             (old_arraySection[index1D(i-1)] + old_arraySection[index1D(i+1)]) / 2.0;
                                          }
                                             arraySection[index1D(i)] = tmp / (2.0*dist);
#endif
#if 0
                                             printf ("arraySection[index1D(i=%d)=%d] = %f \n",i,index1D(i),arraySection[index1D(i)]);
#endif
                                           }
                                      }
                                     else
                                      {
#if 0
                                        printf ("The local size for this arraySection is zero in either the Y or Z axis sectionSize[1] = %d sectionSize[2] = %d \n",sectionSize[1],sectionSize[2]);
#endif
                                      }
                                 }
                                else
                                 {
                                // This is array does not have an interior upon which to relax.
#if 0
                                   printf ("1D array too small (still no interior) \n");
#endif
                                 }
                            }
                       }
                  }
             }
#endif

#if 0
          array.display("after relaxation on interior: array");
       // old_array.display("after relaxation on interior: old_array");
#endif
   }
template <typename T>
void
relax_on_boundary( int coreID, MulticoreArray<T> & array,  MulticoreArray<T> & old_array, int dist )
   {
//     assert(multicoreArray != NULL);

     const int arraySizeX    = array.get_arraySize(0);
     const int arraySizeY    = array.get_arraySize(1);
     const int arraySizeZ    = array.get_arraySize(2);

     int p = coreID;
     Core<T>* core = array.coreArray[coreID];

  // This lifts out loop invariant portions of the code.
     T** arraySectionPointers = array.get_arraySectionPointers();
     T** old_arraySectionPointers = old_array.get_arraySectionPointers();

     assert(arraySectionPointers != NULL);
     assert(old_arraySectionPointers != NULL);

     T* arraySection     = array.get_arraySectionPointers()[p];
     T* old_arraySection = old_array.get_arraySectionPointers()[p];

#if 0
     printf ("\nIterate over all cores: p = %d arraySection = %p old_arraySection = %p \n",p,arraySection,old_arraySection);
#endif
     assert(arraySection     != NULL);
     assert(old_arraySection != NULL);

#if 1
      // **************************************************************
       // Fixup internal bounaries of the memory allocated to each core.
       // **************************************************************
#if 0
          printf ("Fixup boundaries: p = %d Array size (%d,%d,%d) sectionSize(%d,%d,%d) coreArray(%d,%d,%d) \n",p,arraySizeX,arraySizeY,arraySizeZ,sectionSize[0],sectionSize[1],sectionSize[2],array.get_coreArraySize(0),array.get_coreArraySize(1),array.get_coreArraySize(2));
#endif
          if (arraySizeZ > (2*dist))
             {
               if (arraySizeY > (2*dist) && arraySizeX > (2*dist)) 
                  {
                 // This is the case of 3D relaxation
#if 0
                    printf ("This is the case of 3D relaxation \n");

                 // Iterate on the interior of the section (non-shared memory operation, local to the closest local memory declared for each core).
                    printf ("This needs to use sectionSize[0-2] to get the local size instead of the global size! \n");
#endif
                     if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][0] >= 1 || core->coreArrayNeighborhoodSizes_3D[1][1][1][1] >= 1) && core->coreArrayNeighborhoodSizes_3D[1][1][1][2] >= 1)
                        {
                            
                                // ***************************************
                                // Now process the edges along the X axis
                                // ***************************************
                            if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1) && (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1))
                               {
                                   if (core->boundaryCore_3D[1][0] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's UPPER boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0)
                                            {
                                              for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                 {
                                                  for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                          tmp += 
                                                              ( /* array[Z][Y-1][X] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d - idx),k,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])]
                                                                         : old_arraySection[index3D(i,(idx-d),k)] ) +
                                                                /* array[Z][Y+1][X] */ old_arraySection[index3D(i,(idx+d),k)] +
                                                                /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idx,k)] + 
                                                                /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idx,k)] +
                                                                /* array[Z-1][Y][X] */ old_arraySection[index3D(i,idx,k-d)] + 
                                                                /* array[Z+1][Y][X] */ old_arraySection[index3D(i,idx,k+d)]);
                                                         }
                                                       arraySection[index3D(i,idx,k)] = tmp / (6.0*dist);
                                                     }
                                                     }
                                                 }
                                            }
                                      }

                                   if (core->boundaryCore_3D[1][1] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's BOTTOM boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0)
                                            {
                                              for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                 {
                                                  for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                          tmp += 
                                                           (/* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1+d),k)] +
                                                            /* array[Z][Y+1][X] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,(d-idx-1),k,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                         : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1-d),k)]) + 
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k)] + 
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k)] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k+d)]);
                                                         }
                                                       arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k)] = tmp / (6.0*dist); 
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                               }
                            else
                               {
/**TODO: adding special case for X size or Z size is only 1**/
                               }
                                // ***************************************
                                // Now process the edges along the Y axis
                                // ***************************************
                            if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1) && (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1))
                               {
                                   if (core->boundaryCore_3D[0][0] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's LEFT boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0)
                                            {
                                              for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                 {
                                                  for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idx,j-d,k)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(idx,j+d,k)] +
                                                            /* array[Z][Y][X-1] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idx),j,k,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                         : old_arraySection[index3D(idx-d,j,k)])+ 
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(idx+d,j,k)] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(idx,j,k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(idx,j,k+d)]);
                                                         }
                                                       arraySection[index3D(idx,j,k)] = tmp / (6.0*dist);
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                                   if (core->boundaryCore_3D[0][1] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's RIGHT boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0)
                                            {
                                              for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                 {
                                                  for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j-d,k)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j+d,k)] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1+d),j,k)] +
                                                            /* array[Z][Y][X+1] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idx-1),j,k,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])]  
                                                                         : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1-d),j,k)]) +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j,k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j,k+d)]);
                                                         }
                                                       arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j,k)] = tmp / (6.0*dist);
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                               }
                            else
                               {
                               }
                                // ***************************************
                                // Now process the edges along the Z axis
                                // ***************************************
                            if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1) && (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1))
                               {
                                   if (core->boundaryCore_3D[2][0] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's LEFT boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0)
                                            {
                                              for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                 {
                                                  for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,j-d,idx)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(i,j+d,idx)] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,j,idx)] +
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,j,idx)] +
                                                            /* array[Z-1][Y][X] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,j,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idx),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                         : old_arraySection[index3D(i,j,idx-d)]) +
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(i,j,idx+d)]);
                                                         }
                                                       arraySection[index3D(i,j,idx)] = tmp / (6.0*dist); 
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                                   if (core->boundaryCore_3D[2][1] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's RIGHT boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0)
                                            {
                                              for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                 {
                                                  for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(i,j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(i,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1+d))] +
                                                            /* array[Z+1][Y][X] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,j,(d-idx-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                         : old_arraySection[index3D(i,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1-d))]));
                                                         }
                                                       arraySection[index3D(i,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] = tmp / (6.0*dist); 
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                               }
                            else
                               {
                               }

                                // ********************
                                // End of plane updates
                                // ********************

                                // ********************
                                // Edge updates along X axis
                                // ********************
                               if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                  {
                                        if ((core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                              {
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                        ( /* array[Z][Y-1][X] */ 
                                                                             ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),idxz,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                       : old_arraySection[index3D(i,idxy-d,idxz)]) +
                                                                          /* array[Z][Y+1][X] */ old_arraySection[index3D(i,idxy+d,idxz)] +
                                                                          /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idxy,idxz)] +
                                                                          /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idxy,idxz)] +
                                                                          /* array[Z-1][Y][X] */ 
                                                                             ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[0][1][1][1]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                       : old_arraySection[index3D(i,idxy,idxz-d)]) +
                                                                          /* array[Z+1][Y][X] */ old_arraySection[index3D(i,idxy,idxz+d)]);
                                                                    }
                                                                  arraySection[index3D(i,idxy,idxz)] = tmp / (6.0*dist);
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }


                                        if ((core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                              {
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                           ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                     : old_arraySection[index3D(i,idxy-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(i,idxy+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                           ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,idxy,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])]
                                                                                     : old_arraySection[index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                     }
                                                                arraySection[index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }


                                        if ((core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                              {
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,(d-idxy-1),idxz,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[0][1][1][1]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))]);
                                                                  }
                                                                arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] = tmp / (6.0*dist);
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }


                                        if ((core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                              {
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,(d-idxy-1),core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }


                                  }
                               else
                                  {
                                  }

                                // ********************
                                // Edge updates along Y axis
                                // ********************
                               if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                  {
                                        if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[2][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,j-d,idxz)] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,j+d,idxz)] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),j,idxz,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),j,idxz)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),j,idxz)] +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,j,(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,j,(idxz+d))]);
                                                                   }
                                                                arraySection[index3D(idxx,j,idxz)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }

                                        if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[2][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),j,core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] + 
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(idxx,j,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                  }
                                                                arraySection[index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }

                                        if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[2][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j-d,idxz)] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j+d,idxz)] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),j,idxz)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),j,idxz,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),j,idxz)]) +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,(idxz+d))]);
                                                                  }
                                                                arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,idxz)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }

                                        if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[2][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),j,core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]-(idxx+1),j,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                  }
                                                                arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }
                                  }
                               else
                                  {
                                  }
                                // ********************
                                // Edge updates along Z axis
                                // ********************
                               if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                  {
                                        if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                        {
                                                           for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),k,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,idxy-d,k)]) +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,idxy+d,k)] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),idxy,k,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,k)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,k)] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,idxy,k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,idxy,k+d)]);
                                                                  }
                                                                arraySection[index3D(idxx,idxy,k)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }

                                        if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                        {
                                                           for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),k)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d > idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(idxx,(d-idxy-1),k,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                    : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),k)]) +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d > idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1),k,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                    : old_arraySection[index3D((idxx-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k+d)]);
                                                                  }
                                                                arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }
                                        if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                        {
                                                           for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),k,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),k)]) +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),k)] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,k)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),idxy,k,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,k)]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,k+d)]) ;
                                                                  }
                                                                arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,k)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }
                                        if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                        {
                                                           for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),k)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]-(idxx+1),(d-idxy-1),k,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),k)]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1),k,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k+d)]);
                                                                  }
                                                                arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }
                                  }
                               else
                                  {
                                  }
                                // ********************
                                // End of edge updates
                                // ********************

                                // ********************
                                // corners updates
                                // ********************
                                  if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][0] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),idxz,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,(idxy-d),idxz)]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,(idxy+d),idxz)] +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),idxy,idxz,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,idxz)]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,idxz)] +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,idxy,(idxz-d))] ) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,idxy,(idxz+d))] ) ;
                                                                    }
                                                                   arraySection[index3D(idxx,idxy,idxz)] = tmp / (6.0*dist) ; 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][1] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,(idxy-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,(idxy+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] + 
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(idxx,idxy,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                   arraySection[index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][0] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(idxx,(d-idxy-1),idxz,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1),idxz,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D(idxx-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D(idxx+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[0][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))]);
                                                                     }
                                                                   arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] = tmp / (6.0*dist);
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][1] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxz+1))] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(idxx,(d-idxy-1),core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxz+1))]) +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[2][1][1][1]-(idxy+1),(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                   arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][0] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),idxz,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  :  old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),idxz)]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),idxz)] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,idxz)] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),idxy,idxz,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  :  old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,idxz)]) +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,(idxz+d))] );
                                                                    }
                                                                   arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,idxz)] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][1] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]-(idxx+1),idxy,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                   arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][0] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]-(idxx+1),(d-idxy-1),idxz,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1),idxz,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)]) +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[0][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))] );
                                                                    }
                                                                   arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][1] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]-(idxx+1),(d-idxy-1),core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[2][1][1][1]-(idxy+1),(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                   arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist);
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }
                                // ********************
                                // End of corner updates
                                // ********************


                        }
                     else
                        {
#if 0
                           printf ("This array segment can't be processed for edge handling because it is too small in at least one axis: p = %d size = (%d,%d,%d) \n",p,core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][1][1],core->coreArrayNeighborhoodSizes_2D[1][1][2]);
#endif
                        // assert(false);
                        }
#if 0
                 // This is required to avoid valgrind reported errors on some blocks where the local (sectionSize[dim]) is zero.
                 // This is likely because of over flow from size_t type veraibles.
                    if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 2 && core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 2 && core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 2)
                       {
                         for (int k = 1; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-1; k++)
                            {
                              for (int j = 1; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-1; j++)
                                 {
                                   for (int i = 1; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-1; i++)
                                      {
                                     // This is the dominant computation for each array section per core. The compiler will use the 
                                     // user's code to derive the code that will be put here.
#if 0
                                        printf ("p= %d Indexing 3D array (i,j,k) = (%d,%d,%d) \n",p,i,j,k);
#endif
#if 0
                                        arraySection[index3D(i,j,k)] = 
                                           (old_arraySection[index3D(i-1,j-1,k-1)] + old_arraySection[index3D(i+1,j-1,k-1)] + 
                                            old_arraySection[index3D(i-1,j+1,k-1)] + old_arraySection[index3D(i+1,j+1,k-1)] + 
                                            old_arraySection[index3D(i-1,j-1,k+1)] + old_arraySection[index3D(i+1,j-1,k+1)] + 
                                            old_arraySection[index3D(i-1,j+1,k+1)] + old_arraySection[index3D(i+1,j+1,k+1)]) / 8.0;
#endif
                                      }
                                 }
                            }
                       }
#endif
                  }
                 else
                  {
#if 0
                    printf ("3D array too small (still no interior) \n");
#endif
                  }
             }
            else
             {
               if (arraySizeZ == 2)
                  {
#if 0
                    printf ("3D array (with size 2 in Z axis) too small (still no interior) \n");
#endif
                  }
                 else
                  {
                    if (arraySizeY > 2)
                       {
                         if (arraySizeX > 2) 
                            {
                           // This is the case of 2D relaxation (along edges)
#if 0
                              printf ("This is the case of 2D relaxation \n");
                              printf ("This needs to use sectionSize[0-1] to get the local size instead of the global size! \n");
#endif

#if 1
                           // The core array may higher dimensional then the array and if so then the local size along
                           // the Z axis may be zero.  If so, then we don't want to process the local array section.
                           // if ((core->coreArrayNeighborhoodSizes_2D[1][1][0] >= 2 || core->coreArrayNeighborhoodSizes_2D[1][1][1] >= 2) && core->coreArrayNeighborhoodSizes_2D[1][1][2] == 1)
                              if ((core->coreArrayNeighborhoodSizes_2D[1][1][0] >= 1 || core->coreArrayNeighborhoodSizes_2D[1][1][1] >= 1) && core->coreArrayNeighborhoodSizes_2D[1][1][2] == 1)
                                 {
                                // Handle the internal boundary equations along edges of the 2D arrays.

                                // ***************************************
                                // Now process the edges along the X axis.
                                // ***************************************

                                // if (sectionSize[1] > 1)
                                   if (core->coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                      {
#if 0
                                        printf ("-- leftEdgeSection[1] = %s rightEdgeSection[1] = %s \n",leftEdgeSection[1] ? "true" : "false",rightEdgeSection[1] ? "true" : "false");
#endif
                                     // if (leftEdgeSection[1] == true)
                                        if (core->boundaryCore_2D[1][0] == true)
                                           {
#if 0
                                             printf ("--- Apply the 2D array abstraction's UPPER boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
#if 0
                                             printf ("apply 2D equation at left edge of memory segment core->coreArrayNeighborhoodSizes_2D[0][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[0][1][1]);
#endif
                                          // if (previous_sectionSize[1] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0)
                                                {
                                               // Upper edge
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | *XXXX* | *****
                                               // ***** | ****** | *****
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****

                                               // arraySection[0] = (old_arraySectionPointers[previous_coreIndexInLinearArray][previous_sectionSize[0]-1] + old_arraySection[1]) / 2.0;
                                               // for (int i = 1; i < sectionSize[0]-1; i++)
                                                  for (int i = 1; i < core->coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                     {
                                                    // arraySection[index2D(i,0)] = (old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(i-1,previous_sectionSize[1]-1)] + old_arraySection[index2D(i-1,1)] +
                                                    //                               old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(i+1,previous_sectionSize[1]-1)] + old_arraySection[index2D(i+1,1)]) / 4.0;
                                                       arraySection[index2D(i,0)] = 
                                                          ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(i,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                            /* array[Y+1][X] */ old_arraySection[index2D(i,1)] +
                                                            /* array[Y][X-1] */ old_arraySection[index2D(i-1,0)] + 
                                                            /* array[Y][X+1] */ old_arraySection[index2D(i+1,0)]) / 4.0;
                                                     }
                                                }
                                           }

                                     // if (rightEdgeSection[1] == true)
                                        if (core->boundaryCore_2D[1][1] == true)
                                           {
#if 0
                                             printf ("--- Apply the array abstraction's LOWER boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
                                          // center_stencil_cell_rightEdge = (left_stencil_cell_rightEdge + right_stencil_cell_rightEdge) / 2.0;
#if 0
                                             printf ("apply 2D equation at right edge of memory segment core->coreArrayNeighborhoodSizes_2D[2][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[2][1][1]);
#endif
                                          // if (next_sectionSize[1] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[2][1][1] > 0)
                                                {
                                               // Lower edge
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****
                                               // ***** | ****** | *****
                                               // ***** | *XXXX* | *****
                                               // ----------------------
                                               // ***** | ****** | *****

                                               // for (int i = 1; i < sectionSize[0]-1; i++)
                                                  for (int i = 1; i < core->coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                     {
                                                       arraySection[index2D(i,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                          ( /* array[Y-1][X] */ old_arraySection[index2D(i,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                            /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(i,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                            /* array[Y][X-1] */ old_arraySection[index2D(i-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] +
                                                            /* array[Y][X+1] */ old_arraySection[index2D(i+1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
                                                     }
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Trivial case of only one equation (define this to be left edge; use the associated references).
                                     // if (sectionSize[1] == 1)
                                        if (core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                           {
#if 0
                                             printf ("--- Trivial case of only one 2D equation (define this to be UPPER edge) \n");
                                             printf ("--- core->boundaryCore_2D[1][0] = %s core->boundaryCore_2D[1][1] = %s \n",core->boundaryCore_2D[1][0] ? "true" : "false",core->boundaryCore_2D[1][1] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[1] == false && rightEdgeSection[1] == false)
                                             if (core->boundaryCore_2D[1][0] == false && core->boundaryCore_2D[1][1] == false)
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // if (previous_sectionSize[1] > 0 && next_sectionSize[1] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0 && core->coreArrayNeighborhoodSizes_2D[2][1][1] > 0)
                                                     {
                                                    // Upper and Lower edges are the same
                                                    // ***** | ****** | *****
                                                    // ----------------------
                                                    // ***** | *XXXX* | *****
                                                    // ----------------------
                                                    // ***** | ****** | *****

#if 0
                                                       printf ("--- Processing trivial case of only one equation 2D (edge in X axis) \n");
#endif
                                                    // for (int i = 1; i < sectionSize[0]-1; i++)
                                                       for (int i = 1; i < core->coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                          {
                                                            arraySection[index2D(i,0)] = 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(i,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(i,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(i-1,0)] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(i+1,0)]) / 4.0;
                                                          }
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // assert(sectionSize[1] == 0);
                                             assert(core->coreArrayNeighborhoodSizes_2D[1][1][1] == 0);
#if 0
                                             printf ("--- core->coreArrayNeighborhoodSizes_2D[1][1][1] == 0: This is the trival case \n");
#endif
                                           }
                                      }

#if 1
                                // ***************************************
                                // Now process the edges along the Y axis.
                                // ***************************************
#if 0
                                   printf ("---+++ Process the edges of the memory section on core index = %d sectionSize[0] = %d previous_sectionSize[0] = %d next_sectionSize[0] = %d \n",p,sectionSize[0],previous_sectionSize[0],next_sectionSize[0]);
#endif
                                // if (sectionSize[0] > 1)
                                   if (core->coreArrayNeighborhoodSizes_2D[1][1][0] > 1)
                                      {
#if 0
                                        printf ("---+++ leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                     // if (leftEdgeSection[0] == true)
                                        if (core->boundaryCore_2D[0][0] == true)
                                           {
#if 0
                                             printf ("---+++ Apply the array abstraction's LEFT boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
                                          // center_stencil_cell_leftEdge = (left_stencil_cell_leftEdge + right_stencil_cell_leftEdge) / 2.0;
#if 0
                                             printf ("apply equation at left edge of memory segment core->coreArrayNeighborhoodSizes_2D[1][0][0] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][0][0]);
#endif
                                          // if (previous_sectionSize[0] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0)
                                                {
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****
                                               // ***** | X***** | *****
                                               // ***** | X***** | *****
                                               // ***** | X***** | *****
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****

                                               // for (int j = 1; j < sectionSize[1]-1; j++)
                                                  for (int j = 1; j < core->coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                     {
#if 1
                                                       arraySection[index2D(0,j)] = 
                                                          ( /* array[Y-1][X] */ old_arraySection[index2D(0,j-1)] +
                                                            /* array[Y+1][X] */ old_arraySection[index2D(0,j+1)] +
                                                         // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,j)] + 
                                                            /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,j,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                            /* array[Y][X+1] */ old_arraySection[index2D(1,j)]) / 4.0;
#endif
                                                     }
                                                }
                                           }

                                     // if (rightEdgeSection[0] == true)
                                        if (core->boundaryCore_2D[0][1] == true)
                                           {
#if 0
                                             printf ("---+++ Apply the array abstraction's RIGHT boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
                                          // center_stencil_cell_rightEdge = (left_stencil_cell_rightEdge + right_stencil_cell_rightEdge) / 2.0;
#if 0
                                             printf ("apply equation at right edge of memory segment core->coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][2][0]);
#endif
                                          // if (next_sectionSize[0] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                {
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****
                                               // ***** | *****X | *****
                                               // ***** | *****X | *****
                                               // ***** | *****X | *****
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****

                                               // for (int j = 1; j < sectionSize[1]-1; j++)
                                                  for (int j = 1; j < core->coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                     {
                                                    // arraySection[index2D(sectionSize[0]-1,j)] = (old_arraySection[index2D(sectionSize[0]-2,j-1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j-1)] +
                                                    //                                              old_arraySection[index2D(sectionSize[0]-2,j+1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j+1)]) / 4.0;
#if 0
                                                       printf ("array[Y][X]:   old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)]           = %f \n",old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)]);
                                                       printf ("array[Y-1][X]: old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)]          = %f \n",old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)]);
                                                       printf ("array[Y+1][X]: old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)]          = %f \n",old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)]);
                                                       printf ("array[Y][X-1]: old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)]            = %f \n",old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)]);
                                                       printf ("p = %d core->coreArrayNeighborhoodLinearized_2D[1][2] = %d \n",p,core->coreArrayNeighborhoodLinearized_2D[1][2]);
                                                       printf ("p = %d core->coreArrayNeighborhoodSizes_2D[1][1][0] = %d \n",p,core->coreArrayNeighborhoodSizes_2D[1][1][0]);
                                                       printf ("p = %d core->coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",p,core->coreArrayNeighborhoodSizes_2D[1][2][0]);
                                                    // printf ("array[Y][X+1]: old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)] = %f \n",old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]);
                                                       printf ("array[Y][X+1]: old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,core->coreArrayNeighborhoodSizes_2D[1][2][0])] = %f \n",
                                                            old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,core->coreArrayNeighborhoodSizes_2D[1][2][0])]);
#endif
#if 1
// This fails for some random problem...
                                                       arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)] = 
                                                          ( /* array[Y-1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)] + 
                                                            /* array[Y+1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)] +
                                                            /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)] + 
                                                         // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]) / 4.0;
                                                            /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                     }
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Trivial case of only one equation (define this to be left edge; use the associated references).
                                     // if (sectionSize[0] == 1)
                                        if (core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1)
                                           {
#if 0
                                             printf ("---+++ Trivial case of only one equation (define this to be left edge; use the associated references) \n");
                                             printf ("---+++ leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                             if (core->boundaryCore_2D[0][0] == false && core->boundaryCore_2D[0][1] == false)
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0 && core->coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                     {
                                                    // ***** | * | *****
                                                    // ----------------------
                                                    // ***** | * | *****
                                                    // ***** | X | *****
                                                    // ***** | X | *****
                                                    // ***** | X | *****
                                                    // ***** | * | *****
                                                    // ----------------------
                                                    // ***** | * | *****

#if 0
                                                       printf ("---+++ Processing trivial case of only one equation \n");
#endif
                                                    // for (int j = 1; j < sectionSize[1]-1; j++)
                                                       for (int j = 1; j < core->coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                          {
                                                         // arraySection[index2D(0,j)] = (old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(previous_sectionSize[0]-1,j-1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j-1)] +
                                                         //                               old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(previous_sectionSize[0]-1,j+1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j+1)]) / 4.0;
#if 1
                                                            arraySection[index2D(0,j)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(0,j-1)] +
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(0,j+1)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,j)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,j,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // assert(sectionSize[0] == 0);
                                             assert(core->coreArrayNeighborhoodSizes_2D[1][1][0] == 0);
#if 0
                                             printf ("---+++ core->coreArrayNeighborhoodSizes_2D[1][0][0] == 0: This is the trival case \n");
#endif
                                           }
                                      }
                                // ********************
                                // End of Y Axis update
                                // ********************
#endif



#if 1
                                // ********************************************
                                // Now process the corners of the X and Y axis.
                                // ********************************************
#if 0
                                   printf ("---+++ Process the edges of the memory section on core p = %d core->coreArrayNeighborhoodSizes_2D[1][1][0] = %d core->coreArrayNeighborhoodSizes_2D[1][0][0] = %d core->coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",
                                        p,core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][0][0],core->coreArrayNeighborhoodSizes_2D[1][2][0]);
                                   printf ("Sizes of current processor: core->coreArrayNeighborhoodSizes_2D[1][1] = (%d,%d,%d) \n",core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][1][1],core->coreArrayNeighborhoodSizes_2D[1][1][2]);
#endif
                                // First X Axis logic
                                   if (core->coreArrayNeighborhoodSizes_2D[1][1][0] > 1)
                                      {
                                     // Left sice corners
                                        if (core->boundaryCore_2D[0][0] == true)
                                           {
                                          // processor boundary condition enforced here (X axis)
                                           }
                                          else
                                           {
                                             if (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0)
                                                {
                                               // Next Y Axis logic
                                                  if (core->coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                                     {
                                                    // Upper corner
                                                       if (core->boundaryCore_2D[1][0] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][0] > 0);
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0);

                                                         // Upper left corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | X***** | *****
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(0,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(1,0)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(0,1)]) / 4.0;
#endif
                                                          }

                                                    // Lower corner
                                                       if (core->boundaryCore_2D[1][1] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0);
                                                            assert (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0);

                                                         // Lower left corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ***** | X***** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 0
                                                            printf ("--- array[Y][X]:   arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
                                                            printf ("old_array[Y][X]:   old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
                                                            printf ("old_array[Y-1][X]: old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] = %f \n",old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)]);
                                                            printf ("old_array[Y+1][X]: old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)] = %f \n",old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)]);
                                                            printf ("old_array[Y][X-1]: old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,core->coreArrayNeighborhoodSizes_2D[1][0][1]-1)] = %f \n",
                                                                 old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,core->coreArrayNeighborhoodSizes_2D[1][0][1]-1)]);
                                                            printf ("array[Y][X+1]: old_arraySection[index2D(1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",old_arraySection[index2D(1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
#endif
#if 1
                                                            arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,core->coreArrayNeighborhoodSizes_2D[1][0][1]-1)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,core->coreArrayNeighborhoodSizes_2D[1][0][1]-1,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
#endif
#if 0
                                                            printf ("--- array[Y][X]: arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
#endif
                                                          }
                                                     }
                                                    else
                                                     {
                                                    // printf ("core->coreArrayNeighborhoodSizes_2D[1][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][1]);
                                                       if (core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                                          {
                                                         // Case of upper and lower left corners are the same point
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | X***** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(0,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[1][2][0])] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(1,0)]) / 4.0;
#endif
                                                          }
                                                     }
                                                }
                                               else
                                                {
                                                  printf ("We don't support the size on the adjacent being zero! \n");
                                                  assert(false);
                                                }
                                           }

                                     // Right side corners
                                        if (core->boundaryCore_2D[0][1] == true)
                                           {
                                          // Can we test if this is realy a boundary?
                                           }
                                          else
                                           {
                                          // if (next_sectionSize[0] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                {
                                               // printf ("Right boundary corner not implemented! \n");

                                               // Next Y Axis logic
                                                  if (core->coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                                     {
                                                    // Upper corner
                                                       if (core->boundaryCore_2D[1][0] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][0] > 0);
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0);

                                                         // Upper right corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | *****X | *****
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][index2D(core->coreArrayNeighborhoodSizes_2D[0][1][0]-1,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[0][1][0]-1,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,1)] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,0)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }

                                                    // Lower corner
                                                       if (core->boundaryCore_2D[1][1] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0);
                                                            assert (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0);

                                                         // Lower right corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ***** | *****X | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(core->coreArrayNeighborhoodSizes_2D[2][1][0]-1,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[2][1][0]-1,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,core->coreArrayNeighborhoodSizes_2D[2][1][1]-1)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,core->coreArrayNeighborhoodSizes_2D[2][1][1]-1,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }
                                                     }
                                                    else
                                                     {
                                                    // printf ("core->coreArrayNeighborhoodSizes_2D[1][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][1]);
                                                       if (core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                                          {
                                                         // Case of upper and lower right corners are the same point
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | *****X | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][index2D(core->coreArrayNeighborhoodSizes_2D[0][1][0]-1,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1)] +
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[0][1][0]-1,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] +
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(core->coreArrayNeighborhoodSizes_2D[2][1][0]-1,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[2][1][0]-1,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,0)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }
                                                     }
                                                }
                                               else
                                                {
                                                  printf ("We don't support the size on the adjacent being zero! \n");
                                                  assert(false);
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Trivial case of only one equation (define this to be left edge; use the associated references).
#if 0
                                        printf ("Case of core->coreArrayNeighborhoodSizes_2D[1][1][0] == %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][0]);
                                        printf ("Case of core->coreArrayNeighborhoodSizes_2D[1][1][1] == %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][1]);
#endif
                                     // assert(core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1);
                                     // assert(core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1);

                                     // if (sectionSize[0] == 1)
                                     // if (core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1)
                                        if (core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1 && core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                           {
                                          // printf ("Case of core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1 && core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1\n");

                                          // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                          // if (core->boundaryCore_2D[0][0] == false && core->boundaryCore_2D[0][1] == false)
                                             if (core->boundaryCore_2D[0][0] == false && core->boundaryCore_2D[0][1] == false && core->boundaryCore_2D[1][0] == false && core->boundaryCore_2D[1][1] == false)
                                                {
                                               // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0 && core->coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                     {
                                                    // printf ("Case of single point boundary not implemented! \n");
                                                    // ***** | * | *****
                                                    // -----------------
                                                    // ***** | X | *****
                                                    // -----------------
                                                    // ***** | * | *****
#if 1
                                                       arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                          ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] +
                                                            /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                            /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,core->coreArrayNeighborhoodSizes_2D[1][0][0])] +
                                                            /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                     }

#if 0
                                                  printf ("Exiting as a test! \n");
                                                  assert(false);
#endif
                                                }
                                           }
                                          else
                                           {
                                          // assert(sectionSize[0] == 0);
                                             if (core->coreArrayNeighborhoodSizes_2D[1][1][0] != 0)
                                                {
#if 0
                                                   printf ("Warning: p = %d core->coreArrayNeighborhoodSizes_2D[1][1][0] = %d \n",p,core->coreArrayNeighborhoodSizes_2D[1][1][0]);
#endif
                                                }
                                          // assert(core->coreArrayNeighborhoodSizes_2D[1][1][0] == 0);
                                             assert(core->coreArrayNeighborhoodSizes_2D[1][1][0] <= 1);
#if 0
                                             printf ("---+++ core->coreArrayNeighborhoodSizes_2D[1][0][0] == 0: This is the trival case \n");
#endif
                                           }
                                      }

                                // **************************************************
                                // End of processing the corners of the X and Y axis.
                                // **************************************************
#endif
                                 }
                                else
                                 {
#if 0
                                   printf ("This array segment can't be processed for edge handling because it is too small in at least one axis: p = %d size = (%d,%d,%d) \n",p,core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][1][1],core->coreArrayNeighborhoodSizes_2D[1][1][2]);
#endif
                                // assert(false);
                                 }
                              
#endif
                            }
                           else
                            {
#if 0
                              printf ("2D array too small (still no interior) \n");
#endif
                            }
                       }
                      else
                       {
                         if (arraySizeY == 2)
                            {
#if 0
                              printf ("2D array (with size 2 in Y axis) too small (still no interior) \n");
#endif
                            }
                           else
                            {
                              if (arraySizeX > 2)
                                 {
                                // This is the case of 1D relaxation
#if 0
                                   printf ("--- This is the case of 1D relaxation sectionSize[0] = %d \n",sectionSize[0]);
#endif
                                // The core array may higher dimensional then the array and if so then the local size along either 
                                // the Y or Z axis may be zero.  If so, then we don't want to process the local array section.
                                // if (sectionSize[1] == 1 && sectionSize[2] == 1)
                                // if (sectionSize[0] > 0 && ((sectionSize[1] == 1 && sectionSize[2] == 1) || array.get_tableBasedDistribution() == false))
                                // if (sectionSize[0] > 0 && (sectionSize[1] == 1 && sectionSize[2] == 1) )
                                   if (core->coreArrayNeighborhoodSizes_1D[1][0] > 0 && (core->coreArrayNeighborhoodSizes_1D[1][1] == 1 && core->coreArrayNeighborhoodSizes_1D[1][2] == 1) )
                                      {
#if 0
                                         printf ("--- Process the edges of the memory section on core index = %d sectionSize[0] = %d previous_sectionSize[0] = %d next_sectionSize[0] = %d \n",p,sectionSize[0],previous_sectionSize[0],next_sectionSize[0]);
#endif
                                     // if (sectionSize[0] > 1)
                                        if (core->coreArrayNeighborhoodSizes_1D[1][0] > 1)
                                           {
#if 0
                                             printf ("-- leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[0] == true)
                                             if (core->boundaryCore_1D[0] == true)
                                                {
#if 0
                                                  printf ("--- Apply the array abstraction's LEFT boundary condition \n");
#endif
                                                }
                                               else
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // center_stencil_cell_leftEdge = (left_stencil_cell_leftEdge + right_stencil_cell_leftEdge) / 2.0;
#if 0
                                                  printf ("apply equation at left edge of memory segment previous_sectionSize[0] = %d \n",previous_sectionSize[0]);
#endif
                                               // if (previous_sectionSize[0] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_1D[0][0] > 0)
                                                     {
                                                    // arraySection[0] = (old_arraySectionPointers[previous_coreIndexInLinearArray][core->coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySection[1]) / 2.0;
                                                       arraySection[0] = (old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[0]][core->coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySection[1]) / 2.0;
                                                     }
                                                }

                                          // if (rightEdgeSection[0] == true)
                                             if (core->boundaryCore_1D[1] == true)
                                                {
#if 0
                                                  printf ("--- Apply the array abstraction's RIGHT boundary condition \n");
#endif
                                                }
                                               else
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // center_stencil_cell_rightEdge = (left_stencil_cell_rightEdge + right_stencil_cell_rightEdge) / 2.0;
#if 0
                                                  printf ("apply equation at right edge of memory segment next_sectionSize[0] = %d \n",next_sectionSize[0]);
#endif
                                               // if (next_sectionSize[0] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_1D[2][0] > 0)
                                                     {
                                                    // arraySection[sectionSize[0]-1] = (old_arraySection[sectionSize[0]-2] + old_arraySectionPointers[next_coreIndexInLinearArray][0]) / 2.0;
                                                       arraySection[core->coreArrayNeighborhoodSizes_1D[1][0]-1] = (old_arraySection[core->coreArrayNeighborhoodSizes_1D[1][0]-2] + old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[2]][0]) / 2.0;
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // Trivial case of only one equation (define this to be left edge; use the associated references).
                                          // if (sectionSize[0] == 1)
                                             if (core->coreArrayNeighborhoodSizes_1D[1][0] == 1)
                                                {
#if 0
                                                  printf ("--- Trivial case of only one equation (define this to be left edge; use the associated references) \n");
                                                  printf ("--- leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                               // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                                  if (core->boundaryCore_1D[0] == false && core->boundaryCore_1D[1] == false)
                                                     {
                                                    // This is where user specific code is places within the compiler transformation.
                                                    // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                       if (core->coreArrayNeighborhoodSizes_1D[0][0] > 0 && core->coreArrayNeighborhoodSizes_1D[2][0] > 0)
                                                          {
#if 0
                                                            printf ("--- Processing trivial case of only one equation \n");
#endif
                                                         // arraySection[0] = (old_arraySectionPointers[previous_coreIndexInLinearArray][previous_sectionSize[0]-1] + old_arraySectionPointers[next_coreIndexInLinearArray][0]) / 2.0;
                                                            arraySection[0] = (old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[0]][core->coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[2]][0]) / 2.0;
                                                          }
                                                     }
                                                }
                                               else
                                                {
                                               // assert(sectionSize[0] == 0);
                                                  assert(core->coreArrayNeighborhoodSizes_1D[1][0] == 0);
#if 0
                                                  printf ("--- sectionSize[0] == 0: This is the trival case \n");
#endif
                                                }
                                           }
                                      }
                                     else
                                      {
#if 0
                                        printf ("--- The local size for this arraySection is zero in either the Y or Z axis sectionSize[1] = %d sectionSize[2] = %d \n",sectionSize[1],sectionSize[2]);
#endif
                                      }
                                 }
                                else
                                 {
                                // This is array does not have an interior upon which to relax.
#if 0
                                   printf ("--- 1D array too small (still no interior) \n");
#endif
                                 }
                            }
                       }
                  }
             }
#endif

#if 0
     array.display("after relaxation on memory section edges: array");
     old_array.display("after relaxation on memory section edges: old_array");
#endif
   }

// *********************************************************************
// Original boundary relaxation without Halo regions in local memory 

template <typename T>
void
relax_on_boundary_simplified( int coreID, MulticoreArray<T> & array,  MulticoreArray<T> & old_array, int dist )
   {
     const int arraySizeX    = array.get_arraySize(0);
     const int arraySizeY    = array.get_arraySize(1);
     const int arraySizeZ    = array.get_arraySize(2);

     int p = coreID;
     Core<T>* core = array.coreArray[coreID];

  // This lifts out loop invariant portions of the code.
     T** arraySectionPointers = array.get_arraySectionPointers();
     T** old_arraySectionPointers = old_array.get_arraySectionPointers();

     assert(arraySectionPointers != NULL);
     assert(old_arraySectionPointers != NULL);

     T* arraySection     = array.get_arraySectionPointers()[p];
     T* old_arraySection = old_array.get_arraySectionPointers()[p];

#if 0
     printf ("\nIterate over all cores: p = %d arraySection = %p old_arraySection = %p \n",p,arraySection,old_arraySection);
#endif
     assert(arraySection     != NULL);
     assert(old_arraySection != NULL);

      // **************************************************************
       // Fixup internal bounaries of the memory allocated to each core.
       // **************************************************************
#if 0
          printf ("Fixup boundaries: p = %d Array size (%d,%d,%d) sectionSize(%d,%d,%d) coreArray(%d,%d,%d) \n",p,arraySizeX,arraySizeY,arraySizeZ,sectionSize[0],sectionSize[1],sectionSize[2],array.get_coreArraySize(0),array.get_coreArraySize(1),array.get_coreArraySize(2));
#endif
          if (arraySizeZ > (2*dist))
             {
               if (arraySizeY > (2*dist) && arraySizeX > (2*dist)) 
                  {
                 // This is the case of 3D relaxation
#if 0
                    printf ("This is the case of 3D relaxation \n");

                 // Iterate on the interior of the section (non-shared memory operation, local to the closest local memory declared for each core).
                    printf ("This needs to use sectionSize[0-2] to get the local size instead of the global size! \n");
#endif
                     if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][0] >= 1 || core->coreArrayNeighborhoodSizes_3D[1][1][1][1] >= 1) && core->coreArrayNeighborhoodSizes_3D[1][1][1][2] >= 1)
                        {

               int base_X  = (core->boundaryCore_3D[0][0] == true) ? dist : 0;
               int bound_X = (core->boundaryCore_3D[0][1] == true) ? core->coreArrayNeighborhoodSizes_3D[1][1][1][0] - (dist): core->coreArrayNeighborhoodSizes_3D[1][1][1][0];
               int base_Y  = (core->boundaryCore_3D[1][0] == true) ? dist : 0;
               int bound_Y = (core->boundaryCore_3D[1][1] == true) ? core->coreArrayNeighborhoodSizes_3D[1][1][1][1] - (dist): core->coreArrayNeighborhoodSizes_3D[1][1][1][1];
               int base_Z  = (core->boundaryCore_3D[2][0] == true) ? dist : 0;
               int bound_Z = (core->boundaryCore_3D[2][1] == true) ? core->coreArrayNeighborhoodSizes_3D[1][1][1][2] - (dist): core->coreArrayNeighborhoodSizes_3D[1][1][1][2];
                           for (int k = base_Z; k < bound_Z; k++)
                              {
                               for (int j = base_Y; j < bound_Y; j++)
                                  {
                                  for (int i = base_X; i < bound_X; i++)
				     {
                                       if((i >= dist) && (i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist) &&
                                          (j >= dist) && (j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist) &&
                                          (k >= dist) && (k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist)) continue;
                                       T tmp(0.0);
                                       for(int d=1; d <=dist; d++)
                                         {
                                           tmp += (
                                               /* array[Z][Y][X-d] */ ((i-d < 0) ? 
                                                  old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]+(i-d),j,k,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] : 
                                                  old_arraySection[index3D(i-d,j,k)]) +
                                               /* array[Z][Y][X+d] */ ((i+d >= core->coreArrayNeighborhoodSizes_3D[1][1][1][0]) ? 
                                                  old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((i+d-core->coreArrayNeighborhoodSizes_3D[1][1][2][0]),j,k,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] : 
                                                  old_arraySection[index3D(i+d,j,k)]) +
                                               /* array[Z][Y-d][X] */ ((j-d < 0) ? 
                                                  old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]+(j-d),k,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] : 
                                                  old_arraySection[index3D(i,j-d,k)]) +
                                               /* array[Z][Y+d][X] */ ((j+d >= core->coreArrayNeighborhoodSizes_3D[1][1][1][1]) ? 
                                                  old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,(j+d-core->coreArrayNeighborhoodSizes_3D[1][2][1][2]),k,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] : 
                                                  old_arraySection[index3D(i,j+d,k)]) +
                                               /* array[Z-d][Y][X] */ ((k-d < 0) ? 
                                                  old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,j,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]+(k-d),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] : 
                                                  old_arraySection[index3D(i,j,k-d)]) +
                                               /* array[Z+d][Y][X] */ ((k+d >= core->coreArrayNeighborhoodSizes_3D[1][1][1][2]) ? 
                                                  old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,j,(k+d-core->coreArrayNeighborhoodSizes_3D[2][1][1][2]),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] : 
                                                  old_arraySection[index3D(i,j,k+d)]) 
                                               );
                                         }
                                         arraySection[index3D(i,j,k)] = tmp / (6.0*dist);
                                     }
                                  }
                              }
                        }
                  }
             }
   }

// *********************************************************************
// New boundary relaxation with DetachedHalo regions in local memory 
// This is applied only when user construct MulticoreArray with postive halo region size.

template <typename T>
void
relax_on_detachedhalo_boundary( int coreID, MulticoreArray<T> & array,  MulticoreArray<T> & old_array, int dist )
   {
     const int arraySizeX    = array.get_arraySize(0);
     const int arraySizeY    = array.get_arraySize(1);
     const int arraySizeZ    = array.get_arraySize(2);

     int p = coreID;
     Core<T>* core = array.coreArray[coreID];

  // This lifts out loop invariant portions of the code.
     T** arraySectionPointers = array.get_arraySectionPointers();
     T** old_arraySectionPointers = old_array.get_arraySectionPointers();

     assert(arraySectionPointers != NULL);
     assert(old_arraySectionPointers != NULL);

     T* arraySection     = array.get_arraySectionPointers()[p];
     T* old_arraySection = old_array.get_arraySectionPointers()[p];

#if 0
     printf ("\nIterate over all cores: p = %d arraySection = %p old_arraySection = %p \n",p,arraySection,old_arraySection);
#endif
     assert(arraySection     != NULL);
     assert(old_arraySection != NULL);

#if 1
       // **************************************************************
       // Fixup internal bounaries of the memory allocated to each core.
       // **************************************************************
#if 0
          printf ("Fixup boundaries: p = %d Array size (%d,%d,%d) sectionSize(%d,%d,%d) coreArray(%d,%d,%d) \n",p,arraySizeX,arraySizeY,arraySizeZ,sectionSize[0],sectionSize[1],sectionSize[2],array.get_coreArraySize(0),array.get_coreArraySize(1),array.get_coreArraySize(2));
#endif
          if (arraySizeZ > (2*dist))
             {
               if (arraySizeY > (2*dist) && arraySizeX > (2*dist)) 
                  {
                 // This is the case of 3D relaxation
#if 0
                    printf ("This is the case of 3D relaxation \n");

                 // Iterate on the interior of the section (non-shared memory operation, local to the closest local memory declared for each core).
                    printf ("This needs to use sectionSize[0-2] to get the local size instead of the global size! \n");
#endif
                     if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][0] >= 1 || core->coreArrayNeighborhoodSizes_3D[1][1][1][1] >= 1) && core->coreArrayNeighborhoodSizes_3D[1][1][1][2] >= 1)
                        {
                              T** old_haloXBottom  = old_array.get_haloSectionPointers(0,0);
                              T** old_haloXTop  = old_array.get_haloSectionPointers(0,1);
                              T** old_haloYBottom  = old_array.get_haloSectionPointers(1,0);
                              T** old_haloYTop  = old_array.get_haloSectionPointers(1,1);
                              T** old_haloZBottom  = old_array.get_haloSectionPointers(2,0);
                              T** old_haloZTop  = old_array.get_haloSectionPointers(2,1);
                            
                                // ***************************************
                                // Now process the edges along the X axis
                                // ***************************************
                            if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1) && (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1))
                               {
                                   if (core->boundaryCore_3D[1][0] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's UPPER boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0)
                                            {
                                              for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                 {
                                                  for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                       if(old_array.get_haloSectionSizes(1)[coreID])
                                                          tmp += 
                                                              ( /* array[Z][Y-1][X] */ 
                                                                ((d>idx) ? old_haloYBottom[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)-(d-idx)),k,(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))]  
                                                                         : old_arraySection[index3D(i,(idx-d),k)] ) +
                                                                /* array[Z][Y+1][X] */ old_arraySection[index3D(i,(idx+d),k)] +
                                                                /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idx,k)] + 
                                                                /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idx,k)] +
                                                                /* array[Z-1][Y][X] */ old_arraySection[index3D(i,idx,k-d)] + 
                                                                /* array[Z+1][Y][X] */ old_arraySection[index3D(i,idx,k+d)]);
                                                      else 
                                                          tmp += 
                                                              ( /* array[Z][Y-1][X] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d - idx),k,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])]
                                                                         : old_arraySection[index3D(i,(idx-d),k)] ) +
                                                                /* array[Z][Y+1][X] */ old_arraySection[index3D(i,(idx+d),k)] +
                                                                /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idx,k)] + 
                                                                /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idx,k)] +
                                                                /* array[Z-1][Y][X] */ old_arraySection[index3D(i,idx,k-d)] + 
                                                                /* array[Z+1][Y][X] */ old_arraySection[index3D(i,idx,k+d)]);
                                                         }
                                                       arraySection[index3D(i,idx,k)] = tmp / (6.0*dist);
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                                   if (core->boundaryCore_3D[1][1] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's BOTTOM boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0)
                                            {
                                              for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                 {
                                                  for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                      if(old_array.get_haloSectionSizes(1)[coreID])
                                                          tmp += 
                                                           (/* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1+d),k)] +
                                                            /* array[Z][Y+1][X] */ 
                                                                ((d>idx) ? old_haloYTop[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(d-idx-1),k,(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))]  
                                                                         : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1-d),k)]) + 
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k)] + 
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k)] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k+d)]);
                                                      else 
                                                          tmp += 
                                                           (/* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1+d),k)] +
                                                            /* array[Z][Y+1][X] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,(d-idx-1),k,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                         : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1-d),k)]) + 
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k)] + 
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k)] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k+d)]);
                                                         }
                                                       arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idx+1),k)] = tmp / (6.0*dist); 
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                               }
                            else
                               {
/**TODO: adding special case for X size or Z size is only 1**/
                               }
                                // ***************************************
                                // Now process the edges along the Y axis
                                // ***************************************
                            if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1) && (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1))
                               {
                                   if (core->boundaryCore_3D[0][0] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's LEFT boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0)
                                            {
                                              for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                 {
                                                  for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                       if(old_array.get_haloSectionSizes(0)[coreID])
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idx,j-d,k)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(idx,j+d,k)] +
                                                            /* array[Z][Y][X-1] */ 
                                                                ((d>idx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idx)),j,k,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])]  
                                                                         : old_arraySection[index3D(idx-d,j,k)])+ 
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(idx+d,j,k)] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(idx,j,k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(idx,j,k+d)]);
                                                       else
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idx,j-d,k)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(idx,j+d,k)] +
                                                            /* array[Z][Y][X-1] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idx),j,k,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                         : old_arraySection[index3D(idx-d,j,k)])+ 
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(idx+d,j,k)] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(idx,j,k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(idx,j,k+d)]);
                                                         }
                                                       arraySection[index3D(idx,j,k)] = tmp / (6.0*dist);
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                                   if (core->boundaryCore_3D[0][1] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's RIGHT boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0)
                                            {
                                              for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                 {
                                                  for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                       if(old_array.get_haloSectionSizes(0)[coreID])
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j-d,k)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j+d,k)] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1+d),j,k)] +
                                                            /* array[Z][Y][X+1] */ 
                                                                ((d>idx) ? old_haloXTop[coreID][otherCore_index3D((d-idx-1),j,k,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])]  
                                                                         : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1-d),j,k)]) +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j,k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j,k+d)]);
                                                       else
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j-d,k)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j+d,k)] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1+d),j,k)] +
                                                            /* array[Z][Y][X+1] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idx-1),j,k,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])]  
                                                                         : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1-d),j,k)]) +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j,k-d)] + 
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j,k+d)]);
                                                         }
                                                       arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idx+1),j,k)] = tmp / (6.0*dist);
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                               }
                            else
                               {
                               }
                                // ***************************************
                                // Now process the edges along the Z axis
                                // ***************************************
                            if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1) && (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1))
                               {
                                   if (core->boundaryCore_3D[2][0] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's LEFT boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0)
                                            {
                                              for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                 {
                                                  for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                       if(old_array.get_haloSectionSizes(2)[coreID])
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,j-d,idx)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(i,j+d,idx)] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,j,idx)] +
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,j,idx)] +
                                                            /* array[Z-1][Y][X] */ 
                                                                ((d>idx) ? old_haloZBottom[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(j+old_array.get_haloWidth(1)),(old_array.get_haloWidth(2)-(d-idx)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))]  
                                                                         : old_arraySection[index3D(i,j,idx-d)]) +
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(i,j,idx+d)]);
                                                       else
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,j-d,idx)] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(i,j+d,idx)] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,j,idx)] +
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,j,idx)] +
                                                            /* array[Z-1][Y][X] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,j,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idx),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                         : old_arraySection[index3D(i,j,idx-d)]) +
                                                            /* array[Z+1][Y][X] */ old_arraySection[index3D(i,j,idx+d)]);
                                                         }
                                                       arraySection[index3D(i,j,idx)] = tmp / (6.0*dist); 
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                                   if (core->boundaryCore_3D[2][1] == true)
                                      {
#if 0
                                        printf ("--- Apply the 3D array abstraction's RIGHT boundary condition \n");
#endif
                                      }
                                    else
                                      {
                                         if (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0)
                                            {
                                              for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                 {
                                                  for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                     {
                                                     for(int idx=0; idx <dist; idx++)
                                                     {
                                                       T tmp(0.0);
                                                       for(int d=1; d <=dist; d++)
                                                         {
                                                       if(old_array.get_haloSectionSizes(2)[coreID])
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(i,j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(i,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1+d))] +
                                                            /* array[Z+1][Y][X] */ 
                                                                ((d>idx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,j,(d-idx-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                         : old_arraySection[index3D(i,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1-d))]));
                                                       else
                                                          tmp += 
                                                          ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y+1][X] */ old_arraySection[index3D(i,j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] +
                                                            /* array[Z-1][Y][X] */ old_arraySection[index3D(i,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1+d))] +
                                                            /* array[Z+1][Y][X] */ 
                                                                ((d>idx) ? old_haloZTop[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(j+old_array.get_haloWidth(1)),(d-idx-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                         : old_arraySection[index3D(i,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1-d))]));
                                                         }
                                                       arraySection[index3D(i,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idx+1))] = tmp / (6.0*dist); 
                                                     }
                                                     }
                                                 }
                                            }
                                      }
                               }
                            else
                               {
                               }

                                // ********************
                                // End of plane updates
                                // ********************

                                // ********************
                                // Edge updates along X axis
                                // ********************
                               if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                  {
                                        if ((core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                              {
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                               if(old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                        ( /* array[Z][Y-1][X] */ 
                                                                             ((d>idxy) ? old_haloYBottom[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)-(d-idxy)),idxz,(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                       : old_arraySection[index3D(i,idxy-d,idxz)]) +
                                                                          /* array[Z][Y+1][X] */ old_arraySection[index3D(i,idxy+d,idxz)] +
                                                                          /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idxy,idxz)] +
                                                                          /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idxy,idxz)] +
                                                                          /* array[Z-1][Y][X] */ 
                                                                             ((d>idxz) ? old_haloZBottom[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),old_array.get_haloWidth(1)+idxy,(old_array.get_haloWidth(2)-(d-idxz)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                       : old_arraySection[index3D(i,idxy,idxz-d)]) +
                                                                          /* array[Z+1][Y][X] */ old_arraySection[index3D(i,idxy,idxz+d)]);
                                                               else 
                                                                     tmp += 
                                                                        ( /* array[Z][Y-1][X] */ 
                                                                             ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),idxz,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                       : old_arraySection[index3D(i,idxy-d,idxz)]) +
                                                                          /* array[Z][Y+1][X] */ old_arraySection[index3D(i,idxy+d,idxz)] +
                                                                          /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idxy,idxz)] +
                                                                          /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idxy,idxz)] +
                                                                          /* array[Z-1][Y][X] */ 
                                                                             ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[0][1][1][1]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                       : old_arraySection[index3D(i,idxy,idxz-d)]) +
                                                                          /* array[Z+1][Y][X] */ old_arraySection[index3D(i,idxy,idxz+d)]);
                                                                    }
                                                                  arraySection[index3D(i,idxy,idxz)] = tmp / (6.0*dist);
                                                               }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }


                                        if ((core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                              {
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                               if(old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                           ((d>idxy) ? old_haloYBottom[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)-(d-idxy)),(core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1)),(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)))] 
                                                                                     : old_arraySection[index3D(i,idxy-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(i,idxy+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                           ((d>idxz) ? old_haloZTop[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)+idxy),(d-idxz-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))]
                                                                                     : old_arraySection[index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                               else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                           ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                     : old_arraySection[index3D(i,idxy-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(i,idxy+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                           ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,idxy,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])]
                                                                                     : old_arraySection[index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                     }
                                                                arraySection[index3D(i,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist);
                                                               }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }


                                        if ((core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                              {
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_haloYTop[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(d-idxy-1),idxz,(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZBottom[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[1][1][1][1]+old_array.get_haloWidth(1)-(idxy+1)),(old_array.get_haloWidth(2)-(d-idxz)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))]);
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,(d-idxy-1),idxz,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[0][1][1][1]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))]);
                                                                  }
                                                                arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] = tmp / (6.0*dist);
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }


                                        if ((core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int i = dist; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist; i++)
                                                              {
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_haloYTop[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(d-idxy-1),(core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1)),(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZTop[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[1][1][1][1]+old_array.get_haloWidth(1)-(idxy+1)),(d-idxz-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(i,(d-idxy-1),core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(i-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D(i+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                arraySection[index3D(i,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }


                                }
                               else
                                  {
                                  }

                                // ********************
                                // Edge updates along Y axis
                                // ********************
                               if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                  {
                                        if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[2][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,j-d,idxz)] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,j+d,idxz)] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idxx)),j,idxz,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),j,idxz)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),j,idxz)] +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)+idxx),(j+old_array.get_haloWidth(1)),(old_array.get_haloWidth(2)-(d-idxz)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(idxx,j,(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,j,(idxz+d))]);
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,j-d,idxz)] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,j+d,idxz)] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),j,idxz,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),j,idxz)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),j,idxz)] +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,j,(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,j,(idxz+d))]);
                                                                   }
                                                                arraySection[index3D(idxx,j,idxz)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }

                                        if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[2][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idxx)),j,(core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1)),old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] + 
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZTop[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(j+old_array.get_haloWidth(1)),(d-idxz-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),j,core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] + 
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(idxx,j,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                  }
                                                                arraySection[index3D(idxx,j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }

                                        if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[2][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j-d,idxz)] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j+d,idxz)] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),j,idxz)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_haloXTop[coreID][otherCore_index3D((d-idxx-1),j,idxz,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),j,idxz)]) +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZBottom[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(j+old_array.get_haloWidth(1)),(old_array.get_haloWidth(2)-(d-idxz)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,(idxz+d))]);
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j-d,idxz)] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j+d,idxz)] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),j,idxz)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),j,idxz,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),j,idxz)]) +
                                                                     /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,(idxz-d))]) +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,(idxz+d))]);
                                                                  }
                                                                arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,idxz)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }

                                        if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[2][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                        {
                                                           for (int j = dist; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist; j++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_haloXTop[coreID][otherCore_index3D((d-idxx-1),j,(core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1)),old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZTop[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(j+old_array.get_haloWidth(1)),(d-idxz-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),j,core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                     /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]-(idxx+1),j,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                  }
                                                                arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),j,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }
                                  }
                               else
                                  {
                                  }
                                // ********************
                                // Edge updates along Z axis
                                // ********************
                               if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                  {
                                        if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                        {
                                                           for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_haloYBottom[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(old_array.get_haloWidth(1)-(d-idxy)),k,(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(idxx,idxy-d,k)]) +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,idxy+d,k)] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idxx)),idxy,k,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,k)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,k)] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,idxy,k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,idxy,k+d)]);
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),k,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,idxy-d,k)]) +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,idxy+d,k)] +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),idxy,k,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,k)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,k)] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,idxy,k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,idxy,k+d)]);
                                                                  }
                                                                arraySection[index3D(idxx,idxy,k)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }

                                        if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                        {
                                                           for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),k)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d > idxy) ? old_haloYTop[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(d-idxy-1),k,(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                    : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),k)]) +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d > idxx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idxx)),core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1),k,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                    : old_arraySection[index3D((idxx-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k+d)]);
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),k)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d > idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(idxx,(d-idxy-1),k,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                    : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),k)]) +
                                                                     /* array[Z][Y][X-1] */ 
                                                                        ((d > idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1),k,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                    : old_arraySection[index3D((idxx-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)]) +
                                                                     /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k+d)]);
                                                                  }
                                                                arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }
                                        if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][0] == true))
                                           {
                                          // processor boundary condition enforced here (YZ upper corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                        {
                                                           for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_haloYBottom[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(old_array.get_haloWidth(1)-(d-idxy)),k,(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),k)]) +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),k)] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,k)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_haloXTop[coreID][otherCore_index3D((d-idxx-1),idxy,k,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,k)]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,k+d)]) ;
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),k,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),k)]) +
                                                                     /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),k)] +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,k)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),idxy,k,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,k)]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,k+d)]) ;
                                                                  }
                                                                arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,k)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }
                                        if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][1] == true))
                                           {
                                          // processor boundary condition enforced here (YZ bottom corner)
                                           }
                                          else
                                           {
                                             if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0))
                                                {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                     if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                        {
                                                           for (int k = dist; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist; k++)
                                                              {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {

                                                                if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID])
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),k)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_haloYTop[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(d-idxy-1),k,(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),k)]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_haloXTop[coreID][otherCore_index3D((d-idxx-1),(core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1)),k,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k+d)]);
                                                                else 
                                                                     tmp += 
                                                                   ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),k)] +
                                                                     /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]-(idxx+1),(d-idxy-1),k,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),k)]) +
                                                                     /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] +
                                                                     /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1),k,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)]) +
                                                                     /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k-d)] +
                                                                     /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k+d)]);
                                                                  }
                                                                arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),k)] = tmp / (6.0*dist); 
                                                                }
                                                              }
                                                        }
                                                     else
                                                        {
                                                        }
                                                    }
                                                 else
                                                    {
                                                    }
                                                }
                                             else
                                                {
                                                }
                                           }
                                  }
                               else
                                  {
                                  }
                                // ********************
                                // End of edge updates
                                // ********************

                                // ********************
                                // corners updates
                                // ********************
                                  if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][0] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                  if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_haloYBottom[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(old_array.get_haloWidth(1)-(d-idxy)),idxz,(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(idxx,(idxy-d),idxz)]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,(idxy+d),idxz)] +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idxx)),idxy,idxz,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,idxz)]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,idxz)] +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZBottom[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,old_array.get_haloWidth(1)+idxy,(old_array.get_haloWidth(2)-(d-idxz)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(0)))] 
                                                                                  : old_arraySection[index3D(idxx,idxy,(idxz-d))] ) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,idxy,(idxz+d))] ) ;
                                                                  else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),idxz,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,(idxy-d),idxz)]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,(idxy+d),idxz)] +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),idxy,idxz,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,idxz)]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,idxz)] +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,idxy,(idxz-d))] ) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,idxy,(idxz+d))] ) ;
                                                                    }
                                                                   arraySection[index3D(idxx,idxy,idxz)] = tmp / (6.0*dist) ; 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][1] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                  if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_haloYBottom[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(old_array.get_haloWidth(1)-(d-idxy)),(core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1)),(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(idxx,(idxy-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,(idxy+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idxx)),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1),old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] + 
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZTop[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,old_array.get_haloWidth(1)+idxy,(d-idxz-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                  else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,(idxy-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(idxx,(idxy+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] + 
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(idxx,idxy,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                   arraySection[index3D(idxx,idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][0] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                  if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_haloYTop[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(d-idxy-1),idxz,(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idxx)),(core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1)),idxz,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D(idxx-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D(idxx+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZBottom[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+old_array.get_haloWidth(1)-(idxy+1)),old_array.get_haloWidth(2)-(d-idxz),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))]);
                                                                  else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(idxx,(d-idxy-1),idxz,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1),idxz,core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D(idxx-d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D(idxx+d,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[0][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))]);
                                                                     }
                                                                   arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] = tmp / (6.0*dist);
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][0] == true) || (core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][1] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][0][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                  if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxz+1))] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_haloYTop[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(d-idxy-1),(core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1)),(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxz+1))]) +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)-(d-idxx)),(core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1)),(core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1)),old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZTop[coreID][otherCore_index3D(old_array.get_haloWidth(0)+idxx,(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+old_array.get_haloWidth(1)-(idxy+1)),(d-idxz-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))]
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                  else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxz+1))] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(idxx,(d-idxy-1),core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxz+1))]) +
                                                                        /* array[Z][Y][X-1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][0]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][1][0][0]-(d-idxx),core->coreArrayNeighborhoodSizes_3D[1][1][0][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][0][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][0][0],core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] 
                                                                                  : old_arraySection[index3D((idxx-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X+1] */ old_arraySection[index3D((idxx+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(idxx,core->coreArrayNeighborhoodSizes_3D[2][1][1][1]-(idxy+1),(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                   arraySection[index3D(idxx,core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][0] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                  {

                                                                  if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_haloYBottom[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(old_array.get_haloWidth(1)-(d-idxy)),idxz,(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  :  old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),idxz)]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),idxz)] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,idxz)] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_haloXTop[coreID][otherCore_index3D((d-idxx-1),idxy,idxz,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  :  old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,idxz)]) +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZBottom[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+old_array.get_haloWidth(0)-(idxx+1)),old_array.get_haloWidth(1)+idxy,(old_array.get_haloWidth(2)-(d-idxz)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,(idxz+d))] );
                                                                  else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),idxz,core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  :  old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),idxz)]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),idxz)] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,idxz)] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),idxy,idxz,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  :  old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,idxz)]) +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,(idxz+d))] );
                                                                    }
                                                                   arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,idxz)] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][0] == true) || (core->boundaryCore_3D[2][1] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][0][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                  if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_haloYBottom[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(old_array.get_haloWidth(1)-(d-idxy)),(core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1)),(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_haloXTop[coreID][otherCore_index3D((d-idxx-1),idxy,(core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1)),old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZTop[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+old_array.get_haloWidth(0)-(idxx+1)),old_array.get_haloWidth(1)+idxy,(d-idxz-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                  else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][0][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][1]-(d-idxy),core->coreArrayNeighborhoodSizes_3D[1][0][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][0][1][0],core->coreArrayNeighborhoodSizes_3D[1][0][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y+1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),(idxy+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]-(idxx+1),idxy,(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                   arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),idxy,core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][0] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[0][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                  if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_haloYTop[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(d-idxy-1),idxz,(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_haloXTop[coreID][otherCore_index3D((d-idxx-1),(core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1)),idxz,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)]) +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZBottom[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+old_array.get_haloWidth(1)-(idxy+1)),(old_array.get_haloWidth(2)-(d-idxz)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))] );
                                                                  else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),idxz)] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]-(idxx+1),(d-idxy-1),idxz,core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),idxz)]) +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1),idxz,core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)]) +
                                                                        /* array[Z-1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[0][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[0][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[0][1][1][2]-(d-idxz),core->coreArrayNeighborhoodSizes_3D[0][1][1][0],core->coreArrayNeighborhoodSizes_3D[0][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz-d))]) +
                                                                        /* array[Z+1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),(idxz+d))] );
                                                                    }
                                                                   arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),idxz)] = tmp / (6.0*dist); 
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }

                                  if ((core->boundaryCore_3D[0][1] == true) || (core->boundaryCore_3D[1][1] == true) || (core->boundaryCore_3D[2][1] == true))
                                     {
                                    // processor boundary condition enforced here (YZ upper corner)
                                     }
                                  else
                                     {
                                         if ((core->coreArrayNeighborhoodSizes_3D[1][1][2][0] > 0) && (core->coreArrayNeighborhoodSizes_3D[1][2][1][1] > 0) && (core->coreArrayNeighborhoodSizes_3D[2][1][1][2] > 0))
                                            {
                                                 if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 1)
                                                    {
                                                        if (core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 1)
                                                           {
                                                             if (core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 1)
                                                                {
                                                                for(int idxx=0; idxx <dist; idxx++)
                                                                for(int idxy=0; idxy <dist; idxy++)
                                                                for(int idxz=0; idxz <dist; idxz++)
                                                                {
                                                                  T tmp(0.0);
                                                                  for(int d=1; d <=dist; d++)
                                                                    {
                                                                  if(old_array.get_haloSectionSizes(0)[coreID] && old_array.get_haloSectionSizes(1)[coreID] && old_array.get_haloSectionSizes(2)[coreID])
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_haloYTop[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(d-idxy-1),(core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1)),(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_haloXTop[coreID][otherCore_index3D((d-idxx-1),(core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1)),(core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1)),old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_haloZTop[coreID][otherCore_index3D((core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+old_array.get_haloWidth(0)-(idxx+1)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+old_array.get_haloWidth(1)-(idxy+1)),(d-idxz-1),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(0)))]
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                  else 
                                                                     tmp += 
                                                                      ( /* array[Z][Y-1][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y+1][X] */ 
                                                                        ((d>idxy) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][2][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]-(idxx+1),(d-idxy-1),core->coreArrayNeighborhoodSizes_3D[1][2][1][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][2][1][0],core->coreArrayNeighborhoodSizes_3D[1][2][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z][Y][X-1] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1+d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] +
                                                                        /* array[Z][Y][X+1] */ 
                                                                        ((d>idxx) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[1][1][2]][otherCore_index3D((d-idxx-1),core->coreArrayNeighborhoodSizes_3D[1][1][2][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][2][2]-(idxz+1),core->coreArrayNeighborhoodSizes_3D[1][1][2][0],core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1-d),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))]) +
                                                                        /* array[Z-1][Y][X] */ old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1+d))] +
                                                                        /* array[Z+1][Y][X] */ 
                                                                        ((d>idxz) ? old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[2][1][1]][otherCore_index3D(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[2][1][1][1]-(idxy+1),(d-idxz-1),core->coreArrayNeighborhoodSizes_3D[2][1][1][0],core->coreArrayNeighborhoodSizes_3D[2][1][1][1])] 
                                                                                  : old_arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1-d))]));
                                                                    }
                                                                   arraySection[index3D(core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-(idxx+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-(idxy+1),core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-(idxz+1))] = tmp / (6.0*dist);
                                                                }
                                                                }
                                                             else
                                                                {
                                                                }
                                                           }
                                                        else
                                                           {
                                                           }
                                                    }
                                                 else
                                                    {
                                                    }
                                            }
                                     }
                                // ********************
                                // End of corner updates
                                // ********************


                        }
                     else
                        {
#if 0
                           printf ("This array segment can't be processed for edge handling because it is too small in at least one axis: p = %d size = (%d,%d,%d) \n",p,core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][1][1],core->coreArrayNeighborhoodSizes_2D[1][1][2]);
#endif
                        // assert(false);
                        }
#if 0
                 // This is required to avoid valgrind reported errors on some blocks where the local (sectionSize[dim]) is zero.
                 // This is likely because of over flow from size_t type veraibles.
                    if (core->coreArrayNeighborhoodSizes_3D[1][1][1][0] > 2 && core->coreArrayNeighborhoodSizes_3D[1][1][1][1] > 2 && core->coreArrayNeighborhoodSizes_3D[1][1][1][2] > 2)
                       {
                         for (int k = 1; k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-1; k++)
                            {
                              for (int j = 1; j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-1; j++)
                                 {
                                   for (int i = 1; i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-1; i++)
                                      {
                                     // This is the dominant computation for each array section per core. The compiler will use the 
                                     // user's code to derive the code that will be put here.
#if 0
                                        printf ("p= %d Indexing 3D array (i,j,k) = (%d,%d,%d) \n",p,i,j,k);
#endif
#if 0
                                        arraySection[index3D(i,j,k)] = 
                                           (old_arraySection[index3D(i-1,j-1,k-1)] + old_arraySection[index3D(i+1,j-1,k-1)] + 
                                            old_arraySection[index3D(i-1,j+1,k-1)] + old_arraySection[index3D(i+1,j+1,k-1)] + 
                                            old_arraySection[index3D(i-1,j-1,k+1)] + old_arraySection[index3D(i+1,j-1,k+1)] + 
                                            old_arraySection[index3D(i-1,j+1,k+1)] + old_arraySection[index3D(i+1,j+1,k+1)]) / 8.0;
#endif
                                      }
                                 }
                            }
                       }
#endif
                  }
                 else
                  {
#if 0
                    printf ("3D array too small (still no interior) \n");
#endif
                  }
             }
            else
             {
               if (arraySizeZ == 2)
                  {
#if 0
                    printf ("3D array (with size 2 in Z axis) too small (still no interior) \n");
#endif
                  }
                 else
                  {
                    if (arraySizeY > 2)
                       {
                         if (arraySizeX > 2) 
                            {
                           // This is the case of 2D relaxation (along edges)
                              T** old_haloXBottom  = old_array.get_haloSectionPointers(0,0);
                              T** old_haloXTop  = old_array.get_haloSectionPointers(0,1);
                              T** old_haloYBottom  = old_array.get_haloSectionPointers(1,0);
                              T** old_haloYTop  = old_array.get_haloSectionPointers(1,1);
#if 0
                              printf ("This is the case of 2D relaxation \n");
                              printf ("This needs to use sectionSize[0-1] to get the local size instead of the global size! \n");
#endif

#if 1
                           // The core array may higher dimensional then the array and if so then the local size along
                           // the Z axis may be zero.  If so, then we don't want to process the local array section.
                           // if ((core->coreArrayNeighborhoodSizes_2D[1][1][0] >= 2 || core->coreArrayNeighborhoodSizes_2D[1][1][1] >= 2) && core->coreArrayNeighborhoodSizes_2D[1][1][2] == 1)
                              if ((core->coreArrayNeighborhoodSizes_2D[1][1][0] >= 1 || core->coreArrayNeighborhoodSizes_2D[1][1][1] >= 1) && core->coreArrayNeighborhoodSizes_2D[1][1][2] == 1)
                                 {
                                // Handle the internal boundary equations along edges of the 2D arrays.

                                // ***************************************
                                // Now process the edges along the X axis.
                                // ***************************************

                                // if (sectionSize[1] > 1)
                                   if (core->coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                      {
#if 0
                                        printf ("-- leftEdgeSection[1] = %s rightEdgeSection[1] = %s \n",leftEdgeSection[1] ? "true" : "false",rightEdgeSection[1] ? "true" : "false");
#endif
                                     // if (leftEdgeSection[1] == true)
                                        if (core->boundaryCore_2D[1][0] == true)
                                           {
#if 0
                                             printf ("--- Apply the 2D array abstraction's UPPER boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
#if 0
                                             printf ("apply 2D equation at left edge of memory segment core->coreArrayNeighborhoodSizes_2D[0][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[0][1][1]);
#endif
                                          // if (previous_sectionSize[1] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0)
                                                {
                                               // Upper edge
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | *XXXX* | *****
                                               // ***** | ****** | *****
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****

                                               // arraySection[0] = (old_arraySectionPointers[previous_coreIndexInLinearArray][previous_sectionSize[0]-1] + old_arraySection[1]) / 2.0;
                                               // for (int i = 1; i < sectionSize[0]-1; i++)
                                                  for (int i = 1; i < core->coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                     {
                                                    // arraySection[index2D(i,0)] = (old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(i-1,previous_sectionSize[1]-1)] + old_arraySection[index2D(i-1,1)] +
                                                    //                               old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(i+1,previous_sectionSize[1]-1)] + old_arraySection[index2D(i+1,1)]) / 4.0;
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[0][1]])
                                                         arraySection[index2D(i,0)] = 
                                                            ( /* array[Y-1][X] */ old_haloYBottom[coreID][otherCore_index2D((i+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)-1),(core->coreArrayNeighborhoodSizes_2D[0][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                              /* array[Y+1][X] */ old_arraySection[index2D(i,1)] +
                                                              /* array[Y][X-1] */ old_arraySection[index2D(i-1,0)] + 
                                                              /* array[Y][X+1] */ old_arraySection[index2D(i+1,0)]) / 4.0;
                                                       else
                                                         arraySection[index2D(i,0)] = 
                                                            ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(i,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                              /* array[Y+1][X] */ old_arraySection[index2D(i,1)] +
                                                              /* array[Y][X-1] */ old_arraySection[index2D(i-1,0)] + 
                                                              /* array[Y][X+1] */ old_arraySection[index2D(i+1,0)]) / 4.0;
                                                     }
                                                }
                                           }

                                     // if (rightEdgeSection[1] == true)
                                        if (core->boundaryCore_2D[1][1] == true)
                                           {
#if 0
                                             printf ("--- Apply the array abstraction's LOWER boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
                                          // center_stencil_cell_rightEdge = (left_stencil_cell_rightEdge + right_stencil_cell_rightEdge) / 2.0;
#if 0
                                             printf ("apply 2D equation at right edge of memory segment core->coreArrayNeighborhoodSizes_2D[2][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[2][1][1]);
#endif
                                          // if (next_sectionSize[1] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[2][1][1] > 0)
                                                {
                                               // Lower edge
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****
                                               // ***** | ****** | *****
                                               // ***** | *XXXX* | *****
                                               // ----------------------
                                               // ***** | ****** | *****

                                               // for (int i = 1; i < sectionSize[0]-1; i++)
                                                  for (int i = 1; i < core->coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                     {
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[2][1]])
                                                         arraySection[index2D(i,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                            ( /* array[Y-1][X] */ old_arraySection[index2D(i,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                              /* array[Y+1][X] */ old_haloYTop[coreID][otherCore_index2D(i+old_array.get_haloWidth(0),0,(core->coreArrayNeighborhoodSizes_2D[2][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                              /* array[Y][X-1] */ old_arraySection[index2D(i-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] +
                                                              /* array[Y][X+1] */ old_arraySection[index2D(i+1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
                                                       else
                                                         arraySection[index2D(i,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                            ( /* array[Y-1][X] */ old_arraySection[index2D(i,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                              /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(i,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                              /* array[Y][X-1] */ old_arraySection[index2D(i-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] +
                                                              /* array[Y][X+1] */ old_arraySection[index2D(i+1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
                                                     }
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Trivial case of only one equation (define this to be left edge; use the associated references).
                                     // if (sectionSize[1] == 1)
                                        if (core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                           {
#if 0
                                             printf ("--- Trivial case of only one 2D equation (define this to be UPPER edge) \n");
                                             printf ("--- core->boundaryCore_2D[1][0] = %s core->boundaryCore_2D[1][1] = %s \n",core->boundaryCore_2D[1][0] ? "true" : "false",core->boundaryCore_2D[1][1] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[1] == false && rightEdgeSection[1] == false)
                                             if (core->boundaryCore_2D[1][0] == false && core->boundaryCore_2D[1][1] == false)
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // if (previous_sectionSize[1] > 0 && next_sectionSize[1] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0 && core->coreArrayNeighborhoodSizes_2D[2][1][1] > 0)
                                                     {
                                                    // Upper and Lower edges are the same
                                                    // ***** | ****** | *****
                                                    // ----------------------
                                                    // ***** | *XXXX* | *****
                                                    // ----------------------
                                                    // ***** | ****** | *****

#if 0
                                                       printf ("--- Processing trivial case of only one equation 2D (edge in X axis) \n");
#endif
                                                    // for (int i = 1; i < sectionSize[0]-1; i++)
                                                       for (int i = 1; i < core->coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                          {
                                                            if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[0][1]] && old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[2][1]])
                                                              arraySection[index2D(i,0)] = 
                                                                 ( /* array[Y-1][X] */ old_haloYBottom[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(i+old_array.get_haloWidth(0),(old_array.get_haloWidth(1)-1),(core->coreArrayNeighborhoodSizes_2D[0][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                   /* array[Y+1][X] */ old_haloYTop[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(i+old_array.get_haloWidth(0),0,(core->coreArrayNeighborhoodSizes_2D[2][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                   /* array[Y][X-1] */ old_arraySection[index2D(i-1,0)] + 
                                                                   /* array[Y][X+1] */ old_arraySection[index2D(i+1,0)]) / 4.0;
                                                            else
                                                              arraySection[index2D(i,0)] = 
                                                                 ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(i,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                   /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(i,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                   /* array[Y][X-1] */ old_arraySection[index2D(i-1,0)] + 
                                                                   /* array[Y][X+1] */ old_arraySection[index2D(i+1,0)]) / 4.0;
                                                          }
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // assert(sectionSize[1] == 0);
                                             assert(core->coreArrayNeighborhoodSizes_2D[1][1][1] == 0);
#if 0
                                             printf ("--- core->coreArrayNeighborhoodSizes_2D[1][1][1] == 0: This is the trival case \n");
#endif
                                           }
                                      }

#if 1
                                // ***************************************
                                // Now process the edges along the Y axis.
                                // ***************************************
#if 0
                                   printf ("---+++ Process the edges of the memory section on core index = %d sectionSize[0] = %d previous_sectionSize[0] = %d next_sectionSize[0] = %d \n",p,sectionSize[0],previous_sectionSize[0],next_sectionSize[0]);
#endif
                                // if (sectionSize[0] > 1)
                                   if (core->coreArrayNeighborhoodSizes_2D[1][1][0] > 1)
                                      {
#if 0
                                        printf ("---+++ leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                     // if (leftEdgeSection[0] == true)
                                        if (core->boundaryCore_2D[0][0] == true)
                                           {
#if 0
                                             printf ("---+++ Apply the array abstraction's LEFT boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
                                          // center_stencil_cell_leftEdge = (left_stencil_cell_leftEdge + right_stencil_cell_leftEdge) / 2.0;
#if 0
                                             printf ("apply equation at left edge of memory segment core->coreArrayNeighborhoodSizes_2D[1][0][0] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][0][0]);
#endif
                                          // if (previous_sectionSize[0] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0)
                                                {
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****
                                               // ***** | X***** | *****
                                               // ***** | X***** | *****
                                               // ***** | X***** | *****
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****

                                               // for (int j = 1; j < sectionSize[1]-1; j++)
                                                  for (int j = 1; j < core->coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                     {
#if 1
                                                       if(old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][0]])
                                                         arraySection[index2D(0,j)] = 
                                                            ( /* array[Y-1][X] */ old_arraySection[index2D(0,j-1)] +
                                                              /* array[Y+1][X] */ old_arraySection[index2D(0,j+1)] +
                                                              /* array[Y][X-1] */ old_haloXBottom[coreID][otherCore_index2D((old_array.get_haloWidth(0)-1),j,(old_array.get_haloWidth(0)))] + 
                                                              /* array[Y][X+1] */ old_arraySection[index2D(1,j)]) / 4.0;
                                                       else
                                                         arraySection[index2D(0,j)] = 
                                                            ( /* array[Y-1][X] */ old_arraySection[index2D(0,j-1)] +
                                                              /* array[Y+1][X] */ old_arraySection[index2D(0,j+1)] +
                                                           // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,j)] + 
                                                              /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,j,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                              /* array[Y][X+1] */ old_arraySection[index2D(1,j)]) / 4.0;
#endif
                                                     }
                                                }
                                           }

                                     // if (rightEdgeSection[0] == true)
                                        if (core->boundaryCore_2D[0][1] == true)
                                           {
#if 0
                                             printf ("---+++ Apply the array abstraction's RIGHT boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
                                          // center_stencil_cell_rightEdge = (left_stencil_cell_rightEdge + right_stencil_cell_rightEdge) / 2.0;
#if 0
                                             printf ("apply equation at right edge of memory segment core->coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][2][0]);
#endif
                                          // if (next_sectionSize[0] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                {
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****
                                               // ***** | *****X | *****
                                               // ***** | *****X | *****
                                               // ***** | *****X | *****
                                               // ***** | ****** | *****
                                               // ----------------------
                                               // ***** | ****** | *****

                                               // for (int j = 1; j < sectionSize[1]-1; j++)
                                                  for (int j = 1; j < core->coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                     {
                                                    // arraySection[index2D(sectionSize[0]-1,j)] = (old_arraySection[index2D(sectionSize[0]-2,j-1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j-1)] +
                                                    //                                              old_arraySection[index2D(sectionSize[0]-2,j+1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j+1)]) / 4.0;
#if 0
                                                       printf ("array[Y][X]:   old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)]           = %f \n",old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)]);
                                                       printf ("array[Y-1][X]: old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)]          = %f \n",old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)]);
                                                       printf ("array[Y+1][X]: old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)]          = %f \n",old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)]);
                                                       printf ("array[Y][X-1]: old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)]            = %f \n",old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)]);
                                                       printf ("p = %d core->coreArrayNeighborhoodLinearized_2D[1][2] = %d \n",p,core->coreArrayNeighborhoodLinearized_2D[1][2]);
                                                       printf ("p = %d core->coreArrayNeighborhoodSizes_2D[1][1][0] = %d \n",p,core->coreArrayNeighborhoodSizes_2D[1][1][0]);
                                                       printf ("p = %d core->coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",p,core->coreArrayNeighborhoodSizes_2D[1][2][0]);
                                                    // printf ("array[Y][X+1]: old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)] = %f \n",old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]);
                                                       printf ("array[Y][X+1]: old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,core->coreArrayNeighborhoodSizes_2D[1][2][0])] = %f \n",
                                                            old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,core->coreArrayNeighborhoodSizes_2D[1][2][0])]);
#endif
#if 1
// This fails for some random problem...
                                                       if(old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][2]])
                                                         arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)] = 
                                                            ( /* array[Y-1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)] + 
                                                              /* array[Y+1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)] +
                                                              /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)] + 
                                                              /* array[Y][X+1] */ old_haloXTop[coreID][otherCore_index2D(0,j,(old_array.get_haloWidth(0)))]) / 4.0; 
                                                       else 
                                                         arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)] = 
                                                            ( /* array[Y-1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)] + 
                                                              /* array[Y+1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)] +
                                                              /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)] + 
                                                           // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]) / 4.0;
                                                              /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                     }
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Trivial case of only one equation (define this to be left edge; use the associated references).
                                     // if (sectionSize[0] == 1)
                                        if (core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1)
                                           {
#if 0
                                             printf ("---+++ Trivial case of only one equation (define this to be left edge; use the associated references) \n");
                                             printf ("---+++ leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                             if (core->boundaryCore_2D[0][0] == false && core->boundaryCore_2D[0][1] == false)
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0 && core->coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                     {
                                                    // ***** | * | *****
                                                    // ----------------------
                                                    // ***** | * | *****
                                                    // ***** | X | *****
                                                    // ***** | X | *****
                                                    // ***** | X | *****
                                                    // ***** | * | *****
                                                    // ----------------------
                                                    // ***** | * | *****

#if 0
                                                       printf ("---+++ Processing trivial case of only one equation \n");
#endif
                                                    // for (int j = 1; j < sectionSize[1]-1; j++)
                                                       for (int j = 1; j < core->coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                          {
                                                         // arraySection[index2D(0,j)] = (old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(previous_sectionSize[0]-1,j-1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j-1)] +
                                                         //                               old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(previous_sectionSize[0]-1,j+1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j+1)]) / 4.0;
#if 1
                                                       if(old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][0]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][2]])
                                                            arraySection[index2D(0,j)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(0,j-1)] +
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(0,j+1)] +
                                                                 /* array[Y][X-1] */ old_haloXBottom[coreID][otherCore_index2D((old_array.get_haloWidth(0)-1),j,(old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y][X+1] */ old_haloXTop[coreID][otherCore_index2D(0,j,(old_array.get_haloWidth(0)))]) / 4.0; 
                                                       else
                                                            arraySection[index2D(0,j)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(0,j-1)] +
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(0,j+1)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,j)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,j,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // assert(sectionSize[0] == 0);
                                             assert(core->coreArrayNeighborhoodSizes_2D[1][1][0] == 0);
#if 0
                                             printf ("---+++ core->coreArrayNeighborhoodSizes_2D[1][0][0] == 0: This is the trival case \n");
#endif
                                           }
                                      }
                                // ********************
                                // End of Y Axis update
                                // ********************
#endif



#if 1
                                // ********************************************
                                // Now process the corners of the X and Y axis.
                                // ********************************************
#if 0
                                   printf ("---+++ Process the edges of the memory section on core p = %d core->coreArrayNeighborhoodSizes_2D[1][1][0] = %d core->coreArrayNeighborhoodSizes_2D[1][0][0] = %d core->coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",
                                        p,core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][0][0],core->coreArrayNeighborhoodSizes_2D[1][2][0]);
                                   printf ("Sizes of current processor: core->coreArrayNeighborhoodSizes_2D[1][1] = (%d,%d,%d) \n",core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][1][1],core->coreArrayNeighborhoodSizes_2D[1][1][2]);
#endif
                                // First X Axis logic
                                   if (core->coreArrayNeighborhoodSizes_2D[1][1][0] > 1)
                                      {
                                     // Left sice corners
                                        if (core->boundaryCore_2D[0][0] == true)
                                           {
                                          // processor boundary condition enforced here (X axis)
                                           }
                                          else
                                           {
                                             if (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0)
                                                {
                                               // Next Y Axis logic
                                                  if (core->coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                                     {
                                                    // Upper corner
                                                       if (core->boundaryCore_2D[1][0] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][0] > 0);
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0);

                                                         // Upper left corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | X***** | *****
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[0][1]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][0]])
                                                            arraySection[index2D(0,0)] = 
                                                               ( /* array[Y-1][X] */ old_haloYBottom[coreID][otherCore_index2D(0+old_array.get_haloWidth(0),(old_array.get_haloWidth(1)-1),(core->coreArrayNeighborhoodSizes_2D[0][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(1,0)] +
                                                                 /* array[Y][X-1] */ old_haloXBottom[coreID][otherCore_index2D((old_array.get_haloWidth(0)-1),0,(old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(0,1)]) / 4.0;
                                                       else
                                                            arraySection[index2D(0,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(1,0)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(0,1)]) / 4.0;
#endif
                                                          }

                                                    // Lower corner
                                                       if (core->boundaryCore_2D[1][1] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0);
                                                            assert (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0);

                                                         // Lower left corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ***** | X***** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 0
                                                            printf ("--- array[Y][X]:   arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
                                                            printf ("old_array[Y][X]:   old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
                                                            printf ("old_array[Y-1][X]: old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] = %f \n",old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)]);
                                                            printf ("old_array[Y+1][X]: old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)] = %f \n",old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)]);
                                                            printf ("old_array[Y][X-1]: old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,core->coreArrayNeighborhoodSizes_2D[1][0][1]-1)] = %f \n",
                                                                 old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,core->coreArrayNeighborhoodSizes_2D[1][0][1]-1)]);
                                                            printf ("array[Y][X+1]: old_arraySection[index2D(1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",old_arraySection[index2D(1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
#endif
#if 1
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[2][1]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][0]])
                                                            arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                                 /* array[Y+1][X] */ old_haloYTop[coreID][otherCore_index2D(0+old_array.get_haloWidth(0),0,(core->coreArrayNeighborhoodSizes_2D[2][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y][X-1] */ old_haloXBottom[coreID][otherCore_index2D((old_array.get_haloWidth(0)-1),(core->coreArrayNeighborhoodSizes_2D[1][0][1]-1),(old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
                                                       else
                                                            arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,core->coreArrayNeighborhoodSizes_2D[1][0][1]-1)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,core->coreArrayNeighborhoodSizes_2D[1][0][1]-1,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
#endif
#if 0
                                                            printf ("--- array[Y][X]: arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",arraySection[index2D(0,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
#endif
                                                          }
                                                     }
                                                    else
                                                     {
                                                    // printf ("core->coreArrayNeighborhoodSizes_2D[1][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][1]);
                                                       if (core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                                          {
                                                         // Case of upper and lower left corners are the same point
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | X***** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[2][1]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][0]] && old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[2][0]])
                                                            arraySection[index2D(0,0)] = 
                                                               ( /* array[Y-1][X] */ old_haloYBottom[coreID][otherCore_index2D(0+old_array.get_haloWidth(0),(old_array.get_haloWidth(1)-1),(core->coreArrayNeighborhoodSizes_2D[0][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y+1][X] */ old_haloYTop[coreID][otherCore_index2D(0+old_array.get_haloWidth(0),0,(core->coreArrayNeighborhoodSizes_2D[2][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y][X-1] */ old_haloXBottom[coreID][otherCore_index2D((old_array.get_haloWidth(0)-1),0,(old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
                                                       else
                                                            arraySection[index2D(0,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[1][2][0])] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,core->coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(1,0)]) / 4.0;
#endif
                                                          }
                                                     }
                                                }
                                               else
                                                {
                                                  printf ("We don't support the size on the adjacent being zero! \n");
                                                  assert(false);
                                                }
                                           }

                                     // Right side corners
                                        if (core->boundaryCore_2D[0][1] == true)
                                           {
                                          // Can we test if this is realy a boundary?
                                           }
                                          else
                                           {
                                          // if (next_sectionSize[0] > 0)
                                             if (core->coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                {
                                               // printf ("Right boundary corner not implemented! \n");

                                               // Next Y Axis logic
                                                  if (core->coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                                     {
                                                    // Upper corner
                                                       if (core->boundaryCore_2D[1][0] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][0] > 0);
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0);

                                                         // Upper right corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | *****X | *****
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[0][1]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][2]])
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                               ( /* array[Y-1][X] */ old_haloYBottom[coreID][otherCore_index2D((core->coreArrayNeighborhoodSizes_2D[0][1][0]-1+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)-1),(core->coreArrayNeighborhoodSizes_2D[0][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,1)] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,0)] +
                                                                 /* array[Y][X+1] */ old_haloXTop[coreID][otherCore_index2D(0,0,(old_array.get_haloWidth(0)))]) / 4.0; 
                                                       else
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][index2D(core->coreArrayNeighborhoodSizes_2D[0][1][0]-1,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[0][1][0]-1,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,1)] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,0)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }

                                                    // Lower corner
                                                       if (core->boundaryCore_2D[1][1] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (core->coreArrayNeighborhoodSizes_2D[0][1][1] > 0);
                                                            assert (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0);

                                                         // Lower right corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ***** | *****X | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[2][1]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][2]])
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                                 /* array[Y+1][X] */ old_haloYTop[coreID][otherCore_index2D((core->coreArrayNeighborhoodSizes_2D[2][1][0]-1+old_array.get_haloWidth(0)),0,(core->coreArrayNeighborhoodSizes_2D[2][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] +
                                                                 /* array[Y][X+1] */ old_haloXTop[coreID][otherCore_index2D(0,(core->coreArrayNeighborhoodSizes_2D[2][1][1]-1),(old_array.get_haloWidth(0)))]) / 4.0; 
                                                       else
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,core->coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(core->coreArrayNeighborhoodSizes_2D[2][1][0]-1,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[2][1][0]-1,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,core->coreArrayNeighborhoodSizes_2D[1][1][1]-1)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,core->coreArrayNeighborhoodSizes_2D[2][1][1]-1)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,core->coreArrayNeighborhoodSizes_2D[2][1][1]-1,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }
                                                     }
                                                    else
                                                     {
                                                    // printf ("core->coreArrayNeighborhoodSizes_2D[1][1][1] = %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][1]);
                                                       if (core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                                          {
                                                         // Case of upper and lower right corners are the same point
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | *****X | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[0][1]] && old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[2][1]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][2]])
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                               ( /* array[Y-1][X] */ old_haloYBottom[coreID][otherCore_index2D((core->coreArrayNeighborhoodSizes_2D[0][1][0]-1+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)-1),(core->coreArrayNeighborhoodSizes_2D[0][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y+1][X] */ old_haloYTop[coreID][otherCore_index2D((core->coreArrayNeighborhoodSizes_2D[2][1][0]-1+old_array.get_haloWidth(0)),0,(core->coreArrayNeighborhoodSizes_2D[2][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,0)] +
                                                                 /* array[Y][X+1] */ old_haloXTop[coreID][otherCore_index2D(0,0,(old_array.get_haloWidth(0)))]) / 4.0; 
                                                       else
                                                            arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][index2D(core->coreArrayNeighborhoodSizes_2D[0][1][0]-1,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1)] +
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[0][1][0]-1,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] +
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][index2D(core->coreArrayNeighborhoodSizes_2D[2][1][0]-1,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[2][1][0]-1,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-2,0)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }
                                                     }
                                                }
                                               else
                                                {
                                                  printf ("We don't support the size on the adjacent being zero! \n");
                                                  assert(false);
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Trivial case of only one equation (define this to be left edge; use the associated references).
#if 0
                                        printf ("Case of core->coreArrayNeighborhoodSizes_2D[1][1][0] == %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][0]);
                                        printf ("Case of core->coreArrayNeighborhoodSizes_2D[1][1][1] == %d \n",core->coreArrayNeighborhoodSizes_2D[1][1][1]);
#endif
                                     // assert(core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1);
                                     // assert(core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1);

                                     // if (sectionSize[0] == 1)
                                     // if (core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1)
                                        if (core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1 && core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                           {
                                          // printf ("Case of core->coreArrayNeighborhoodSizes_2D[1][1][0] == 1 && core->coreArrayNeighborhoodSizes_2D[1][1][1] == 1\n");

                                          // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                          // if (core->boundaryCore_2D[0][0] == false && core->boundaryCore_2D[0][1] == false)
                                             if (core->boundaryCore_2D[0][0] == false && core->boundaryCore_2D[0][1] == false && core->boundaryCore_2D[1][0] == false && core->boundaryCore_2D[1][1] == false)
                                                {
                                               // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_2D[1][0][0] > 0 && core->coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                     {
                                                    // printf ("Case of single point boundary not implemented! \n");
                                                    // ***** | * | *****
                                                    // -----------------
                                                    // ***** | X | *****
                                                    // -----------------
                                                    // ***** | * | *****
#if 1
                                                       if(old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[0][1]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][0]] &&
                                                          old_array.get_haloSectionSizes(1)[core->coreArrayNeighborhoodLinearized_2D[2][1]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_2D[1][2]])
                                                       arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                          ( /* array[Y-1][X] */ old_haloYBottom[coreID][otherCore_index2D((0+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)-1),(core->coreArrayNeighborhoodSizes_2D[0][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                            /* array[Y+1][X] */ old_haloYTop[coreID][otherCore_index2D((0+old_array.get_haloWidth(0)),0,(core->coreArrayNeighborhoodSizes_2D[2][1][0]+2*old_array.get_haloWidth(0)))] + 
                                                            /* array[Y][X-1] */ old_haloXBottom[coreID][otherCore_index2D((old_array.get_haloWidth(0)-1),(old_array.get_haloWidth(1)-1),(old_array.get_haloWidth(0)))] + 
                                                            /* array[Y][X+1] */ old_haloXTop[coreID][otherCore_index2D(0,0,(old_array.get_haloWidth(0)))]) / 4.0; 
                                                       else
                                                       arraySection[index2D(core->coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                          ( /* array[Y-1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,core->coreArrayNeighborhoodSizes_2D[0][1][1]-1,core->coreArrayNeighborhoodSizes_2D[0][1][0])] +
                                                            /* array[Y+1][X] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                            /* array[Y][X-1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(core->coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,core->coreArrayNeighborhoodSizes_2D[1][0][0])] +
                                                            /* array[Y][X+1] */ old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,core->coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                     }

#if 0
                                                  printf ("Exiting as a test! \n");
                                                  assert(false);
#endif
                                                }
                                           }
                                          else
                                           {
                                          // assert(sectionSize[0] == 0);
                                             if (core->coreArrayNeighborhoodSizes_2D[1][1][0] != 0)
                                                {
#if 0
                                                   printf ("Warning: p = %d core->coreArrayNeighborhoodSizes_2D[1][1][0] = %d \n",p,core->coreArrayNeighborhoodSizes_2D[1][1][0]);
#endif
                                                }
                                          // assert(core->coreArrayNeighborhoodSizes_2D[1][1][0] == 0);
                                             assert(core->coreArrayNeighborhoodSizes_2D[1][1][0] <= 1);
#if 0
                                             printf ("---+++ core->coreArrayNeighborhoodSizes_2D[1][0][0] == 0: This is the trival case \n");
#endif
                                           }
                                      }

                                // **************************************************
                                // End of processing the corners of the X and Y axis.
                                // **************************************************
#endif
                                 }
                                else
                                 {
#if 0
                                   printf ("This array segment can't be processed for edge handling because it is too small in at least one axis: p = %d size = (%d,%d,%d) \n",p,core->coreArrayNeighborhoodSizes_2D[1][1][0],core->coreArrayNeighborhoodSizes_2D[1][1][1],core->coreArrayNeighborhoodSizes_2D[1][1][2]);
#endif
                                // assert(false);
                                 }
                              
#endif
                            }
                           else
                            {
#if 0
                              printf ("2D array too small (still no interior) \n");
#endif
                            }
                       }
                      else
                       {
                         if (arraySizeY == 2)
                            {
#if 0
                              printf ("2D array (with size 2 in Y axis) too small (still no interior) \n");
#endif
                            }
                           else
                            {
                              if (arraySizeX > 2)
                                 {
                                // This is the case of 1D relaxation
                                   T** old_haloXBottom  = old_array.get_haloSectionPointers(0,0);
                                   T** old_haloXTop  = old_array.get_haloSectionPointers(0,1);
#if 0
                                   printf ("--- This is the case of 1D relaxation sectionSize[0] = %d \n",sectionSize[0]);
#endif
                                // The core array may higher dimensional then the array and if so then the local size along either 
                                // the Y or Z axis may be zero.  If so, then we don't want to process the local array section.
                                // if (sectionSize[1] == 1 && sectionSize[2] == 1)
                                // if (sectionSize[0] > 0 && ((sectionSize[1] == 1 && sectionSize[2] == 1) || array.get_tableBasedDistribution() == false))
                                // if (sectionSize[0] > 0 && (sectionSize[1] == 1 && sectionSize[2] == 1) )
                                   if (core->coreArrayNeighborhoodSizes_1D[1][0] > 0 && (core->coreArrayNeighborhoodSizes_1D[1][1] == 1 && core->coreArrayNeighborhoodSizes_1D[1][2] == 1) )
                                      {
#if 0
                                         printf ("--- Process the edges of the memory section on core index = %d sectionSize[0] = %d previous_sectionSize[0] = %d next_sectionSize[0] = %d \n",p,sectionSize[0],previous_sectionSize[0],next_sectionSize[0]);
#endif
                                     // if (sectionSize[0] > 1)
                                        if (core->coreArrayNeighborhoodSizes_1D[1][0] > 1)
                                           {
#if 0
                                             printf ("-- leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[0] == true)
                                             if (core->boundaryCore_1D[0] == true)
                                                {
#if 0
                                                  printf ("--- Apply the array abstraction's LEFT boundary condition \n");
#endif
                                                }
                                               else
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // center_stencil_cell_leftEdge = (left_stencil_cell_leftEdge + right_stencil_cell_leftEdge) / 2.0;
#if 0
                                                  printf ("apply equation at left edge of memory segment previous_sectionSize[0] = %d \n",previous_sectionSize[0]);
#endif
                                               // if (previous_sectionSize[0] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_1D[0][0] > 0)
                                                     {
                                                       if(old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_1D[0]])
                                                         arraySection[0] = (old_haloXBottom[coreID][0] + old_arraySection[1]) / 2.0;
                                                       else
                                                    // arraySection[0] = (old_arraySectionPointers[previous_coreIndexInLinearArray][core->coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySection[1]) / 2.0;
                                                         arraySection[0] = (old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[0]][core->coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySection[1]) / 2.0;
                                                     }
                                                }

                                          // if (rightEdgeSection[0] == true)
                                             if (core->boundaryCore_1D[1] == true)
                                                {
#if 0
                                                  printf ("--- Apply the array abstraction's RIGHT boundary condition \n");
#endif
                                                }
                                               else
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // center_stencil_cell_rightEdge = (left_stencil_cell_rightEdge + right_stencil_cell_rightEdge) / 2.0;
#if 0
                                                  printf ("apply equation at right edge of memory segment next_sectionSize[0] = %d \n",next_sectionSize[0]);
#endif
                                               // if (next_sectionSize[0] > 0)
                                                  if (core->coreArrayNeighborhoodSizes_1D[2][0] > 0)
                                                     {
                                                       if(old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_1D[2]])
                                                         arraySection[core->coreArrayNeighborhoodSizes_1D[1][0]-1] = (old_arraySection[core->coreArrayNeighborhoodSizes_1D[1][0]-2] + old_haloXTop[coreID][0]) / 2.0;
                                                       else
                                                    // arraySection[sectionSize[0]-1] = (old_arraySection[sectionSize[0]-2] + old_arraySectionPointers[next_coreIndexInLinearArray][0]) / 2.0;
                                                         arraySection[core->coreArrayNeighborhoodSizes_1D[1][0]-1] = (old_arraySection[core->coreArrayNeighborhoodSizes_1D[1][0]-2] + old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[2]][0]) / 2.0;
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // Trivial case of only one equation (define this to be left edge; use the associated references).
                                          // if (sectionSize[0] == 1)
                                             if (core->coreArrayNeighborhoodSizes_1D[1][0] == 1)
                                                {
#if 0
                                                  printf ("--- Trivial case of only one equation (define this to be left edge; use the associated references) \n");
                                                  printf ("--- leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                               // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                                  if (core->boundaryCore_1D[0] == false && core->boundaryCore_1D[1] == false)
                                                     {
                                                    // This is where user specific code is places within the compiler transformation.
                                                    // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                       if (core->coreArrayNeighborhoodSizes_1D[0][0] > 0 && core->coreArrayNeighborhoodSizes_1D[2][0] > 0)
                                                          {
#if 0
                                                            printf ("--- Processing trivial case of only one equation \n");
#endif
                                                         // arraySection[0] = (old_arraySectionPointers[previous_coreIndexInLinearArray][previous_sectionSize[0]-1] + old_arraySectionPointers[next_coreIndexInLinearArray][0]) / 2.0;
                                                            if(old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_1D[0]] && old_array.get_haloSectionSizes(0)[core->coreArrayNeighborhoodLinearized_1D[2]])
                                                              arraySection[0] = (old_haloXBottom[coreID][0] + old_haloXTop[coreID][0]) / 2.0;
                                                            else 
                                                              arraySection[0] = (old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[0]][core->coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[2]][0]) / 2.0;
                                                          }
                                                     }
                                                }
                                               else
                                                {
                                               // assert(sectionSize[0] == 0);
                                                  assert(core->coreArrayNeighborhoodSizes_1D[1][0] == 0);
#if 0
                                                  printf ("--- sectionSize[0] == 0: This is the trival case \n");
#endif
                                                }
                                           }
                                      }
                                     else
                                      {
#if 0
                                        printf ("--- The local size for this arraySection is zero in either the Y or Z axis sectionSize[1] = %d sectionSize[2] = %d \n",sectionSize[1],sectionSize[2]);
#endif
                                      }
                                 }
                                else
                                 {
                                // This is array does not have an interior upon which to relax.
#if 0
                                   printf ("--- 1D array too small (still no interior) \n");
#endif
                                 }
                            }
                       }
                  }
             }
#endif

#if 0
     array.display("after relaxation on memory section edges: array");
     old_array.display("after relaxation on memory section edges: old_array");
#endif
   }


template <typename T>
void
relax_on_detachedhalo_boundary_simplified( int coreID, MulticoreArray<T> & array,  MulticoreArray<T> & old_array, int dist )
   {
     const int arraySizeX    = array.get_arraySize(0);
     const int arraySizeY    = array.get_arraySize(1);
     const int arraySizeZ    = array.get_arraySize(2);

     int p = coreID;
     Core<T>* core = array.coreArray[coreID];

  // This lifts out loop invariant portions of the code.
     T** arraySectionPointers = array.get_arraySectionPointers();
     T** old_arraySectionPointers = old_array.get_arraySectionPointers();

     assert(arraySectionPointers != NULL);
     assert(old_arraySectionPointers != NULL);

     T* arraySection     = array.get_arraySectionPointers()[p];
     T* old_arraySection = old_array.get_arraySectionPointers()[p];

#if 0
     printf ("\nIterate over all cores: p = %d arraySection = %p old_arraySection = %p \n",p,arraySection,old_arraySection);
#endif
     assert(arraySection     != NULL);
     assert(old_arraySection != NULL);

      // **************************************************************
       // Fixup internal bounaries of the memory allocated to each core.
       // **************************************************************
#if 0
          printf ("Fixup boundaries: p = %d Array size (%d,%d,%d) sectionSize(%d,%d,%d) coreArray(%d,%d,%d) \n",p,arraySizeX,arraySizeY,arraySizeZ,sectionSize[0],sectionSize[1],sectionSize[2],array.get_coreArraySize(0),array.get_coreArraySize(1),array.get_coreArraySize(2));
#endif
          if (arraySizeZ > (2*dist))
             {
               if (arraySizeY > (2*dist) && arraySizeX > (2*dist)) 
                  {
                 // This is the case of 3D relaxation
#if 0
                    printf ("This is the case of 3D relaxation \n");

                 // Iterate on the interior of the section (non-shared memory operation, local to the closest local memory declared for each core).
                    printf ("This needs to use sectionSize[0-2] to get the local size instead of the global size! \n");
#endif
                     if ((core->coreArrayNeighborhoodSizes_3D[1][1][1][0] >= 1 || core->coreArrayNeighborhoodSizes_3D[1][1][1][1] >= 1) && core->coreArrayNeighborhoodSizes_3D[1][1][1][2] >= 1)
                        {
                           T** old_haloXBottom  = old_array.get_haloSectionPointers(0,0);
                           T** old_haloXTop  = old_array.get_haloSectionPointers(0,1);
                           T** old_haloYBottom  = old_array.get_haloSectionPointers(1,0);
                           T** old_haloYTop  = old_array.get_haloSectionPointers(1,1);
                           T** old_haloZBottom  = old_array.get_haloSectionPointers(2,0);
                           T** old_haloZTop  = old_array.get_haloSectionPointers(2,1);

                           int base_X  = (core->boundaryCore_3D[0][0] == true) ? dist : 0;
                           int bound_X = (core->boundaryCore_3D[0][1] == true) ? core->coreArrayNeighborhoodSizes_3D[1][1][1][0] - (dist): core->coreArrayNeighborhoodSizes_3D[1][1][1][0];
                           int base_Y  = (core->boundaryCore_3D[1][0] == true) ? dist : 0;
                           int bound_Y = (core->boundaryCore_3D[1][1] == true) ? core->coreArrayNeighborhoodSizes_3D[1][1][1][1] - (dist): core->coreArrayNeighborhoodSizes_3D[1][1][1][1];
                           int base_Z  = (core->boundaryCore_3D[2][0] == true) ? dist : 0;
                           int bound_Z = (core->boundaryCore_3D[2][1] == true) ? core->coreArrayNeighborhoodSizes_3D[1][1][1][2] - (dist): core->coreArrayNeighborhoodSizes_3D[1][1][1][2];

                           for (int k = base_Z; k < bound_Z; k++)
                              {
                               for (int j = base_Y; j < bound_Y; j++)
                                  {
                                  for (int i = base_X; i < bound_X; i++)
				     {
                                       if((i >= dist) && (i < core->coreArrayNeighborhoodSizes_3D[1][1][1][0]-dist) &&
                                          (j >= dist) && (j < core->coreArrayNeighborhoodSizes_3D[1][1][1][1]-dist) &&
                                          (k >= dist) && (k < core->coreArrayNeighborhoodSizes_3D[1][1][1][2]-dist)) continue;
                                       T tmp(0.0);
                                       for(int d=1; d <=dist; d++)
                                         {
                                           tmp += (
                                               /* array[Z][Y][X-d] */ ((i-d < 0) ? 
                                                  old_haloXBottom[coreID][otherCore_index3D((old_array.get_haloWidth(0)+(i-d)),j,k,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][0][1])] :
                                                  old_arraySection[index3D(i-d,j,k)]) +
                                               /* array[Z][Y][X+d] */ ((i+d >= core->coreArrayNeighborhoodSizes_3D[1][1][1][0]) ? 
                                                  old_haloXTop[coreID][otherCore_index3D((i+d-core->coreArrayNeighborhoodSizes_3D[1][1][2][0]),j,k,old_array.get_haloWidth(0),core->coreArrayNeighborhoodSizes_3D[1][1][2][1])] :
                                                  old_arraySection[index3D(i+d,j,k)]) +
                                               /* array[Z][Y-d][X] */ ((j-d < 0) ? 
                                                  old_haloYBottom[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(old_array.get_haloWidth(1)+(j-d)),k,(core->coreArrayNeighborhoodSizes_3D[1][0][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] : 
                                                  old_arraySection[index3D(i,j-d,k)]) +
                                               /* array[Z][Y+d][X] */ ((j+d >= core->coreArrayNeighborhoodSizes_3D[1][1][1][1]) ? 
                                                  old_haloYTop[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(j+d-core->coreArrayNeighborhoodSizes_3D[1][2][1][2]),k,(core->coreArrayNeighborhoodSizes_3D[1][2][1][0]+2*old_array.get_haloWidth(0)),old_array.get_haloWidth(1))] : 
                                                  old_arraySection[index3D(i,j+d,k)]) +
                                               /* array[Z-d][Y][X] */ ((k-d < 0) ? 
                                                  old_haloZBottom[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(j+old_array.get_haloWidth(1)),(old_array.get_haloWidth(2)+(k-d)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[0][1][1][1]+2*old_array.get_haloWidth(1)))] :
                                                  old_arraySection[index3D(i,j,k-d)]) +
                                               /* array[Z+d][Y][X] */ ((k+d >= core->coreArrayNeighborhoodSizes_3D[1][1][1][2]) ? 
                                                  old_haloZTop[coreID][otherCore_index3D((i+old_array.get_haloWidth(0)),(j+old_array.get_haloWidth(1)),(k+d-core->coreArrayNeighborhoodSizes_3D[2][1][1][2]),(core->coreArrayNeighborhoodSizes_3D[2][1][1][0]+2*old_array.get_haloWidth(0)),(core->coreArrayNeighborhoodSizes_3D[2][1][1][1]+2*old_array.get_haloWidth(1)))] : 
                                                  old_arraySection[index3D(i,j,k+d)]) 
                                               );
                                         }
                                         arraySection[index3D(i,j,k)] = tmp / (6.0*dist);
                                     }
                                  }
                              }
                        }
                  }
             }
   }
