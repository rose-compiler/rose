#include "core.h"
// Simplify using macro to support linearized indexing of array.
#define index1D(i)     (i)
#define index2D(i,j)   (((j)*coreArrayNeighborhoodSizes_2D[1][1][0])+(i))

// I think this should be in terms of the size for X and Y, not X, Y, and Z!
// #define index3D(i,j,k) (((k)*coreArrayNeighborhoodSizes_3D[1][1][1][2]*coreArrayNeighborhoodSizes_3D[1][1][1][1])+((j)*coreArrayNeighborhoodSizes_3D[1][1][1][0])+(i))
#define index3D(i,j,k) (((k)*coreArrayNeighborhoodSizes_3D[1][1][1][0]*coreArrayNeighborhoodSizes_3D[1][1][1][1])+((j)*coreArrayNeighborhoodSizes_3D[1][1][1][0])+(i))

// And we need another macro for the general case where the memory segment is a different size than coreArrayNeighborhoodSizes_2D[1][1][0] in the X (and Y axis)
// since the lenght of the data in each axis can be different along the same axis of the core array.  These macros take the length of the array in the requires
// axis to properly referne the element on the associated "other core".
#define otherCore_index2D(i,j,sizeX)   (((j)*sizeX)+(i))
#define otherCore_index3D(i,j,k,sizeX,sizeY) (((k)*sizeX*sizeY)+((j)*sizeX)+(i))


template <typename T>
T & 
Core<T>::operator()(const int & i, const int & j, const int & k)
   {
//     assert(i >= 0);
//     assert(j >= 0);
//     assert(k >= 0);


//     assert(multicoreArray      != NULL);
     assert(arraySectionPointer != NULL);

     int coreArrayIndex_X = 1;
     int coreArrayIndex_Y = 1;
     int coreArrayIndex_Z = 1;
     int elementIndex_X = i;
     int elementIndex_Y = j;
     int elementIndex_Z = k;

#if 0
     printf ("In Core<T>::operator(): i = %d j = %d k = %d coreArrayIndex_X = %d coreArrayIndex_Y = %d coreArrayIndex_Z = %d \n",i,j,k,coreArrayIndex_X,coreArrayIndex_Y,coreArrayIndex_Z);
#endif

     if (i >= 0 && j >= 0 && k >=0)
        {
          if (i < coreArrayNeighborhoodSizes_3D[1][1][1][0] && j < coreArrayNeighborhoodSizes_3D[1][1][1][1] && k < coreArrayNeighborhoodSizes_3D[1][1][1][2])
             {
            // Return the element from the current core
               return arraySectionPointer[1][1][1][(k*coreArrayNeighborhoodSizes_3D[1][1][1][0]*coreArrayNeighborhoodSizes_3D[1][1][1][1])+(j*coreArrayNeighborhoodSizes_3D[1][1][1][0])+i];
             }
            else
             {
            // For the case where the index is out of bounds for the current core, we could compute the X and Y entry core array directly

            // Accesses of areas outside of the current core return the associated element in the adjacent core.
               if (i >= coreArrayNeighborhoodSizes_3D[1][1][1][0])
                  {
                 // For now if it is outside of the existing core then just access the edge of the adjacent core.
                    coreArrayIndex_X = 2; 
                    elementIndex_X   = i - coreArrayNeighborhoodSizes_3D[1][1][1][0];
                 // return multicoreArray->getCore(coreArrayNeighborhood_2D[2][1][0],coreArrayNeighborhood_2D[1][1][1],0)(0,j,0);
                  }

               if (j >= coreArrayNeighborhoodSizes_3D[1][1][1][1])
                  {
                    coreArrayIndex_Y = 2; 
                    elementIndex_Y   = j - coreArrayNeighborhoodSizes_3D[1][1][1][1];
                 // return multicoreArray->getCore(coreArrayNeighborhood_2D[1][1][0],coreArrayNeighborhood_2D[1][2][1],0)(i,0,0);
                  }

               if (k >= coreArrayNeighborhoodSizes_3D[1][1][1][2])
                  {
                    coreArrayIndex_Z = 2;
                    elementIndex_Z   = k - coreArrayNeighborhoodSizes_3D[1][1][1][2];
                 // return multicoreArray->getCore(coreArrayNeighborhood_2D[1][1][0],coreArrayNeighborhood_2D[1][2][1],0)(i,0,0);
                  }
             }
        }
       else
        {
       // Accesses of areas outside of the current core return the associated element in the adjacent core.
          if (i < 0)
             {
               coreArrayIndex_X = 0; 
               elementIndex_X   = coreArrayNeighborhoodSizes_3D[1][1][0][0]+i;
            // return multicoreArray->getCore(coreArrayNeighborhood_2D[1][0][0],coreArrayNeighborhood_2D[1][1][1],0)(coreArrayNeighborhoodSizes_2D[1][0][0],j,0);
             }

          if (j < 0)
             {
               coreArrayIndex_Y = 0; 
               elementIndex_Y   = coreArrayNeighborhoodSizes_3D[1][0][1][1]+j;
            // return multicoreArray->getCore(coreArrayNeighborhood_2D[1][1][0],coreArrayNeighborhood_2D[1][0][1],0)(i,coreArrayNeighborhoodSizes_2D[0][1][1],0);
             }

          if (k < 0)
             {
               coreArrayIndex_Z = 0; 
               elementIndex_Z   = coreArrayNeighborhoodSizes_3D[0][1][1][2]+k;
            // return multicoreArray->getCore(coreArrayNeighborhood_2D[1][1][0],coreArrayNeighborhood_2D[1][0][1],0)(i,coreArrayNeighborhoodSizes_2D[0][1][1],0);
             }
        }

     assert(coreArrayIndex_X >= 0);
     assert(coreArrayIndex_Y >= 0);
     assert(coreArrayIndex_Z >= 0);

#if 0
     printf ("In Core<T>::operator(): i = %d\t j = %d\t k = %d\t coreArrayIndex_X = %d\t coreArrayIndex_Y = %d\t coreArrayIndex_Z = %d\t elementIndex_X   = %d\t elementIndex_Y   = %d\t elementIndex_Z  = %d \n",i,j,k,coreArrayIndex_X,coreArrayIndex_Y,coreArrayIndex_Z,elementIndex_X,elementIndex_Y,elementIndex_Z);
  // printf ("elementIndex_X   = %d elementIndex_Y   = %d \n",elementIndex_X,elementIndex_Y);
#endif

//     return multicoreArray->getCore(coreArrayIndex_X,coreArrayIndex_Y,coreArrayIndex_Z)(elementIndex_X,elementIndex_Y,elementIndex_Z);
     return arraySectionPointer[coreArrayIndex_Z][coreArrayIndex_Y][coreArrayIndex_X][(k*coreArrayNeighborhoodSizes_3D[coreArrayIndex_Z][coreArrayIndex_Y][coreArrayIndex_X][0]*coreArrayNeighborhoodSizes_3D[coreArrayIndex_Z][coreArrayIndex_Y][coreArrayIndex_X][1])+(j*coreArrayNeighborhoodSizes_3D[coreArrayIndex_Z][coreArrayIndex_Y][coreArrayIndex_X][0])+i];
   }







#if 0
template <typename T>
int
Core<T>::get_left_section_index( int dim, int p ) const
   {
  // Search as required from the right side to the left to find the first non-zero sized partition
  // (this assumes a linear and contigious ordering of the memory segments representing the array abstraction).

     assert(multicoreArray != NULL);

     int tmp_p = p;
     int partitionSize = 0;

  // The input should be the index in the multidimensional core array.
     assert(p >= 0);
     assert(p < multicoreArray->get_coreArraySize(dim));

  // Since we only want the position in a single dimension we need not worry about the other dimensions of the core array, so use 0 for them.
  // This is possible also because the distribution is restricted to be the same for all cores along each axis (so the entry for zero is representative).
     int indexSet[3] = {0,0,0};

     do {
          tmp_p = (tmp_p > 0) ? tmp_p-1 : multicoreArray->get_coreArraySize(dim) - 1;
#if 0
          printf ("In MulticoreArray<T>::get_left_section_index(): We should not mix linearized core indexes with the indexes into the multidimensional core array \n");
#endif
       // Compute the adjacent processor in the muti-dimensional core array.
          indexSet[dim] = tmp_p;
          int computedLinearizedIndex = multicoreArray->coreArrayElement(indexSet[0],indexSet[1],indexSet[2]);

       // partitionSize = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[tmp_p] : algorithmicComputationOfSize(dim,tmp_p);
       // partitionSize = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[computedLinearizedIndex] : algorithmicComputationOfSize(dim,tmp_p);
          partitionSize = (multicoreArray->get_tableBasedDistribution() == true) ? multicoreArray->get_arraySectionSizes(dim)[computedLinearizedIndex] : multicoreArray->algorithmicComputationOfSize(dim,tmp_p);
#if 0
          printf ("In get_left_section_index(p=%d): tmp_p = %d partitionSize = %d computedLinearizedIndex = %d \n",p,tmp_p,partitionSize,computedLinearizedIndex);
#endif
        }
     while (tmp_p != p && partitionSize == 0);

#if 0
     printf ("In get_left_section_index(dim=%d,p=%d): returning tmp_p = %d \n",dim,p,tmp_p);
#endif

     return tmp_p;
   }
#endif

#if 0
template <typename T>
int
Core<T>::get_right_section_index( int dim, int p ) const
   {
  // This function is similar to the MulticoreArray<T>::get_left_section_index() function (which has comments).

     assert(multicoreArray != NULL);

     int tmp_p = p;
     int partitionSize = 0;

     int indexSet[3] = {0,0,0};

     do {
          tmp_p = (tmp_p < multicoreArray->get_coreArraySize(dim)-1) ? tmp_p+1 : 0;
#if 0
          printf ("In MulticoreArray<T>::get_right_section_index(): We should not mix linearized core indexes with the indexes into the multidimensional core array \n");
#endif
          indexSet[dim] = tmp_p;
          int computedLinearizedIndex = multicoreArray->coreArrayElement(indexSet[0],indexSet[1],indexSet[2]);

       // partitionSize = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[computedLinearizedIndex] : algorithmicComputationOfSize(dim,tmp_p);
          partitionSize = (multicoreArray->get_tableBasedDistribution() == true) ? multicoreArray->get_arraySectionSizes(dim)[computedLinearizedIndex] : multicoreArray->algorithmicComputationOfSize(dim,tmp_p);
#if 0
          printf ("In get_right_section_index(p=%d): tmp_p = %d partitionSize = %d computedLinearizedIndex = %d \n",p,tmp_p,partitionSize,computedLinearizedIndex);
#endif
         }
     while (tmp_p != p && partitionSize == 0);

#if 0
     printf ("In get_right_section_index(dim=%d,p=%d): returning tmp_p = %d \n",dim,p,tmp_p);
#endif

     return tmp_p;
   }
#endif

template <typename T>
int
Core<T>::get_arraySectionSize(int dim) const
   {
  // Access function.
     return arraySectionSize[dim];
   }

#if 0
template <typename T>
bool
Core<T>::is_left_edge_section( int p , int previous_p ) const
   {
  // Not sure if this is general enough (but it works for direchlet boundary conditions).

  // bool leftEdgeSection  = (previous_p > p);
     bool leftEdgeSection  = (previous_p >= p);
     return leftEdgeSection;
   }

template <typename T>
bool
Core<T>::is_right_edge_section( int p, int next_p) const
   {
  // Not sure if this is general enough (but it works for direchlet boundary conditions).

  // bool rightEdgeSection = (next_p < p);
     bool rightEdgeSection = (next_p <= p);
     return rightEdgeSection;
   }
#endif




// *********************************************************************
// New boundary relaxation with DetachedHalo regions in local memory 
// This is applied only when user construct MulticoreArray with postive halo region size.

#if 0
template <typename T>
// void MulticoreArray<T>::initializeCoreArrayNeighborhoods()
void
Core<T>::initializeCoreArrayNeighborhoods( int p )
   {
  // Note that we want to allow for p to wrap as an index so that we can interprete the memory sections as a circular buffer.
  // Also we can't use a simplere for such as: "previous_p = (p-1) % numberOfCores" and "next_p = (p+1) % numberOfCores".
  // assert(p >= 0 && p < numberOfCores);

  // This lifts out loop invariant portions of the code.
  // T* arraySection     = arraySectionPointers[p];
  // T* old_arraySection = old_arraySectionPointers[p];

#if 0
     printf ("In Core<T>::initializeCoreArrayNeighborhoods( int p = %d ) \n",p);
#endif
#if 0
     printf ("\nIterate over all cores: p = %d arraySection = %p old_arraySection = %p \n",p,arraySection,old_arraySection);
#endif
  // assert(arraySection     != NULL);
  // assert(old_arraySection != NULL);

     assert(multicoreArray != NULL);

     int indexIntoMultidimensionalCoreArrayForThisDimension[3] = {0,0,0};
     for (int dim = 0; dim < 3; dim++)
        {
          indexIntoMultidimensionalCoreArrayForThisDimension[dim] = multicoreArray->indexIntoCoreArray(dim,p);
        }

#if 0
     printf ("Compute the core array neighborhood \n");
#endif
#if 0
  // Compute the core array neighborhood
  // This is the index of adjacent array sections to the current array section "p"
     typedef  int index_set_type[3];
     index_set_type coreArrayNeighborhood_1D[3];       // = {0,0,0};
     index_set_type coreArrayNeighborhood_2D[3][3];    // = { {0,0,0},{0,0,0},{0,0,0} };
     index_set_type coreArrayNeighborhood_3D[3][3][3]; // = { { {0,0,0},{0,0,0},{0,0,0} }, { {0,0,0},{0,0,0},{0,0,0} }, { {0,0,0},{0,0,0},{0,0,0} } };
#endif

#if 1
  // Initialize the core array neighborhood
     for (int dim = 0; dim < 3; dim++)
        {
          for (int k = 0; k < 3; k++)
             {
#if 1
               coreArrayNeighborhood_1D[k][dim] = 0;
#endif
               for (int j = 0; j < 3; j++)
                  {
#if 1
                    coreArrayNeighborhood_2D[k][j][dim] = 0;
#endif
                    for (int i = 0; i < 3; i++)
                       {
#if 1
                         coreArrayNeighborhood_3D[k][j][i][dim] = 0;
#endif
                       }
                  }
             }
        }
#endif

#if 0
     printf ("indexIntoMultidimensionalCoreArrayForThisDimension = (%d,%d,%d) \n",indexIntoMultidimensionalCoreArrayForThisDimension[0],indexIntoMultidimensionalCoreArrayForThisDimension[1],indexIntoMultidimensionalCoreArrayForThisDimension[2]);

     printf ("Left of indexIntoMultidimensionalCoreArrayForThisDimension = (%d,%d,%d) \n",
          array.get_left_section_index (0,indexIntoMultidimensionalCoreArrayForThisDimension[0]),
          array.get_left_section_index (1,indexIntoMultidimensionalCoreArrayForThisDimension[1]),
          array.get_left_section_index (2,indexIntoMultidimensionalCoreArrayForThisDimension[2]));

     printf ("Right of indexIntoMultidimensionalCoreArrayForThisDimension = (%d,%d,%d) \n",
          array.get_right_section_index (0,indexIntoMultidimensionalCoreArrayForThisDimension[0]),
          array.get_right_section_index (1,indexIntoMultidimensionalCoreArrayForThisDimension[1]),
          array.get_right_section_index (2,indexIntoMultidimensionalCoreArrayForThisDimension[2]));
#endif

  // *******************************************************************************
  // The current core's index into the core array in the center of the Neighborhood:
  // *******************************************************************************
     coreArrayNeighborhood_1D[1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
  // coreArrayNeighborhood_1D[1][1] = 0;
  // coreArrayNeighborhood_1D[1][2] = 0;
     coreArrayNeighborhood_1D[1][1] = indexIntoMultidimensionalCoreArrayForThisDimension[1];
     coreArrayNeighborhood_1D[1][2] = indexIntoMultidimensionalCoreArrayForThisDimension[2];

     coreArrayNeighborhood_2D[1][1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
     coreArrayNeighborhood_2D[1][1][1] = indexIntoMultidimensionalCoreArrayForThisDimension[1];
  // coreArrayNeighborhood_2D[1][1][2] = 0;
     coreArrayNeighborhood_2D[1][1][2] = indexIntoMultidimensionalCoreArrayForThisDimension[2];

     coreArrayNeighborhood_3D[1][1][1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
     coreArrayNeighborhood_3D[1][1][1][1] = indexIntoMultidimensionalCoreArrayForThisDimension[1];
     coreArrayNeighborhood_3D[1][1][1][2] = indexIntoMultidimensionalCoreArrayForThisDimension[2];

  // **************************************************************
  // Setup the 1D case (other entries already initialized to zero):
  // **************************************************************
//     coreArrayNeighborhood_1D[1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
     coreArrayNeighborhood_1D[0][0] = get_left_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
     coreArrayNeighborhood_1D[2][0] = get_right_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);

     coreArrayNeighborhood_1D[0][1] = coreArrayNeighborhood_1D[1][1];
     coreArrayNeighborhood_1D[2][1] = coreArrayNeighborhood_1D[1][1];

     coreArrayNeighborhood_1D[0][2] = coreArrayNeighborhood_1D[1][2];
     coreArrayNeighborhood_1D[2][2] = coreArrayNeighborhood_1D[1][2];


  // *****************************************************************************
  // Setup the 2D case (other entries (center index) already initialized to zero):
  // *****************************************************************************
  // Indexing order:          Y::X::axis
     coreArrayNeighborhood_2D[1][0][0] = get_left_section_index (0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
     coreArrayNeighborhood_2D[1][2][0] = get_right_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
     coreArrayNeighborhood_2D[0][1][1] = get_left_section_index (1,indexIntoMultidimensionalCoreArrayForThisDimension[1]);
     coreArrayNeighborhood_2D[2][1][1] = get_right_section_index(1,indexIntoMultidimensionalCoreArrayForThisDimension[1]);

  // Setup the 2D case (other entries (center index) already initialized to zero):
  // Turn this into a loop to see how to make it a formula
     for (int j = 0; j < 3; j++)
        {
          for (int i = 0; i < 3; i++)
             {
#if 0
            // Initialize to zero and then reset
               for (int dim = 0; dim < 3; dim++)
                  {
                    coreArrayNeighborhood_2D[j][i][dim] = 0;
                  }
#endif
               coreArrayNeighborhood_2D[j][i][2] = coreArrayNeighborhood_2D[1][1][2];
               if (i == 1 || j != 1)
                  {
                    coreArrayNeighborhood_2D[j][i][0] = coreArrayNeighborhood_2D[1][1][0];
                  }

               if (i != 1 || j == 1)
                  {
                    coreArrayNeighborhood_2D[j][i][1] = coreArrayNeighborhood_2D[1][1][1];
                  }

               if (i != 1 && j != 1)
                  {
                    coreArrayNeighborhood_2D[j][i][0] = coreArrayNeighborhood_2D[1][i][0];
                    coreArrayNeighborhood_2D[j][i][1] = coreArrayNeighborhood_2D[j][1][1];
                  }
             }
        }


  // *****************************************************************************
  // Setup the 3D case (other entries (center index) already initialized to zero):
  // *****************************************************************************
  // Indexing order:          Z::Y::X::axis
     coreArrayNeighborhood_3D[1][1][0][0] = get_left_section_index (0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
     coreArrayNeighborhood_3D[1][1][2][0] = get_right_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
     coreArrayNeighborhood_3D[1][0][1][1] = get_left_section_index (1,indexIntoMultidimensionalCoreArrayForThisDimension[1]);
     coreArrayNeighborhood_3D[1][2][1][1] = get_right_section_index(1,indexIntoMultidimensionalCoreArrayForThisDimension[1]);
     coreArrayNeighborhood_3D[0][1][1][2] = get_left_section_index (2,indexIntoMultidimensionalCoreArrayForThisDimension[2]);
     coreArrayNeighborhood_3D[2][1][1][2] = get_right_section_index(2,indexIntoMultidimensionalCoreArrayForThisDimension[2]);

     for (int k = 0; k < 3; k++)
        {
          for (int j = 0; j < 3; j++)
             {
               for (int i = 0; i < 3; i++)
                  {
#if 0
                 // Initialize to zero and then reset
                    for (int dim = 0; dim < 3; dim++)
                       {
                         coreArrayNeighborhood_3D[k][j][i][dim] = 0;
                       }
#endif
                    if(j != 1 || k != 1)
                    {
                         coreArrayNeighborhood_3D[k][j][i][0] = coreArrayNeighborhood_3D[1][1][i][0];
                    }
                    if(i != 1 || k != 1)
                    {
                         coreArrayNeighborhood_3D[k][j][i][1] = coreArrayNeighborhood_3D[1][j][1][1];
                    }
                    if(i != 1 || j != 1)
                    {
                         coreArrayNeighborhood_3D[k][j][i][2] = coreArrayNeighborhood_3D[k][1][1][2];
                    }
//                 // Not sure what this should be for 3D
//                    if (i == 1 || j != 1 || k == 1)
//                       {
//                         coreArrayNeighborhood_3D[k][j][i][0] = coreArrayNeighborhood_3D[1][1][1][0];
//                       }
//
//                 // Not sure what this should be for 3D
//                    if (i != 1 || j == 1 || k == 1)
//                       {
//                         coreArrayNeighborhood_3D[k][j][i][1] = coreArrayNeighborhood_3D[1][1][1][1];
//                       }
//
//                 // Not sure what this should be for 3D
//                    if (i == 1 || j == 1 || k != 1)
//                       {
//                         coreArrayNeighborhood_3D[k][j][i][2] = coreArrayNeighborhood_3D[1][1][1][2];
//                       }
//
//                    if (i != 1 && j != 1 && k != 1)
//                       {
//                         coreArrayNeighborhood_3D[k][j][i][0] = coreArrayNeighborhood_3D[1][1][i][0];
//                         coreArrayNeighborhood_3D[k][j][i][1] = coreArrayNeighborhood_3D[1][j][1][1];
//                         coreArrayNeighborhood_3D[k][j][i][2] = coreArrayNeighborhood_3D[k][1][1][2];
//                       }
                  }
             }
        }
#if 0
     printf ("p = %d coreArrayNeighborhood_1D (after setting edge core index) get_tableBasedDistribution() = %s \n",p,multicoreArray->get_tableBasedDistribution() ? "true" : "false");
     for (int i = 0; i < 3; i++)
        {
          printf ("(%d,%d,%d)  ",coreArrayNeighborhood_1D[i][0],coreArrayNeighborhood_1D[i][1],coreArrayNeighborhood_1D[i][2]);
        }
     printf ("\n\n");

     printf ("coreArrayNeighborhood_2D (after setting edge core index):\n");
     for (int j = 0; j < 3; j++)
        {
          for (int i = 0; i < 3; i++)
             {
               printf ("(%d,%d,%d)  ",coreArrayNeighborhood_2D[j][i][0],coreArrayNeighborhood_2D[j][i][1],coreArrayNeighborhood_2D[j][i][2]);
             }
          printf ("\n");
        }
     printf ("\n\n");
#endif
   }
#endif




#if 0
template <typename T>
void
Core<T>::initializeLinearizedCoreArrayNeighborhoods( int p )
   {
  // Initialize the mapping of the core array neighborhoods to the linearized array of cores.
  // This provides a simple API used for the compiler generated code.

     assert(multicoreArray != NULL);

#if 0
     printf ("In Core<T>::initializeLinearizedCoreArrayNeighborhoods( int p = %d ) \n",p);
#endif

  // **************************************************
  // Setup index entries into the linearized core array.
  // **************************************************

#if 0
  // This is the index of adjacent array sections to the current array section "p"
     typedef  int index_set_type[3];
     index_set_type coreArrayNeighborhood_1D[3];
     index_set_type coreArrayNeighborhood_2D[3][3];
     index_set_type coreArrayNeighborhood_3D[3][3][3];
#endif
#if 0
     int coreArrayNeighborhoodLinearized_1D[3]       = {0,0,0};
     int coreArrayNeighborhoodLinearized_2D[3][3]    = { {0,0,0},{0,0,0},{0,0,0} };
     int coreArrayNeighborhoodLinearized_3D[3][3][3] = { { {0,0,0},{0,0,0},{0,0,0} }, { {0,0,0},{0,0,0},{0,0,0} }, { {0,0,0},{0,0,0},{0,0,0} } };
#endif

  // Setup the linearized index entries for 1D
     for (int i = 0; i < 3; i++)
        {
          coreArrayNeighborhoodLinearized_1D[i] = multicoreArray->coreArrayElement(coreArrayNeighborhood_1D[i][0],coreArrayNeighborhood_1D[i][1],coreArrayNeighborhood_1D[i][2]);
        }

  // Setup the linearized index entries for 2D
     for (int j = 0; j < 3; j++)
        {
          for (int i = 0; i < 3; i++)
             {
               coreArrayNeighborhoodLinearized_2D[j][i] = multicoreArray->coreArrayElement(coreArrayNeighborhood_2D[j][i][0],coreArrayNeighborhood_2D[j][i][1],coreArrayNeighborhood_2D[j][i][2]);
             }
        }

     for (int k = 0; k < 3; k++)
        {
          for (int j = 0; j < 3; j++)
             {
               for (int i = 0; i < 3; i++)
                  {
                    coreArrayNeighborhoodLinearized_3D[k][j][i] = multicoreArray->coreArrayElement(coreArrayNeighborhood_3D[k][j][i][0],coreArrayNeighborhood_3D[k][j][i][1],coreArrayNeighborhood_3D[k][j][i][2]);
                  }
             }
        }
#if 0
     printf ("p = %d coreArrayNeighborhoodLinearized_1D get_tableBasedDistribution() = %s \n",p,multicoreArray->get_tableBasedDistribution() ? "true" : "false");
     for (int i = 0; i < 3; i++)
        {
          printf ("(%d)  ",coreArrayNeighborhoodLinearized_1D[i]);
        }
     printf ("\n\n");

     printf ("coreArrayNeighborhoodLinearized_2D:\n");
     for (int j = 0; j < 3; j++)
        {
          for (int i = 0; i < 3; i++)
             {
               printf ("(%d)  ",coreArrayNeighborhoodLinearized_2D[j][i]);
             }
          printf ("\n");
        }
     printf ("\n\n");
#endif
   }

// ******************************************************
#endif

#if 0
template <typename T>
void
Core<T>::initializeCoreArraySizeNeighborhoods( int p )
   {
  // This is the size information for each element in the core array neighborhoods (and for each axis direction of multidimensional arrays).
  // This stored data simplifies how we can reference precomputed data.

     assert(multicoreArray != NULL);

#if 0
     printf ("In Core<T>::initializeCoreArraySizeNeighborhoods( int p = %d ) \n",p);
#endif

  // **********************************************************
  // Setup size data for the array sections in the neighborhood
  // **********************************************************

#if 0
     if (array.get_tableBasedDistribution() == true)
        {
          for (int i = 0; i < array.get_numberOfCores(); i++)
             {
               printf ("core = %d array.get_arraySectionSizes(0,1,2) = (%d,%d,%d) \n",i,array.get_arraySectionSizes(0)[i],array.get_arraySectionSizes(1)[i],array.get_arraySectionSizes(2)[i]);
             }
        }
#endif
#if 0
  // This is the index of adjacent array sections to the current array section "p"
     typedef  int index_set_type[3];
     index_set_type coreArrayNeighborhood_1D[3];
     index_set_type coreArrayNeighborhood_2D[3][3];
     index_set_type coreArrayNeighborhood_3D[3][3][3];

     int coreArrayNeighborhoodLinearized_1D[3]       = {0,0,0};
     int coreArrayNeighborhoodLinearized_2D[3][3]    = { {0,0,0},{0,0,0},{0,0,0} };
     int coreArrayNeighborhoodLinearized_3D[3][3][3] = { { {0,0,0},{0,0,0},{0,0,0} }, { {0,0,0},{0,0,0},{0,0,0} }, { {0,0,0},{0,0,0},{0,0,0} } };
#endif
#if 0
  // Arrays up to 3D can be distributed over 1D, 2D and 3D arrays of cores (last index is data array dimension).
     int coreArrayNeighborhoodSizes_1D[3][3];
     int coreArrayNeighborhoodSizes_2D[3][3][3];
     int coreArrayNeighborhoodSizes_3D[3][3][3][3];
#endif
  // Setup the linearized index entries for 1D
     for (int i = 0; i < 3; i++)
        {
          for (int dim = 0; dim < 3; dim++)
             {
            // coreArrayNeighborhoodSizes_1D[i] = (array.get_tableBasedDistribution() == true) ? array.get_arraySectionSizes(0)[coreArrayNeighborhoodLinearized_1D[i]] : array.algorithmicComputationOfSize(0,i);
               coreArrayNeighborhoodSizes_1D[i][dim] = 0;

           //  coreArrayNeighborhoodSizes_1D[i][dim] = (multicoreArray->get_tableBasedDistribution() == true) ? multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_1D[i]] : multicoreArray->algorithmicComputationOfSize(dim,coreArrayNeighborhood_1D[i][dim]);
               coreArrayNeighborhoodSizes_1D[i][dim] = multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_1D[i]];
             }
        }

  // Setup the linearized index entries for 2D
     for (int j = 0; j < 3; j++)
        {
          for (int i = 0; i < 3; i++)
             {
               for (int dim = 0; dim < 3; dim++)
                  {
               //   coreArrayNeighborhoodSizes_2D[j][i][dim] = (multicoreArray->get_tableBasedDistribution() == true) ? multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_2D[j][i]] : multicoreArray->algorithmicComputationOfSize(dim,coreArrayNeighborhood_2D[j][i][dim]);
                    coreArrayNeighborhoodSizes_2D[j][i][dim] = multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_2D[j][i]];
                  }
             }
        }

  // Setup the linearized index entries for 3D
     for (int k = 0; k < 3; k++)
        {
          for (int j = 0; j < 3; j++)
             {
               for (int i = 0; i < 3; i++)
                  {
                    for (int dim = 0; dim < 3; dim++)
                       {
                     //  coreArrayNeighborhoodSizes_3D[k][j][i][dim] = (multicoreArray->get_tableBasedDistribution() == true) ? multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_3D[k][j][i]] : multicoreArray->algorithmicComputationOfSize(dim,coreArrayNeighborhood_3D[k][j][i][dim]);
                         coreArrayNeighborhoodSizes_3D[k][j][i][dim] = multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_3D[k][j][i]];
                       }
                  }
             }
        }

#if 0
     printf ("p = %d coreArrayNeighborhoodSizes_1D get_tableBasedDistribution() = %s \n",p,multicoreArray->get_tableBasedDistribution() ? "true" : "false");
     for (int i = 0; i < 3; i++)
        {
          printf ("(%d,%d,%d)  ",coreArrayNeighborhoodSizes_1D[i][0],coreArrayNeighborhoodSizes_1D[i][1],coreArrayNeighborhoodSizes_1D[i][2]);
        }
     printf ("\n\n");

     printf ("coreArrayNeighborhoodSizes_2D:\n");
     for (int j = 0; j < 3; j++)
        {
          for (int i = 0; i < 3; i++)
             {
               printf ("(%d,%d,%d)  ",coreArrayNeighborhoodSizes_2D[j][i][0],coreArrayNeighborhoodSizes_2D[j][i][1],coreArrayNeighborhoodSizes_2D[j][i][2]);
             }
          printf ("\n");
        }
     printf ("\n\n");
#endif

  // Bounds checking
     for (int i = 0; i < 3; i++)
        {
          for (int dim = 0; dim < 3; dim++)
             {
               assert(coreArrayNeighborhoodSizes_1D[i][dim] >= 0 && coreArrayNeighborhoodSizes_1D[i][dim] < 1000000);
             }
        }

  // Bounds checking
     for (int j = 0; j < 3; j++)
        {
          for (int i = 0; i < 3; i++)
             {
               for (int dim = 0; dim < 3; dim++)
                  {
                    assert(coreArrayNeighborhoodSizes_2D[j][i][dim] >= 0 && coreArrayNeighborhoodSizes_2D[j][i][dim] < 1000000);
                  }
             }
        }
   }
#endif

// ****************************************


#if 0
template <typename T>
void
Core<T>::initializeCoreArrayBoundaryFlags( int p )
   {
  // Mark where core array sections represent boundaries of the many-core distributed array (along each axis).

  // 3D supp0ort not in place yet.

     assert(multicoreArray != NULL);

#if 0
  // This is the index of adjacent array sections to the current array section "p"
     typedef  int index_set_type[3];
     index_set_type coreArrayNeighborhood_1D[3];
     index_set_type coreArrayNeighborhood_2D[3][3];
     index_set_type coreArrayNeighborhood_3D[3][3][3];
#endif
  // ***************************************************************************************************
  // Setup boolean values to define cross-chip boundary handling (e.g. MPI between many-core processors).
  // Answers the questions of is the lower and upper bound for each axis a cross-chip boundary.
  // ***************************************************************************************************

  // bool boundaryCore_1D[2]    = {false,false};
  // bool boundaryCore_2D[2][2] = { {false,false}, {false,false} };

     boundaryCore_1D[0]    = is_left_edge_section(coreArrayNeighborhood_1D [1][0],coreArrayNeighborhood_1D[0][0]);
     boundaryCore_1D[1]    = is_right_edge_section(coreArrayNeighborhood_1D[1][0],coreArrayNeighborhood_1D[2][0]);

  // X axis        X:Y L:U
     boundaryCore_2D[0][0] = is_left_edge_section(coreArrayNeighborhood_2D [1][1][0],coreArrayNeighborhood_2D[1][0][0]);
     boundaryCore_2D[0][1] = is_right_edge_section(coreArrayNeighborhood_2D[1][1][0],coreArrayNeighborhood_2D[1][2][0]);

  // Y axis
     boundaryCore_2D[1][0] = is_left_edge_section(coreArrayNeighborhood_2D [1][1][1],coreArrayNeighborhood_2D[0][1][1]);
     boundaryCore_2D[1][1] = is_right_edge_section(coreArrayNeighborhood_2D[1][1][1],coreArrayNeighborhood_2D[2][1][1]);

  // X axis      X:Y:Z L:U
     boundaryCore_3D[0][0] = is_left_edge_section(coreArrayNeighborhood_3D [1][1][1][0],coreArrayNeighborhood_3D[1][1][0][0]);
     boundaryCore_3D[0][1] = is_right_edge_section(coreArrayNeighborhood_3D[1][1][1][0],coreArrayNeighborhood_3D[1][1][2][0]);

  // Y axis      X:Y:Z L:U
     boundaryCore_3D[1][0] = is_left_edge_section(coreArrayNeighborhood_3D [1][1][1][1],coreArrayNeighborhood_3D[1][0][1][1]);
     boundaryCore_3D[1][1] = is_right_edge_section(coreArrayNeighborhood_3D[1][1][1][1],coreArrayNeighborhood_3D[1][2][1][1]);

  // X axis      X:Y:Z L:U
     boundaryCore_3D[2][0] = is_left_edge_section(coreArrayNeighborhood_3D [1][1][1][2],coreArrayNeighborhood_3D[0][1][1][2]);
     boundaryCore_3D[2][1] = is_right_edge_section(coreArrayNeighborhood_3D[1][1][1][2],coreArrayNeighborhood_3D[2][1][1][2]);

#if 0
     printf ("p = %d initializeCoreArrayBoundaryFlags get_tableBasedDistribution() = %s \n",p,multicoreArray->get_tableBasedDistribution() ? "true" : "false");
     printf ("boundaryCore_1D: (%s,%s) \n",boundaryCore_1D[0] ? "true" : "false",boundaryCore_1D[1] ? "true" : "false");
     printf ("boundaryCore_2D: X(%s,%s)Y(%s,%s) \n",boundaryCore_2D[0][0] ? "true" : "false",boundaryCore_2D[0][1] ? "true" : "false",boundaryCore_2D[1][0] ? "true" : "false",boundaryCore_2D[1][1] ? "true" : "false");
#endif
   }
#endif

template class Core<int>;
template class Core<long>;
template class Core<float>;
template class Core<double>;
