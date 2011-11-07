
template <typename T>
Core<T>::Core( const MulticoreArray<T>* input_multicoreArray )
   : multicoreArray(input_multicoreArray)
  // arraySectionPointer(input_multicoreArray->arraySectionPointer)
   {
     assert(multicoreArray != NULL);
  // assert(arraySectionPointer != NULL);
   }




template <typename T>
T & 
Core<T>::operator()(const int & i, const int & j, const int & k)
   {
  // assert(i >= 0);
  // assert(j >= 0);

     assert(k == 0);

     assert(multicoreArray      != NULL);
     assert(arraySectionPointer != NULL);

     int coreArrayIndex_X = coreArrayNeighborhood_2D[1][1][0];
     int coreArrayIndex_Y = coreArrayNeighborhood_2D[1][1][1];
     int elementIndex_X = i;
     int elementIndex_Y = j;

#if 0
     printf ("In Core<T>::operator(): i = %d j = %d coreArrayIndex_X = %d coreArrayIndex_Y = %d \n",i,j,coreArrayIndex_X,coreArrayIndex_Y);
#endif

     if (i >= 0 && j >= 0)
        {
          if (i < coreArrayNeighborhoodSizes_2D[1][1][0] && j < coreArrayNeighborhoodSizes_2D[1][1][1])
             {
            // Return the element from the current core
               return arraySectionPointer[(j*coreArrayNeighborhoodSizes_2D[1][1][0])+i];
             }
            else
             {
            // For the case where the index is out of bounds for the current core, we could compute the X and Y entry core array directly

            // Accesses of areas outside of the current core return the associated element in the adjacent core.
               if (i >= coreArrayNeighborhoodSizes_2D[1][1][0])
                  {
                 // For now if it is outside of the existing core then just access the edge of the adjacent core.
                    coreArrayIndex_X = coreArrayNeighborhood_2D[1][2][0];
                    elementIndex_X   = 0;
                 // return multicoreArray->getCore(coreArrayNeighborhood_2D[2][1][0],coreArrayNeighborhood_2D[1][1][1],0)(0,j,0);
                  }

               if (j >= coreArrayNeighborhoodSizes_2D[1][1][1])
                  {
                    coreArrayIndex_Y = coreArrayNeighborhood_2D[2][1][1];
                    elementIndex_Y   = 0;
                 // return multicoreArray->getCore(coreArrayNeighborhood_2D[1][1][0],coreArrayNeighborhood_2D[1][2][1],0)(i,0,0);
                  }
             }
        }
       else
        {
       // Accesses of areas outside of the current core return the associated element in the adjacent core.
          if (i < 0)
             {
               coreArrayIndex_X = coreArrayNeighborhood_2D[1][0][0];
               elementIndex_X   = coreArrayNeighborhoodSizes_2D[1][0][0]-1;
            // return multicoreArray->getCore(coreArrayNeighborhood_2D[1][0][0],coreArrayNeighborhood_2D[1][1][1],0)(coreArrayNeighborhoodSizes_2D[1][0][0],j,0);
             }

          if (j < 0)
             {
               coreArrayIndex_Y = coreArrayNeighborhood_2D[0][1][1];
               elementIndex_Y   = coreArrayNeighborhoodSizes_2D[0][1][1]-1;
            // return multicoreArray->getCore(coreArrayNeighborhood_2D[1][1][0],coreArrayNeighborhood_2D[1][0][1],0)(i,coreArrayNeighborhoodSizes_2D[0][1][1],0);
             }
        }

     assert(coreArrayIndex_X >= 0);
     assert(coreArrayIndex_Y >= 0);

#if 0
     printf ("In Core<T>::operator(): i = %d j = %d coreArrayIndex_X = %d coreArrayIndex_Y = %d elementIndex_X   = %d elementIndex_Y   = %d \n",i,j,coreArrayIndex_X,coreArrayIndex_Y,elementIndex_X,elementIndex_Y);
  // printf ("elementIndex_X   = %d elementIndex_Y   = %d \n",elementIndex_X,elementIndex_Y);
#endif

     return multicoreArray->getCore(coreArrayIndex_X,coreArrayIndex_Y,0)(elementIndex_X,elementIndex_Y,0);
   }









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

template <typename T>
int
Core<T>::get_arraySectionSize(int dim) const
   {
  // Access function.
     return arraySectionSize[dim];
   }

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




// **********************************************************************

template <typename T>
void
Core<T>::relax( int core, MulticoreArray<T> & array,  MulticoreArray<T> & old_array )
   {
     assert(multicoreArray != NULL);

     const int arraySizeX    = array.get_arraySize(0);
     const int arraySizeY    = array.get_arraySize(1);
     const int arraySizeZ    = array.get_arraySize(2);

     int p = core;

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
                    assert(coreArrayNeighborhoodSizes_3D[1][1][1][0] >= 0);
                    assert(coreArrayNeighborhoodSizes_3D[1][1][1][1] >= 0);
                    assert(coreArrayNeighborhoodSizes_3D[1][1][1][2] >= 0);
                    if (coreArrayNeighborhoodSizes_3D[1][1][1][0] > 2 && coreArrayNeighborhoodSizes_3D[1][1][1][1] > 2 && coreArrayNeighborhoodSizes_3D[1][1][1][2] > 2)
                       {
                         for (int k = 1; k < coreArrayNeighborhoodSizes_3D[1][1][1][2]-1; k++)
                            {
                              for (int j = 1; j < coreArrayNeighborhoodSizes_3D[1][1][1][1]-1; j++)
                                 {
                                   for (int i = 1; i < coreArrayNeighborhoodSizes_3D[1][1][1][0]-1; i++)
                                      {
                                     // This is the dominant computation for each array section per core. The compiler will use the 
                                     // user's code to derive the code that will be put here.
#if 0
                                        printf ("p= %d Indexing 3D array (i,j,k) = (%d,%d,%d) \n",p,i,j,k);
#endif
#if 1
                                        arraySection[index3D(i,j,k)] = 
                                           (old_arraySection[index3D(i-1,j,k)] + old_arraySection[index3D(i+1,j,k)] + 
                                            old_arraySection[index3D(i,j-1,k)] + old_arraySection[index3D(i,j+1,k)] + 
                                            old_arraySection[index3D(i,j,k-1)] + old_arraySection[index3D(i,j,k+1)]) / 6.0;
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
                              printf ("coreArrayNeighborhoodSizes_2D[1][1][0] = %d coreArrayNeighborhoodSizes_2D[1][1][1] = %d \n",coreArrayNeighborhoodSizes_2D[1][1][0],coreArrayNeighborhoodSizes_2D[1][1][1]);
#endif
                           // The core array may higher dimensional then the array and if so then the local size along
                           // the Z axis may be zero.  If so, then we don't want to process the local array section.
                           // if (sectionSize[2] == 1)
                           // if (sectionSize[0] > 2 && sectionSize[1] > 2 && sectionSize[2] == 1)
                           // if (coreArrayNeighborhoodSizes_2D[1][1][0] > 2 && coreArrayNeighborhoodSizes_2D[1][1][1] > 2)
                              if (coreArrayNeighborhoodSizes_2D[1][1][0] > 2 && coreArrayNeighborhoodSizes_2D[1][1][1] > 2 && coreArrayNeighborhoodSizes_2D[1][1][2] == 1)
                                 {
                                   for (int j = 1; j < coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                      {
                                        for (int i = 1; i < coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                           {
                                          // This is the dominant computation for each array section per core. The compiler will use the 
                                          // user's code to derive the code that will be put here.
#if 1
                                             arraySection[index2D(i,j)] = 
                                                (old_arraySection[index2D(i-1,j)] + old_arraySection[index2D(i+1,j)] + 
                                                 old_arraySection[index2D(i,j-1)] + old_arraySection[index2D(i,j+1)]) / 4.0;
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
                                   if (coreArrayNeighborhoodSizes_1D[1][0] > 0 && (coreArrayNeighborhoodSizes_1D[1][1] == 1 && coreArrayNeighborhoodSizes_1D[1][2] == 1))
                                      {
                                        for (int i = 1; i < coreArrayNeighborhoodSizes_1D[1][0]-1; i++)
                                           {
                                          // This is the dominant computation for each array section per core. The compiler will use the 
                                          // user's code to derive the code that will be put here.
#if 0
                                             printf ("i = %d old_arraySection[index1D(i-1)=%d] = %f \n",i,index1D(i-1),arraySection[index1D(i-1)]);
                                             printf ("i = %d old_arraySection[index1D(i+1)=%d] = %f \n",i,index1D(i+1),arraySection[index1D(i+1)]);
#endif
#if 1
                                             arraySection[index1D(i)] = (old_arraySection[index1D(i-1)] + old_arraySection[index1D(i+1)]) / 2.0;
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

// *********************************************************************

template <typename T>
void
Core<T>::relax_on_boundary( int core, MulticoreArray<T> & array,  MulticoreArray<T> & old_array )
   {
     assert(multicoreArray != NULL);

     const int arraySizeX    = array.get_arraySize(0);
     const int arraySizeY    = array.get_arraySize(1);
     const int arraySizeZ    = array.get_arraySize(2);

     int p = core;

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
#if 1
                 // This is required to avoid valgrind reported errors on some blocks where the local (sectionSize[dim]) is zero.
                 // This is likely because of over flow from size_t type veraibles.
                    if (coreArrayNeighborhoodSizes_3D[1][1][1][0] > 2 && coreArrayNeighborhoodSizes_3D[1][1][1][1] > 2 && coreArrayNeighborhoodSizes_3D[1][1][1][2] > 2)
                       {
                         for (int k = 1; k < coreArrayNeighborhoodSizes_3D[1][1][1][2]-1; k++)
                            {
                              for (int j = 1; j < coreArrayNeighborhoodSizes_3D[1][1][1][1]-1; j++)
                                 {
                                   for (int i = 1; i < coreArrayNeighborhoodSizes_3D[1][1][1][0]-1; i++)
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
                           // if ((coreArrayNeighborhoodSizes_2D[1][1][0] >= 2 || coreArrayNeighborhoodSizes_2D[1][1][1] >= 2) && coreArrayNeighborhoodSizes_2D[1][1][2] == 1)
                              if ((coreArrayNeighborhoodSizes_2D[1][1][0] >= 1 || coreArrayNeighborhoodSizes_2D[1][1][1] >= 1) && coreArrayNeighborhoodSizes_2D[1][1][2] == 1)
                                 {
                                // Handle the internal boundary equations along edges of the 2D arrays.

                                // ***************************************
                                // Now process the edges along the X axis.
                                // ***************************************

                                // if (sectionSize[1] > 1)
                                   if (coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                      {
#if 0
                                        printf ("-- leftEdgeSection[1] = %s rightEdgeSection[1] = %s \n",leftEdgeSection[1] ? "true" : "false",rightEdgeSection[1] ? "true" : "false");
#endif
                                     // if (leftEdgeSection[1] == true)
                                        if (bounaryCore_2D[1][0] == true)
                                           {
#if 0
                                             printf ("--- Apply the 2D array abstraction's UPPER boundary condition \n");
#endif
                                           }
                                          else
                                           {
                                          // This is where user specific code is places within the compiler transformation.
#if 0
                                             printf ("apply 2D equation at left edge of memory segment coreArrayNeighborhoodSizes_2D[0][1][1] = %d \n",coreArrayNeighborhoodSizes_2D[0][1][1]);
#endif
                                          // if (previous_sectionSize[1] > 0)
                                             if (coreArrayNeighborhoodSizes_2D[0][1][1] > 0)
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
                                                  for (int i = 1; i < coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                     {
                                                    // arraySection[index2D(i,0)] = (old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(i-1,previous_sectionSize[1]-1)] + old_arraySection[index2D(i-1,1)] +
                                                    //                               old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(i+1,previous_sectionSize[1]-1)] + old_arraySection[index2D(i+1,1)]) / 4.0;
                                                       arraySection[index2D(i,0)] = 
                                                          ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(i,coreArrayNeighborhoodSizes_2D[0][1][1]-1,coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                            /* array[Y+1][X] */ old_arraySection[index2D(i,1)] +
                                                            /* array[Y][X-1] */ old_arraySection[index2D(i-1,0)] + 
                                                            /* array[Y][X+1] */ old_arraySection[index2D(i+1,0)]) / 4.0;
                                                     }
                                                }
                                           }

                                     // if (rightEdgeSection[1] == true)
                                        if (bounaryCore_2D[1][1] == true)
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
                                             printf ("apply 2D equation at right edge of memory segment coreArrayNeighborhoodSizes_2D[2][1][1] = %d \n",coreArrayNeighborhoodSizes_2D[2][1][1]);
#endif
                                          // if (next_sectionSize[1] > 0)
                                             if (coreArrayNeighborhoodSizes_2D[2][1][1] > 0)
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
                                                  for (int i = 1; i < coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                     {
                                                       arraySection[index2D(i,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                          ( /* array[Y-1][X] */ old_arraySection[index2D(i,coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                            /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(i,0,coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                            /* array[Y][X-1] */ old_arraySection[index2D(i-1,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] +
                                                            /* array[Y][X+1] */ old_arraySection[index2D(i+1,coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
                                                     }
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Trivial case of only one equation (define this to be left edge; use the associated references).
                                     // if (sectionSize[1] == 1)
                                        if (coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                           {
#if 0
                                             printf ("--- Trivial case of only one 2D equation (define this to be UPPER edge) \n");
                                             printf ("--- bounaryCore_2D[1][0] = %s bounaryCore_2D[1][1] = %s \n",bounaryCore_2D[1][0] ? "true" : "false",bounaryCore_2D[1][1] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[1] == false && rightEdgeSection[1] == false)
                                             if (bounaryCore_2D[1][0] == false && bounaryCore_2D[1][1] == false)
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // if (previous_sectionSize[1] > 0 && next_sectionSize[1] > 0)
                                                  if (coreArrayNeighborhoodSizes_2D[0][1][1] > 0 && coreArrayNeighborhoodSizes_2D[2][1][1] > 0)
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
                                                       for (int i = 1; i < coreArrayNeighborhoodSizes_2D[1][1][0]-1; i++)
                                                          {
                                                            arraySection[index2D(i,0)] = 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(i,coreArrayNeighborhoodSizes_2D[0][1][1]-1,coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(i,0,coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(i-1,0)] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(i+1,0)]) / 4.0;
                                                          }
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // assert(sectionSize[1] == 0);
                                             assert(coreArrayNeighborhoodSizes_2D[1][1][1] == 0);
#if 0
                                             printf ("--- coreArrayNeighborhoodSizes_2D[1][1][1] == 0: This is the trival case \n");
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
                                   if (coreArrayNeighborhoodSizes_2D[1][1][0] > 1)
                                      {
#if 0
                                        printf ("---+++ leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                     // if (leftEdgeSection[0] == true)
                                        if (bounaryCore_2D[0][0] == true)
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
                                             printf ("apply equation at left edge of memory segment coreArrayNeighborhoodSizes_2D[1][0][0] = %d \n",coreArrayNeighborhoodSizes_2D[1][0][0]);
#endif
                                          // if (previous_sectionSize[0] > 0)
                                             if (coreArrayNeighborhoodSizes_2D[1][0][0] > 0)
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
                                                  for (int j = 1; j < coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                     {
#if 1
                                                       arraySection[index2D(0,j)] = 
                                                          ( /* array[Y-1][X] */ old_arraySection[index2D(0,j-1)] +
                                                            /* array[Y+1][X] */ old_arraySection[index2D(0,j+1)] +
                                                         // /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,j)] + 
                                                            /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,j,coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                            /* array[Y][X+1] */ old_arraySection[index2D(1,j)]) / 4.0;
#endif
                                                     }
                                                }
                                           }

                                     // if (rightEdgeSection[0] == true)
                                        if (bounaryCore_2D[0][1] == true)
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
                                             printf ("apply equation at right edge of memory segment coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",coreArrayNeighborhoodSizes_2D[1][2][0]);
#endif
                                          // if (next_sectionSize[0] > 0)
                                             if (coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
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
                                                  for (int j = 1; j < coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                     {
                                                    // arraySection[index2D(sectionSize[0]-1,j)] = (old_arraySection[index2D(sectionSize[0]-2,j-1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j-1)] +
                                                    //                                              old_arraySection[index2D(sectionSize[0]-2,j+1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j+1)]) / 4.0;
#if 0
                                                       printf ("array[Y][X]:   old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)]           = %f \n",old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)]);
                                                       printf ("array[Y-1][X]: old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)]          = %f \n",old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)]);
                                                       printf ("array[Y+1][X]: old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)]          = %f \n",old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)]);
                                                       printf ("array[Y][X-1]: old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)]            = %f \n",old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)]);
                                                       printf ("p = %d coreArrayNeighborhoodLinearized_2D[1][2] = %d \n",p,coreArrayNeighborhoodLinearized_2D[1][2]);
                                                       printf ("p = %d coreArrayNeighborhoodSizes_2D[1][1][0] = %d \n",p,coreArrayNeighborhoodSizes_2D[1][1][0]);
                                                       printf ("p = %d coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",p,coreArrayNeighborhoodSizes_2D[1][2][0]);
                                                    // printf ("array[Y][X+1]: old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)] = %f \n",old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]);
                                                       printf ("array[Y][X+1]: old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,coreArrayNeighborhoodSizes_2D[1][2][0])] = %f \n",
                                                            old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,coreArrayNeighborhoodSizes_2D[1][2][0])]);
#endif
#if 1
// This fails for some random problem...
                                                       arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j)] = 
                                                          ( /* array[Y-1][X] */ old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j-1)] + 
                                                            /* array[Y+1][X] */ old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,j+1)] +
                                                            /* array[Y][X-1] */ old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-2,j)] + 
                                                         // /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]) / 4.0;
                                                            /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                     }
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // Trivial case of only one equation (define this to be left edge; use the associated references).
                                     // if (sectionSize[0] == 1)
                                        if (coreArrayNeighborhoodSizes_2D[1][1][0] == 1)
                                           {
#if 0
                                             printf ("---+++ Trivial case of only one equation (define this to be left edge; use the associated references) \n");
                                             printf ("---+++ leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                             if (bounaryCore_2D[0][0] == false && bounaryCore_2D[0][1] == false)
                                                {
                                               // This is where user specific code is places within the compiler transformation.
                                               // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                  if (coreArrayNeighborhoodSizes_2D[1][0][0] > 0 && coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
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
                                                       for (int j = 1; j < coreArrayNeighborhoodSizes_2D[1][1][1]-1; j++)
                                                          {
                                                         // arraySection[index2D(0,j)] = (old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(previous_sectionSize[0]-1,j-1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j-1)] +
                                                         //                               old_arraySectionPointers[previous_coreIndexInLinearArray][index2D(previous_sectionSize[0]-1,j+1)] + old_arraySectionPointers[next_coreIndexInLinearArray][index2D(0,j+1)]) / 4.0;
#if 1
                                                            arraySection[index2D(0,j)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(0,j-1)] +
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(0,j+1)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,j)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,j,coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,j)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,j,coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // assert(sectionSize[0] == 0);
                                             assert(coreArrayNeighborhoodSizes_2D[1][1][0] == 0);
#if 0
                                             printf ("---+++ coreArrayNeighborhoodSizes_2D[1][0][0] == 0: This is the trival case \n");
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
                                   printf ("---+++ Process the edges of the memory section on core p = %d coreArrayNeighborhoodSizes_2D[1][1][0] = %d coreArrayNeighborhoodSizes_2D[1][0][0] = %d coreArrayNeighborhoodSizes_2D[1][2][0] = %d \n",
                                        p,coreArrayNeighborhoodSizes_2D[1][1][0],coreArrayNeighborhoodSizes_2D[1][0][0],coreArrayNeighborhoodSizes_2D[1][2][0]);
                                   printf ("Sizes of current processor: coreArrayNeighborhoodSizes_2D[1][1] = (%d,%d,%d) \n",coreArrayNeighborhoodSizes_2D[1][1][0],coreArrayNeighborhoodSizes_2D[1][1][1],coreArrayNeighborhoodSizes_2D[1][1][2]);
#endif
                                // First X Axis logic
                                   if (coreArrayNeighborhoodSizes_2D[1][1][0] > 1)
                                      {
                                     // Left sice corners
                                        if (bounaryCore_2D[0][0] == true)
                                           {
                                          // processor boundary condition enforced here (X axis)
                                           }
                                          else
                                           {
                                             if (coreArrayNeighborhoodSizes_2D[1][0][0] > 0)
                                                {
                                               // Next Y Axis logic
                                                  if (coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                                     {
                                                    // Upper corner
                                                       if (bounaryCore_2D[1][0] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (coreArrayNeighborhoodSizes_2D[0][1][0] > 0);
                                                            assert (coreArrayNeighborhoodSizes_2D[0][1][1] > 0);

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
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][index2D(0,coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,coreArrayNeighborhoodSizes_2D[0][1][1]-1,coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(1,0)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,0)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(0,1)]) / 4.0;
#endif
                                                          }

                                                    // Lower corner
                                                       if (bounaryCore_2D[1][1] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (coreArrayNeighborhoodSizes_2D[0][1][1] > 0);
                                                            assert (coreArrayNeighborhoodSizes_2D[1][0][0] > 0);

                                                         // Lower left corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ***** | X***** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 0
                                                            printf ("--- array[Y][X]:   arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
                                                            printf ("old_array[Y][X]:   old_arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",old_arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
                                                            printf ("old_array[Y-1][X]: old_arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-2)] = %f \n",old_arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-2)]);
                                                            printf ("old_array[Y+1][X]: old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)] = %f \n",old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)]);
                                                            printf ("old_array[Y][X-1]: old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,coreArrayNeighborhoodSizes_2D[1][0][1]-1)] = %f \n",
                                                                 old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,coreArrayNeighborhoodSizes_2D[1][0][1]-1)]);
                                                            printf ("array[Y][X+1]: old_arraySection[index2D(1,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",old_arraySection[index2D(1,coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
#endif
#if 1
                                                            arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][index2D(0,0)] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,coreArrayNeighborhoodSizes_2D[1][0][1]-1)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,coreArrayNeighborhoodSizes_2D[1][0][1]-1,coreArrayNeighborhoodSizes_2D[1][0][0])] + 
                                                                 /* array[Y][X+1] */ old_arraySection[index2D(1,coreArrayNeighborhoodSizes_2D[1][1][1]-1)]) / 4.0;
#endif
#if 0
                                                            printf ("--- array[Y][X]: arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = %f \n",arraySection[index2D(0,coreArrayNeighborhoodSizes_2D[1][1][1]-1)]);
#endif
                                                          }
                                                     }
                                                    else
                                                     {
                                                    // printf ("coreArrayNeighborhoodSizes_2D[1][1][1] = %d \n",coreArrayNeighborhoodSizes_2D[1][1][1]);
                                                       if (coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                                          {
                                                         // Case of upper and lower left corners are the same point
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | X***** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(0,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][index2D(0,coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,coreArrayNeighborhoodSizes_2D[0][1][1]-1,coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,coreArrayNeighborhoodSizes_2D[1][2][0])] +
                                                              // /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,0)] + 
                                                                 /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,coreArrayNeighborhoodSizes_2D[1][0][0])] + 
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
                                        if (bounaryCore_2D[0][1] == true)
                                           {
                                          // Can we test if this is realy a boundary?
                                           }
                                          else
                                           {
                                          // if (next_sectionSize[0] > 0)
                                             if (coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                {
                                               // printf ("Right boundary corner not implemented! \n");

                                               // Next Y Axis logic
                                                  if (coreArrayNeighborhoodSizes_2D[1][1][1] > 1)
                                                     {
                                                    // Upper corner
                                                       if (bounaryCore_2D[1][0] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (coreArrayNeighborhoodSizes_2D[0][1][0] > 0);
                                                            assert (coreArrayNeighborhoodSizes_2D[0][1][1] > 0);

                                                         // Upper right corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | *****X | *****
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][index2D(coreArrayNeighborhoodSizes_2D[0][1][0]-1,coreArrayNeighborhoodSizes_2D[0][1][1]-1)] + 
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[0][1][0]-1,coreArrayNeighborhoodSizes_2D[0][1][1]-1,coreArrayNeighborhoodSizes_2D[0][1][0])] + 
                                                                 /* array[Y+1][X] */ old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,1)] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][1]-2,0)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }

                                                    // Lower corner
                                                       if (bounaryCore_2D[1][1] == true)
                                                          {
                                                         // processor boundary condition enforced here (Y axis)
                                                          }
                                                         else
                                                          {
                                                            assert (coreArrayNeighborhoodSizes_2D[0][1][1] > 0);
                                                            assert (coreArrayNeighborhoodSizes_2D[1][0][0] > 0);

                                                         // Lower right corner
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
                                                         // ***** | ****** | *****
                                                         // ***** | *****X | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] = 
                                                               ( /* array[Y-1][X] */ old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,coreArrayNeighborhoodSizes_2D[1][1][1]-2)] + 
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][index2D(coreArrayNeighborhoodSizes_2D[2][1][0]-1,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[2][1][0]-1,0,coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-2,coreArrayNeighborhoodSizes_2D[1][1][1]-1)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,coreArrayNeighborhoodSizes_2D[2][1][1]-1)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,coreArrayNeighborhoodSizes_2D[2][1][1]-1,coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
#endif
                                                          }
                                                     }
                                                    else
                                                     {
                                                    // printf ("coreArrayNeighborhoodSizes_2D[1][1][1] = %d \n",coreArrayNeighborhoodSizes_2D[1][1][1]);
                                                       if (coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                                          {
                                                         // Case of upper and lower right corners are the same point
                                                         // ***** | ****** | *****
                                                         // ----------------------
                                                         // ***** | *****X | *****
                                                         // ----------------------
                                                         // ***** | ****** | *****
#if 1
                                                            arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                            // ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][index2D(coreArrayNeighborhoodSizes_2D[0][1][0]-1,coreArrayNeighborhoodSizes_2D[0][1][1]-1)] +
                                                               ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[0][1][0]-1,coreArrayNeighborhoodSizes_2D[0][1][1]-1,coreArrayNeighborhoodSizes_2D[0][1][0])] +
                                                              // /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][index2D(coreArrayNeighborhoodSizes_2D[2][1][0]-1,0)] +
                                                                 /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[2][1][0]-1,0,coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                                 /* array[Y][X-1] */ old_arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-2,0)] +
                                                              // /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][index2D(0,0)]) / 4.0;
                                                                 /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
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
                                        printf ("Case of coreArrayNeighborhoodSizes_2D[1][1][0] == %d \n",coreArrayNeighborhoodSizes_2D[1][1][0]);
                                        printf ("Case of coreArrayNeighborhoodSizes_2D[1][1][1] == %d \n",coreArrayNeighborhoodSizes_2D[1][1][1]);
#endif
                                     // assert(coreArrayNeighborhoodSizes_2D[1][1][0] == 1);
                                     // assert(coreArrayNeighborhoodSizes_2D[1][1][1] == 1);

                                     // if (sectionSize[0] == 1)
                                     // if (coreArrayNeighborhoodSizes_2D[1][1][0] == 1)
                                        if (coreArrayNeighborhoodSizes_2D[1][1][0] == 1 && coreArrayNeighborhoodSizes_2D[1][1][1] == 1)
                                           {
                                          // printf ("Case of coreArrayNeighborhoodSizes_2D[1][1][0] == 1 && coreArrayNeighborhoodSizes_2D[1][1][1] == 1\n");

                                          // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                          // if (bounaryCore_2D[0][0] == false && bounaryCore_2D[0][1] == false)
                                             if (bounaryCore_2D[0][0] == false && bounaryCore_2D[0][1] == false && bounaryCore_2D[1][0] == false && bounaryCore_2D[1][1] == false)
                                                {
                                               // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                  if (coreArrayNeighborhoodSizes_2D[1][0][0] > 0 && coreArrayNeighborhoodSizes_2D[1][2][0] > 0)
                                                     {
                                                    // printf ("Case of single point boundary not implemented! \n");
                                                    // ***** | * | *****
                                                    // -----------------
                                                    // ***** | X | *****
                                                    // -----------------
                                                    // ***** | * | *****
#if 1
                                                       arraySection[index2D(coreArrayNeighborhoodSizes_2D[1][1][0]-1,0)] = 
                                                          ( /* array[Y-1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[0][1]][otherCore_index2D(0,coreArrayNeighborhoodSizes_2D[0][1][1]-1,coreArrayNeighborhoodSizes_2D[0][1][0])] +
                                                            /* array[Y+1][X] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[2][1]][otherCore_index2D(0,0,coreArrayNeighborhoodSizes_2D[2][1][0])] +
                                                            /* array[Y][X-1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][0]][otherCore_index2D(coreArrayNeighborhoodSizes_2D[1][0][0]-1,0,coreArrayNeighborhoodSizes_2D[1][0][0])] +
                                                            /* array[Y][X+1] */ old_arraySectionPointers[coreArrayNeighborhoodLinearized_2D[1][2]][otherCore_index2D(0,0,coreArrayNeighborhoodSizes_2D[1][2][0])]) / 4.0;
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
                                             if (coreArrayNeighborhoodSizes_2D[1][1][0] != 0)
                                                {
#if 0
                                                   printf ("Warning: p = %d coreArrayNeighborhoodSizes_2D[1][1][0] = %d \n",p,coreArrayNeighborhoodSizes_2D[1][1][0]);
#endif
                                                }
                                          // assert(coreArrayNeighborhoodSizes_2D[1][1][0] == 0);
                                             assert(coreArrayNeighborhoodSizes_2D[1][1][0] <= 1);
#if 0
                                             printf ("---+++ coreArrayNeighborhoodSizes_2D[1][0][0] == 0: This is the trival case \n");
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
                                   printf ("This array segment can't be processed for edge handling because it is too small in at least one axis: p = %d size = (%d,%d,%d) \n",p,coreArrayNeighborhoodSizes_2D[1][1][0],coreArrayNeighborhoodSizes_2D[1][1][1],coreArrayNeighborhoodSizes_2D[1][1][2]);
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
                                   if (coreArrayNeighborhoodSizes_1D[1][0] > 0 && (coreArrayNeighborhoodSizes_1D[1][1] == 1 && coreArrayNeighborhoodSizes_1D[1][2] == 1) )
                                      {
#if 0
                                         printf ("--- Process the edges of the memory section on core index = %d sectionSize[0] = %d previous_sectionSize[0] = %d next_sectionSize[0] = %d \n",p,sectionSize[0],previous_sectionSize[0],next_sectionSize[0]);
#endif
                                     // if (sectionSize[0] > 1)
                                        if (coreArrayNeighborhoodSizes_1D[1][0] > 1)
                                           {
#if 0
                                             printf ("-- leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                          // if (leftEdgeSection[0] == true)
                                             if (bounaryCore_1D[0] == true)
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
                                                  if (coreArrayNeighborhoodSizes_1D[0][0] > 0)
                                                     {
                                                    // arraySection[0] = (old_arraySectionPointers[previous_coreIndexInLinearArray][coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySection[1]) / 2.0;
                                                       arraySection[0] = (old_arraySectionPointers[coreArrayNeighborhoodLinearized_1D[0]][coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySection[1]) / 2.0;
                                                     }
                                                }

                                          // if (rightEdgeSection[0] == true)
                                             if (bounaryCore_1D[1] == true)
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
                                                  if (coreArrayNeighborhoodSizes_1D[2][0] > 0)
                                                     {
                                                    // arraySection[sectionSize[0]-1] = (old_arraySection[sectionSize[0]-2] + old_arraySectionPointers[next_coreIndexInLinearArray][0]) / 2.0;
                                                       arraySection[coreArrayNeighborhoodSizes_1D[1][0]-1] = (old_arraySection[coreArrayNeighborhoodSizes_1D[1][0]-2] + old_arraySectionPointers[coreArrayNeighborhoodLinearized_1D[2]][0]) / 2.0;
                                                     }
                                                }
                                           }
                                          else
                                           {
                                          // Trivial case of only one equation (define this to be left edge; use the associated references).
                                          // if (sectionSize[0] == 1)
                                             if (coreArrayNeighborhoodSizes_1D[1][0] == 1)
                                                {
#if 0
                                                  printf ("--- Trivial case of only one equation (define this to be left edge; use the associated references) \n");
                                                  printf ("--- leftEdgeSection[0] = %s rightEdgeSection[0] = %s \n",leftEdgeSection[0] ? "true" : "false",rightEdgeSection[0] ? "true" : "false");
#endif
                                               // if (leftEdgeSection[0] == false && rightEdgeSection[0] == false)
                                                  if (bounaryCore_1D[0] == false && bounaryCore_1D[1] == false)
                                                     {
                                                    // This is where user specific code is places within the compiler transformation.
                                                    // if (previous_sectionSize[0] > 0 && next_sectionSize[0] > 0)
                                                       if (coreArrayNeighborhoodSizes_1D[0][0] > 0 && coreArrayNeighborhoodSizes_1D[2][0] > 0)
                                                          {
#if 0
                                                            printf ("--- Processing trivial case of only one equation \n");
#endif
                                                         // arraySection[0] = (old_arraySectionPointers[previous_coreIndexInLinearArray][previous_sectionSize[0]-1] + old_arraySectionPointers[next_coreIndexInLinearArray][0]) / 2.0;
                                                            arraySection[0] = (old_arraySectionPointers[coreArrayNeighborhoodLinearized_1D[0]][coreArrayNeighborhoodSizes_1D[0][0]-1] + old_arraySectionPointers[coreArrayNeighborhoodLinearized_1D[2]][0]) / 2.0;
                                                          }
                                                     }
                                                }
                                               else
                                                {
                                               // assert(sectionSize[0] == 0);
                                                  assert(coreArrayNeighborhoodSizes_1D[1][0] == 0);
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
     coreArrayNeighborhood_1D[0][0] = get_left_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);
     coreArrayNeighborhood_1D[1][0] = indexIntoMultidimensionalCoreArrayForThisDimension[0];
     coreArrayNeighborhood_1D[2][0] = get_right_section_index(0,indexIntoMultidimensionalCoreArrayForThisDimension[0]);


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
                 // Not sure what this should be for 3D
                    if (i == 1 || j != 1 || k == 1)
                       {
                         coreArrayNeighborhood_3D[k][j][i][0] = coreArrayNeighborhood_3D[k][1][1][0];
                       }

                 // Not sure what this should be for 3D
                    if (i != 1 || j == 1 || k == 1)
                       {
                         coreArrayNeighborhood_3D[k][j][i][1] = coreArrayNeighborhood_3D[1][1][1][1];
                       }

                 // Not sure what this should be for 3D
                    if (i == 1 || j == 1 || k != 1)
                       {
                         coreArrayNeighborhood_3D[k][j][i][2] = coreArrayNeighborhood_3D[1][1][1][2];
                       }

                    if (i != 1 && j != 1 && k != 1)
                       {
                         coreArrayNeighborhood_3D[k][j][i][0] = coreArrayNeighborhood_3D[1][1][i][0];
                         coreArrayNeighborhood_3D[k][j][i][1] = coreArrayNeighborhood_3D[1][j][1][1];
                         coreArrayNeighborhood_3D[k][j][i][2] = coreArrayNeighborhood_3D[k][1][1][2];
                       }
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

               coreArrayNeighborhoodSizes_1D[i][dim] = (multicoreArray->get_tableBasedDistribution() == true) ? multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_1D[i]] : multicoreArray->algorithmicComputationOfSize(dim,coreArrayNeighborhood_1D[i][dim]);
             }
        }

  // Setup the linearized index entries for 2D
     for (int j = 0; j < 3; j++)
        {
          for (int i = 0; i < 3; i++)
             {
               for (int dim = 0; dim < 3; dim++)
                  {
                    coreArrayNeighborhoodSizes_2D[j][i][dim] = (multicoreArray->get_tableBasedDistribution() == true) ? multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_2D[j][i]] : multicoreArray->algorithmicComputationOfSize(dim,coreArrayNeighborhood_2D[j][i][dim]);
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
                         coreArrayNeighborhoodSizes_3D[k][j][i][dim] = (multicoreArray->get_tableBasedDistribution() == true) ? multicoreArray->get_arraySectionSizes(dim)[coreArrayNeighborhoodLinearized_3D[k][j][i]] : multicoreArray->algorithmicComputationOfSize(dim,coreArrayNeighborhood_3D[k][j][i][dim]);
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


// ****************************************



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

  // bool bounaryCore_1D[2]    = {false,false};
  // bool bounaryCore_2D[2][2] = { {false,false}, {false,false} };

     bounaryCore_1D[0]    = is_left_edge_section(coreArrayNeighborhood_1D [1][0],coreArrayNeighborhood_1D[0][0]);
     bounaryCore_1D[1]    = is_right_edge_section(coreArrayNeighborhood_1D[1][0],coreArrayNeighborhood_1D[2][0]);

  // X axis        X:Y L:U
     bounaryCore_2D[0][0] = is_left_edge_section(coreArrayNeighborhood_2D [1][1][0],coreArrayNeighborhood_2D[1][0][0]);
     bounaryCore_2D[0][1] = is_right_edge_section(coreArrayNeighborhood_2D[1][1][0],coreArrayNeighborhood_2D[1][2][0]);

  // Y axis
     bounaryCore_2D[1][0] = is_left_edge_section(coreArrayNeighborhood_2D [1][1][1],coreArrayNeighborhood_2D[0][1][1]);
     bounaryCore_2D[1][1] = is_right_edge_section(coreArrayNeighborhood_2D[1][1][1],coreArrayNeighborhood_2D[2][1][1]);

#if 0
     printf ("p = %d initializeCoreArrayBoundaryFlags get_tableBasedDistribution() = %s \n",p,multicoreArray->get_tableBasedDistribution() ? "true" : "false");
     printf ("bounaryCore_1D: (%s,%s) \n",bounaryCore_1D[0] ? "true" : "false",bounaryCore_1D[1] ? "true" : "false");
     printf ("bounaryCore_2D: X(%s,%s)Y(%s,%s) \n",bounaryCore_2D[0][0] ? "true" : "false",bounaryCore_2D[0][1] ? "true" : "false",bounaryCore_2D[1][0] ? "true" : "false",bounaryCore_2D[1][1] ? "true" : "false");
#endif
   }

