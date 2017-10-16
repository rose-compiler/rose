#include "multicore.h"
#define HALO_DBG 0

template <typename T>
int
MulticoreArray<T>::algorithmicComputationOfSize( int dim, int p ) const
   {
  // This function represents the algorithmic distribution of data across the many core processor.
  // It is simple, requires little data storage, but is not as flexable as the table-based distribution.

  // This may over compute the size (by 1) required for the last cores...not a problem (adjusted below).

  // int remainder = (arraySize[dim] % coreArraySize[dim]);
     int size = (arraySize[dim] / coreArraySize[dim]) + (((arraySize[dim] % coreArraySize[dim]) != 0) ? 1 : 0);

#if 0
     printf ("In algorithmic distribution: p = %d arraySize[dim=%d] = %2d coreArraySize[dim=%d] = %2d \n",p,dim,arraySize[dim],dim,coreArraySize[dim]);
     printf ("In algorithmic distribution: arraySize/numberOfCores = %2d arraySize MOD numberOfCores = %2d \n",(arraySize[dim]/coreArraySize[dim]),(arraySize[dim] % coreArraySize[dim]));
#endif

  // DQ (10/22/2011): I think we can assert this.
     assert(p < coreArraySize[dim]);

     if (p >= (arraySize[dim] % coreArraySize[dim]))
        {
#if 0
          printf ("p=%2d: p >= arraySize MOD numberOfCores \n",p);
#endif
          size -= ((arraySize[dim] % coreArraySize[dim]) != 0) ? 1 : 0;
        }
       else
        {
#if 0
          printf ("p=%2d: p < arraySize MOD numberOfCores \n",p);
#endif
        }
#if 0
     printf ("In algorithmic distribution (p=%2d): arraySize/numberOfCores = %2d arraySize MOD numberOfCores = %2d size = %d \n",p,(arraySize[dim]/coreArraySize[dim]),(arraySize[dim] % coreArraySize[dim]),size);
#endif

  // Alternative computation:
  // int altSize = (p >= (arraySize[dim] % coreArraySize[dim]) ? (arraySize[dim] / coreArraySize[dim]) + 
     return size;
   }



template <typename T>
void
MulticoreArray<T>::display( const std::string & label ) const
   {
  // Output the values of 1D, 2D, and 3D arrays (with formatting for debugging).

     printf ("===== %s ===== size(%2d,%2d,%2d) on multi-core array size(%2d,%2d,%2d)\n",label.c_str(),arraySize[0],arraySize[1],arraySize[2],coreArraySize[0],coreArraySize[1],coreArraySize[2]);
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
                      printf ("%5.2f ",operator()(i,j,k));
                  }
               printf ("\n");
             }
          printf ("\n");
        }
     printf ("\n");
   }


template <typename T>
void
MulticoreArray<T>::initializeBoundary( const T & x )
   {
  // Initialize the boundary for 1D, 2D and 3D arrays (set to negative value of original value for debugging)

#define DEBUG_CHANGE_SIGN_OF_BOUNDARY 0

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
                              (*this)(i,j,0)              = x;
                              (*this)(i,j,arraySize[2]-1) = x;
#else
                              (*this)(i,j,0)              = -(*this)(i,j,0);
                              (*this)(i,j,arraySize[2]-1) = -(*this)(i,j,arraySize[2]-1);
#endif
                            }
                       }

                    for (int i = 0; i < arraySize[0]; i++)
                       {
                         for (int k = 0; k < arraySize[2]; k++)
                            {
#if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
                              (*this)(i,0,k)              = x;
                              (*this)(i,arraySize[1]-1,k) = x;
#else
                              (*this)(i,0,k)              = -(*this)(i,0,k);
                              (*this)(i,arraySize[1]-1,k) = -(*this)(i,arraySize[1]-1,k);
#endif
                            }
                       }

                    for (int k = 0; k < arraySize[2]; k++)
                       {
                         for (int j = 0; j < arraySize[1]; j++)
                            {
#if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
                              (*this)(0,j,k)              = x;
                              (*this)(arraySize[0]-1,j,k) = x;
#else
                              (*this)(0,j,k)              = -(*this)(0,j,k);
                              (*this)(arraySize[0]-1,j,k) = -(*this)(arraySize[0]-1,j,k);
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
                         (*this)(i,0,0)              = x;
                         (*this)(i,arraySize[1]-1,0) = x;
#else
                         (*this)(i,0,0)              = -(*this)(i,0,0);
                         (*this)(i,arraySize[1]-1,0) = -(*this)(i,arraySize[1]-1,0);
#endif
                       }

                    for (int j = 0; j < arraySize[1]; j++)
                       {
#if (DEBUG_CHANGE_SIGN_OF_BOUNDARY == 0)
                         (*this)(0,j,0)              = x;
                         (*this)(arraySize[0]-1,j,0) = x;
#else
                         (*this)(0,j,0)              = -(*this)(0,j,0);
                         (*this)(arraySize[0]-1,j,0) = -(*this)(arraySize[0]-1,j,0);
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
               (*this)(0,0,0)              = x;
               (*this)(arraySize[0]-1,0,0) = x;
#else
               (*this)(0,0,0)              = -(*this)(0,0,0);
               (*this)(arraySize[0]-1,0,0) = -(*this)(arraySize[0]-1,0,0);
#endif
             }
        }
   }


template <typename T>
size_t
MulticoreArray<T>::distanceBetweenMemoryAllocatedPerCore()
   {
  // This function supports debugging where the memory is being allocated, namely how far apart the 
  // memory is allocated for each core.  If it is too close then it will cause false sharing.  It 
  // appears the with libnuma, the allocations are on page boundaries (which is likely fine).  Not
  // clear what is best for Intel processor, the best location would be architecture dependent.

     size_t totalDistanceBetweenMemoryAllocatedPerCore = 0;
     for (int p = 0; p < numberOfCores; p++)
        {
#if 0
          printf("core specific data address = %p arraySize = %d p = %d padding = %d \n",arraySectionPointers[p],arraySize,p,padding);
#endif
          if (p > 0)
             {
               size_t distance = abs((char*)(arraySectionPointers[p]) - (char*)(arraySectionPointers[p-1]));
#if 0
               printf("core specific data distance p(%2d,%2d) = %zu \n",p-1,p,distance);
#endif
            // Need to make sure this does not overflow.
               totalDistanceBetweenMemoryAllocatedPerCore += distance;
             }
        }
#if 0
     printf("totalDistanceBetweenMemoryAllocatedPerCore = %zu \n",totalDistanceBetweenMemoryAllocatedPerCore);
#endif

     return totalDistanceBetweenMemoryAllocatedPerCore;
   }


template <typename T>
void
MulticoreArray<T>::verifyArraySize() const
   {
  // This allows testing of the generated array size using both the table-based and algorithmic based approaches.

     int accumulatedSize[3] = {0,0,0};

#if 0
     printf ("In verifyArraySize() \n");
#endif

     for (int dim = 0; dim < 3; dim++)
        {
#if 0
          printf ("In verifyArraySize(): coreArraySize[dim=%2d] = %2d \n",dim,coreArraySize[dim]);
#endif
          int indexInCoreArray[DIM] = { 0, 0, 0 };
#if 0
          printf ("In verifyArraySize() before loop along axis: indexInCoreArray[0] = %d indexInCoreArray[1] = %d indexInCoreArray[2] = %d \n",indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#endif
          for (int core = 0; core < coreArraySize[dim]; core++)
             {
            // int coreIndexInLinearArray = indexIntoCoreArray(dim,core);
               indexInCoreArray[dim] = core;
#if 0
               printf ("In verifyArraySize(): indexInCoreArray[0] = %d indexInCoreArray[1] = %d indexInCoreArray[2] = %d \n",indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#endif
               int coreIndexInLinearArray = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);

            // int local_size = (useArraySectionRanges == true) ? arraySectionSizes[dim][core] : algorithmicComputationOfSize(dim,core);
            // int local_size = (useArraySectionRanges == true) ? arraySectionSizes[dim][coreIndexInLinearArray] : algorithmicComputationOfSize(dim,core);
               int local_size = arraySectionSizes[dim][coreIndexInLinearArray];
#if 0
               printf ("In verifyArraySize(): core = %2d coreIndexInLinearArray = %2d local_size = %d \n",core,coreIndexInLinearArray,local_size);
#endif
               accumulatedSize[dim] += local_size;
             }
             if(boundaryType == MulticoreArray<T>::attached)
             {
               if (accumulatedSize[dim] != (arraySize[dim] + 2 * coreArraySize[dim] * haloSize[dim]))
                  {
                    printf ("In verifyArraySize() attached Halo: overallSize[%d] = %2d accumulatedSize[%d] = %2d \n",dim,arraySize[dim]+ 2 * coreArraySize[dim] * haloSize[dim],dim,accumulatedSize[dim]);
                  }

               assert(accumulatedSize[dim] == (arraySize[dim] + 2 * coreArraySize[dim] * haloSize[dim]));
             }
             else
             {
               if (accumulatedSize[dim] != arraySize[dim])
                  {
                    printf ("In verifyArraySize() non-attached halo: arraySize[%d] = %2d accumulatedSize[%d] = %2d \n",dim,arraySize[dim],dim,accumulatedSize[dim]);
                  }

               assert(accumulatedSize[dim] == arraySize[dim]);
             }
        }
   }


template <typename T>
Core<T>**
MulticoreArray<T>::get_coreArray() const
   {
  // Access function
     return coreArray;
   }


template <typename T>
int 
MulticoreArray<T>::get_arraySize(int dim) const
   {
  // This is access function for the global array size for the array distributed across all of the cores.

     return arraySize[dim];
   }

#if HAVE_NUMA_H
template <typename T>
int 
MulticoreArray<T>::get_NUMAcoreArraySize(int dim) const
   {
  // Access function.
     return NUMAcoreArraySize[dim];
   }
#endif

template <typename T>
int 
MulticoreArray<T>::get_coreArraySize(int dim) const
   {
  // Access function.
     return coreArraySize[dim];
   }


template <typename T>
int 
MulticoreArray<T>::numberOfArrayElements() const
   {
  // Compute the number of elements in the array.
     if(boundaryType == MulticoreArray<T>::attached)
         return (arraySize[0]+2*coreArraySize[0]*haloSize[0]) * (arraySize[1]+2*coreArraySize[1]*haloSize[1]) * (arraySize[2]+2*coreArraySize[2]*haloSize[2]);
     else
         return arraySize[0] * arraySize[1] * arraySize[2];
   }


template <typename T>
int 
MulticoreArray<T>::get_numberOfCores() const
   {
  // Access function.
     return numberOfCores;
   }

template <typename T>
int 
MulticoreArray<T>::get_padding() const
   {
  // Access function.
     return padding;
   }

template <typename T>
T** 
MulticoreArray<T>::get_arraySectionPointers() const
   {
  // Access function.
     return arraySectionPointers;
   }

template <typename T>
T** 
MulticoreArray<T>::get_haloSectionPointers(int dim, int boundary) const
   {
  // Access function.
     return haloSectionPointers[dim][boundary];
   }

template <typename T>
const std::vector<std::pair<size_t,size_t> > & 
MulticoreArray<T>::get_arraySectionRanges(int dim) const
   {
  // Access function.
     return arraySectionRanges[dim];
   }

template <typename T>
const std::vector<size_t> & 
MulticoreArray<T>::get_arraySectionSizes(int dim) const
   {
  // Access function.
     return arraySectionSizes[dim];
   }

template <typename T>
const std::vector<size_t> & 
MulticoreArray<T>::get_haloSectionSizes(int dim) const
   {
  // Access function.
     return haloRegionsSizes[dim];
   }

template <typename T>
int
MulticoreArray<T>::get_haloWidth(int dim) const
   {
  // Access function.
     return haloSize[dim];
   }

template <typename T>
bool
MulticoreArray<T>::get_tableBasedDistribution() const
   {
  // Access function.
     return useArraySectionRanges;
   }

template <typename T>
void
MulticoreArray<T>::verifyMultidimensionalCoreArray() const
   {
  // This is a test to verify the correctness of the internal functions used to do 
  // translation between the linearized indxing into the array of 1D array of cores 
  // and the multidimensional indexing into the multidimensional array of cores.

     int linearizedCoreIndex = 0;
     for (int k = 0; k < coreArraySize[2]; k++)
        {
          for (int j = 0; j < coreArraySize[1]; j++)
             {
               for (int i = 0; i < coreArraySize[0]; i++)
                  {
                    int indexX = indexIntoCoreArray(0,linearizedCoreIndex);
                    int indexY = indexIntoCoreArray(1,linearizedCoreIndex);
                    int indexZ = indexIntoCoreArray(2,linearizedCoreIndex);

                 // Test against the index position in the multi-dimensional core array.
                    assert(i == indexX);
                    assert(j == indexY);
                    assert(k == indexZ);

                    int computedLinearizedIndex = coreArrayElement(i,j,k);

                 // Test against the linearized index position in the 1D array of cores.
                    assert(computedLinearizedIndex == linearizedCoreIndex);
#if 0
                 // Unused variables...

                 // This is relative to the multidimensional array data (so not just the adjacent index in the multidimensional core array).
                    int lowerIndexInMutidimensionalCoreArray_X = get_left_section_index(0,i);
                    int lowerIndexInMutidimensionalCoreArray_Y = get_left_section_index(1,j);
                    int lowerIndexInMutidimensionalCoreArray_Z = get_left_section_index(2,k);

                    int upperIndexInMutidimensionalCoreArray_X = get_right_section_index(0,i);
                    int upperIndexInMutidimensionalCoreArray_Y = get_right_section_index(1,j);
                    int upperIndexInMutidimensionalCoreArray_Z = get_right_section_index(2,k);
#endif
                 // Need tests on these lowerIndexInMutidimensionalCoreArray_* and upperIndexInMutidimensionalCoreArray_* values.


                 // Increment the linearized core index
                    linearizedCoreIndex++;
                  }
             }
        }
   }

template <typename T>
int
MulticoreArray<T>::get_left_section_index( int dim, int p ) const
   {
  // Search as required from the right side to the left to find the first non-zero sized partition
  // (this assumes a linear and contigious ordering of the memory segments representing the array abstraction).

     int tmp_p = p;
     int partitionSize = 0;

  // The input should be the index in the multidimensional core array.
     assert(p >= 0);
     assert(p < coreArraySize[dim]);

  // Since we only want the position in a single dimension we need not worry about the other dimensions of the core array, so use 0 for them.
  // This is possible also because the distribution is restricted to be the same for all cores along each axis (so the entry for zero is representative).
     int indexSet[3] = {0,0,0};

     do {
          tmp_p = (tmp_p > 0) ? tmp_p-1 : coreArraySize[dim] - 1;
#if 0
          printf ("In MulticoreArray<T>::get_left_section_index(): We should not mix linearized core indexes with the indexes into the multidimensional core array \n");
#endif
       // Compute the adjacent processor in the muti-dimensional core array.
          indexSet[dim] = tmp_p;
          int computedLinearizedIndex = coreArrayElement(indexSet[0],indexSet[1],indexSet[2]);

       // partitionSize = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[tmp_p] : algorithmicComputationOfSize(dim,tmp_p);
          partitionSize = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[computedLinearizedIndex] : algorithmicComputationOfSize(dim,tmp_p);
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

template <typename T>
int
MulticoreArray<T>::get_right_section_index( int dim, int p ) const
   {
  // This function is similar to the MulticoreArray<T>::get_left_section_index() function (which has comments).

     int tmp_p = p;
     int partitionSize = 0;

     int indexSet[3] = {0,0,0};

     do {
          tmp_p = (tmp_p < coreArraySize[dim]-1) ? tmp_p+1 : 0;
#if 0
          printf ("In MulticoreArray<T>::get_right_section_index(): We should not mix linearized core indexes with the indexes into the multidimensional core array \n");
#endif
          indexSet[dim] = tmp_p;
          int computedLinearizedIndex = coreArrayElement(indexSet[0],indexSet[1],indexSet[2]);

          partitionSize = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[computedLinearizedIndex] : algorithmicComputationOfSize(dim,tmp_p);
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

template <typename T>
bool
MulticoreArray<T>::is_left_edge_section( int p , int previous_p ) const
   {
  // Not sure if this is general enough (but it works for direchlet boundary conditions).

  // bool leftEdgeSection  = (previous_p > p);
     bool leftEdgeSection  = (previous_p >= p);
     return leftEdgeSection;
   }

template <typename T>
bool
MulticoreArray<T>::is_right_edge_section( int p, int next_p) const
   {
  // Not sure if this is general enough (but it works for direchlet boundary conditions).

  // bool rightEdgeSection = (next_p < p);
     bool rightEdgeSection = (next_p <= p);
     return rightEdgeSection;
   }


template <typename T>
int
MulticoreArray<T>::coreArrayElement(int p, int q, int r) const
   {
#if 0
      printf ("In MulticoreArray<T>::coreArrayElement(): p = %d q = %d r = %d coreArraySize[0] = %d coreArraySize[1] = %d coreArraySize[2] = %d numberOfCores = %d \n",p,q,r,coreArraySize[0],coreArraySize[1],coreArraySize[2],numberOfCores);
#endif

     int core = (r*coreArraySize[1]*coreArraySize[0]) + (q*coreArraySize[0]) + p;

#if 0
     printf ("In coreArrayElement(p=%2d,q=%2d,r=%2d) = %2d \n",p,q,r,core);
     printf ("coreArraySize[0] = %d coreArraySize[1] = %d coreArraySize[2] = %d \n",coreArraySize[0],coreArraySize[1],coreArraySize[2]);
#endif

  // Assert this fact about when "core" maps to "zero".
  // assert(core == 0 && (p == 0 && q == 0 && r == 0));
  // assert(core != 0 || (p != 0 || q != 0 || r != 0));
     assert(core != 0 || (p == 0 && q == 0 && r == 0));

     assert (core >= 0);

  // We need to support computing one core too far (used in MulticoreArray<T>::computeMemorySectionAndOffset()).
  // assert (core < numberOfCores);
  // In multidemensional core arrays a core too far in an axis other than the X axis will fail this assertion.
  // assert (core <= numberOfCores);

  // This is not a strong assertion, but it avoids at least some strange errors.
     if (core > numberOfCores*numberOfCores)
        {
       // printf ("core = %d numberOfCores = %d numberOfCores*numberOfCores = %d numberOfCores*numberOfCores*numberOfCores = %d \n",core,numberOfCores,numberOfCores*numberOfCores,numberOfCores*numberOfCores*numberOfCores);
          printf ("core = %d numberOfCores = %d numberOfCores*numberOfCores = %d \n",core,numberOfCores,numberOfCores*numberOfCores);
        }
  // assert (core <= numberOfCores*numberOfCores*numberOfCores);
     assert (core <= numberOfCores*numberOfCores);

     return core;
   }


template <typename T>
int
MulticoreArray<T>::memorySectionSize(int p, int q, int r) const
   {
  // This function mostly supports debugging or general information about the array segment per core.
  // This function computes the size (number of elements) in a memory segment given by the index (p,q,r) 
  // into the multi-dimensional coreArray.

     assert(p >= 0);
     assert(p < coreArraySize[0]);

     assert(q >= 0);
     assert(q < coreArraySize[1]);

     assert(r >= 0);
     assert(r < coreArraySize[2]);

     int size = 1;
     for (int dim = 0; dim < 3; dim++)
        {
          int core = coreArrayElement(p,q,r);
#if 0
       // DQ (10/22/2011): I just spotted this error!
          printf ("ERROR: This should only use 'p' for dim == 1 \n");
          assert(false);
#endif
//          int indexOfCore = (dim == 0) ? p : ((dim == 1) ? q : r);
       // size *= (useArraySectionRanges == true) ? arraySectionSizes[dim][core] + padding : algorithmicComputationOfSize(dim,p) + padding;
       // size *= (useArraySectionRanges == true) ? arraySectionSizes[dim][core] + padding : algorithmicComputationOfSize(dim,indexOfCore) + padding;
          size *= arraySectionSizes[dim][core] + padding;
        }

#if 0
     printf ("In memorySectionSize(p=%2d,q=%2d,r=%2d) = %2d \n",p,q,r,size);
#endif

     assert(size >= 0);

     return size;
   }


template <typename T>
int
MulticoreArray<T>::indexIntoCoreArray(int dimension, int coreIndexInLinearArrayOfCores) const
   {
  // This function returns the mapping from the linearized core array into the multidimensional core array.

     assert(coreArraySize[0] > 0);
     assert(coreArraySize[1] > 0);
     assert(coreArraySize[2] > 0);

     assert(coreIndexInLinearArrayOfCores < numberOfCores);

     int indexAlongDimensionOfCoreArray = 0;
     if (dimension == 2)
        {
       // 3D case
          indexAlongDimensionOfCoreArray = coreIndexInLinearArrayOfCores / (coreArraySize[1] * coreArraySize[0]);
        }
       else 
        {
          if (dimension == 1)
             {
            // 2D case
            // indexAlongDimension = (coreIndexInLinearArrayOfCores / coreArraySize[0]);
               indexAlongDimensionOfCoreArray = (coreIndexInLinearArrayOfCores / coreArraySize[0]) % coreArraySize[1];
             }
            else
             {
            // 1D case
               assert(dimension == 0);
               indexAlongDimensionOfCoreArray = coreIndexInLinearArrayOfCores % coreArraySize[0];
             }
       }

#if 0
     printf ("In indexIntoCoreArray(dimension=%2d,coreIndexInLinearArrayOfCores=%2d) = %2d coreArraySize[dimension] = %2d \n",dimension,coreIndexInLinearArrayOfCores,indexAlongDimensionOfCoreArray,coreArraySize[dimension]);
#endif

     assert(indexAlongDimensionOfCoreArray >= 0);

     assert(indexAlongDimensionOfCoreArray < coreArraySize[dimension]);

     return indexAlongDimensionOfCoreArray;
   }


template <typename T>
int
MulticoreArray<T>::mappingFromMultidimentionalMemorySegmentArray(int i, int j, int k, int coreIndex) const
   {
  // This computes the linearized element index for (i,j,k) index in the memory segment at coreIndex
  // Note that this function calls the indexIntoCoreArray() function (which would not be required if 
  // we passed in the index position in the core array directly.

     assert(i >= 0);
     assert(j >= 0);
     assert(k >= 0);

  // Note that the 2nd parameter to algorithmicComputationOfSize() must be in terms of the coreArrayIndex (and not the linearized form).
  // So we have to convert the linearized index using the  indexIntoCoreArray() function.
//     int indexIntoCoreArray_X = indexIntoCoreArray(0,coreIndex);
//     int indexIntoCoreArray_Y = indexIntoCoreArray(1,coreIndex);
//     int indexIntoCoreArray_Z = indexIntoCoreArray(2,coreIndex);

  //   int size_X = get_tableBasedDistribution() ? arraySectionSizes[0][coreIndex] : algorithmicComputationOfSize(0,indexIntoCoreArray_X);
  //   int size_Y = get_tableBasedDistribution() ? arraySectionSizes[1][coreIndex] : algorithmicComputationOfSize(1,indexIntoCoreArray_Y);
  //   int size_Z = get_tableBasedDistribution() ? arraySectionSizes[2][coreIndex] : algorithmicComputationOfSize(2,indexIntoCoreArray_Z);

     int size_X = arraySectionSizes[0][coreIndex];
     int size_Y = arraySectionSizes[1][coreIndex];
     int size_Z = arraySectionSizes[2][coreIndex];

#if 0
  // Debugging
     printf ("In mappingFromMultidimintionalMemorySegmentArray(i=%2d,j=%2d,k=%2d,coreIndex=%2d) get_tableBasedDistribution() = %s \n",i,j,k,coreIndex,get_tableBasedDistribution() ? "true" : "false");
     printf ("arraySectionSizes[0][coreIndex=%d] = %d \n",coreIndex,arraySectionSizes[0][coreIndex]);
     printf ("arraySectionSizes[1][coreIndex=%d] = %d \n",coreIndex,arraySectionSizes[1][coreIndex]);
     printf ("arraySectionSizes[2][coreIndex=%d] = %d \n",coreIndex,arraySectionSizes[2][coreIndex]);
     printf ("size_X                             = %d \n",size_X);
     printf ("size_Y                             = %d \n",size_Y);
     printf ("size_Z                             = %d \n",size_Z);
#endif

#if 1
  // This would be most similar to a Fortran ordering.
  // int elementIndex = (k*arraySectionSizes[1][coreIndex]*arraySectionSizes[0][coreIndex]) + (j*arraySectionSizes[0][coreIndex]) + i;
     int elementIndex = (k*size_Y*size_X) + (j*size_X) + i;
     assert(i < size_X);
     assert(j < size_Y);
     assert(k < size_Z);
#else
  // DQ (10/5/2011): This case is not debugged yet.
  // In indexing: a(i,j,k), let k be the fastest moving index (a la C/C++, instead of Fortran).
     int elementIndex = (i*arraySectionSizes[1][coreIndex]*arraySectionSizes[0][coreIndex]) + (j*arraySectionSizes[0][coreIndex]) + k;
     assert(i < arraySectionSizes[2][coreIndex]);
     assert(j < arraySectionSizes[1][coreIndex]);
     assert(k < arraySectionSizes[0][coreIndex]);
#endif

#if 0
     printf ("In mappingFromMultidimintionalMemorySegmentArray(i=%2d,j=%2d,k=%2d,coreIndex=%2d) = %2d \n",i,j,k,coreIndex,elementIndex);
#endif

     assert(elementIndex >= 0);

  // These assertions should be the same.
  // assert(elementIndex < arraySectionSizes[2][coreIndex]*arraySectionSizes[1][coreIndex]*arraySectionSizes[0][coreIndex]);
     assert(elementIndex < size_X*size_Y*size_Z);
     assert(elementIndex < memorySectionSize(coreIndex));

     return elementIndex;
   }




template <typename T>
int
MulticoreArray<T>::memorySectionSize(int core) const
   {
  // This function mostly supports debugging or general information about the array segment per core.
  // This function computes the size (number of elements) in a memory segment given by the index of the 
  // core in the linearized array of cores.

#if 0
     printf ("In memorySectionSize(core=%2d): numberOfCores = %d \n",core,numberOfCores);
#endif

     assert(core < numberOfCores);

     int size = 1;
     for (int dim = 0; dim < DIM; dim++)
        {
//          int index = indexIntoCoreArray(dim,core);
       // size *= (useArraySectionRanges == true) ? arraySectionSizes[dim][core] + padding : algorithmicComputationOfSize(dim,index) + padding;
          size *= arraySectionSizes[dim][core] + padding;
        }
#if 0
     printf ("In memorySectionSize(core=%2d) = %2d \n",core,size);
#endif

     return size;
   }



template <typename T>
void
MulticoreArray<T>::computeArraySectionDistribution()
   {
  // This is the support for the table-based distribution of data across the many-core processor.
  // It is more flexible than the algorithmic-base distribution, but requires more storage.

#if 0
     printf ("In MulticoreArray<T>::computeArraySectionDistribution() useArraySectionRanges = %s \n",useArraySectionRanges ? "true" : "false");
#endif

  // The algorithm based approach does not need or use this sort of support.
     if (useArraySectionRanges == true)
        {
       // int indexInCoreArray[DIM] = { 0, 0, 0 };

          int accumulatedSize[DIM] = {0,0,0};
#if 0
          printf ("BEFORE loop over dimensions \n");
#endif
       // Compute a default table based distribution (could be user specified in a number of ways).
          for (int dim = 0; dim < DIM; dim++)
             {
#if 0
               printf ("^^^^^^^^^ In computeArraySectionDistribution(): dim = %d \n",dim);
#endif
            // These are the remaining two dimensions that are not the same as "dim".
               const int indexOneAway = (dim+1) % 3;
               const int indexTwoAway = (dim+2) % 3;
#if 0
               printf ("indexOneAway = %d indexTwoAway = %d \n",indexOneAway,indexTwoAway);
#endif
               size_t counter = 0;

               int indexInCoreArray[DIM] = { 0, 0, 0 };
#if 0
               printf ("BEFORE loop along axis: indexInCoreArray[0] = %d indexInCoreArray[1] = %d indexInCoreArray[2] = %d \n",indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#endif
            // This is a serial loop over the number of cores (filling in the table of array section distributions amongst cores).
            // Since the default table based distribution could use the algorithmic based distribution, we could simplify this code.
            // for (int p = 0; p < numberOfCores; p++)
               for (int p = 0; p < coreArraySize[dim]; p++)
                  {
#if 0
                    printf ("In computeArraySectionDistribution(): p = %d \n",p);
#endif
                    assert(arraySectionRanges[dim].size() > (size_t)p);
                    assert(arraySectionSizes[dim].size()  > (size_t)p);

                    indexInCoreArray[dim] = p;
#if 0
                    printf ("indexInCoreArray[0] = %d indexInCoreArray[1] = %d indexInCoreArray[2] = %d \n",indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#endif
                    int coreIndexInLinearArray = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#if 0
                    printf ("coreIndexInLinearArray = %d \n",coreIndexInLinearArray);
#endif
                 // arraySectionRanges[dim][p].first = counter;
                    arraySectionRanges[dim][coreIndexInLinearArray].first = counter;

                 // Use the same distribution, but with a table based approach.
                    int tmpBound = counter + algorithmicComputationOfSize(dim,p);

                 // Truncate to the the arraySize[dim] if required...
                    counter = (tmpBound <= arraySize[dim]) ? tmpBound : arraySize[dim];

                 // arraySectionRanges[dim][p].second = counter;
                    arraySectionRanges[dim][coreIndexInLinearArray].second = counter;

                 // int localSize = (arraySectionRanges[dim][p].second - arraySectionRanges[dim][p].first);
                    int localSize = (arraySectionRanges[dim][coreIndexInLinearArray].second - arraySectionRanges[dim][coreIndexInLinearArray].first);
#if 0
                    printf ("In computeArraySectionDistribution(): arraySize[dim=%d] = %d localSize = %d \n",dim,arraySize[dim],localSize);
#endif
                 // arraySectionSizes[dim][p] = localSize;
                    arraySectionSizes[dim][coreIndexInLinearArray] = localSize;
                    if(boundaryType == MulticoreArray<T>::attached)
                      arraySectionSizes[dim][coreIndexInLinearArray] += 2 * haloSize[dim];

                 // assert(arraySize[dim] == 0 || localSize != 0);
                    assert(arraySize[dim] == 0 || localSize != 0 || p > 0);

                    assert(localSize == 0 || arraySectionSizes[dim][coreIndexInLinearArray] != 0);
#if 0
                    printf ("Setting size on target core arraySectionSizes[dim=%d][coreIndexInLinearArray=%d] = %d \n",dim,coreIndexInLinearArray,arraySectionSizes[dim][coreIndexInLinearArray]);
#endif
                 // Now set all sizes for the cores in this axis of the coreArray to have value
#if 0
                    printf ("----- Set the other core entries in this row/column of the core array to have the same value. \n");
#endif
                    int indexInCoreArrayOfOtherCores[DIM] = { indexInCoreArray[0], indexInCoreArray[1], indexInCoreArray[2] };
#if 0
                    printf ("coreArraySize[0] = %d coreArraySize[1] = %d coreArraySize[2] = %d \n",coreArraySize[0],coreArraySize[1],coreArraySize[2]);
                    printf ("coreArraySize[indexOneAway=%d] = %d \n",indexOneAway,coreArraySize[indexOneAway]);
                    printf ("coreArraySize[indexTwoAway=%d] = %d \n",indexTwoAway,coreArraySize[indexTwoAway]);
#endif
                    assert(indexOneAway >= 0 && indexOneAway < 3);
                    assert(indexTwoAway >= 0 && indexTwoAway < 3);

                 // Set the associated array size for "dim" in all the other cores entries for this value of "p" along axis "dim".
                    for (int j = 0; j < coreArraySize[indexTwoAway]; j++)
                       {
                      // Compute the index that is 2 away from the current value of "dim"
                         indexInCoreArrayOfOtherCores[indexTwoAway] = j;

                         for (int i = 0; i < coreArraySize[indexOneAway]; i++)
                            {
                              indexInCoreArrayOfOtherCores[indexOneAway] = i;

                              int coreIndexInLinearArrayOfOtherCores = coreArrayElement(indexInCoreArrayOfOtherCores[0],indexInCoreArrayOfOtherCores[1],indexInCoreArrayOfOtherCores[2]);
#if 0
                              printf ("coreIndexInLinearArrayOfOtherCores = %d \n",coreIndexInLinearArrayOfOtherCores);
#endif
                              arraySectionSizes[dim][coreIndexInLinearArrayOfOtherCores] = localSize;
                              if(boundaryType == MulticoreArray<T>::attached)
                                arraySectionSizes[dim][coreIndexInLinearArrayOfOtherCores] += 2 * haloSize[dim];

                           // Also set the ranges on all of the other processors.
                              arraySectionRanges[dim][coreIndexInLinearArrayOfOtherCores].first  = arraySectionRanges[dim][coreIndexInLinearArray].first;
                              arraySectionRanges[dim][coreIndexInLinearArrayOfOtherCores].second = arraySectionRanges[dim][coreIndexInLinearArray].second;
#if 0
                              printf ("Setting size on other cores (i=%d,j=%d) arraySectionSizes[dim=%d][coreIndexInLinearArray=%d] = %d \n",i,j,dim,coreIndexInLinearArrayOfOtherCores,arraySectionSizes[dim][coreIndexInLinearArrayOfOtherCores]);
#endif
                            }
                       }

#if 0
                 // printf ("In computeArraySectionDistribution(): arraySectionSizes[dim][p] = %zu \n",arraySectionSizes[dim][p]);
                    printf ("In computeArraySectionDistribution(): arraySectionSizes[dim=%2d][coreIndexInLinearArray=%2d] = %zu \n",dim,coreIndexInLinearArray,arraySectionSizes[dim][coreIndexInLinearArray]);
#endif
#if 0
                 // printf ("In computeArraySectionDistribution(): arraySize[dim=%d] = %d localSize = %2d arraySectionRanges[dim=%d][p=%2d].first = %3zu arraySectionRanges[dim=%d][p=%2d].second = %3zu \n",
                 //      dim,arraySize[dim],localSize,dim,p,arraySectionRanges[dim][p].first,dim,p,arraySectionRanges[dim][p].second);
                    printf ("In computeArraySectionDistribution(): arraySize[dim=%d] = %d localSize = %2d arraySectionRanges[dim=%d][coreIndexInLinearArray=%2d].first = %3zu arraySectionRanges[dim=%d][coreIndexInLinearArray=%2d].second = %3zu \n",
                         dim,arraySize[dim],localSize,dim,coreIndexInLinearArray,arraySectionRanges[dim][coreIndexInLinearArray].first,dim,coreIndexInLinearArray,arraySectionRanges[dim][coreIndexInLinearArray].second);
#endif
                 // assert (arraySectionRanges[dim][p].second <= arraySize[dim]);
                    assert (arraySectionRanges[dim][coreIndexInLinearArray].second <= (size_t)arraySize[dim]);
                  }

#if 1
            // ******************************************************************************************************
            // Verification code (make sure that each dimension stores the same size for each core in the same axis).
            // And also that the size for each memory segment for all cores along an axis of the coreArray adds up to 
            // the size of the array for that dimension (axis).
            // ******************************************************************************************************
#if 0
               printf ("In computeArraySectionDistribution(VERIFY): coreArraySize[dim=%2d] = %2d \n",dim,coreArraySize[dim]);
               printf ("In computeArraySectionDistribution(VERIFY): arraySectionSizes[dim=%2d][0] = %2d \n",dim,arraySectionSizes[dim][0]);
#endif
               for (int core = 0; core < coreArraySize[dim]; core++)
                  {
                 // This is calling the wrong function, indexIntoCoreArray() is designed to "take as input" the core 
                 // index in the linear array and return the index in the core array associated with a specific dimension.
                 // int coreIndexInLinearArray = indexIntoCoreArray(dim,core);

                    indexInCoreArray[dim] = core;
#if 0
                    printf ("indexInCoreArray[0] = %d indexInCoreArray[1] = %d indexInCoreArray[2] = %d \n",indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#endif
                    int coreIndexInLinearArray = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);

                 // int local_size = (useArraySectionRanges == true) ? arraySectionSizes[dim][core] : algorithmicComputationOfSize(dim,core);
                    int local_size = (useArraySectionRanges == true) ? arraySectionSizes[dim][coreIndexInLinearArray] : algorithmicComputationOfSize(dim,core);

                    int indexInCoreArrayOfOtherCores[DIM] = { indexInCoreArray[0], indexInCoreArray[1], indexInCoreArray[2] };

                    for (int j = 0; j < coreArraySize[indexTwoAway]; j++)
                       {
                      // Compute the index that is 2 away from the current value of "dim"
                         indexInCoreArrayOfOtherCores[indexTwoAway] = j;

                         for (int i = 0; i < coreArraySize[indexOneAway]; i++)
                            {
                              indexInCoreArrayOfOtherCores[indexOneAway] = i;

                           // arraySectionSizes[dim][coreIndexInLinearArrayForOtherCores] = localSize;
                              int coreIndexInLinearArrayOfOtherCores = coreArrayElement(indexInCoreArrayOfOtherCores[0],indexInCoreArrayOfOtherCores[1],indexInCoreArrayOfOtherCores[2]);
                           // printf ("coreIndexInLinearArrayOfOtherCores = %d \n",coreIndexInLinearArrayOfOtherCores);
                              int sizeOfOtherCores = (useArraySectionRanges == true) ? arraySectionSizes[dim][coreIndexInLinearArrayOfOtherCores] : algorithmicComputationOfSize(dim,core);

                           // printf ("sizeOfOtherCores = %d local_size = %d \n",sizeOfOtherCores,local_size);

                              assert(sizeOfOtherCores == local_size);
                            }
                       }

#if 0
                    printf ("In computeArraySectionDistribution(VERIFY): core = %2d coreIndexInLinearArray = %2d local_size = %d \n",core,coreIndexInLinearArray,local_size);
#endif
                    accumulatedSize[dim] += local_size;
                  }
               if(boundaryType == MulticoreArray<T>::attached)
               {
                 if (accumulatedSize[dim] != (arraySize[dim] + 2 * coreArraySize[dim] * haloSize[dim]))
                    {
                      printf ("In computeArraySectionDistribution(VERIFY) attached Halo: overallSize[%d] = %2d accumulatedSize[%d] = %2d \n",dim,arraySize[dim]+ 2 * coreArraySize[dim] * haloSize[dim],dim,accumulatedSize[dim]);
                    }

                 assert(accumulatedSize[dim] == (arraySize[dim] + 2 * coreArraySize[dim] * haloSize[dim]));
               }
               else
               {
                 if (accumulatedSize[dim] != arraySize[dim])
                    {
                      printf ("In computeArraySectionDistribution(VERIFY) non-attached halo: arraySize[%d] = %2d accumulatedSize[%d] = %2d \n",dim,arraySize[dim],dim,accumulatedSize[dim]);
                    }

                 assert(accumulatedSize[dim] == arraySize[dim]);
               }
#endif

            // We can have valid zero sizes parts of an array abstraction along any dimension where arraySectionSizes[dim][0] > 0.
               assert(arraySectionSizes[dim][0] > 0);
             }

       // Additional error checking...
          for (int coreIndex = 0; coreIndex < numberOfCores; coreIndex++)
             {
               for (int dim = 0; dim < DIM; dim++)
                  {
                    if (arraySectionSizes[dim][0] == 0 && arraySectionSizes[dim][coreIndex] == 0)
                       {
                         printf ("Error: arraySectionSizes[dim=%d][coreIndex=%d] == 0 \n",dim,coreIndex);
                       }
                    assert(arraySectionSizes[dim][0] > 0 || arraySectionSizes[dim][coreIndex] == 0);
                  }

            // Compute the indexing into the coreArray both ways and verify it is the same.
               int indexInCoreArray[DIM] = { indexIntoCoreArray(0,coreIndex), indexIntoCoreArray(1,coreIndex), indexIntoCoreArray(2,coreIndex) };
               int coreIndexInLinearArray = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);

            // Double check that we compute the correct core index.
               assert(coreIndexInLinearArray == coreIndex);

            // Double check against the other implementation of this function.
               int coreSizeFromLinearizedCoreIndex            = memorySectionSize(coreIndex);
               int coreSizeFromMultidimensionalCoreArrayIndex = memorySectionSize(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);

               assert(coreSizeFromLinearizedCoreIndex == coreSizeFromMultidimensionalCoreArrayIndex);
             }
        }
        else
        {
          int indexInCoreArray[DIM] = {0,0,0};
          for (int dim = 0; dim < DIM; dim++)
             {
            for (int p = 0; p < get_numberOfCores(); p++)
               {
                 indexInCoreArray[0] = p % coreArraySize[0];
                 indexInCoreArray[1] = (p % (coreArraySize[0] * coreArraySize[1]) / coreArraySize[0]);
                 indexInCoreArray[2] = p / (coreArraySize[0] * coreArraySize[1]); 
                 arraySectionSizes[dim][p] = algorithmicComputationOfSize(dim, indexInCoreArray[dim]);
                 if(boundaryType == MulticoreArray<T>::attached)
                   arraySectionSizes[dim][p] += 2 * haloSize[dim]; 
               }
             }
        }

#if 0
     printf ("Leaving MulticoreArray<T>::computeArraySectionDistribution() \n");
#endif
   }

template <typename T>
void
MulticoreArray<T>::computeHaloRegionSize()
   {
//     if (useArraySectionRanges == true)
        {
          for (int p = 0; p < get_numberOfCores(); p++)
             {
               if(haloSize[0] > 0)
                 haloRegionsSizes[0][p] = haloSize[0] * arraySectionSizes[1][p] * arraySectionSizes[2][p];
               if(haloSize[1] > 0)
                 haloRegionsSizes[1][p] = haloSize[1] * (arraySectionSizes[0][p] + 2 * haloSize[0]) * arraySectionSizes[2][p];
               if(haloSize[2] > 0)
                 haloRegionsSizes[2][p] = haloSize[2] * (arraySectionSizes[1][p] + 2 * haloSize[0]) * (arraySectionSizes[1][p] + 2 * haloSize[1]);
//               cout <<  "core ="<< p << " halo size=" << haloRegionsSizes[0][p] << " " << haloRegionsSizes[1][p]<< " " <<  haloRegionsSizes[2][p] << endl;
	     }
         }
/*
       else
         {
     int indexInCoreArray[DIM] = {0,0,0};
     for (int p = 0; p < numberOfCores; p++)
    {
        indexInCoreArray[0] = p % coreArraySize[0];
        indexInCoreArray[1] = (p % (coreArraySize[0] * coreArraySize[1]) / coreArraySize[0]);
        indexInCoreArray[2] = p / (coreArraySize[0] * coreArraySize[1]); 

                     if(haloSize[0] > 0)
                       haloRegionsSizes[0][p] = haloSize[0] * algorithmicComputationOfSize(1, indexInCoreArray[1]) * algorithmicComputationOfSize(2, indexInCoreArray[2]);
                     if(haloSize[1] > 0)
                       haloRegionsSizes[1][p] = haloSize[1] * (algorithmicComputationOfSize(0, indexInCoreArray[0]) + 2 * haloSize[0]) * algorithmicComputationOfSize(2, indexInCoreArray[2]);
                     if(haloSize[2] > 0)
                       haloRegionsSizes[2][p] = haloSize[2] * (algorithmicComputationOfSize(0, indexInCoreArray[0]) + 2 * haloSize[0]) * (algorithmicComputationOfSize(1, indexInCoreArray[1]) + 2 * haloSize[1]);
                     cout <<  "W/O useArraySectionRanges core ="<< p << " halo size=" << haloRegionsSizes[0][p] << " " << haloRegionsSizes[1][p]<< " " <<  haloRegionsSizes[2][p] << endl;
                   }
         }
*/
  }

template <typename T>
void
MulticoreArray<T>::allocateMemorySectionsPerCore()
   {
  // This is the memory allocation support for each core to allocate memory that is as close as possible to it
  // within the NUMA processor architecture (requires libnuma for best portable allocation of closest memory 
  // to each core).

     assert(arraySectionPointers != NULL);
     assert(haloSectionPointers != NULL);

  // This should be an threaded operation so that allocations will be in memory 
  // best suited to where that thread will run (when allocated using libnuma).

  // This is the structure we want to have so that we can most easily support operations over all cores (allows us to use OpenMP for control parallelism).
#if HAVE_NUMA_H
//printf("dist into %d %d %d ====> assigned to %d %d %d\n",coreArraySize[0],coreArraySize[1],coreArraySize[2],NUMAcoreArraySize[0], NUMAcoreArraySize[1], NUMAcoreArraySize[2]);

// Using omp parallel for and sched_setaffinity to lock hardware core to the OpenMP thread.
     int scheduledNUMAcores = NUMAcoreArraySize[0] * NUMAcoreArraySize[1] * NUMAcoreArraySize[2];
     omp_set_num_threads(scheduledNUMAcores);
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
       int idz = physicalCore / (NUMAcoreArraySize[0] * NUMAcoreArraySize[1]);
       int idy = (physicalCore - idz * (NUMAcoreArraySize[0] * NUMAcoreArraySize[1])) / NUMAcoreArraySize[0];
       int idx = physicalCore % NUMAcoreArraySize[0];
       int XcorePerNUMAcore = coreArraySize[0]/NUMAcoreArraySize[0];
       int YcorePerNUMAcore = coreArraySize[1]/NUMAcoreArraySize[1];
       int ZcorePerNUMAcore = coreArraySize[2]/NUMAcoreArraySize[2];
       int baseX = idx * (XcorePerNUMAcore);
       int baseY = idy * (YcorePerNUMAcore);
       int baseZ = idz * (ZcorePerNUMAcore);
       for(int ix = 0 ; ix < XcorePerNUMAcore; ++ix) 
       for(int iy = 0 ; iy < YcorePerNUMAcore; ++iy) 
       for(int iz = 0 ; iz < ZcorePerNUMAcore; ++iz)
       {
         int coreIdx = coreArrayElement(baseX+ix, baseY+iy, baseZ+iz);
//         printf("HW core ID: %d thread ID: %d is allocating logical core %d: %d %d %d\n",sched_getcpu(), tid, coreIdx, baseX+ix, baseY+iy, baseZ+iz);
          int size = memorySectionSize(coreIdx);
       // Allocate memory using libnuma to get local memory for the associated core.
	// printf("In NUMA: core %d will allocate size %d\n",sched_getcpu(), size);
          arraySectionPointers[coreIdx] = (T*) numa_alloc_local((size_t)(size*sizeof(T)));
//          arraySectionPointers[coreIdx] = (T*) numa_alloc_onnode((size_t)(size*sizeof(T)), sched_getcpu()/8);

       // Interestingly, libnuma will return a NULL pointer if ask to allocate zero bytes (but we want the semantics to be consistant with C++ allocation).
          if (size == 0 && arraySectionPointers[coreIdx] == NULL)
             {
               arraySectionPointers[coreIdx] = new T[size];
               assert(arraySectionPointers[coreIdx] != NULL);
             }

          for(int dim=0; dim < DIM; dim++)
          {
            if((boundaryType == MulticoreArray<T>::detached) && (haloSize[dim] > 0))
            {
                assert(haloRegionsSizes[dim][coreIdx] != 0);
		haloSectionPointers[dim][0][coreIdx] = (T*) numa_alloc_local((size_t)(haloRegionsSizes[dim][coreIdx]*sizeof(T)));
		haloSectionPointers[dim][1][coreIdx] = (T*) numa_alloc_local((size_t)(haloRegionsSizes[dim][coreIdx]*sizeof(T)));
                assert(haloSectionPointers[dim][0][coreIdx] != NULL);
                assert(haloSectionPointers[dim][1][coreIdx] != NULL);
            }
          }

       // assert(size == 0 || arraySectionPointers[coreIdx] != NULL);
          assert(arraySectionPointers[coreIdx] != NULL);

       // Initialize the memory section pointer stored in the Core<T>.
       //   assert(coreArray[coreIdx] != NULL);
       //   coreArray[coreIdx]->arraySectionPointer[1][1][1] = arraySectionPointers[coreIdx];
       //   assert(coreArray[coreIdx]->arraySectionPointer[1][1][1] != NULL);

          for(int dim=0; dim < DIM; dim++)
          {
            if((boundaryType == MulticoreArray<T>::detached) && (haloSize[dim] > 0))
            {
              coreArray[coreIdx]->haloRegionPointer[dim][0] = haloSectionPointers[dim][0][coreIdx];
              coreArray[coreIdx]->haloRegionPointer[dim][1] = haloSectionPointers[dim][1][coreIdx];
              assert(coreArray[coreIdx]->haloRegionPointer[dim][0] != NULL);
              assert(coreArray[coreIdx]->haloRegionPointer[dim][1] != NULL);
            }
          }
       } 
     }
#pragma omp barrier

#else
#pragma omp parallel for
     for (int core = 0; core < numberOfCores; core++)
        {
          int size = memorySectionSize(core);
// #ifndef NUMA_NUM_NODES
          arraySectionPointers[core] = new T[size];
          for(int dim=0; dim < DIM; dim++)
          {
            if((boundaryType == MulticoreArray<T>::detached) && (haloSize[dim] > 0))
            {
		haloSectionPointers[dim][0][core] = new T[haloRegionsSizes[dim][core]]; 
		haloSectionPointers[dim][1][core] = new T[haloRegionsSizes[dim][core]]; 
            }
          }

       // assert(size == 0 || arraySectionPointers[core] != NULL);
          assert(arraySectionPointers[core] != NULL);

       // Initialize the memory section pointer stored in the Core<T>.
       //   assert(coreArray[core] != NULL);
       //   coreArray[core]->arraySectionPointer[1][1][1] = arraySectionPointers[core];
       //   assert(coreArray[core]->arraySectionPointer[1][1][1] != NULL);

          for(int dim=0; dim < DIM; dim++)
          {
            if((boundaryType == MulticoreArray<T>::detached) && (haloSize[dim] > 0))
            {
              coreArray[core]->haloRegionPointer[dim][0] = haloSectionPointers[dim][0][core];
              coreArray[core]->haloRegionPointer[dim][1] = haloSectionPointers[dim][1][core];
              assert(coreArray[core]->haloRegionPointer[dim][0] != NULL);
              assert(coreArray[core]->haloRegionPointer[dim][1] != NULL);
            }
          }
#if 0
          printf("tableBasedDistribution = %s data size for arraySize[0] = %d core = %2d has size = %2d with padding = %d arraySectionPointers[%d] = %p \n",(useArraySectionRanges == true) ? "true" : "false",arraySize[0],core,size,padding,core,arraySectionPointers[core]);
#endif
        }
#endif
   }


template <typename T>
void
MulticoreArray<T>::initializeDataPlusPadding(const T & x)
   {
  // This function initializes all of the allocated memory (including padding) and is mostly
  // useful to support debugging (avoids possible random values from uninitialized memory).
  // However, not initializing memory allows dynamic analysis tools like valgrind to support
  // detection of errors within debugging, so it should be used as needed.

     assert(arraySectionPointers != NULL);

  // This is the faster way to support the initialization of partitioned array
  // assuming that we should even both to do the initialization.
#pragma omp parallel for
     for (int core = 0; core < numberOfCores; core++)
        {
//printf("core %d is updating thread %d\n",sched_getcpu(),core);
          T* arraySection = arraySectionPointers[core];

          int size = memorySectionSize(core);
          int sizeWithPadding = size + padding;
          assert(size == 0 || arraySection != NULL);

          for (int i = 0; i < sizeWithPadding; i++)
             {
               arraySection[i] = x;
             }
        }
   }


template <typename T>
MulticoreArray<T> &
MulticoreArray<T>::operator=(const T & x)
   {
  // This is support for simple scalar assignment of the array abstraction (useful in writting test codes).

     assert(arraySectionPointers != NULL);

  // This is the faster way to support the assignment of the partitioned array.
#pragma omp parallel for
     for (int core = 0; core < numberOfCores; core++)
        {
          T* arraySection = arraySectionPointers[core];

          int size = memorySectionSize(core);
          assert(size == 0 || arraySection != NULL);

          for (int i = 0; i < size; i++)
             {
               arraySection[i] = x;
             }
        }

     return *this;
   }


template <typename T>
void
MulticoreArray<T>::sequenceFill3D(const T & x)
   {
  // This is support for simple scalar assignment of the array abstraction (useful in writting test codes).
  // This is expensive since it is implemented using the scalar indexing support (which forces a lot of
  // index computaions).

     assert(arraySectionPointers != NULL);

  // This is a sequential way to iterate through the multi-dimensional array, and slow, but it
  // is only to support debugging. A faster way would be to parallelize over all of the cores.
//     int count = 1;
#pragma omp parallel for
     for (int k = 0; k < arraySize[2]; k++)
        {
          for (int j = 0; j < arraySize[1]; j++)
             {
               for (int i = 0; i < arraySize[0]; i++)
                  {
                      (*this)(i,j,k) = x * (i + j * arraySize[0] + k * arraySize[0]*arraySize[1]);
                  }
             }
        }
   }


template <typename T>
void
MulticoreArray<T>::sequenceFill(const T & x)
   {
  // This is support for simple scalar assignment of the array abstraction (useful in writting test codes).
  // This is expensive since it is implemented using the scalar indexing support (which forces a lot of
  // index computaions).

     assert(arraySectionPointers != NULL);

  // This is a sequential way to iterate through the multi-dimensional array, and slow, but it
  // is only to support debugging. A faster way would be to parallelize over all of the cores.
//     int count = 1;
     for (int k = 0; k < arraySize[2]; k++)
        {
// #pragma omp for
#pragma omp parallel for
          for (int j = 0; j < arraySize[1]; j++)
             {
               for (int i = 0; i < arraySize[0]; i++)
                  {
                 // Fill in with values starting at 1
#if 0
                    (*this)(i,j,k) = x * count;
#else
                      (*this)(i,j,k) = x * (i + j * arraySize[0]);
#endif
                  }
             }
        }
   }


template <typename T>
void
MulticoreArray<T>::initializeDataWithCoreArrayIndex()
   {
  // This is support for debugging (testing different initializations).
  // This initializes the memory on each core with the index of the 
  // linearized array of cores.  This permits some visual representation 
  // of the memory assighments per for when using the display() function
  // for different array sizes on different coreArray sizes.

     assert(arraySectionPointers != NULL);

  // This is the faster way to support the assignment of the partitioned array.
#pragma omp parallel for
     for (int core = 0; core < numberOfCores; core++)
        {
          T* arraySection = arraySectionPointers[core];

          int size = memorySectionSize(core);
          assert(size == 0 || arraySection != NULL);

          for (int i = 0; i < size; i++)
             {
               arraySection[i] = core;
             }
        }
   }

// *************************************************
template <typename T>
void
MulticoreArray<T>::initializeCoreArrayNeighborhoods()
   {
#pragma omp parallel for 
     for (int p = 0; p < numberOfCores; p++)
        {
       // Note that we want to allow for p to wrap as an index so that we can interprete the memory sections as a circular buffer.
       // Also we can't use a simplere for such as: "previous_p = (p-1) % numberOfCores" and "next_p = (p+1) % numberOfCores".
          assert(p >= 0 && p < numberOfCores);
          assert(coreArray != NULL);
          Core<T>* core = coreArray[p];
          assert(core != NULL);

//          coreArray[p]->initializeCoreArrayNeighborhoods(p);

          int indexIntoMultidimensionalCoreArrayForThisDimension[3] = {0,0,0};
          for (int dim = 0; dim < DIM; dim++)
             {
               indexIntoMultidimensionalCoreArrayForThisDimension[dim] = indexIntoCoreArray(dim,p);
               core->arraySectionSize[dim] = get_arraySectionSizes(dim)[p]; 
             }
          // Initialize the core array neighborhood
             for (int dim = 0; dim < 3; dim++)
                {
                  for (int k = 0; k < 3; k++)
                     {
                       core->coreArrayNeighborhood_1D[k][dim] = 0;
                       for (int j = 0; j < 3; j++)
                          {
                            core->coreArrayNeighborhood_2D[k][j][dim] = 0;
                            for (int i = 0; i < 3; i++)
                               {
                                 core->coreArrayNeighborhood_3D[k][j][i][dim] = 0;
                               }
                          }
                     }
                }

              // *******************************************************************************
              // The current core's index into the core array in the center of the Neighborhood:
              // *******************************************************************************
                 core->coreArrayNeighborhood_1D[1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
                 core->coreArrayNeighborhood_1D[1][1] = indexIntoMultidimensionalCoreArrayForThisDimension[1];
                 core->coreArrayNeighborhood_1D[1][2] = indexIntoMultidimensionalCoreArrayForThisDimension[2];
              
                 core->coreArrayNeighborhood_2D[1][1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
                 core->coreArrayNeighborhood_2D[1][1][1] = indexIntoMultidimensionalCoreArrayForThisDimension[1];
                 core->coreArrayNeighborhood_2D[1][1][2] = indexIntoMultidimensionalCoreArrayForThisDimension[2];
              
                 core->coreArrayNeighborhood_3D[1][1][1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
                 core->coreArrayNeighborhood_3D[1][1][1][1] = indexIntoMultidimensionalCoreArrayForThisDimension[1];
                 core->coreArrayNeighborhood_3D[1][1][1][2] = indexIntoMultidimensionalCoreArrayForThisDimension[2];
              
              // **************************************************************
              // Setup the 1D case (other entries already initialized to zero):
              // **************************************************************
//                core->coreArrayNeighborhood_1D[1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
                 core->coreArrayNeighborhood_1D[0][0] = get_left_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
                 core->coreArrayNeighborhood_1D[2][0] = get_right_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);

                 core->coreArrayNeighborhood_1D[0][1] = core->coreArrayNeighborhood_1D[1][1];
                 core->coreArrayNeighborhood_1D[2][1] = core->coreArrayNeighborhood_1D[1][1];

                 core->coreArrayNeighborhood_1D[0][2] = core->coreArrayNeighborhood_1D[1][2];
                 core->coreArrayNeighborhood_1D[2][2] = core->coreArrayNeighborhood_1D[1][2];


              // *****************************************************************************
              // Setup the 2D case (other entries (center index) already initialized to zero):
              // *****************************************************************************
              // Indexing order:          Y::X::axis
                 core->coreArrayNeighborhood_2D[1][0][0] = get_left_section_index (0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
                 core->coreArrayNeighborhood_2D[1][2][0] = get_right_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
                 core->coreArrayNeighborhood_2D[0][1][1] = get_left_section_index (1,indexIntoMultidimensionalCoreArrayForThisDimension[1]);
                 core->coreArrayNeighborhood_2D[2][1][1] = get_right_section_index(1,indexIntoMultidimensionalCoreArrayForThisDimension[1]);
             
              // Setup the 2D case (other entries (center index) already initialized to zero):
              // Turn this into a loop to see how to make it a formula
                 for (int j = 0; j < 3; j++)
                    {
                      for (int i = 0; i < 3; i++)
                         {
                           core->coreArrayNeighborhood_2D[j][i][2] = core->coreArrayNeighborhood_2D[1][1][2];
                           if (i == 1 || j != 1)
                              {
                                core->coreArrayNeighborhood_2D[j][i][0] = core->coreArrayNeighborhood_2D[1][1][0];
                              }
              
                           if (i != 1 || j == 1)
                              {
                                core->coreArrayNeighborhood_2D[j][i][1] = core->coreArrayNeighborhood_2D[1][1][1];
                              }
              
                           if (i != 1 && j != 1)
                              {
                                core->coreArrayNeighborhood_2D[j][i][0] = core->coreArrayNeighborhood_2D[1][i][0];
                                core->coreArrayNeighborhood_2D[j][i][1] = core->coreArrayNeighborhood_2D[j][1][1];
                              }
                         }
                    }
              
              
              // *****************************************************************************
              // Setup the 3D case (other entries (center index) already initialized to zero):
              // *****************************************************************************
              // Indexing order:          Z::Y::X::axis
                 core->coreArrayNeighborhood_3D[1][1][0][0] = get_left_section_index (0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
                 core->coreArrayNeighborhood_3D[1][1][2][0] = get_right_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
                 core->coreArrayNeighborhood_3D[1][0][1][1] = get_left_section_index (1,indexIntoMultidimensionalCoreArrayForThisDimension[1]);
                 core->coreArrayNeighborhood_3D[1][2][1][1] = get_right_section_index(1,indexIntoMultidimensionalCoreArrayForThisDimension[1]);
                 core->coreArrayNeighborhood_3D[0][1][1][2] = get_left_section_index (2,indexIntoMultidimensionalCoreArrayForThisDimension[2]);
                 core->coreArrayNeighborhood_3D[2][1][1][2] = get_right_section_index(2,indexIntoMultidimensionalCoreArrayForThisDimension[2]);
              
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
                         core->coreArrayNeighborhood_3D[k][j][i][dim] = 0;
                       }
#endif
                               if(j != 1 || k != 1)
                               {
                                    core->coreArrayNeighborhood_3D[k][j][i][0] = core->coreArrayNeighborhood_3D[1][1][i][0];
                               }
                               if(i != 1 || k != 1)
                               {
                                    core->coreArrayNeighborhood_3D[k][j][i][1] = core->coreArrayNeighborhood_3D[1][j][1][1];
                               }
                               if(i != 1 || j != 1)
                               {
                                    core->coreArrayNeighborhood_3D[k][j][i][2] = core->coreArrayNeighborhood_3D[k][1][1][2];
                               }
                            }
                       }
                  }
        }
   }


// *************************************************









// *************************************************
template <typename T>
void
MulticoreArray<T>::initializeLinearizedCoreArrayNeighborhoods()
   {
  // Initialize the mapping of the core array neighborhoods to the linearized array of cores.
  // This provides a simple API used for the compiler generated code.

#pragma omp parallel for 
     for (int p = 0; p < numberOfCores; p++)
        {
          assert(p >= 0 && p < numberOfCores);
          assert(coreArray != NULL);
          Core<T>* core = coreArray[p];
          assert(core != NULL);

//          coreArray[p]->initializeLinearizedCoreArrayNeighborhoods(p);
          for (int i = 0; i < 3; i++)
             {
               core->coreArrayNeighborhoodLinearized_1D[i] = coreArrayElement(core->coreArrayNeighborhood_1D[i][0],core->coreArrayNeighborhood_1D[i][1],core->coreArrayNeighborhood_1D[i][2]);
//               core->arraySectionPointer[1][1][i] = arraySectionPointers[core->coreArrayNeighborhoodLinearized_1D[i]]; 
             }

 //       Setup the linearized index entries for 2D
          for (int j = 0; j < 3; j++)
             {
               for (int i = 0; i < 3; i++)
                  {
                    core->coreArrayNeighborhoodLinearized_2D[j][i] = coreArrayElement(core->coreArrayNeighborhood_2D[j][i][0],core->coreArrayNeighborhood_2D[j][i][1],core->coreArrayNeighborhood_2D[j][i][2]);
//                    core->arraySectionPointer[1][j][i] = arraySectionPointers[core->coreArrayNeighborhoodLinearized_2D[j][i]]; 
                  }
             }

          for (int k = 0; k < 3; k++)
             {
               for (int j = 0; j < 3; j++)
                  {
                    for (int i = 0; i < 3; i++)
                       {
                         core->coreArrayNeighborhoodLinearized_3D[k][j][i] = coreArrayElement(core->coreArrayNeighborhood_3D[k][j][i][0],core->coreArrayNeighborhood_3D[k][j][i][1],core->coreArrayNeighborhood_3D[k][j][i][2]);
                         core->arraySectionPointer[k][j][i] = arraySectionPointers[core->coreArrayNeighborhoodLinearized_3D[k][j][i]];
                       }
                  }
             }
        }
   }


template <typename T>
void
MulticoreArray<T>::initializeCoreArraySizeNeighborhoods()
   {
  // This is the size information for each element in the core array neighborhoods (and for each axis direction of multidimensional arrays).
  // This stored data simplifies how we can reference precomputed data.

#pragma omp parallel for 
     for (int p = 0; p < numberOfCores; p++)
        {
          assert(p >= 0 && p < numberOfCores);
          assert(coreArray != NULL);
          Core<T>* core = coreArray[p];
          assert(core != NULL);

//          coreArray[p]->initializeCoreArraySizeNeighborhoods(p);
           // Setup the linearized index entries for 1D
              for (int i = 0; i < 3; i++)
                 {
                   for (int dim = 0; dim < 3; dim++)
                      {
                     // core->coreArrayNeighborhoodSizes_1D[i] = (array.get_tableBasedDistribution() == true) ? array.get_arraySectionSizes(0)[core->coreArrayNeighborhoodLinearized_1D[i]] : array.algorithmicComputationOfSize(0,i);
                        core->coreArrayNeighborhoodSizes_1D[i][dim] = 0;
           
                    //  core->coreArrayNeighborhoodSizes_1D[i][dim] = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[core->coreArrayNeighborhoodLinearized_1D[i]] : algorithmicComputationOfSize(dim,core->coreArrayNeighborhood_1D[i][dim]);
                        core->coreArrayNeighborhoodSizes_1D[i][dim] = get_arraySectionSizes(dim)[core->coreArrayNeighborhoodLinearized_1D[i]];
                      }
                 }
           
           // Setup the linearized index entries for 2D
              for (int j = 0; j < 3; j++)
                 {
                   for (int i = 0; i < 3; i++)
                      {
                        for (int dim = 0; dim < 3; dim++)
                           {
                        //   core->coreArrayNeighborhoodSizes_2D[j][i][dim] = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[core->coreArrayNeighborhoodLinearized_2D[j][i]] : algorithmicComputationOfSize(dim,core->coreArrayNeighborhood_2D[j][i][dim]);
                             core->coreArrayNeighborhoodSizes_2D[j][i][dim] = get_arraySectionSizes(dim)[core->coreArrayNeighborhoodLinearized_2D[j][i]];
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
                              //  core->coreArrayNeighborhoodSizes_3D[k][j][i][dim] = (get_tableBasedDistribution() == true) ? get_arraySectionSizes(dim)[core->coreArrayNeighborhoodLinearized_3D[k][j][i]] : algorithmicComputationOfSize(dim,core->coreArrayNeighborhood_3D[k][j][i][dim]);
                                  core->coreArrayNeighborhoodSizes_3D[k][j][i][dim] = get_arraySectionSizes(dim)[core->coreArrayNeighborhoodLinearized_3D[k][j][i]];
                                }
                           }
                      }
                 }

            // Bounds checking
               for (int i = 0; i < 3; i++)
                  {
                    for (int dim = 0; dim < 3; dim++)
                       {
                         assert(core->coreArrayNeighborhoodSizes_1D[i][dim] >= 0 && core->coreArrayNeighborhoodSizes_1D[i][dim] < 1000000);
                       }
                  }
            
            // Bounds checking
               for (int j = 0; j < 3; j++)
                  {
                    for (int i = 0; i < 3; i++)
                       {
                         for (int dim = 0; dim < 3; dim++)
                            {
                              assert(core->coreArrayNeighborhoodSizes_2D[j][i][dim] >= 0 && core->coreArrayNeighborhoodSizes_2D[j][i][dim] < 1000000);
                            }
                       }
                  }
        }
   }

template <typename T>
void
MulticoreArray<T>::initializeCoreArrayBoundaryFlags()
   {
  // Mark where core array sections represent boundaries of the many-core distributed array (along each axis).

  // 3D supp0ort not in place yet.

#pragma omp parallel for 
     for (int p = 0; p < numberOfCores; p++)
        {
          assert(p >= 0 && p < numberOfCores);
          assert(coreArray != NULL);
          Core<T>* core = coreArray[p];
          assert(core != NULL);

//          coreArray[p]->initializeCoreArrayBoundaryFlags(p);
          core->boundaryCore_1D[0]    = is_left_edge_section(core->coreArrayNeighborhood_1D [1][0],core->coreArrayNeighborhood_1D[0][0]);
          core->boundaryCore_1D[1]    = is_right_edge_section(core->coreArrayNeighborhood_1D[1][0],core->coreArrayNeighborhood_1D[2][0]);
       
       // X axis        X:Y L:U
          core->boundaryCore_2D[0][0] = is_left_edge_section(core->coreArrayNeighborhood_2D [1][1][0],core->coreArrayNeighborhood_2D[1][0][0]);
          core->boundaryCore_2D[0][1] = is_right_edge_section(core->coreArrayNeighborhood_2D[1][1][0],core->coreArrayNeighborhood_2D[1][2][0]);
       
       // Y axis
          core->boundaryCore_2D[1][0] = is_left_edge_section(core->coreArrayNeighborhood_2D [1][1][1],core->coreArrayNeighborhood_2D[0][1][1]);
          core->boundaryCore_2D[1][1] = is_right_edge_section(core->coreArrayNeighborhood_2D[1][1][1],core->coreArrayNeighborhood_2D[2][1][1]);
       
       // X axis      X:Y:Z L:U
          core->boundaryCore_3D[0][0] = is_left_edge_section(core->coreArrayNeighborhood_3D [1][1][1][0],core->coreArrayNeighborhood_3D[1][1][0][0]);
          core->boundaryCore_3D[0][1] = is_right_edge_section(core->coreArrayNeighborhood_3D[1][1][1][0],core->coreArrayNeighborhood_3D[1][1][2][0]);
       
       // Y axis      X:Y:Z L:U
          core->boundaryCore_3D[1][0] = is_left_edge_section(core->coreArrayNeighborhood_3D [1][1][1][1],core->coreArrayNeighborhood_3D[1][0][1][1]);
          core->boundaryCore_3D[1][1] = is_right_edge_section(core->coreArrayNeighborhood_3D[1][1][1][1],core->coreArrayNeighborhood_3D[1][2][1][1]);
       
       // X axis      X:Y:Z L:U
          core->boundaryCore_3D[2][0] = is_left_edge_section(core->coreArrayNeighborhood_3D [1][1][1][2],core->coreArrayNeighborhood_3D[0][1][1][2]);
          core->boundaryCore_3D[2][1] = is_right_edge_section(core->coreArrayNeighborhood_3D[1][1][1][2],core->coreArrayNeighborhood_3D[2][1][1][2]);
        }
   }



// ****************************************





template <typename T>
bool
MulticoreArray<T>::operator==(const MulticoreArray<T> & X) const
   {
  // This is support for simple equivalence test of the array abstraction (useful in writting test codes).

  // If the distributions are the same then we could do this the fast way, but for now just focus on this as support for debugging.

     T error           = 0.0;
     T error_tolerance = 0.00001;

  // Note that if they are the same size then they are assumed to be distributed similarly.
     bool returnValue = (arraySize[0] == X.arraySize[0]);
     returnValue &= (arraySize[1] == X.arraySize[1]);
     returnValue &= (arraySize[2] == X.arraySize[2]);

#if 0
     printf ("Inside of MulticoreArray<T>::operator==() returnValue = %s \n",returnValue ? "true" : "false");
#endif

     if (returnValue == true)
        {
#if 0
       // This using the indexing operator (and is slow).
// #pragma omp for
          for (int i = 0; i < arraySize; i++)
             {
            // Make sure that errors can't cancle...
            // error += ((*this)(i) - X(i));
               T element_error = ((*this)(i) - X(i));
               error += (element_error * element_error);
             }

          if (error > error_tolerance)
             {
               printf ("Arrays are not equal within error_tolerance = %f error = %f \n",error_tolerance,error);

               this->display("lhs");
               X.display("rhs");

               returnValue = false;
             }
#else
       // This uses the local memory and is expressed for each core seperately (faster).
// #pragma omp parallel for
          for (int core = 0; core < numberOfCores; core++)
             {
               T* arraySection_this = arraySectionPointers[core];
               T* arraySection_X    = X.arraySectionPointers[core];

               int size = memorySectionSize(core);
               assert(size == 0 || (arraySection_this != NULL && arraySection_X != NULL));

               for (int i = 0; i < size; i++)
                  {
                    T element_error = arraySection_this[i] - arraySection_X[i];
                    error += (element_error * element_error);
#ifdef _TEST_OPTIMIZER_
                    if (error > error_tolerance) {
                        int ii, jj, kk;
                        for (kk = 0; kk < arraySize[2]; kk++) {
                            for (jj = 0; jj < arraySize[1]; jj++) {
                                for (ii = 0; ii < arraySize[0]; ii++) {
                                    int tcore, telement_index;
                                    computeMemorySectionAndOffset(ii, jj, kk, tcore, telement_index);
                                    if (tcore == core && i == telement_index) {
                                        goto label;
                                    }
                                }
                            }
                        }
                     label:
                        assert(kk < arraySize[2] && jj < arraySize[1] && ii < arraySize[0]);
                        printf ("===== lhs ===== size(%2d,%2d,%2d) on multi-core array size(%2d,%2d,%2d)\n",arraySize[0],arraySize[1],arraySize[2],coreArraySize[0],coreArraySize[1],coreArraySize[2]);
                        printf ("===== Core Id: %d, Core Index: %d\n", core, i);
                        printf ("===== 3D: k = %d\n", kk);
                        printf ("===== 3D: j = %d:  ", jj);
                        for (ii = 0; ii < arraySize[0]; ii++) {
                            if (operator()(ii, jj, kk) != X(ii, jj, kk))
                                printf ("##%5.2f## ", operator()(ii, jj, kk));
                            else
                                printf ("%5.2f ", operator()(ii, jj, kk));
                        }
                        printf("\n\n");
                        printf ("===== rhs ===== size(%2d,%2d,%2d) on multi-core array size(%2d,%2d,%2d)\n",X.arraySize[0],X.arraySize[1],X.arraySize[2],X.coreArraySize[0],X.coreArraySize[1],X.coreArraySize[2]);
                        printf ("===== Core Id: %d, Core Index: %d\n", core, i);
                        printf ("===== 3D: k = %d\n", kk);
                        printf ("===== 3D: j = %d:  ", jj);
                        for (ii = 0; ii < arraySize[0]; ii++) {
                            if (operator()(ii, jj, kk) != X(ii, jj, kk))
                                printf ("##%5.2f## ", X(ii, jj, kk));
                            else
                                printf ("%5.2f ", X(ii, jj, kk));
                        }
                        printf("\n\n");
                        return false;
                    }
#endif
                  }
             }

          if (error > error_tolerance)
             {
               printf ("Arrays are not equal within error_tolerance = %f error = %f \n",error_tolerance,error);

               this->display("lhs");
               X.display("rhs");

               returnValue = false;
             }
#endif
        }

     return returnValue;
   }


template <typename T>
void
// MulticoreArray<T>::computeMemorySectionAndOffset(const int & i, int & core, int & element_index) const
MulticoreArray<T>::computeMemorySectionAndOffset(const int & i, const int & j, const int & k, int & core, int & element_index) const
   {
  // This function takes the scalar index value (for the array abstraction) and computes the index (core) into the 
  // array of memory segments (one per core) and for that core computes the offset (element_index) in its associated 
  // memory segment. This function is slow and not meant to be used in anything but debugging code or to present a 
  // higher level of interface that would be optimized using compiler transformations.

  // This demonstrates the logic required to support the mapping and a way to support debugging of the mapping but 
  // it is note intended to be used in computations.  The compiler should suport transformations that would avoid
  // the use of anything this inefficent on an element by element basis.

  // Bounds check
     assert(i >= 0);
     assert(j >= 0);
     assert(k >= 0);
     assert(i < arraySize[0]);
     assert(j < arraySize[1]);
     assert(k < arraySize[2]);

#if 0
     printf ("\n####### In MulticoreArray<T>::computeMemorySectionAndOffset(i=%2d,j=%2d,k=%2d) useArraySectionRanges = %s arraySize = (%d,%d,%d) numberOfCores = %2d \n",i,j,k,useArraySectionRanges ? "true" : "false",arraySize[0],arraySize[1],arraySize[2],numberOfCores);
#endif

     int indexInMulticoreArray    [DIM] = { i, j, k };
     int indexInCoreArray         [DIM] = { 0, 0, 0 };
     int indexInMemorySectionArray[DIM] = { 0, 0, 0 };

  // Initialize these input/output function parameters to error values (to avoid them being used until we set them).
     core          = -1;
     element_index = -1;

     if (get_tableBasedDistribution() == true)
        {
       // Assume that ranges are ordered (so that we can find the range matching an index via a linear search).
       // A better implementation could use a map of bases and bounds (with appropriate relational operators).
#if 0
          printf ("arraySectionRanges[0].size() = %zu \n",arraySectionRanges[0].size());
          printf ("arraySectionRanges[1].size() = %zu \n",arraySectionRanges[1].size());
          printf ("arraySectionRanges[2].size() = %zu \n",arraySectionRanges[2].size());
#endif
          assert(arraySectionRanges[0].size() > 0);
          assert(arraySectionRanges[1].size() == arraySectionRanges[0].size());
          assert(arraySectionRanges[2].size() == arraySectionRanges[0].size());

          assert(coreArraySize[0] > 0);
          assert(coreArraySize[1] > 0);
          assert(coreArraySize[2] > 0);

          assert(i < arraySize[0]);
          assert(j < arraySize[1]);
          assert(k < arraySize[2]);


          for (int dim = 0; dim < DIM; dim++)
             {
#if 0
               printf ("\n@@@@@ In computeMemorySectionAndOffset(): processing dimension = %d \n",dim);
#endif
               int coreIndexInMultidimensionalCoreArray        = 0;
               int elementIndexInMultidimensionalMemorySegment = 0;

               int aCoreToFar             = 0;
               int coreIndexInLinearArray = 0;

            // The table requires either a linear search or some sort of map...use a linear search for now.
            // int p = 0;
               do {
                    coreIndexInLinearArray = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#if 0
                    printf ("\n*** coreIndexInLinearArray = %d aCoreToFar = %d \n",coreIndexInLinearArray,aCoreToFar);
#endif
#if 0
                    printf ("indexInCoreArray[dim=%d] = %d coreIndexInLinearArray = %d \n",dim,indexInCoreArray[dim],coreIndexInLinearArray);
                    printf ("arraySectionRanges[dim=%d][coreIndexInLinearArray=%2d].second = %3zu arraySectionRanges[dim=%d][coreIndexInLinearArray=%2d].second = %3zu \n",
                         dim,coreIndexInLinearArray,arraySectionRanges[dim][coreIndexInLinearArray].first,dim,coreIndexInLinearArray,arraySectionRanges[dim][coreIndexInLinearArray].second);
#endif
#if 0
                    printf ("Assert: arraySectionRanges[dim][coreIndexInLinearArray].first (%d) <= indexInMulticoreArray[dim] (%d) \n",arraySectionRanges[dim][coreIndexInLinearArray].first,indexInMulticoreArray[dim]);
#endif
                 // I think we can assert this!
                    assert(arraySectionRanges[dim][coreIndexInLinearArray].first <= (size_t)indexInMulticoreArray[dim]);
#if 0
                    printf ("Assert: indexInCoreArray[dim] < coreArraySize[dim] \n");
#endif
                    assert(indexInCoreArray[dim] < coreArraySize[dim]);
#if 0
                    printf ("indexInCoreArray[dim=%d] = %d \n",dim,indexInCoreArray[dim]);
#endif
                    coreIndexInMultidimensionalCoreArray = indexInCoreArray[dim];

                    elementIndexInMultidimensionalMemorySegment = indexInMulticoreArray[dim] - arraySectionRanges[dim][coreIndexInLinearArray].first;
#if 0
                    printf ("In loop: coreIndexInMultidimensionalCoreArray = %d elementIndexInMultidimensionalMemorySegment = %d \n",coreIndexInMultidimensionalCoreArray,elementIndexInMultidimensionalMemorySegment);

                    printf ("arraySectionRanges[dim=%d][core=%d].first  = %d \n",dim,coreIndexInLinearArray,arraySectionRanges[dim][coreIndexInLinearArray].first);
                    printf ("arraySectionRanges[dim=%d][core=%d].second = %d \n",dim,coreIndexInLinearArray,arraySectionRanges[dim][coreIndexInLinearArray].second);
#endif
                    indexInCoreArray[dim]++;
                    aCoreToFar = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
                    assert(aCoreToFar >= 0);
#if 0
                    printf ("arraySectionRanges[dim=%d][aCoreToFar=%d].first  = %d \n",dim,aCoreToFar,(aCoreToFar < numberOfCores) ? arraySectionRanges[dim][aCoreToFar].first : -99);
                    printf ("arraySectionRanges[dim=%d][aCoreToFar=%d].second = %d \n",dim,aCoreToFar,(aCoreToFar < numberOfCores) ? arraySectionRanges[dim][aCoreToFar].second : -99);

                    printf ("aCoreToFar < numberOfCores                 = %s \n",(aCoreToFar < numberOfCores) ? "true" : "false");
                    printf ("indexInCoreArray[dim] < coreArraySize[dim] = %s \n",(indexInCoreArray[dim] < coreArraySize[dim]) ? "true" : "false");
#endif
                    if (aCoreToFar < numberOfCores)
                       {
#if 0
                         printf ("(arraySectionRanges[dim][aCoreToFar].first <= indexInMulticoreArray[dim]) = %s \n",(arraySectionRanges[dim][aCoreToFar].first <= indexInMulticoreArray[dim]) ? "true" : "false");
#endif
                       }
                      else
                       {
                         assert((int)aCoreToFar >= (int)numberOfCores);
#if 0
                         printf ("aCoreToFar is out of bounds, can't evaluate (arraySectionRanges[dim][aCoreToFar].first <= indexInMulticoreArray[dim]) \n");
#endif
                       }
#if 0
                    printf ("*** At bottom or loop: aCoreToFar = %d indexInCoreArray[dim=%d] = %d numberOfCores = %d \n",aCoreToFar,dim,indexInCoreArray[dim],numberOfCores);
#endif
                  }
               while ( (aCoreToFar < numberOfCores) && (indexInCoreArray[dim] < coreArraySize[dim]) && (arraySectionRanges[dim][aCoreToFar].first <= (size_t)indexInMulticoreArray[dim]) );
#if 0
               printf ("After loop: coreIndexInMultidimensionalCoreArray = %d elementIndexInMultidimensionalMemorySegment = %d \n",coreIndexInMultidimensionalCoreArray,elementIndexInMultidimensionalMemorySegment);
#endif
            // Reset to the index before we went too far.
               indexInCoreArray[dim]          = coreIndexInMultidimensionalCoreArray;
               indexInMemorySectionArray[dim] = elementIndexInMultidimensionalMemorySegment;
             }
#if 0
          printf ("In computeMemorySectionAndOffset(): indexInCoreArray[0] = %d indexInCoreArray[1] = %d indexInCoreArray[2] = %d \n",indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#endif
       // Only here do we set the "core" and "element_index"
          core = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#if 0
          printf ("In computeMemorySectionAndOffset(): core = %d \n",core);
#endif
          assert(core >= 0);
#if 0
          printf ("In computeMemorySectionAndOffset(): indexInMemorySectionArray[0] = %d indexInMemorySectionArray[1] = %d indexInMemorySectionArray[2] = %d \n",indexInMemorySectionArray[0],indexInMemorySectionArray[1],indexInMemorySectionArray[2]);
#endif

       // Only here do we set the "element_index" in/out function parameter
          if(boundaryType == MulticoreArray<T>::attached)
            element_index = mappingFromMultidimentionalMemorySegmentArray(indexInMemorySectionArray[0]+haloSize[0],indexInMemorySectionArray[1]+haloSize[1],indexInMemorySectionArray[2]+haloSize[2],core);
          else
            element_index = mappingFromMultidimentionalMemorySegmentArray(indexInMemorySectionArray[0],indexInMemorySectionArray[1],indexInMemorySectionArray[2],core);
#if 0
          printf ("In computeMemorySectionAndOffset(): element_index = %d \n",element_index);
#endif
        }
       else
        {
#if 0
          printf ("In computeMemorySectionAndOffset(): get_tableBasedDistribution() == false, we compute the offsets algorithmically. \n");
#endif
        }

  // Modified this block to always be executed so that the code in it can be evaluated against the computation of
  // the "core" and "element_index" above. This will allow the testing of the algorithmic distribution against the
  // table-based distribution to support the same distribution both ways.  This will allow us to evaluate the table
  // based distribution's performance against that of the algorithm-based distribution's performance (later).
  //   else
        {
#if 0
          printf ("In computeMemorySectionAndOffset(): algorithm-based distribution support is not implemented yet! \n");
#endif
       // Note that a more direct computation should be possible for the algorithmic distribution
       // but this code is not yet debugged (logic should be a bit more complex on the memory 
       // segment boundaries).

#if 0
       // This is the implementation in algorithmicComputationOfSize() to compute the size.
          int remainder = (arraySize[dim] % coreArraySize[dim]);
          int size = (arraySize[dim] / coreArraySize[dim]) + ((remainder != 0) ? 1 : 0);
          if (coreIndex >= remainder)
             {
               size -= (remainder != 0) ? 1 : 0;
             }

       // OR
          int offset = (remainder != 0) ? 1 : 0;
          int size = (arraySize[dim] / coreArraySize[dim]) + offset - (coreIndex >= remainder ? offset : 0);
       // OR
          int size = (arraySize[dim] / coreArraySize[dim]) + (coreIndex < remainder ? offset : 0);
       // OR (all together now)
          int remainder = (arraySize[dim] % coreArraySize[dim]);
          int offset = (remainder != 0) ? 1 : 0;
          int size = (coreIndex < remainder) ? (arraySize[dim] / coreArraySize[dim]) + offset) : (arraySize[dim] / coreArraySize[dim]);

          int arraySizeDivCoreArraySize = (arraySize[dim] / coreArraySize[dim]);
          int sizeForCoresLessThanRemainder = arraySizeDivCoreArraySize + offset;

          position(p) = p*arraySizeDivCoreArraySize + remainder;
          
#endif
          int remainder[3] = {0,0,0};

          for (int dim = 0; dim < DIM; dim++)
             {
#if 0
               printf ("In computeMemorySectionAndOffset() algorithm-based distribution: dim = %d \n",dim);

               printf ("arraySize[dim=%d] = %d coreArraySize[dim=%d] = %d \n",dim,arraySize[dim],dim,coreArraySize[dim]);
#endif
            // remainder[dim] = ((arraySize[dim] % coreArraySize[dim]) != 0) ? 1 : 0;
               remainder[dim] = arraySize[dim] % coreArraySize[dim];
#if 0
               printf ("remainder[dim=%d] = %d \n",dim,remainder[dim]);
#endif

               int size        = 0;
               int sizePerCore = (arraySize[dim] / coreArraySize[dim]) + ((remainder != 0) ? 1 : 0);
#if 0
               printf ("sizePerCore = %d \n",sizePerCore);
#endif
               if (indexInMulticoreArray[dim] >= remainder[dim])
                  {
                    size = sizePerCore*remainder[dim];
                  }
#if 0
               printf ("indexInMulticoreArray[dim=%d] = %d \n",dim,indexInMulticoreArray[dim]);
#endif
            // Compute the indexes into the local memory segment for use on each core.
            // indexInCoreArray[dim]          = (indexInMulticoreArray[dim] / ((arraySize[dim] + remainder[dim]) / coreArraySize[dim]));
            // indexInCoreArray[dim]          = (indexInMulticoreArray[dim] / coreArraySize[dim]);
#if 0
               printf ("indexInCoreArray[dim=%d] = %d \n",dim,indexInCoreArray[dim]);
#endif
            // This is the code that is in the function: computeArraySectionDistribution() which defines the boundaries of the memory segments.
            // I would like to have a version of this code that don't require a loop.  But let's work up to that...
            // If we did have to have a loop it could at least just be a "while" loop instead of a "for" loop.
               bool found = false;
               int counter = 0;
               for (int p = 0; p < coreArraySize[dim]; p++)
                  {
#if 0
                    printf ("dim = %d p = %d \n",dim,p);
#endif
                 // Use the same distribution, but with a table based approach.
                    int tmpBound = counter + algorithmicComputationOfSize(dim,p);
#if 0
                    printf ("tmpBound = %d counter = %d \n",tmpBound,counter);
#endif
                    int previousBoundary = counter;
                 // Truncate to the the arraySize[dim] if required...
                    counter = (tmpBound <= arraySize[dim]) ? tmpBound : arraySize[dim];
#if 0
                    printf ("after update: counter = %d found = %s \n",counter,found ? "true" : "false");
#endif
                    if (found == false && counter > indexInMulticoreArray[dim])
                       {
                         indexInCoreArray[dim] = p;

                         assert(previousBoundary <= indexInMulticoreArray[dim]);

                         indexInMemorySectionArray[dim] = indexInMulticoreArray[dim] - previousBoundary;

                         found = true;
#if 0
                         printf ("found new value for indexInCoreArray[dim=%d] \n",dim,indexInCoreArray[dim]);
#endif
                       }
                  }
#if 0
               printf ("After loop: indexInCoreArray[dim=%d] \n",dim,indexInCoreArray[dim]);
#endif
            // indexInMemorySectionArray[dim] = (indexInMulticoreArray[dim] % ((arraySize[dim] + remainder[dim]) / coreArraySize[dim]));
            // indexInMemorySectionArray[dim] = (indexInMulticoreArray[dim] % coreArraySize[dim]);
#if 0
               printf ("indexInMemorySectionArray[dim=%d] = %d \n",dim,indexInMemorySectionArray[dim]);
#endif
             }

#if 0
       // This is what could be a simpler more direct way to compute the "core" and "element_index", but this code is not correct.
          remainder[0] = ((arraySize[0] % coreArraySize[0]) != 0) ? 1 : 0;
          remainder[1] = ((arraySize[1] % coreArraySize[1]) != 0) ? 1 : 0;
          remainder[2] = ((arraySize[2] % coreArraySize[2]) != 0) ? 1 : 0;

       // Compute the indexes into the local memory segment for use on each core.
          indexInCoreArray[0]          = (indexInMulticoreArray[0] / ((arraySize[0] + remainder[0]) / coreArraySize[0]));
          indexInCoreArray[1]          = (indexInMulticoreArray[1] / ((arraySize[1] + remainder[1]) / coreArraySize[1]));
          indexInCoreArray[2]          = (indexInMulticoreArray[2] / ((arraySize[2] + remainder[2]) / coreArraySize[2]));

          indexInMemorySectionArray[0] = (indexInMulticoreArray[0] % ((arraySize[0] + remainder[0]) / coreArraySize[0]));
          indexInMemorySectionArray[1] = (indexInMulticoreArray[1] % ((arraySize[1] + remainder[1]) / coreArraySize[1]));
          indexInMemorySectionArray[2] = (indexInMulticoreArray[2] % ((arraySize[2] + remainder[2]) / coreArraySize[2]));
#endif

#if 0
          printf ("In computeMemorySectionAndOffset(): indexInCoreArray[0] = %d indexInCoreArray[1] = %d indexInCoreArray[2] = %d \n",indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#endif
       // Set the value to return through the function parameter list.
          int test_core          = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]);
#if 0
          printf ("***** test_core = %d core = %d \n",test_core,core);
          printf ("In computeMemorySectionAndOffset(): indexInMemorySectionArray[0] = %d indexInMemorySectionArray[1] = %d indexInMemorySectionArray[2] = %d \n",indexInMemorySectionArray[0],indexInMemorySectionArray[1],indexInMemorySectionArray[2]);
#endif
          assert(test_core >= 0);

       // Set the value to return through the function parameter list.
          int test_element_index = mappingFromMultidimentionalMemorySegmentArray(indexInMemorySectionArray[0],indexInMemorySectionArray[1],indexInMemorySectionArray[2],test_core);
          if(boundaryType == MulticoreArray<T>::attached)
            test_element_index = mappingFromMultidimentionalMemorySegmentArray(indexInMemorySectionArray[0]+haloSize[0],indexInMemorySectionArray[1]+haloSize[1],indexInMemorySectionArray[2]+haloSize[2],test_core);
#if 0
          printf ("test_element_index = %d element_index = %d\n",test_element_index,element_index);
#endif
          if (get_tableBasedDistribution() == true)
             {
            // Use the algorithmically computed values to test the table-based values.
               assert(test_core == core);
               assert(test_element_index == element_index);
             }
            else
             {
            // We only have algorithmic based values, so use them directly.
               core          = test_core;
               element_index = test_element_index;
             }

          assert(test_core == core);
          assert(test_element_index == element_index);
#if 0
          printf ("Exiting as a test! \n");
          assert(false);
#endif
        }

  // Some general assertions for both cases.
     assert(core >= 0);
     assert(core < numberOfCores);
     assert(element_index >= 0);
     assert(element_index < numberOfArrayElements());

#if 0
     printf ("In MulticoreArray<T>::computeMemorySectionAndOffset(i=%2d,j=%2d,k=%2d) numberOfArrayElements() = %d core = %2d element_index = %2d \n",i,j,k,numberOfArrayElements(),core,element_index);
#endif
   }


// ****************************************
//                Destructor
// ****************************************

template <typename T>
MulticoreArray<T>::~MulticoreArray()
   {
  // Destructor

  // Iterate over the cores and delete the allocated memory segment for each core.
     for (int core = 0; core < numberOfCores; core++)
        {
       // Memory allocated on each core optionally uses libnuma.
// #ifndef NUMA_NUM_NODES
#if HAVE_NUMA_H
       // Note that libnuma requires the size of the allocated data (which we have fortunately saved).
       // size_t size = (useArraySectionRanges == true) ? arraySectionSizes[p] : algorithmicComputationOfSize(p);
          size_t size = memorySectionSize(core);
          numa_free(arraySectionPointers[core],size*sizeof(T));
          arraySectionPointers[core] = NULL;
          assert(arraySectionPointers[core] == NULL);
          for(int dim=0; dim < DIM; dim++)
          {
            if((boundaryType == MulticoreArray<T>::detached) && (haloSize[dim] > 0))
            {
              numa_free(haloSectionPointers[dim][0][core],haloRegionsSizes[dim][core]*sizeof(T));
              numa_free(haloSectionPointers[dim][1][core],haloRegionsSizes[dim][core]*sizeof(T));
              haloSectionPointers[dim][0][core] = NULL;
              haloSectionPointers[dim][1][core] = NULL;
              assert(haloSectionPointers[dim][0][core] == NULL);
              assert(haloSectionPointers[dim][1][core] == NULL);
            }
          }
#else
          delete [] arraySectionPointers[core];
          arraySectionPointers[core] = NULL;
          assert(arraySectionPointers[core] == NULL);
          for(int dim=0; dim < DIM; dim++)
          {
            if((boundaryType == MulticoreArray<T>::detached) && (haloSize[dim] > 0))
            {
              delete [] haloSectionPointers[dim][0][core];
              delete [] haloSectionPointers[dim][1][core];
              haloSectionPointers[dim][0][core] = NULL;
              haloSectionPointers[dim][1][core] = NULL;
              assert(haloSectionPointers[dim][0][core] == NULL);
              assert(haloSectionPointers[dim][1][core] == NULL);
            }
          }
#endif
        }

  // Memory allocated to hold the pointers to the memory allocated on each core uses C++ new/delete mechanism.
     delete [] arraySectionPointers;
     arraySectionPointers = NULL;
     assert(arraySectionPointers == NULL);
   }



#if HAVE_NUMA_H
template <typename T>
void
MulticoreArray<T>::computeNUMACoreArray()
{

     int NUMAcores = numa_num_configured_cpus();

     NUMAcoreArraySize[0] = 1;
     NUMAcoreArraySize[1] = 1;
     NUMAcoreArraySize[2] = 1;
     if(numberOfCores <= NUMAcores)
     {
       NUMAcoreArraySize[0] = coreArraySize[0]; 
       NUMAcoreArraySize[1] = coreArraySize[1]; 
       NUMAcoreArraySize[2] = coreArraySize[2]; 
     }
     else
     {
       int tmp = NUMAcores;
       int idx=0;
       while((tmp & 0x1) == 0)
       {
         NUMAcoreArraySize[idx] *= 2;
         idx = (idx >1) ? 0:(idx+1);
         tmp >>= 1;
       }
       NUMAcoreArraySize[idx] *= tmp;
       int factor = 1;
       for(int i=0; i < 3; ++i)
       {
         if(NUMAcoreArraySize[i] > coreArraySize[i])
           {
             factor *= (NUMAcoreArraySize[i] / coreArraySize[i]);
             NUMAcoreArraySize[i] = coreArraySize[i];
           }
       }
       for(int i=0; i < 3; ++i)
       {
         if(NUMAcoreArraySize[i] < coreArraySize[i] && factor != 1)
           {
              if(NUMAcoreArraySize[i] * factor < coreArraySize[i])
                {
                   NUMAcoreArraySize[i] *= factor;
                   break;
                }
              else 
                {
                  factor = coreArraySize[i] / NUMAcoreArraySize[i]; 
                  NUMAcoreArraySize[i] = coreArraySize[i];
                }
           }
       }
     }
}
#endif
// ****************************************
//               Constructor with Halos
// ****************************************

template <typename T>
MulticoreArray<T>::MulticoreArray(int arraySize_I, int arraySize_J, int arraySize_K, const int numberOfCores_X, const int numberOfCores_Y, const int numberOfCores_Z, const int numberOfHalos_X, const int numberOfHalos_Y, const int numberOfHalos_Z, MulticoreArray<T>::haloType haloDataType, const int padding, bool useTableBasedDistribution)
   : numberOfCores(numberOfCores_X*numberOfCores_Y*numberOfCores_Z),
     padding(padding),
     useArraySectionRanges(useTableBasedDistribution)
   {
  // Constructor to allocated required memory for each core and do the initialization
  // (initialization of memory makes this a more expensive abstractions).

  // assert(get_tableBasedDistribution() == false);

     arraySize[0] = arraySize_I;
     arraySize[1] = arraySize_J;
     arraySize[2] = arraySize_K;

     coreArraySize[0] = numberOfCores_X;
     coreArraySize[1] = numberOfCores_Y;
     coreArraySize[2] = numberOfCores_Z;

#if HAVE_NUMA_H
     computeNUMACoreArray();
#endif

     haloSize[0] = numberOfHalos_X;
     haloSize[1] = numberOfHalos_Y;
     haloSize[2] = numberOfHalos_Z;

     boundaryType = haloDataType; 
#if 0
     printf ("In MulticoreArray<T> constructor arraySize = (%d,%d,%d) numberOfCores = %d useTableBasedDistribution = %s \n",arraySize[0],arraySize[1],arraySize[2],numberOfCores,useTableBasedDistribution ? "true" : "false");
#endif

  // We want to assume at least a non-zero length array (for not at least while debugging).
     assert(arraySize_I > 0);
     assert(arraySize_J > 0);
     assert(arraySize_K > 0);

  // We must have at least a single core upon which to build the array.
     assert(numberOfCores_X > 0);
     assert(numberOfCores_Y > 0);
     assert(numberOfCores_Z > 0);

     arraySectionPointers = new T*[numberOfCores];
     assert(arraySectionPointers != NULL);

     coreArray = new Core<T>*[numberOfCores];
     assert(coreArray != NULL);
     for (int core = 0; core < numberOfCores; core++)
        {
          coreArray[core] = new Core<T>();
        }

  // Storage for the range for each axis for each memory segment (associated with each core).
     arraySectionRanges[0] = std::vector<std::pair<size_t,size_t> >(numberOfCores);
     arraySectionRanges[1] = std::vector<std::pair<size_t,size_t> >(numberOfCores);
     arraySectionRanges[2] = std::vector<std::pair<size_t,size_t> >(numberOfCores);

     assert(arraySectionRanges[0].size() == (size_t)numberOfCores);
     assert(arraySectionRanges[1].size() == (size_t)numberOfCores);
     assert(arraySectionRanges[2].size() == (size_t)numberOfCores);

  // Storage for the size of each axis for each memory segment (associated with each core).
     arraySectionSizes [0] = std::vector<size_t>(numberOfCores),
     arraySectionSizes [1] = std::vector<size_t>(numberOfCores),
     arraySectionSizes [2] = std::vector<size_t>(numberOfCores),

     assert(arraySectionSizes[0].size() == (size_t)numberOfCores);
     assert(arraySectionSizes[1].size() == (size_t)numberOfCores);
     assert(arraySectionSizes[2].size() == (size_t)numberOfCores);
     
  // Initialize the pointers to NULL (not really required since we will allocate memory and reset the pointer values shortly).
     for (int core = 0; core < numberOfCores; core++)
        {
          arraySectionPointers[core] = NULL;
          assert(arraySectionPointers[core] == NULL);
        }

#if 0
     printf ("Calling computeArraySectionDistribution() \n");
#endif

     assert(coreArraySize[0] > 0);
     assert(coreArraySize[1] > 0);
     assert(coreArraySize[2] > 0);

  // This is a NOP in the case of an algorithm-based distribution.
     computeArraySectionDistribution();

  // Storage for the halo region of each axis for each memory segment (associated with each core).
    if(boundaryType == MulticoreArray<T>::detached)
    { 
     haloRegionsSizes [0] = std::vector<size_t>(numberOfCores),
     haloRegionsSizes [1] = std::vector<size_t>(numberOfCores),
     haloRegionsSizes [2] = std::vector<size_t>(numberOfCores),

     assert(haloRegionsSizes[0].size() == (size_t)numberOfCores);
     assert(haloRegionsSizes[1].size() == (size_t)numberOfCores);
     assert(haloRegionsSizes[2].size() == (size_t)numberOfCores);
  
     for (int dim =0; dim < DIM; dim++)
     {
       if(haloSize[dim])   
       {
         haloSectionPointers[dim][0] = new T*[numberOfCores];
         assert(haloSectionPointers[dim][0] != NULL);
         haloSectionPointers[dim][1] = new T*[numberOfCores];
         assert(haloSectionPointers[dim][1] != NULL);
       }
     }
     // Compute the actual halo region size
     computeHaloRegionSize();
    }
#if 0
     printf ("Calling allocateMemorySectionsPerCore() \n");
#endif

  // Allocate memory per core (uses libnuma if available).
     allocateMemorySectionsPerCore();

#if 0
     printf ("Calling verifyArraySize() \n");
#endif

  // Error Checking: make sure we have only allocated an array of the correct size (tests logic).
     verifyArraySize();

  // More verification support.
     verifyMultidimensionalCoreArray();

#if 0
     printf ("Calling distanceBetweenMemoryAllocatedPerCore() \n");
#endif

  // Debugging: check the distance (bytes) between memory segments allocated for each core.
     distanceBetweenMemoryAllocatedPerCore();

#if 0
     printf ("In MulticoreArray<T> constructor: data pointers allocated \n");
#endif

  // Initialize the allocated memory.
     initializeDataPlusPadding();

     const int firstCoreIndex = 0;

#if 0
     printf ("In constructor: arraySectionSizes[0][firstCoreIndex=%d] = %d \n",firstCoreIndex,arraySectionSizes[0][firstCoreIndex]);
     printf ("In constructor: arraySectionSizes[1][firstCoreIndex=%d] = %d \n",firstCoreIndex,arraySectionSizes[1][firstCoreIndex]);
     printf ("In constructor: arraySectionSizes[2][firstCoreIndex=%d] = %d \n",firstCoreIndex,arraySectionSizes[2][firstCoreIndex]);
#endif

  // There should be a valid size for an array (not a zero length array).
     assert(useTableBasedDistribution == false || arraySectionSizes[0][firstCoreIndex] > 0);
     assert(useTableBasedDistribution == false || arraySectionSizes[1][firstCoreIndex] > 0);
     assert(useTableBasedDistribution == false || arraySectionSizes[2][firstCoreIndex] > 0);

  // Initialize data required for
     initializeCoreArrayNeighborhoods();
     initializeLinearizedCoreArrayNeighborhoods();
     initializeCoreArraySizeNeighborhoods();
     initializeCoreArrayBoundaryFlags();
   }


// ****************************************
//               Constructor
// ****************************************

template <typename T>
MulticoreArray<T>::MulticoreArray(int arraySize_I, int arraySize_J, int arraySize_K, const int numberOfCores_X, const int numberOfCores_Y, const int numberOfCores_Z, const int padding, bool useTableBasedDistribution)
   : numberOfCores(numberOfCores_X*numberOfCores_Y*numberOfCores_Z),
     padding(padding),
     useArraySectionRanges(useTableBasedDistribution)
   {
  // Constructor to allocated required memory for each core and do the initialization
  // (initialization of memory makes this a more expensive abstractions).

  // assert(get_tableBasedDistribution() == false);

     arraySize[0] = arraySize_I;
     arraySize[1] = arraySize_J;
     arraySize[2] = arraySize_K;

     coreArraySize[0] = numberOfCores_X;
     coreArraySize[1] = numberOfCores_Y;
     coreArraySize[2] = numberOfCores_Z;

#if HAVE_NUMA_H
     computeNUMACoreArray();
#endif

     haloSize[0] = 0;
     haloSize[1] = 0;
     haloSize[2] = 0;
     boundaryType = MulticoreArray<T>::remote ; 
#if 0
     printf ("In MulticoreArray<T> constructor arraySize = (%d,%d,%d) numberOfCores = %d useTableBasedDistribution = %s \n",arraySize[0],arraySize[1],arraySize[2],numberOfCores,useTableBasedDistribution ? "true" : "false");
#endif

  // We want to assume at least a non-zero length array (for not at least while debugging).
     assert(arraySize_I > 0);
     assert(arraySize_J > 0);
     assert(arraySize_K > 0);

  // We must have at least a single core upon which to build the array.
     assert(numberOfCores_X > 0);
     assert(numberOfCores_Y > 0);
     assert(numberOfCores_Z > 0);

     arraySectionPointers = new T*[numberOfCores];
     assert(arraySectionPointers != NULL);

     coreArray = new Core<T>*[numberOfCores];
     assert(coreArray != NULL);
     for (int core = 0; core < numberOfCores; core++)
        {
          coreArray[core] = new Core<T>();
        }

  // Storage for the range for each axis for each memory segment (associated with each core).
     arraySectionRanges[0] = std::vector<std::pair<size_t,size_t> >(numberOfCores);
     arraySectionRanges[1] = std::vector<std::pair<size_t,size_t> >(numberOfCores);
     arraySectionRanges[2] = std::vector<std::pair<size_t,size_t> >(numberOfCores);

     assert(arraySectionRanges[0].size() == (size_t)numberOfCores);
     assert(arraySectionRanges[1].size() == (size_t)numberOfCores);
     assert(arraySectionRanges[2].size() == (size_t)numberOfCores);

  // Storage for the size of each axis for each memory segment (associated with each core).
     arraySectionSizes [0] = std::vector<size_t>(numberOfCores),
     arraySectionSizes [1] = std::vector<size_t>(numberOfCores),
     arraySectionSizes [2] = std::vector<size_t>(numberOfCores),

     assert(arraySectionSizes[0].size() == (size_t)numberOfCores);
     assert(arraySectionSizes[1].size() == (size_t)numberOfCores);
     assert(arraySectionSizes[2].size() == (size_t)numberOfCores);
     
  // Initialize the pointers to NULL (not really required since we will allocate memory and reset the pointer values shortly).
     for (int core = 0; core < numberOfCores; core++)
        {
          arraySectionPointers[core] = NULL;
          assert(arraySectionPointers[core] == NULL);
        }

#if 0
     printf ("Calling computeArraySectionDistribution() \n");
#endif

     assert(coreArraySize[0] > 0);
     assert(coreArraySize[1] > 0);
     assert(coreArraySize[2] > 0);

  // This is a NOP in the case of an algorithm-based distribution.
     computeArraySectionDistribution();

#if 0
     printf ("Calling allocateMemorySectionsPerCore() \n");
#endif

  // Allocate memory per core (uses libnuma if available).
     allocateMemorySectionsPerCore();

#if 0
     printf ("Calling verifyArraySize() \n");
#endif

  // Error Checking: make sure we have only allocated an array of the correct size (tests logic).
     verifyArraySize();

  // More verification support.
     verifyMultidimensionalCoreArray();

#if 0
     printf ("Calling distanceBetweenMemoryAllocatedPerCore() \n");
#endif

  // Debugging: check the distance (bytes) between memory segments allocated for each core.
     distanceBetweenMemoryAllocatedPerCore();

#if 0
     printf ("In MulticoreArray<T> constructor: data pointers allocated \n");
#endif

  // Initialize the allocated memory.
     initializeDataPlusPadding();

     const int firstCoreIndex = 0;

#if 0
     printf ("In constructor: arraySectionSizes[0][firstCoreIndex=%d] = %d \n",firstCoreIndex,arraySectionSizes[0][firstCoreIndex]);
     printf ("In constructor: arraySectionSizes[1][firstCoreIndex=%d] = %d \n",firstCoreIndex,arraySectionSizes[1][firstCoreIndex]);
     printf ("In constructor: arraySectionSizes[2][firstCoreIndex=%d] = %d \n",firstCoreIndex,arraySectionSizes[2][firstCoreIndex]);
#endif

  // There should be a valid size for an array (not a zero length array).
     assert(useTableBasedDistribution == false || arraySectionSizes[0][firstCoreIndex] > 0);
     assert(useTableBasedDistribution == false || arraySectionSizes[1][firstCoreIndex] > 0);
     assert(useTableBasedDistribution == false || arraySectionSizes[2][firstCoreIndex] > 0);

  // Initialize data required for
     initializeCoreArrayNeighborhoods();
     initializeLinearizedCoreArrayNeighborhoods();
     initializeCoreArraySizeNeighborhoods();
     initializeCoreArrayBoundaryFlags();
   }


template <typename T>
T & MulticoreArray<T>::operator()(const int & i, const int & j, const int & k)
   {
  // This implementation handles non-const arrays (lhs values).
  // This is the expensive way to support the abstraction of an array separated over multiple cores.
  // Thie more efficent way is to do the compiler transformation on the code instead of abstracting 
  // the indexing to an inlined function.

  // Factor this into an API that can support C, C++, and Fortran.
     int core, element_index;
     computeMemorySectionAndOffset(i, j, k, core, element_index);
     return arraySectionPointers[core][element_index];
   }


template <typename T>
const T & MulticoreArray<T>::operator()(const int & i, const int & j, const int & k) const
   {
  // This implementation handles const references (e.g. rhs values).
  // This is the expensive way to support the abstraction of an array separated over multiple cores.
  // Thie more efficent way is to do the compiler transformation on the code instead of abstracting 
  // the indexing to an inlined function.

  // Factor this into an API that can support C, C++, and Fortran.
     int core, element_index;
     computeMemorySectionAndOffset(i, j, k, core, element_index);
     return arraySectionPointers[core][element_index];
   }

// *************************************************
template <typename T>
void
MulticoreArray<T>::haloExchange()
{
if(boundaryType == MulticoreArray<T>::attached)
{
     int indexInCoreArray[DIM] = {0,0,0};
#define index3D(i,j,k) (((k)*sizeX*sizeY)+(((j)*sizeX)+(i)))
        // X direction exchange
    if(haloSize[0] > 0)
    {
        for (int core = 0; core < numberOfCores; core++)
        {
        indexInCoreArray[0] = core % coreArraySize[0];
        indexInCoreArray[1] = (core % (coreArraySize[0] * coreArraySize[1]) / coreArraySize[0]);
        indexInCoreArray[2] = core / (coreArraySize[0] * coreArraySize[1]); 

          if(indexInCoreArray[0] > 0)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0]-1,indexInCoreArray[1],indexInCoreArray[2]);
#if HALO_DBG
            printf("core %d copy data from its left neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_arraySectionPointers()[core];
            int haloX = haloSize[0];
//            int haloY = arraySectionSizes[1][core] - 2*haloSize[1];
//            int haloZ = arraySectionSizes[2][core] - 2*haloSize[2];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][0][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][0][1];
            int sizeZ = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][0][2];
            for(int k=haloSize[2]; k< sizeZ - haloSize[2]; ++k)
            for(int j=haloSize[1]; j< sizeY - haloSize[1]; ++j)
            for(int i=0; i< haloX; ++i)
            {
              localHaloSection[index3D(i,j,k)] = neighborSection[index3D(sizeX-(2*haloX-i),j,k)];
            }
#if HALO_DBG
            printf("3D Halo region in lower X:\n");
            for(int k=haloSize[2]; k< sizeZ - haloSize[2]; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=haloSize[1]; j< sizeY - haloSize[1]; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
          if(indexInCoreArray[0] < coreArraySize[0] - 1)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0]+1,indexInCoreArray[1],indexInCoreArray[2]);
#if HALO_DBG
            printf("core %d copy data from its right neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_arraySectionPointers()[core];
            int haloX = haloSize[0];
//            int haloY = arraySectionSizes[1][core] - 2*haloSize[1];
//            int haloZ = arraySectionSizes[2][core] - 2*haloSize[2];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][2][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][2][1];
            int sizeZ = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][2][2];
            for(int k=haloSize[2]; k< sizeZ - haloSize[2]; ++k)
            for(int j=haloSize[1]; j< sizeY - haloSize[1]; ++j)
            for(int i=0; i< haloX; ++i)
            {
              localHaloSection[index3D(i+(arraySectionSizes[0][core]-haloX),j,k)] = neighborSection[index3D(i+haloX,j,k)];
            }
#if HALO_DBG
            printf("3D Halo region in upper X:\n");
            for(int k=haloSize[2]; k< sizeZ - haloSize[2]; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=haloSize[1]; j< sizeY - haloSize[1]; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[index3D(i+(arraySectionSizes[0][core]-haloX),j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
        }
    }
        // Y direction exchange
    if(haloSize[1] > 0)
    {
       for (int core = 0; core < numberOfCores; core++)
       {
        indexInCoreArray[0] = core % coreArraySize[0];
        indexInCoreArray[1] = (core % (coreArraySize[0] * coreArraySize[1]) / coreArraySize[0]);
        indexInCoreArray[2] = core / (coreArraySize[0] * coreArraySize[1]); 

          if(indexInCoreArray[1] > 0)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1]-1,indexInCoreArray[2]);
#if HALO_DBG
            printf("core %d copy data from its bottom neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_arraySectionPointers()[core];
            int haloX = arraySectionSizes[0][core];
            int haloY = haloSize[1];
//            int haloZ = arraySectionSizes[2][core] - 2 * haloSize[2];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][0][1][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][0][1][1];
            int sizeZ = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][0][1][2];
            for(int k=haloSize[2]; k< sizeZ-haloSize[2]; ++k)
            for(int j=0; j< haloY; ++j)
            for(int i=0; i< haloX; ++i)
            {
                localHaloSection[index3D(i,j,k)] = neighborSection[index3D(i,sizeY-(2*haloY-j),k)];
            }
#if HALO_DBG
            printf("3D Halo region in lower Y:\n");
            for(int k=haloSize[2]; k< sizeZ-haloSize[2]; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< haloY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
          if(indexInCoreArray[1] < coreArraySize[1] - 1)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1]+1,indexInCoreArray[2]);
#if HALO_DBG
            printf("core %d copy data from its top neighbor core %d\n", core,neighborId); 
#endif
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_arraySectionPointers()[core];
            int haloX = arraySectionSizes[0][core];
            int haloY = haloSize[1];
//            int haloZ = arraySectionSizes[2][core] - 2 * haloSize[2];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][2][1][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][2][1][1];
            int sizeZ = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][2][1][2];
            for(int k=haloSize[2]; k< sizeZ-haloSize[2]; ++k)
            for(int j=0; j< haloY; ++j)
            for(int i=0; i< haloX; ++i)
            {
                localHaloSection[index3D(i,j+(arraySectionSizes[1][core]-haloY),k)] = neighborSection[index3D(i,j+haloY,k)];
            }
#if HALO_DBG
            printf("3D Halo region in upper Y:\n");
            for(int k=haloSize[2]; k< sizeZ-haloSize[2]; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< haloY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[index3D(i,j+(arraySectionSizes[1][core]-haloY),k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
        }
    }
        // Z direction exchange
    if(haloSize[2] > 0)
    {
       for (int core = 0; core < numberOfCores; core++)
       {
        indexInCoreArray[0] = core % coreArraySize[0];
        indexInCoreArray[1] = (core % (coreArraySize[0] * coreArraySize[1]) / coreArraySize[0]);
        indexInCoreArray[2] = core / (coreArraySize[0] * coreArraySize[1]); 

          if(indexInCoreArray[2] > 0)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]-1);
#if HALO_DBG
            printf("core %d copy data from its far neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_arraySectionPointers()[core];
//            int haloX = arraySectionSizes[0][core];
//            int haloY = arraySectionSizes[1][core];
            int haloZ = haloSize[2];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[0][1][1][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[0][1][1][1];
            int sizeZ = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[0][1][1][2];
            // copy real cells in XY plane
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< sizeY; ++j)
            for(int i=0; i< sizeX; ++i)
            {
              localHaloSection[index3D(i,j,k)] = neighborSection[index3D(i,j,sizeZ-(2*haloZ-k))];
            }
#if HALO_DBG
            printf("3D Halo region in lower Z:\n");
            for(int k=0; k< haloZ; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< sizeY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< sizeX; ++i)
                {
                  printf("%5.2f ",localHaloSection[index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
          if(indexInCoreArray[2] < coreArraySize[2] - 1)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]+1);
#if HALO_DBG
            printf("core %d copy data from its near neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_arraySectionPointers()[core];
//            int haloX = arraySectionSizes[0][core];
//            int haloY = arraySectionSizes[1][core];
            int haloZ = haloSize[2];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[2][1][1][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[2][1][1][1];
//            int sizeZ = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[2][1][1][2];
            // copy real cells in XY plane
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< sizeY; ++j)
            for(int i=0; i< sizeX; ++i)
            {
              localHaloSection[index3D(i,j,k+(arraySectionSizes[2][core]-haloZ))] = neighborSection[index3D(i,j,k+haloZ)];
            }
#if HALO_DBG
            printf("3D Halo region in upper Z:\n");
            for(int k=0; k< haloZ; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< sizeY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< sizeX; ++i)
                {
                  printf("%5.2f ",localHaloSection[index3D(i,j,k+(arraySectionSizes[2][core]-haloZ))]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
        }
     }
#undef index3D
}
else if(boundaryType == MulticoreArray<T>::detached)
{
     int indexInCoreArray[DIM] = {0,0,0};
#define neightbor_index3D(i,j,k) (((k)*sizeX*sizeY)+(((j)*sizeX)+(i)))
#define halo_index3D(i,j,k) (((k)*haloX*haloY)+(((j)*haloX)+(i)))
        // X direction exchange
    if(haloSize[0] > 0)
    {
        for (int core = 0; core < numberOfCores; core++)
        {
        indexInCoreArray[0] = core % coreArraySize[0];
        indexInCoreArray[1] = (core % (coreArraySize[0] * coreArraySize[1]) / coreArraySize[0]);
        indexInCoreArray[2] = core / (coreArraySize[0] * coreArraySize[1]); 

          if(indexInCoreArray[0] > 0)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0]-1,indexInCoreArray[1],indexInCoreArray[2]);
#if HALO_DBG
            printf("core %d copy data from its left neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_haloSectionPointers(0,0)[core];
            int haloX = haloSize[0];
            int haloY = arraySectionSizes[1][core];
            int haloZ = arraySectionSizes[2][core];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][0][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][0][1];
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< haloY; ++j)
            for(int i=0; i< haloX; ++i)
            {
              localHaloSection[halo_index3D(i,j,k)] = neighborSection[neightbor_index3D(sizeX-(haloX-i),j,k)];
            }
#if HALO_DBG
            printf("3D Halo region in lower X:\n");
            for(int k=0; k< haloZ; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< haloY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[halo_index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
          if(indexInCoreArray[0] < coreArraySize[0] - 1)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0]+1,indexInCoreArray[1],indexInCoreArray[2]);
#if HALO_DBG
            printf("core %d copy data from its right neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_haloSectionPointers(0,1)[core];
            int haloX = haloSize[0];
            int haloY = arraySectionSizes[1][core];
            int haloZ = arraySectionSizes[2][core];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][2][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][1][2][1];
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< haloY; ++j)
            for(int i=0; i< haloX; ++i)
            {
              localHaloSection[halo_index3D(i,j,k)] = neighborSection[neightbor_index3D(i,j,k)];
            }
#if HALO_DBG
            printf("3D Halo region in upper X:\n");
            for(int k=0; k< haloZ; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< haloY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[halo_index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
        }
    }
        // Y direction exchange
    if(haloSize[1] > 0)
    {
       for (int core = 0; core < numberOfCores; core++)
       {
        indexInCoreArray[0] = core % coreArraySize[0];
        indexInCoreArray[1] = (core % (coreArraySize[0] * coreArraySize[1]) / coreArraySize[0]);
        indexInCoreArray[2] = core / (coreArraySize[0] * coreArraySize[1]); 

          if(indexInCoreArray[1] > 0)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1]-1,indexInCoreArray[2]);
#if HALO_DBG
            printf("core %d copy data from its bottom neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_haloSectionPointers(1,0)[core];
            int haloX = arraySectionSizes[0][core] + 2 * haloSize[0];
            int haloY = haloSize[1];
            int haloZ = arraySectionSizes[2][core];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][0][1][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][0][1][1];
#define neighborhalo_index3D(i,j,k) (((k)*haloSize[0]*haloY)+(((j)*haloSize[0])+(i)))
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< haloY; ++j)
            for(int i=0; i< sizeX+2*haloSize[0]; ++i)
            {
              if(i < haloSize[0])
                localHaloSection[halo_index3D(i,j,k)] = get_haloSectionPointers(0,0)[neighborId][neighborhalo_index3D(i,sizeY-(haloY-j),k)];
              else if(i >= sizeX+haloSize[0])
                localHaloSection[halo_index3D(i,j,k)] = get_haloSectionPointers(0,1)[neighborId][neighborhalo_index3D(i-(sizeX+haloSize[0]),sizeY-(haloY-j),k)];
              else
                localHaloSection[halo_index3D(i,j,k)] = neighborSection[neightbor_index3D(i-haloSize[0],sizeY-(haloY-j),k)];
            }
#if HALO_DBG
            printf("3D Halo region in lower Y:\n");
            for(int k=0; k< haloZ; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< haloY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[halo_index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
          if(indexInCoreArray[1] < coreArraySize[1] - 1)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1]+1,indexInCoreArray[2]);
#if HALO_DBG
            printf("core %d copy data from its top neighbor core %d\n", core,neighborId); 
#endif
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_haloSectionPointers(1,1)[core];
            int haloX = arraySectionSizes[0][core] + 2 * haloSize[0];
            int haloY = haloSize[1];
            int haloZ = arraySectionSizes[2][core];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][2][1][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[1][2][1][1];
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< haloY; ++j)
            for(int i=0; i< sizeX+2*haloSize[0]; ++i)
            {
              if(i < haloSize[0])
                localHaloSection[halo_index3D(i,j,k)] = get_haloSectionPointers(0,0)[neighborId][neighborhalo_index3D(i,j,k)];
              else if(i >= sizeX+haloSize[0])
                localHaloSection[halo_index3D(i,j,k)] = get_haloSectionPointers(0,1)[neighborId][neighborhalo_index3D(i-(sizeX+haloSize[0]),j,k)];
	      else
                localHaloSection[halo_index3D(i,j,k)] = neighborSection[neightbor_index3D(i-haloSize[0],j,k)];
            }
#if HALO_DBG
            printf("3D Halo region in upper Y:\n");
            for(int k=0; k< haloZ; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< haloY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[halo_index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
#undef neighborhalo_index3D
        }
    }
        // Z direction exchange
    if(haloSize[2] > 0)
    {
       for (int core = 0; core < numberOfCores; core++)
       {
        indexInCoreArray[0] = core % coreArraySize[0];
        indexInCoreArray[1] = (core % (coreArraySize[0] * coreArraySize[1]) / coreArraySize[0]);
        indexInCoreArray[2] = core / (coreArraySize[0] * coreArraySize[1]); 

          if(indexInCoreArray[2] > 0)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]-1);
#if HALO_DBG
            printf("core %d copy data from its far neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_haloSectionPointers(2,0)[core];
            int haloX = arraySectionSizes[0][core] + 2 * haloSize[0];
            int haloY = arraySectionSizes[1][core] + 2 * haloSize[1];
            int haloZ = haloSize[2];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[0][1][1][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[0][1][1][1];
            int sizeZ = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[0][1][1][2];
            // copy real cells in XY plane
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< sizeY; ++j)
            for(int i=0; i< sizeX; ++i)
            {
              localHaloSection[halo_index3D(i+haloSize[0],j+haloSize[1],k)] = neighborSection[neightbor_index3D(i,j,sizeZ-(haloZ-k))];
            }
            // copy X-direction halo from neighbor
#define neighborhalo_index3D(i,j,k) (((k)*haloSize[0]*arraySectionSizes[1][core])+(((j)*haloSize[0])+(i)))
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< sizeY; ++j)
            for(int i=0; i< haloSize[0]; ++i)
            {
              localHaloSection[halo_index3D(i,j+haloSize[1],k)] = get_haloSectionPointers(0,0)[neighborId][neighborhalo_index3D(i,j,sizeZ-(haloZ-k))]; 
              localHaloSection[halo_index3D(i+arraySectionSizes[0][core]+haloSize[0],j+haloSize[1],k)] = get_haloSectionPointers(0,1)[neighborId][neighborhalo_index3D(i,j,sizeZ-(haloZ-k))]; 
            }
#undef neighborhalo_index3D 
            // copy Y-direction halo from neighbor
#define neighborhalo_index3D(i,j,k) (((k)*haloX*haloSize[1])+(((j)*haloX)+(i)))
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< haloSize[1]; ++j)
            for(int i=0; i< haloX; ++i)
            {
              localHaloSection[halo_index3D(i,j,k)] = get_haloSectionPointers(1,0)[neighborId][neighborhalo_index3D(i,j,sizeZ-(haloZ-k))]; 
              localHaloSection[halo_index3D(i,j+arraySectionSizes[1][core]+haloSize[1],k)] = get_haloSectionPointers(1,1)[neighborId][neighborhalo_index3D(i,j,sizeZ-(haloZ-k))]; 
            }
#undef neighborhalo_index3D 
#if HALO_DBG
            printf("3D Halo region in lower Z:\n");
            for(int k=0; k< haloZ; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< haloY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[halo_index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
          if(indexInCoreArray[2] < coreArraySize[2] - 1)
          {
            int neighborId = coreArrayElement(indexInCoreArray[0],indexInCoreArray[1],indexInCoreArray[2]+1);
#if HALO_DBG
            printf("core %d copy data from its near neighbor core %d\n", core,neighborId);
#endif 
            T* neighborSection     = get_arraySectionPointers()[neighborId];
            T* localHaloSection = get_haloSectionPointers(2,1)[core];
            int haloX = arraySectionSizes[0][core] + 2 * haloSize[0];
            int haloY = arraySectionSizes[1][core] + 2 * haloSize[1];
            int haloZ = haloSize[2];
            int sizeX = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[2][1][1][0];
            int sizeY = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[2][1][1][1];
//            int sizeZ = get_coreArray()[neighborId]->coreArrayNeighborhoodSizes_3D[2][1][1][2];
            // copy real cells in XY plane
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< sizeY; ++j)
            for(int i=0; i< sizeX; ++i)
            {
              localHaloSection[halo_index3D(i+haloSize[0],j+haloSize[1],k)] = neighborSection[neightbor_index3D(i,j,k)];
            }
            // copy X-direction halo from neighbor
#define neighborhalo_index3D(i,j,k) (((k)*haloSize[0]*arraySectionSizes[1][core])+(((j)*haloSize[0])+(i)))
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< sizeY; ++j)
            for(int i=0; i< haloSize[0]; ++i)
            {
              localHaloSection[halo_index3D(i,j+haloSize[1],k)] = get_haloSectionPointers(0,0)[neighborId][neighborhalo_index3D(i,j,k)]; 
              localHaloSection[halo_index3D(i+arraySectionSizes[0][core]+haloSize[0],j+haloSize[1],k)] = get_haloSectionPointers(0,1)[neighborId][neighborhalo_index3D(i,j,k)]; 
            }
#undef neighborhalo_index3D 
            // copy Y-direction halo from neighbor
#define neighborhalo_index3D(i,j,k) (((k)*haloX*haloSize[1])+(((j)*haloX)+(i)))
            for(int k=0; k< haloZ; ++k)
            for(int j=0; j< haloSize[1]; ++j)
            for(int i=0; i< haloX; ++i)
            {
              localHaloSection[halo_index3D(i,j,k)] = get_haloSectionPointers(1,0)[neighborId][neighborhalo_index3D(i,j,k)]; 
              localHaloSection[halo_index3D(i,j+arraySectionSizes[1][core]+haloSize[1],k)] = get_haloSectionPointers(1,1)[neighborId][neighborhalo_index3D(i,j,k)]; 
            }
#undef neighborhalo_index3D 
#if HALO_DBG
            printf("3D Halo region in upper Z:\n");
            for(int k=0; k< haloZ; ++k)
            {
              printf("k=%d:\t \n",k);
              for(int j=0; j< haloY; ++j)
              {
                printf("j=%d:\t",j);
                for(int i=0; i< haloX; ++i)
                {
                  printf("%5.2f ",localHaloSection[halo_index3D(i,j,k)]); 
                }
                printf("\n");
              }
              printf("\n");
            }
#endif
          }
        }
#undef neighborhalo_index3D 
     }
#undef neightbor_index3D
#undef neightbor_index3D
}
else
{
printf ("Error: haloExchange is not allowed in this mode \n");
  
}
}

template <typename T>
Core<T> & 
// MulticoreArray<T>::operator()(const int & i, const int & j, const int & k)
MulticoreArray<T>::getCore(const int & i, const int & j, const int & k) const
   {
     assert(i >= 0);
     assert(j >= 0);
     assert(k >= 0);

     int core = (k*coreArraySize[1]*coreArraySize[0]) + (j*coreArraySize[0]) + i;
     assert(core >= 0);

     return *(coreArray[coreArrayElement(i,j,k)]);
   }

// *************************************************
template <typename T>
bool
MulticoreArray<T>::isHaloExist()
{
  return ((haloSize[0]>0) || (haloSize[1]>0) || (haloSize[2]>0));  
}
// *************************************************
template <typename T>
bool
MulticoreArray<T>::hasAttachedHalo()
{
  return (boundaryType == MulticoreArray<T>::attached);
}
template <typename T>
bool
MulticoreArray<T>::hasDetachedHalo()
{
  return (boundaryType == MulticoreArray<T>::detached);
}

// Copy constructor
template <typename T>
MulticoreArray<T>::MulticoreArray(const MulticoreArray<T> &X)
: numberOfCores(X.get_numberOfCores()),
   padding(X.get_padding())
{
     arraySize[0] = X.get_arraySize(0);
     arraySize[1] = X.get_arraySize(1);
     arraySize[2] = X.get_arraySize(2);
  
     coreArraySize[0] = X.get_coreArraySize(0);
     coreArraySize[1] = X.get_coreArraySize(1);
     coreArraySize[2] = X.get_coreArraySize(2);

     coreArray = X.get_coreArray();
#if HAVE_NUMA_H
     NUMAcoreArraySize[0] = X.get_NUMAcoreArraySize(0);
     NUMAcoreArraySize[1] = X.get_NUMAcoreArraySize(1);
     NUMAcoreArraySize[2] = X.get_NUMAcoreArraySize(2);
#endif

     haloSize[0] = X.get_haloWidth(0);
     haloSize[1] = X.get_haloWidth(1);
     haloSize[2] = X.get_haloWidth(2);

     boundaryType = X.boundaryType;

     arraySectionRanges[0]= X.get_arraySectionRanges(0);
     arraySectionRanges[1]= X.get_arraySectionRanges(1);
     arraySectionRanges[2]= X.get_arraySectionRanges(2);
     
     arraySectionSizes[0] = X.get_arraySectionSizes(0);
     arraySectionSizes[1] = X.get_arraySectionSizes(1);
     arraySectionSizes[2] = X.get_arraySectionSizes(2);
     arraySectionPointers = X.get_arraySectionPointers();
    if(boundaryType == MulticoreArray<T>::detached)
    { 
     haloRegionsSizes[0] = get_haloSectionSizes(0);
     haloRegionsSizes[1] = get_haloSectionSizes(1);
     haloRegionsSizes[2] = get_haloSectionSizes(2);

     for (int dim =0; dim < DIM; dim++)
     {
       if(haloSize[dim])   
       {
         haloSectionPointers[dim][0] = get_haloSectionPointers(dim,0);
         haloSectionPointers[dim][1] = get_haloSectionPointers(dim,1);
       }
     }
    }
}

// Assignment operator
template <typename T>
MulticoreArray<T>&
MulticoreArray<T>::operator=(const MulticoreArray<T> &X)
{
     numberOfCores = X.get_numberOfCores();
     padding =X.get_padding();
     arraySize[0] = X.get_arraySize(0);
     arraySize[1] = X.get_arraySize(1);
     arraySize[2] = X.get_arraySize(2);
  
     coreArraySize[0] = X.get_coreArraySize(0);
     coreArraySize[1] = X.get_coreArraySize(1);
     coreArraySize[2] = X.get_coreArraySize(2);

     coreArray = X.get_coreArray();
#if HAVE_NUMA_H
     NUMAcoreArraySize[0] = X.get_NUMAcoreArraySize(0);
     NUMAcoreArraySize[1] = X.get_NUMAcoreArraySize(1);
     NUMAcoreArraySize[2] = X.get_NUMAcoreArraySize(2);
#endif
     haloSize[0] = X.get_haloWidth(0);
     haloSize[1] = X.get_haloWidth(1);
     haloSize[2] = X.get_haloWidth(2);

     boundaryType = X.boundaryType;

     arraySectionRanges[0]= X.get_arraySectionRanges(0);
     arraySectionRanges[1]= X.get_arraySectionRanges(1);
     arraySectionRanges[2]= X.get_arraySectionRanges(2);
     
     arraySectionSizes[0] = X.get_arraySectionSizes(0);
     arraySectionSizes[1] = X.get_arraySectionSizes(1);
     arraySectionSizes[2] = X.get_arraySectionSizes(2);
     arraySectionPointers = X.get_arraySectionPointers();
    if(boundaryType == MulticoreArray<T>::detached)
    { 
     haloRegionsSizes[0] = get_haloSectionSizes(0);
     haloRegionsSizes[1] = get_haloSectionSizes(1);
     haloRegionsSizes[2] = get_haloSectionSizes(2);

     for (int dim =0; dim < DIM; dim++)
     {
       if(haloSize[dim])   
       {
         haloSectionPointers[dim][0] = get_haloSectionPointers(dim,0);
         haloSectionPointers[dim][1] = get_haloSectionPointers(dim,1);
       }
     }
    }
}

template class MulticoreArray<int>;
template class MulticoreArray<long>;
template class MulticoreArray<float>;
template class MulticoreArray<double>;
