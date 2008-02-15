// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if !defined(COMPILE_APP)
#define COMPILE_PPP
#endif

#include<A++.h>
#include<dimension_macros.h>

// After PADRE is more stable within A++/P++ we can remove the Internal_Partitioning_Type
// object completely and greatly reduce the complexity of this file and of P++ in general.

#if defined(PPP)
#if !defined(USE_PADRE)
// Specify block distribution (choice of "*"-Undistributed or "B"-Block or "C"-Cyclic distribution)
char Internal_Partitioning_Type::DefaultDistribution_String      [MAX_ARRAY_DIMENSION] = { REPEATING_MACRO('B') };
int  Internal_Partitioning_Type::DefaultArrayDimensionsToAlign   [MAX_ARRAY_DIMENSION] = { IOTA_MACRO };
int  Internal_Partitioning_Type::DefaultInternalGhostCellWidth   [MAX_ARRAY_DIMENSION] = { REPEATING_MACRO(0) };
int  Internal_Partitioning_Type::DefaultExternalGhostCellWidth   [MAX_ARRAY_DIMENSION] = { REPEATING_MACRO(0) };
int  Internal_Partitioning_Type::DefaultPartitionControlFlags    [MAX_ARRAY_DIMENSION] = { REPEATING_MACRO(0) };
int  Internal_Partitioning_Type::DefaultDecomposition_Dimensions [MAX_ARRAY_DIMENSION] = { IOTA_MACRO };

// int  Internal_Partitioning_Type::DefaultNumber_Of_Dimensions_To_Partition = 4;
int                       Internal_Partitioning_Type::DefaultStarting_Processor = 0;
int                       Internal_Partitioning_Type::DefaultEnding_Processor   = Communication_Manager::Number_Of_Processors - 1;
List_Of_doubleArray       Internal_Partitioning_Type::DefaultdoubleArrayList;
List_Of_floatArray        Internal_Partitioning_Type::DefaultfloatArrayList;
List_Of_intArray          Internal_Partitioning_Type::DefaultintArrayList;
List_Of_Partitioning_Type Internal_Partitioning_Type::DefaultPartitioningObjectList;
#endif
#endif

void
Internal_Partitioning_Type::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

void
Internal_Partitioning_Type::freeMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > -1)
          printf ("Inside of Internal_Partitioning_Type::freeMemoryInUse() \n");
#endif

#if defined(PPP)
#if defined(USE_PADRE)
  // This is called when PADRE is cleaned up more generally from the GlobalMemoryRelease()
  // PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::freeMemoryInUse();
#else
  // Delete the lists of associated P++ array objects
     DefaultdoubleArrayList.clean();
     DefaultfloatArrayList.clean();
     DefaultintArrayList.clean();
     DefaultPartitioningObjectList.clean();
#endif
#endif

  // free memory allocated for memory pools!
     for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
          if (Memory_Block_List [i] != NULL)
               free ((char*) (Memory_Block_List[i]));
   }

Internal_Partitioning_Type::~Internal_Partitioning_Type()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of destructor for Internal_Partitioning_Type! \n");
#endif

  // Modified to match correction to the ReferenceCountBase used in a Partitioning_Type
     APP_ASSERT (referenceCount == getReferenceCountBase()-1);

     if (referenceCount > getReferenceCountBase())
        {
          printf ("ERROR: Partition object destructor called while reference count = %d \n",referenceCount);
          APP_ABORT();
        }
     APP_ASSERT (referenceCount >= getReferenceCountBase()-1);

     isRootOfPartitionTree    = TRUE;
     parentPartitioningObject = NULL;

#if defined(PPP)
#if defined(USE_PADRE)
     if (distributionPointer != NULL)
        {
          distributionPointer->decrementReferenceCount();
       // printf ("distributionPointer->getReferenceCount() = %d \n",distributionPointer->getReferenceCount());
          if (distributionPointer->getReferenceCount() < distributionPointer->getReferenceCountBase())
               delete distributionPointer;
          distributionPointer = NULL;
        }
#else
  // ... This seems to cause problems ... ???
#if 0
     APP_ASSERT (doubleArrayList.getLength() == 0);
     APP_ASSERT (floatArrayList.getLength() == 0);
     APP_ASSERT (intArrayList.getLength() == 0);
#else
     printf ("list.clean asserts commented out in ~Internal_Partitioning_Type()! \n");
#endif

     doubleArrayList.clean();
     floatArrayList.clean();
     intArrayList.clean();

     APP_ASSERT (PartitioningObjectList.getLength() == 0);
     PartitioningObjectList.clean();

#endif
#endif
   }

#if defined(PPP) || defined(SERIAL_APP)
intSerialArray
Internal_Partitioning_Type::getDefaultProcessorSet ()
   {
  // It is often handy to get access to the set of processors that are in use by a
  // partitoning object.
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::getDefaultProcessorSet ()! \n");
#endif

#if defined(USE_PADRE)
     printf ("WARNING: PADRE case of getDefaultProcessorSet() not accurate! \n");
     
     int defaultStartingProcessor = 0;
     int defaultEndingProcessor   = Communication_Manager::Number_Of_Processors - 1;

     intSerialArray Temp_Set ((defaultEndingProcessor - defaultStartingProcessor)+1);
     Temp_Set.seqAdd (defaultStartingProcessor);
     return Temp_Set;
#else
     intSerialArray Temp_Set ((DefaultEnding_Processor - DefaultStarting_Processor) + 1);
     Temp_Set.seqAdd (DefaultStarting_Processor);
     return Temp_Set;
#endif
   }
#else
// The A++ definition of getDefaultProcessorSet
intArray Internal_Partitioning_Type::getDefaultProcessorSet ()
   {
     intArray Temp_Set (1);
     Temp_Set = 0;
     return Temp_Set;
   }
#endif

#if 0
// Aparently, this is not used.  I changed the interface to PADRE_Distribution::getProcessorSet
// BTNG.
#if defined(PPP) || defined(SERIAL_APP)
intSerialArray
Internal_Partitioning_Type::getProcessorSet ()
   {
  // It is often handy to get access to the set of processors that are in use by a
  // partitoning object.
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::getProcessorSet ()! \n");
#endif

#if defined(USE_PADRE)
     int*processorArray     = NULL;
     int numberOfProcessors = 0;
  // PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::getProcessorSet(processorArray,numberOfProcessors);
     distributionPointer->getProcessorSet(processorArray,numberOfProcessors);
     return intSerialArray (processorArray,numberOfProcessors);
#else
     intSerialArray Temp_Set ((Ending_Processor - Starting_Processor) + 1);
     Temp_Set.seqAdd (Starting_Processor);

     return Temp_Set;
#endif
   }
#else
// The A++ definition of getProcessorSet
intArray
Internal_Partitioning_Type::getProcessorSet ()
   {
     intArray Temp_Set (1);
     Temp_Set = 0;
     return Temp_Set;
   }
#endif
#endif

void
Internal_Partitioning_Type::Initialize_Internal_Partitioning_Type ( const Range & Processor_Subcollection )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Initialize_Internal_Partitioning_Type (const Range & Processor_Subcollection)! \n");
#endif
     referenceCount           = getReferenceCountBase();

     isRootOfPartitionTree    = TRUE;
     parentPartitioningObject = NULL;

#if defined(PPP)
#if defined(USE_PADRE)
     printf ("ERROR: This function should not be called if PADRE is being used! \n");
     APP_ABORT();
#else
     Starting_Processor       = Processor_Subcollection.getBase();
     int Number_Of_Processors = Processor_Subcollection.length();
     if ( Number_Of_Processors > Communication_Manager::Number_Of_Processors )
        {
          printf ("WARNING: Number_Of_Processors (%d) specified in Internal_Partitioning_Type::Initialize_Internal_Partitioning_Type (Range) \n",
               Number_Of_Processors);
          printf ("         is greater than the existing size of the virtual machine (= %d) resetting: Number_Of_Processors = %d \n",
                    Communication_Manager::Number_Of_Processors, Communication_Manager::Number_Of_Processors);
          Number_Of_Processors = Communication_Manager::Number_Of_Processors;
        }

     Ending_Processor = Starting_Processor + Number_Of_Processors-1;

     for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // The default is to distribute all dimensions
          Distribution_String         [i] = 'B';
       // (7/14/2000) this fixes bug2000_07.C: The ghost cell width should
       // be taken from the default value which can be set by the user
       // LocalGhostCellWidth         [i] =  0;
          LocalGhostCellWidth         [i] =  DefaultInternalGhostCellWidth [i];
          ArrayDimensionsToAlign      [i] =  i;
          PartitionControlFlags       [i] =  0;
          Decomposition_Dimensions    [i] =  i;
        }
#endif
#endif
   }

Internal_Partitioning_Type::Internal_Partitioning_Type()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of constructor for Internal_Partitioning_Type! \n");
#endif

     isRootOfPartitionTree    = TRUE;
     parentPartitioningObject = NULL;

     referenceCount           = getReferenceCountBase();

#if defined(USE_PADRE)
  // Bugfix (9/15/2000) Need to build a new partitioning object not get the default one!
#if 0
     printf ("WARNING: Internal_Partitioning_Type constructor should not call PADRE_Distribution::getDefaultDistributionPointer(); \n");
     distributionPointer = PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::getDefaultDistributionPointer();
#else
     distributionPointer = new PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>( 0, Communication_Manager::Number_Of_Processors-1 );
#endif

  // This is just one of multiple references to this object so record the reference
  // distributionPointer->incrementReferenceCount();
#else
     Range Subcollection (0,Communication_Manager::Number_Of_Processors-1);
     Initialize_Internal_Partitioning_Type ( Subcollection );
#endif
   }

Internal_Partitioning_Type::Internal_Partitioning_Type ( const Range & Processor_Subcollection )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of constructor for Internal_Partitioning_Type (const Range & Processor_Subcollection)! \n");
#endif

     isRootOfPartitionTree    = TRUE;
     parentPartitioningObject = NULL;

     referenceCount          = getReferenceCountBase();

#if defined(USE_PADRE)
  // Try to stay within the range of the number of processors that are available
     int startingProcessor   = Processor_Subcollection.getBase();
     int numberOfProcessors = Processor_Subcollection.length();
     if ( numberOfProcessors > Communication_Manager::Number_Of_Processors )
        {
          printf ("WARNING: numberOfProcessors (%d) specified in Internal_Partitioning_Type constructor (PADRE) \n",
               numberOfProcessors);
          printf ("         is greater than the existing size of the virtual machine (= %d) resetting: numberOfProcessors = %d \n",
                    Communication_Manager::Number_Of_Processors, Communication_Manager::Number_Of_Processors);
          numberOfProcessors = Communication_Manager::Number_Of_Processors;
        }

     int endingProcessor = startingProcessor + numberOfProcessors-1;

  // This builds a new PADRE_Distribution object with no other refeences to it
     distributionPointer = new PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (startingProcessor,endingProcessor);
     APP_ASSERT (distributionPointer != NULL);
#else
     Initialize_Internal_Partitioning_Type ( Processor_Subcollection );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of constructor for Internal_Partitioning_Type (const Range) referenceCount = %d  \n",referenceCount);
#endif
   }


Internal_Partitioning_Type::Internal_Partitioning_Type ( int Number_Of_Processors )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of constructor for Internal_Partitioning_Type(int Number_Of_Processors)! \n");

     if (Number_Of_Processors < 1)
        {
          printf ("ERROR: Number_Of_Processors (%d) < 1 in initialization of Internal_Partitioning_Type! \n",Number_Of_Processors);
          APP_ABORT();
        }
#endif

     isRootOfPartitionTree    = TRUE;
     parentPartitioningObject = NULL;

#if defined(USE_PADRE)
     referenceCount      = getReferenceCountBase();
  // This builds a new PADRE_Distribution object with no other references to it
     distributionPointer = new PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (Number_Of_Processors);
#else
     Range Subcollection (0,Number_Of_Processors-1);
     Initialize_Internal_Partitioning_Type ( Subcollection );
#endif
   }

#if defined(USE_PADRE)
Internal_Partitioning_Type::Internal_Partitioning_Type ( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *X_Pointer )
   {
     isRootOfPartitionTree    = TRUE;
     parentPartitioningObject = NULL;

     referenceCount      = getReferenceCountBase();
     distributionPointer = X_Pointer;
     distributionPointer->incrementReferenceCount();
   }
#endif

void
Internal_Partitioning_Type::partitionAlongAxis ( int Axis, bool Partition_Axis, int GhostBoundaryWidth ) 
   {
  // This function changes the data internal to the partitioning object to reflect a
  // new specification for the distribution.  This function does not change the regestered
  // P++ arrays and must be used in connection with the member function applyPartition().
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::partitionAlongAxis (int,bool,int) \n");
#endif

#if defined(PPP)
#if defined(USE_PADRE)
     APP_ASSERT (distributionPointer != NULL);
     distributionPointer->DistributeAlongAxis(Axis,Partition_Axis,GhostBoundaryWidth);
#else
     if (Partition_Axis == TRUE)
          Distribution_String [Axis] = 'B';
       else
          Distribution_String [Axis] = '*';

     LocalGhostCellWidth [Axis] = GhostBoundaryWidth;
  // printf ("WARNING: Internal_Partitioning_Type::partitionAlongAxis (int,bool,int) GhostBoundaryWidth ignored \n");
  // APP_ABORT();
#endif
#endif
   }

void
Internal_Partitioning_Type::setParentPartitioningObject ( Internal_Partitioning_Type* Xptr )
   {
  // establish parent/child relationship between partitionings
  // to support reshape mechanism (and other future mechanisms)
  // Note that we allows the input pointeer to be NULL (which would 
  // imply that the the parent is the default partitioning!)
  // Specifically -- that the current node is not the root and that 
  // the parent is NULL implies that the parent is the default partitioning!
     isRootOfPartitionTree       = FALSE;
     parentPartitioningObject    = Xptr;

  // (11/17/2000) Added reference count increment
     printf ("Increment reference count in setParentPartitioningObject() \n");
     Xptr->incrementReferenceCount();

  // Just because it is a parent does not imply it is a root (of all the parents)!
  // if (Xptr != NULL)
  //      Xptr->isRootOfPartitionTree = TRUE;
   }

void
Internal_Partitioning_Type::applyPartition ()
   {
  // This function forces the repartitioning of the registered P++ array objects using the
  // data associated with this partitioning object (and the properties of the P++ arrays).
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::applyPartition () \n");
#endif

  // For now the semantics of this function is that it will currently not 
  // recopy arrays data to change the distributions.  At a later point this function will modify existing
  // arrays that are associated with it.

     printf ("Sorry not implemented, Internal_Partitioning_Type::applyPartition()! \n");

#if defined(PPP)
#if defined(USE_PADRE)
     printf ("ERROR in Internal_Partitioning_Type::applyPartition(): No cooresponding test in PADRE yet! \n");
     APP_ABORT();
#else
     int LengthOfDefaultdoubleArrayList = DefaultdoubleArrayList.getLength();
     int LengthOfDefaultfloatArrayList  = DefaultfloatArrayList.getLength();
     int LengthOfDefaultintArrayList    = DefaultintArrayList.getLength();

     if ( (LengthOfDefaultdoubleArrayList != 0) || (LengthOfDefaultfloatArrayList != 0) || (LengthOfDefaultintArrayList != 0) )
        {
          printf ("ERROR in Internal_Partitioning_Type::SpecifyDecompositionAxes(int) -- Default partition already in use! \n");
          printf ("Internal_Partitioning_Type::SpecifyDecompositionAxes() must be called before any arrays are assigned a defalut partitioning! ... \n");
       // printf ("Exiting ... \n");
       // APP_ABORT();
        }
     APP_ASSERT ( (LengthOfDefaultdoubleArrayList == 0) && (LengthOfDefaultfloatArrayList == 0) && (LengthOfDefaultintArrayList == 0) );
#endif
#endif

# if 0
// As long as there are no existing arrays already built we don't need to implement this function (I think).
#if defined(PPP)
     printf ("Sorry, Internal_Partitioning_Type::applyPartition () not implemented! \n");
     APP_ABORT();
#endif
#endif
   }

void
Internal_Partitioning_Type::SpecifyDecompositionAxes ( int Input_Number_Of_Dimensions_To_Partition )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > -1)
          printf ("Inside of Internal_Partitioning_Type::SpecifyDecompositionAxes (int=%d) \n",Input_Number_Of_Dimensions_To_Partition);
#endif

#if defined(PPP)
#if defined(USE_PADRE)
     APP_ASSERT (distributionPointer != NULL);

     int i = 0;
     for (i=0; i < Input_Number_Of_Dimensions_To_Partition; i++)
        {
          int localGhostCellWidth = distributionPointer->getGhostCellWidth(i);
          distributionPointer->DistributeAlongAxis(i,TRUE,localGhostCellWidth);
        }

     for (i=Input_Number_Of_Dimensions_To_Partition; i < MAX_ARRAY_DIMENSION; i++)
        {
          int localGhostCellWidth = distributionPointer->getGhostCellWidth(i);
          distributionPointer->DistributeAlongAxis(i,FALSE,localGhostCellWidth);
        }
#else
     int LengthOfdoubleArrayList = doubleArrayList.getLength();
     int LengthOffloatArrayList  = floatArrayList.getLength();
     int LengthOfintArrayList    = intArrayList.getLength();

  // (1/18/2001) Fixed to print warning and ignor attempt to modify the Partition object
     if ( (LengthOfdoubleArrayList == 0) && (LengthOffloatArrayList == 0) && (LengthOfintArrayList == 0) )
        {
          int i = 0;
          for (i=0; i < Input_Number_Of_Dimensions_To_Partition; i++)
             {
               partitionAlongAxis (i,TRUE,LocalGhostCellWidth[i]);
             }

          for (i=Input_Number_Of_Dimensions_To_Partition; i < MAX_ARRAY_DIMENSION; i++)
             {
               partitionAlongAxis (i,FALSE,LocalGhostCellWidth[i]);
             }

#if 0
  // Comment this out for now because the semantics of this function is that it will currently not 
  // recopy arrays data to change the distributions.  At a later point this function will modify existing
  // arrays that are associated with it.
#if 0
     applyPartition();
#else
     int LengthOfdoubleArrayList = doubleArrayList.getLength();
     int LengthOffloatArrayList  = floatArrayList.getLength();
     int LengthOfintArrayList    = intArrayList.getLength();

     if ( (LengthOfdoubleArrayList != 0) || (LengthOffloatArrayList != 0) || (LengthOfintArrayList != 0) )
        {
          printf ("ERROR in Internal_Partitioning_Type::SpecifyDecompositionAxes(int) -- partition already in use! \n");
          printf ("Internal_Partitioning_Type::SpecifyDecompositionAxes() must be called before any arrays are assigned a partitioning! ... \n");
       // printf ("Exiting ... \n");
       // APP_ABORT();
        }

  // (1/18/2001) Commented out to permit this to be treated as a warning
  // APP_ASSERT ( (LengthOfdoubleArrayList == 0) && (LengthOffloatArrayList == 0) && (LengthOfintArrayList == 0) );
#endif
#endif

        }
       else
        {
          printf ("WARNING: in Internal_Partitioning_Type::SpecifyDecompositionAxes(int) -- partition already in use! \n");
          printf ("Internal_Partitioning_Type::SpecifyDecompositionAxes() must be called before any arrays are assigned a partitioning! ... \n");
        }
#endif
#endif
   }

#if !defined(USE_PADRE)
void
Internal_Partitioning_Type::SpecifyDecompositionEmbeddingIntoVirtualProcessorSpace (
   DECOMP* BlockParti_Decomposition_Pointer , int Input_Starting_Processor , int Input_Ending_Processor )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::SpecifyDecompositionEmbeddingIntoVirtualProcessorSpace (%d,%d) \n",
               Input_Starting_Processor,Input_Ending_Processor);
#endif

#if defined(PPP)
  // Starting_Processor = Input_Starting_Processor;
  // Ending_Processor   = Input_Ending_Processor;

  // error checking
     APP_ASSERT( BlockParti_Decomposition_Pointer != NULL );
     APP_ASSERT( Communication_Manager::VirtualProcessorSpace != NULL );
     APP_ASSERT( Input_Starting_Processor >= 0 );
     APP_ASSERT( Input_Ending_Processor   >= 0 );
     APP_ASSERT( Input_Ending_Processor   < MAX_PROCESSORS );
     APP_ASSERT( Input_Starting_Processor <= Input_Ending_Processor );

  // printf ("BEFORE: Embed function! \n");
  // displayDECOMP (BlockParti_Decomposition_Pointer);

     embed ( BlockParti_Decomposition_Pointer , 
             Communication_Manager::VirtualProcessorSpace , 
             Input_Starting_Processor , Input_Ending_Processor );

  // printf ("AFTER: Embed function! \n");
  // displayDECOMP (BlockParti_Decomposition_Pointer);
#endif
   }
#endif

#if 0
// The KCC compiler reports that this function is not used (so comment it out for now).

inline void
Error_Checking_For_Fewer_Array_Elements_Than_Processors ( 
     int *Array_Sizes , int Number_Of_Dimensions_To_Partition ,
     int Starting_Processor , int Ending_Processor )
   {
#if defined(USE_PADRE)
     printf ("ERROR: This function should not be called if PADRE is being used! \n");
     APP_ABORT();
#else
     int Number_Of_Elements_To_Partition = 1;  // initialize with multiplicative identity
     for (int i=0; i < Number_Of_Dimensions_To_Partition; i++)
        {
          Number_Of_Elements_To_Partition *= Array_Sizes[i];
        }

  // We can't use PARTI to partition an array with fewer elements that processors (to bad)!
  // This problem has been reported to Alan Sussman (we will have to see if there is anything that can be done)
     int Number_Of_Processor_Overwhich_To_Partition = (Ending_Processor - Starting_Processor) + 1;
     if (Number_Of_Elements_To_Partition < Number_Of_Processor_Overwhich_To_Partition)
        {
          printf ("ERROR: Fewer array elements than processors (PARTI and/or P++ can't currently handle this case! \n");
          APP_ABORT();
        }
#endif // PADRE is not defined
   }
#endif

#if !defined(USE_PADRE)
inline bool
Is_Virtual_Processor_Space_Too_Large ()
   {
  // for the moment we return FALSE to simplify initial debugging
     return TRUE;
   }
#endif // PADRE is not defined

#if !defined(USE_PADRE)
DECOMP* Internal_Partitioning_Type::Build_BlockPartiDecompostion ( 
     int *Array_Sizes , char* Local_Distribution_String, int Local_Starting_Processor, int Local_Ending_Processor )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Build_BlockPartiDecomposition(int*,char*,int,int)! \n");
#endif

#if defined(PPP)
  // Resize the selected subset of the virtual processor space in the case where
  // the number of processors is greater than the number of elements in the array.
  // Steps:
  //     1. figure out the size of the largest dimension
  //     2. and the size of the prime factors of the Number of processors

     int Number_Of_Dimensions_To_Partition = Array_Domain_Type::computeArrayDimension ( Array_Sizes );
     bool Subset_Of_Virtual_Processor_Space_Too_Large = FALSE;
     DECOMP* BlockPartiArrayDecomposition = NULL;
  
     do {
       // do ... while not generating a case with fewer grid points along each
       // array dimension than the prime factors of the value representing the
       // number of processors.  Each iteration will reduce the range of the
       // virtual processor space.

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
             {
               printf ("Number_Of_Dimensions_To_Partition = %d \n",Number_Of_Dimensions_To_Partition);
               printf ("Subset_Of_Virtual_Processor_Space_Too_Large = %s \n",
                    (Subset_Of_Virtual_Processor_Space_Too_Large) ? "TRUE" : "FALSE");
               for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
                    printf ("Array_Sizes [%d] = %d \n",i,Array_Sizes[i]);
             }
#endif

       // Bugfix (9/17/95) for memory leak in case of more than a single pass through the while loop!
          if (BlockPartiArrayDecomposition != NULL)
             {
               delete_DECOMP (BlockPartiArrayDecomposition);
               BlockPartiArrayDecomposition = NULL;
             }

          BlockPartiArrayDecomposition  = create_decomp ( Number_Of_Dimensions_To_Partition, Array_Sizes );
          APP_ASSERT ( BlockPartiArrayDecomposition != NULL);
          APP_ASSERT ( BlockPartiArrayDecomposition->referenceCount == 0);

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
             {
               printf ("Call embedding function using the partitioining objects Starting_Processor and Ending_Processor \n");
            // displayDECOMP (BlockPartiArrayDecomposition);
             }
#endif
       // Call embedding function using the partitioining objects Starting_Processor and Ending_Processor
          SpecifyDecompositionEmbeddingIntoVirtualProcessorSpace (
               BlockPartiArrayDecomposition , Local_Starting_Processor , Local_Ending_Processor );

       // Must be called before the align function!
          distribute( BlockPartiArrayDecomposition, Local_Distribution_String );

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("After call to SpecifyDecompositionEmbeddingIntoVirtualProcessorSpace AND distribute \n");
               displayDECOMP (BlockPartiArrayDecomposition);
             }
#endif

          Subset_Of_Virtual_Processor_Space_Too_Large = FALSE;
          for (int i=0; i < BlockPartiArrayDecomposition->nDims; i++)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Axis = %d \n",i);
#endif
               if ( (Array_Sizes[i] < BlockPartiArrayDecomposition->dimProc[i]) &&
                    !Subset_Of_Virtual_Processor_Space_Too_Large )
                  {
                    Subset_Of_Virtual_Processor_Space_Too_Large = TRUE;
                    Local_Ending_Processor--;
                    if (Local_Ending_Processor < Local_Starting_Processor)
                       {
                         printf ("ERROR: Local_Ending_Processor < Local_Starting_Processor \n");
                         APP_ABORT();
                       }
                  }
               // ... (1/6/96,kdb) add a test to make sure this dimension
	       //  isn't partitioned because of too many processors ...
               if ( (BlockPartiArrayDecomposition->dimProc[i]>1) &&
                    (BlockPartiArrayDecomposition->dimDist[i] == '*'))
                  {
                    Subset_Of_Virtual_Processor_Space_Too_Large = TRUE;
                    Local_Ending_Processor--;
                    if (Local_Ending_Processor < Local_Starting_Processor)
                       {
                         printf ("ERROR: Local_Ending_Processor < Local_Starting_Processor \n");
                         APP_ABORT();
                       }
                  }
             }
        }
     while (Subset_Of_Virtual_Processor_Space_Too_Large);

  // Error checking (better checking required)
  // Error_Checking_For_Fewer_Array_Elements_Than_Processors ( Array_Sizes , Number_Of_Dimensions_To_Partition ,
  //                                                           Local_Starting_Processor , Local_Ending_Processor );

  // printf ("In Build_BlockPartiDecompostion -- Local_Starting_Processor = %d   Local_Ending_Processor = %d \n",
  //      Local_Starting_Processor , Local_Ending_Processor );

     return BlockPartiArrayDecomposition;
#else
  // The function must return a value to be properly defined outside of P++
     return new DECOMP;
#endif
   }
  // PADRE is not defined
#endif

#if !defined(USE_PADRE)
DECOMP*
Internal_Partitioning_Type::Build_BlockPartiDecompostion ( int *Array_Sizes )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Build_BlockPartiDecomposition(int*)! \n");
#endif

#if defined(PPP)
     APP_ASSERT( Communication_Manager::VirtualProcessorSpace != NULL );

     int i;
     for ( i=0; i<MAX_ARRAY_DIMENSION; i++ ) {
       APP_ASSERT(Array_Sizes[i] >= 1);
     }

  // printf ("In Build_BlockPartiDecompostion -- Starting_Processor = %d   Ending_Processor = %d \n",
  //      Starting_Processor , Ending_Processor );

     DECOMP* BlockPartiArrayDecomposition = Build_BlockPartiDecompostion (Array_Sizes,Distribution_String,
                                                                          Starting_Processor,Ending_Processor);

     return BlockPartiArrayDecomposition;
#else
  // The function must return a value to be properly defined outside of P++
     return new DECOMP;
#endif
   }
#endif // PADRE is not defined

#if !defined(USE_PADRE)
DARRAY*
Internal_Partitioning_Type::Build_BlockPartiArrayDomain ( 
   DECOMP* BlockPartiArrayDecomposition , int *Array_Sizes , 
   int* InternalGhostCellWidth , int* ExternalGhostCellWidth )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Build_BlockPartiArrayDomain(DECOMP*,int*,int*,int*)! \n");
#endif

#if defined(PPP)
     APP_ASSERT( Communication_Manager::VirtualProcessorSpace != NULL );
     APP_ASSERT ( BlockPartiArrayDecomposition != NULL);

  // display("*this BlockPartiArrayDecomposition");

     int Array_Dimension = Array_Domain_Type::computeArrayDimension ( Array_Sizes );
  // Make the Temp_Decomposition_Dimensions a permutation of the member Internal_Partitioning_Type::Decomposition_Dimensions
  // and the Input_Decomposition_Dimensions.

     DARRAY* Return_BlockPartiArrayDomain = align ( BlockPartiArrayDecomposition ,
                                                    Array_Dimension ,
                                                    ArrayDimensionsToAlign ,
                                                    Array_Sizes ,
                                                    InternalGhostCellWidth ,
                                                    ExternalGhostCellWidth ,
                                                    ExternalGhostCellWidth ,
                                                    PartitionControlFlags ,
                                                    Decomposition_Dimensions );

     APP_ASSERT(Return_BlockPartiArrayDomain != NULL);

  // displayDECOMP (BlockPartiArrayDecomposition);
  // displayDARRAY (Return_BlockPartiArrayDomain);

     return Return_BlockPartiArrayDomain;
#else
  // The function must return a value to be properly defined outside of P++
     return new DARRAY;
#endif
   }
#endif // PADRE is not defined

#if !defined(USE_PADRE)
DARRAY*
Internal_Partitioning_Type::Build_BlockPartiArrayDomain ( 
   int *Array_Sizes , int* InternalGhostCellWidth , int* ExternalGhostCellWidth )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Build_BlockPartiArrayDomain(int*,int*,int*)! \n");
#endif

     DECOMP* BlockPartiArrayDecomposition = Build_BlockPartiDecompostion ( Array_Sizes );
     APP_ASSERT ( BlockPartiArrayDecomposition != NULL);

     return Build_BlockPartiArrayDomain ( BlockPartiArrayDecomposition , Array_Sizes , 
                 InternalGhostCellWidth , ExternalGhostCellWidth );
   }
#endif // PADRE is not defined

void
Internal_Partitioning_Type::setDefaultProcessorRange ( const Range & Processor_Range )
   {
  // This function can only be called if no array objects have been already been built
  // using the default partitioning.  This is because of the complexity of changing the 
  // array objects that would have already been built.  Use explicit partition objects if this
  // is a serious problem.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::setDefaultStarting_Processor (int) \n");
#endif

#if defined(PPP)
#if defined(USE_PADRE)
     int startingProcessor = Processor_Range.getBase();
     int endingProcessor   = Processor_Range.getBound();
     PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::setDefaultProcessorRange(startingProcessor,endingProcessor);
#else
  // Check to make sure that there are no array objects in use with the 
  // default partition.  Specifically we have to check to lengths of the
  // Default lists to make sure they are all length ZERO.

     int LengthOfDefaultdoubleArrayList = DefaultdoubleArrayList.getLength();
     int LengthOfDefaultfloatArrayList  = DefaultfloatArrayList.getLength();
     int LengthOfDefaultintArrayList    = DefaultintArrayList.getLength();

     if ( (LengthOfDefaultdoubleArrayList != 0) || (LengthOfDefaultfloatArrayList != 0) || (LengthOfDefaultintArrayList != 0) )
        {
          printf ("ERROR in Internal_Partitioning_Type::setDefaultProcessorRange (const Range) -- Default partition already in use! \n");
          printf ("Internal_Partitioning_Type::setDefaultProcessorRange() must be called before any arrays are assigned a defalut partitioning! ... \n");
       // printf ("Exiting ... \n");
       // APP_ABORT();
        }
     APP_ASSERT ( (LengthOfDefaultdoubleArrayList == 0) && (LengthOfDefaultfloatArrayList == 0) && (LengthOfDefaultintArrayList == 0) );

     DefaultStarting_Processor = Processor_Range.getBase();
     DefaultEnding_Processor   = Processor_Range.getBound();
#endif
#endif
   }

#if !defined(USE_PADRE)
DECOMP*
Internal_Partitioning_Type::Build_DefaultBlockPartiDecompostion ( int *Array_Sizes )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Build_DefaultBlockPartiDecomposition(int*)! \n");
#endif

#if defined(PPP)
     APP_ASSERT( Communication_Manager::VirtualProcessorSpace != NULL );

  // Not initialize properly as static members (since Communication_Manager::Number_Of_Processors 
  // is too late (I think)).
  // Internal_Partitioning_Type::DefaultStarting_Processor = 0;
  // Internal_Partitioning_Type::DefaultEnding_Processor   = Communication_Manager::Number_Of_Processors - 1;
  // DefaultStarting_Processor = 0;
  // DefaultEnding_Processor   = Communication_Manager::Number_Of_Processors - 1;

     DECOMP* BlockPartiArrayDecomposition = Build_BlockPartiDecompostion (
          Array_Sizes,DefaultDistribution_String,DefaultStarting_Processor,DefaultEnding_Processor);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In Build_DefaultBlockPartiDecompostion --- BlockPartiArrayDecomposition->referenceCount = %d \n",
               BlockPartiArrayDecomposition->referenceCount);
          printf ("DefaultStarting_Processor = %d  DefaultEnding_Processor = %d  Communication_Manager::Number_Of_Processors = %d \n",
               DefaultStarting_Processor,DefaultEnding_Processor,Communication_Manager::Number_Of_Processors);
        }
#endif

     APP_ASSERT ( BlockPartiArrayDecomposition->referenceCount == 0 );
     APP_ASSERT ( DefaultStarting_Processor >= 0 );
     APP_ASSERT ( DefaultEnding_Processor <= Communication_Manager::Number_Of_Processors - 1 );
     APP_ASSERT ( DefaultStarting_Processor <= DefaultEnding_Processor );

     return BlockPartiArrayDecomposition;
#else
  // The function must return a value to be properly defined outside of P++
     return new DECOMP;
#endif
   }
  // PADRE is not defined
#endif

#if !defined(USE_PADRE)
DARRAY*
Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain ( 
   DECOMP* BlockPartiArrayDecomposition , 
   int* Array_Sizes , const int* InternalGhostCellWidth , 
   const int* ExternalGhostCellWidth )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain(DECOMP*,int*,int*,int*)! \n");
#endif

#if defined(PPP)
     APP_ASSERT( Communication_Manager::VirtualProcessorSpace != NULL );
     APP_ASSERT ( BlockPartiArrayDecomposition != NULL);

     int i;
     for ( i=0; i<MAX_ARRAY_DIMENSION; i++ ) {
       APP_ASSERT (InternalGhostCellWidth[i] >= 0);
     }

  // Temp code assuming ghost boundary width is <= 10 (remove later)
     for ( i=0; i<MAX_ARRAY_DIMENSION; i++ ) {
       APP_ASSERT (InternalGhostCellWidth[i] <= 10);
     }

  // Temp code assuming ghost boundary width is == 0 (remove later)
     for ( i=0; i<MAX_ARRAY_DIMENSION; i++ ) {
       APP_ASSERT (ExternalGhostCellWidth[i] == 0);
     }

     int Number_Of_Dimensions_To_Partition = Array_Domain_Type::computeArrayDimension (Array_Sizes);

     APP_ASSERT ( BlockPartiArrayDecomposition->referenceCount == 0);
     DARRAY* Return_BlockPartiArrayDomain = 
	align ( BlockPartiArrayDecomposition ,
                Number_Of_Dimensions_To_Partition ,
                DefaultArrayDimensionsToAlign ,
                Array_Sizes ,
                (int*)InternalGhostCellWidth ,
                (int*)ExternalGhostCellWidth ,
                (int*)ExternalGhostCellWidth ,
                DefaultPartitionControlFlags ,
                DefaultDecomposition_Dimensions );

     APP_ASSERT(Return_BlockPartiArrayDomain != NULL);
     APP_ASSERT ( BlockPartiArrayDecomposition->referenceCount == 1);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          displayDARRAY ( Return_BlockPartiArrayDomain );
#endif

     return Return_BlockPartiArrayDomain;
#else
  // The function must return a value to be properly defined outside of P++
     return new DARRAY;
#endif
   }
#endif // PADRE is not defined

#if !defined(USE_PADRE)
DARRAY*
Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain ( int *Array_Sizes , int* InternalGhostCellWidth , int* ExternalGhostCellWidth )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain(int*,int*,int*)! \n");
#endif

#if defined(PPP)
     DECOMP* BlockPartiArrayDecomposition = Build_DefaultBlockPartiDecompostion ( Array_Sizes );
     APP_ASSERT ( BlockPartiArrayDecomposition != NULL);

     DARRAY* Return_DARRAY = Build_DefaultBlockPartiArrayDomain 
                                ( BlockPartiArrayDecomposition , Array_Sizes , 
                                  InternalGhostCellWidth , ExternalGhostCellWidth );
     APP_ASSERT (Return_DARRAY != NULL);

  // The use of a DECOMP within a DARRAY increments its reference count so we have to
  // delete the additional (initial) reference to the BlockPartiArrayDecomposition structure.
     delete_DECOMP (BlockPartiArrayDecomposition);

     return Return_DARRAY;
#else
     DARRAY* Return_DARRAY = NULL;
     return Return_DARRAY;
#endif
   }
#endif // PADRE is not defined

void
Internal_Partitioning_Type::SpecifyExternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0) {
          printf ("Inside of Internal_Partitioning_Type::SpecifyExternalGhostBoundaryWidths(");
          printf (IO_CONTROL_STRING_MACRO_INTEGER, VARIABLE_LIST_MACRO_INTEGER );
          printf (")! \n");
     }
#endif

     printf ("Sorry, not implemented -- Internal_Partitioning_Type::SpecifyExternalGhostBoundaryWidths \n");
     APP_ABORT();
   }

void
Internal_Partitioning_Type::SpecifyDefaultInternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER )
   {
#if defined(PPP)
     INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO;
     int temp;
#if defined(USE_PADRE)
     for ( temp=0; temp<MAX_ARRAY_DIMENSION; temp++ ) {
       PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
	 setDefaultGhostCellWidth (temp,Integer_List[temp]);
     }
#else
     for ( temp=0; temp<MAX_ARRAY_DIMENSION; temp++ ) {
       DefaultInternalGhostCellWidth [temp] = Integer_List[temp];
     }
#endif
#endif
   }

void
Internal_Partitioning_Type::SpecifyInternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER )
   {
  // The number of ghost boundaries is required to be the same on the left and right (forward back,
  // or top bottom) along each axis.  This is reasonable!  Default dimensions are set to ZERO.
  // This function is forced to copy the original data into temporary storage and then deallocate the
  // serial array do allocate a larger one with ghost boundaries and then copy the data back!

  // Note that this function will change the existing ghost boundary sizes in all the P++ arrays 
  // using this Partition_Type object.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Internal_Partitioning_Type::SpecifyInternalGhostBoundaryWidths(");
	  printf( IO_CONTROL_STRING_MACRO_INTEGER, VARIABLE_LIST_MACRO_INTEGER );
	  printf( ")\n");
        }
#endif

#if defined(PPP)
     int temp = 0;

#if defined(USE_PADRE)
  // This warning might apply to a function that does not yet exist in PADRE which would
  // permit the arrays associated with an existing distribution to vary their ghost boundary widths
  // it might be that the correct fix is to add a function to PADRE similar to what is called for NON-PADRE!
     printf ("Warning: this function does not change the widths of existing arrays using this distribution (feature is not implemented yet)! \n");

  // something like:
  // PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::setGhostCellWidth( Integer_List );

     INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO
     for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          printf ("Adding Ghost boundaries to PADRE arrays: \n");
          distributionPointer->setGhostCellWidth( temp, Integer_List[temp] );
        }
#else
  // Bugfix (12/30/2000) Set the ghost boundary widths to the input values
     INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO
     for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
          LocalGhostCellWidth[temp] = Integer_List[temp];

     for (temp=0; temp < doubleArrayList.getLength(); temp++)
        {
          printf ("Adding Ghost boundaries to doubleArray's (Array_ID=%d) (%d of %d in list) \n", 
               doubleArrayList[temp].Array_ID(), temp+1, doubleArrayList.getLength() );

          doubleArrayList[temp].setInternalGhostCellWidth ( VARIABLE_LIST_MACRO_INTEGER );
        }

     for (temp=0; temp < floatArrayList.getLength(); temp++)
        {
          printf ("Adding Ghost boundaries to floatArray's (Array_ID=%d) (%d of %d in list) \n", 
               floatArrayList[temp].Array_ID(), temp+1, floatArrayList.getLength() );

          floatArrayList[temp].setInternalGhostCellWidth ( VARIABLE_LIST_MACRO_INTEGER );
        }

     for (temp=0; temp < intArrayList.getLength(); temp++)
        {
          printf ("Adding Ghost boundaries to intArray's (Array_ID=%d) (%d of %d in list) \n", 
               intArrayList[temp].Array_ID(), temp+1, intArrayList.getLength() );

          intArrayList[temp].setInternalGhostCellWidth ( VARIABLE_LIST_MACRO_INTEGER );
        }

  // printf ("Sorry, not implemented: Internal_Partitioning_Type::SpecifyGhostBoundaryWidths \n");
  // APP_ABORT();
#endif
#endif
   }

void
Internal_Partitioning_Type::display ( const char *Label ) const
   {
  // printf ("************************************************ \n");
     printf ("Internal_Partitioning_Type::display() (CONST) -- %s \n",Label);

#if defined(PPP)
#if defined(USE_PADRE)
     APP_ASSERT (distributionPointer != NULL);
     distributionPointer->display(Label);
#else
  // printf ("Number_Of_Dimensions_To_Partition = %d \n", Number_Of_Dimensions_To_Partition);

     printf ("Starting_Processor = %d \n",Starting_Processor);
     printf ("Ending_Processor   = %d \n",Ending_Processor);
     printf ("referenceCount     = %d \n",referenceCount);

     int i = 0;
     printf ("Distribution_String      = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %c ",Distribution_String[i]);
     printf ("\n");

     printf ("LocalGhostCellWidth      = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",LocalGhostCellWidth[i]);
     printf ("\n");

     printf ("ArrayDimensionsToAlign   = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",ArrayDimensionsToAlign[i]);
     printf ("\n");

     printf ("PartitionControlFlags    = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",PartitionControlFlags[i]);
     printf ("\n");

     printf ("Decomposition_Dimensions = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",Decomposition_Dimensions[i]);
     printf ("\n");

#if 0
     printf ("doubleArrayList[0 - %d].Array_ID() = ",doubleArrayList.getLength()-1);
     for (i=0; i < doubleArrayList.getLength(); i++)
          printf (" %d", doubleArrayList[i].Array_ID());
     printf ("\n");

     printf ("floatArrayList[0 - %d].Array_ID() = ",floatArrayList.getLength()-1);
     for (i=0; i < floatArrayList.getLength(); i++)
          printf (" %d", floatArrayList[i].Array_ID());
     printf ("\n");

     printf ("intArrayList[0 - %d].Array_ID() = ",intArrayList.getLength()-1);
     for (i=0; i < intArrayList.getLength(); i++)
          printf (" %d", intArrayList[i].Array_ID());
     printf ("\n");
#endif

  // Do not display the other partitioning objects in the list of 
  // partition objects that represent smaller arrays which are forced to
  // be distributed over smaller subsets of processors.
     if (PartitioningObjectList.getLength() > 0)
        {
          printf ("(For smaller arrays) PartitioningObjectList[i].Array_ID() = ");
          for (i=0; i < PartitioningObjectList.getLength(); i++)
               PartitioningObjectList[i].display("PartitioningObjectList");
          printf ("\n");
          printf ("************************************************ \n");
        }
       else
        {
          printf ("There are no arrays associated with this PartitioningObjectList object! \n");
          APP_ASSERT (PartitioningObjectList.getLength() == 0);
        }

     if (doubleArrayList.getLength() > 0)
        {
          printf ("doubleArrayList[i].Array_ID() = ");
          for (i=0; i < doubleArrayList.getLength(); i++)
               printf (" %d",doubleArrayList[i].Array_ID());
            // doubleArrayList[i].display("doubleArrayList");
          printf ("\n");
        }
       else
        {
          printf ("There are no arrays associated with this doubleArrayList object! \n");
          APP_ASSERT (doubleArrayList.getLength() == 0);
        }

     if (floatArrayList.getLength() > 0)
        {
          printf ("floatArrayList[i].Array_ID() = ");
          for (i=0; i < floatArrayList.getLength(); i++)
               printf (" %d",floatArrayList[i].Array_ID());
            // floatArrayList[i].display("floatArrayList");
          printf ("\n");
        }
       else
        {
          printf ("There are no arrays associated with this floatArrayList object! \n");
          APP_ASSERT (floatArrayList.getLength() == 0);
        }

     if (intArrayList.getLength() > 0)
        {
          printf ("intArrayList[i].Array_ID() = ");
          for (i=0; i < intArrayList.getLength(); i++)
               printf (" %d",intArrayList[i].Array_ID());
            // intArrayList[i].display("intArrayList");
          printf ("\n");
        }
       else
        {
          printf ("There are no arrays associated with this intArrayList object! \n");
          APP_ASSERT (intArrayList.getLength() == 0);
        }
#endif
#endif
   }

void
Internal_Partitioning_Type::displayDefaultValues ( const char *Label )
   {
     printf ("************************************************ \n");
     printf ("PARTITIONING_TYPE::displayDefaultValues() -- %s \n",Label);

#if defined(PPP)
#if defined(USE_PADRE)
  // We need a static function to call so PADRE_Distribution should have a display function.
     PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::displayDefaultValues(Label);
#else
  // printf ("Partitioning_Type::display() not implemented yet! \n");

  // printf ("DefaultNumber_Of_Dimensions_To_Partition = %d \n", DefaultNumber_Of_Dimensions_To_Partition);

     int i = 0;
     printf ("DefaultDistribution_String         = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %c ",DefaultDistribution_String[i]);
     printf ("\n");

     printf ("DefaultArrayDimensionsToAlign      = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",DefaultArrayDimensionsToAlign[i]);
     printf ("\n");

     printf ("DefaultInternalGhostCellWidth      = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",DefaultInternalGhostCellWidth[i]);
     printf ("\n");

     printf ("DefaultExternalGhostCellWidth      = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",DefaultExternalGhostCellWidth[i]);
     printf ("\n");

  // printf ("DefaultExternalGhostCellArrayRight = ");
  // for (i=0; i < MAX_ARRAY_DIMENSION; i++)
  //      printf (" %d ",DefaultExternalGhostCellArrayRight[i]);
  // printf ("\n");

     printf ("DefaultPartitionControlFlags       = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",DefaultPartitionControlFlags[i]);
     printf ("\n");

     printf ("DefaultDecomposition_Dimensions    = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",DefaultDecomposition_Dimensions[i]);
     printf ("\n");

  // Not display the other partitioning objects in the list of
  // partition objects that represent smaller arrays whic are forced to
  // be distributed over smaller subsets of processors.
     if (DefaultPartitioningObjectList.getLength() > 0)
        {
          printf ("DefaultPartitioningObjectList[i] is of length %d \n",DefaultPartitioningObjectList.getLength());
       // printf ("DefaultPartitioningObjectList[i].Array_ID() = ");
       // for (i=0; i < DefaultPartitioningObjectList.getLength(); i++)
       //      DefaultPartitioningObjectList[i].display("DefaultPartitioningObjectList");
       // printf ("\n");
        }
       else
        {
          printf ("There are no arrays associated with this partitioning object! \n");
          APP_ASSERT (DefaultPartitioningObjectList.getLength() == 0);
        }

      if (DefaultdoubleArrayList.getLength() > 0)
        {
          printf ("DefaultdoubleArrayList[i].Array_ID() = ");
          for (i=0; i < DefaultdoubleArrayList.getLength(); i++)
               printf (" %d",DefaultdoubleArrayList[i].Array_ID());
            // DefaultdoubleArrayList[i].display("DefaultdoubleArrayList");
          printf ("\n");
        }
       else
        {
          printf ("There are no arrays associated with this DefaultdoubleArrayList object! \n");
          APP_ASSERT (DefaultdoubleArrayList.getLength() == 0);
        }

      if (DefaultfloatArrayList.getLength() > 0)
        {
          printf ("DefaultfloatArrayList[i].Array_ID() = ");
          for (i=0; i < DefaultfloatArrayList.getLength(); i++)
               printf (" %d",DefaultfloatArrayList[i].Array_ID());
            // DefaultfloatArrayList[i].display("DefaultfloatArrayList");
          printf ("\n");
        }
       else
        {
          printf ("There are no arrays associated with this DefaultfloatArrayList object! \n");
          APP_ASSERT (DefaultfloatArrayList.getLength() == 0);
        }

      if (DefaultintArrayList.getLength() > 0)
        {
          printf ("DefaultintArrayList[i].Array_ID() = ");
          for (i=0; i < DefaultintArrayList.getLength(); i++)
               printf (" %d",DefaultintArrayList[i].Array_ID());
            // DefaultintArrayList[i].display("DefaultintArrayList");
          printf ("\n");
        }
       else
        {
          printf ("There are no arrays associated with this DefaultintArrayList object! \n");
          APP_ASSERT (DefaultintArrayList.getLength() == 0);
        }
#endif
#endif
     printf ("************************************************ \n");
   }

#if !defined(USE_PADRE)
void
Internal_Partitioning_Type::testPartiConsistency( DARRAY *BlockPartiArrayDomain , const char *Label )
   {
  // Static error checking (for static data)
  // Build base and size values to build SerialArray

  // Only used when EXTRA_ERROR_CHECKING is TRUE
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since Internal_Partitioning_Type::testPartiConsistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::testPartiConsistency (Label = %s) \n",Label);
#endif

#if defined(PPP)
     int i = 0;
     int Max_Partition_Dimension = BlockPartiArrayDomain->nDims;
     int Local_Sizes[MAX_ARRAY_DIMENSION];
     APP_ASSERT(BlockPartiArrayDomain != NULL);
     laSizes(BlockPartiArrayDomain,Local_Sizes);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Local SerialArray size:");
          for (i=0; i < Max_Partition_Dimension; i++)
               printf (" %d",Local_Sizes[i]);
          printf ("\n");
        }
#endif

     int Global_Lower_Bound[MAX_ARRAY_DIMENSION];
     for (i=0; i < Max_Partition_Dimension; i++)
          Global_Lower_Bound[i] = gLBnd(BlockPartiArrayDomain,i);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Global_Lower_Bound[0-%d]:",Max_Partition_Dimension-1);
          for (i=0; i < Max_Partition_Dimension; i++)
               printf (" %d",Global_Lower_Bound[i]);
          printf ("\n");
        }
#endif

     int Global_Upper_Bound[MAX_ARRAY_DIMENSION];
     for (i=0; i < Max_Partition_Dimension; i++)
          Global_Upper_Bound[i] = gUBnd(BlockPartiArrayDomain,i);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Global_Upper_Bound[0-%d]:",Max_Partition_Dimension-1);
          for (i=0; i < Max_Partition_Dimension; i++)
               printf (" %d",Global_Upper_Bound[i]);
          printf ("\n");
        }
#endif

     int Global_To_Local_Base[MAX_ARRAY_DIMENSION];
     for (i=0; i < Max_Partition_Dimension; i++)
          Global_To_Local_Base[i] = globalToLocal(BlockPartiArrayDomain,Global_Lower_Bound[i],i);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Global_To_Local_Base[0-%d]:",Max_Partition_Dimension-1);
          for (i=0; i < Max_Partition_Dimension; i++)
               printf (" %d",Global_To_Local_Base[i]);
          printf ("\n");
        }
#endif

     int Global_To_Local_With_Ghost_Base[MAX_ARRAY_DIMENSION];
     for (i=0; i < Max_Partition_Dimension; i++)
          Global_To_Local_With_Ghost_Base[i] = globalToLocalWithGhost(BlockPartiArrayDomain,Global_Lower_Bound[i],i);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Global_To_Local_With_Ghost_Base[0-%d]:",Max_Partition_Dimension-1);
          for (i=0; i < Max_Partition_Dimension; i++)
               printf (" %d",Global_To_Local_With_Ghost_Base[i]);
          printf ("\n");
        }
#endif

  // Test the convertion of base functions in PARTI!
     for (i=0; i < Max_Partition_Dimension; i++)
        {
          if ( (Global_Lower_Bound[i] != -1) && (localToGlobal(BlockPartiArrayDomain,Global_To_Local_Base[i],i) != -1) )
               APP_ASSERT( localToGlobal(BlockPartiArrayDomain,Global_To_Local_Base[i],i) == Global_Lower_Bound[i]);
        }

     for (i=0; i < Max_Partition_Dimension; i++)
        {
          if (Global_To_Local_With_Ghost_Base[i] != -1)
               APP_ASSERT( localToGlobalWithGhost(BlockPartiArrayDomain,Global_To_Local_With_Ghost_Base[i],i) == Global_Lower_Bound[i] );
        }

  // P++ not defined
#endif
   }
#endif


#if !defined(USE_PADRE)
void
Internal_Partitioning_Type::Test_Consistency( DARRAY *BlockPartiArrayDomain , const char *Label )
   {
  // Build base and size values to build SerialArray

  // Only used when EXTRA_ERROR_CHECKING is TRUE
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since Internal_Partitioning_Type::Test_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::Test_Consistency (Label = %s) \n",Label);
#endif

#if defined(PPP)
     testPartiConsistency (BlockPartiArrayDomain,Label);

  // Reference counting error checking
     int intArrayListLength    = intArrayList.getLength();
     int floatArrayListLength  = floatArrayList.getLength();
     int doubleArrayListLength = doubleArrayList.getLength();
     int totalListLength = intArrayListLength + floatArrayListLength + doubleArrayListLength;

     APP_ASSERT (Internal_Partitioning_Type::getReferenceCountBase() == 1);

#if 0
     if (totalListLength + Internal_Partitioning_Type::getReferenceCountBase() != referenceCount)
        {
          printf ("referenceCount              = %d \n",referenceCount);
          printf ("intArrayList.getLength()    = %d \n",intArrayListLength);
          printf ("floatArrayList.getLength()  = %d \n",floatArrayListLength);
          printf ("doubleArrayList.getLength() = %d \n",doubleArrayListLength);
        }

  // Verify the number of outstanding references
  // We can not use this as a test since the number of references to a partitioning object can be 
  // greater than the number of references to the made by array objects under special circumstances
  // (in reshape for example!).
  // APP_ASSERT (totalListLength + Internal_Partitioning_Type::getReferenceCountBase() == referenceCount);
#endif

  // If we are not a root partitioning then we must have a parent by definition!
     if (isRootOfPartitionTree == FALSE)
        {
          APP_ASSERT (parentPartitioningObject != NULL);
        }

  // P++ not defined
#endif
   }

void
Internal_Partitioning_Type::staticTestConsistency( DARRAY *BlockPartiArrayDomain , const char *Label )
   {
  // Static error checking (for static data)
  // Build base and size values to build SerialArray

  // Only used when EXTRA_ERROR_CHECKING is TRUE
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since Internal_Partitioning_Type::staticTestConsistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::staticTestConsistency (Label = %s) \n",Label);
#endif

#if defined(PPP)
#if defined(USE_PADRE)
     printf ("ERROR: This function should not be called if PADRE is being used! \n");
     APP_ABORT();
#else
     testPartiConsistency (BlockPartiArrayDomain,Label);
  // PADRE not defined
#endif

  // Cannot reference the referenceCount in this static member function
  // I can not currently think of a good test for this case!
#if 0
  // Reference counting error checking
     int intArrayListLength    = DefaultintArrayList.getLength();
     int floatArrayListLength  = DefaultfloatArrayList.getLength();
     int doubleArrayListLength = DefaultdoubleArrayList.getLength();
     int totalListLength = intArrayListLength + floatArrayListLength + doubleArrayListLength;

     APP_ASSERT (Internal_Partitioning_Type::getReferenceCountBase() == 1);

     if (totalListLength + Internal_Partitioning_Type::getReferenceCountBase() != referenceCount)
        {
          printf ("referenceCount              = %d \n",referenceCount);
          printf ("intArrayList.getLength()    = %d \n",intArrayListLength);
          printf ("floatArrayList.getLength()  = %d \n",floatArrayListLength);
          printf ("doubleArrayList.getLength() = %d \n",doubleArrayListLength);
        }

  // Verify the number of outstanding references
     APP_ASSERT (totalListLength + Internal_Partitioning_Type::getReferenceCountBase() == referenceCount);

  // If we are not a root partitioning then we must have a parent by definition!
     if (isRootOfPartitionTree == FALSE)
        {
          APP_ASSERT (parentPartitioningObject != NULL);
        }
#endif

  // P++ not defined
#endif
   }

// USE_PADRE not defined
#endif

#if 1
// We certainly don't want silly functions in A++/P++
bool Internal_Partitioning_Type::Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & X_Domain )
   {
  // We might not even want this silly function!

   if (&X_Domain);  // avoid compiler warning

#if defined(USE_PADRE)
     return TRUE;
#else
     return TRUE;
#endif
   }
#endif

bool Internal_Partitioning_Type::Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & Lhs_Domain,
                                                                     const Array_Domain_Type & Rhs_Domain )
   {
     bool Return_Has_Same_Ghost_Boundary_Widths = TRUE;

#if defined(PPP)
#if defined(USE_PADRE)
  // APP_ASSERT (distributionPointer != NULL);
     Return_Has_Same_Ghost_Boundary_Widths = 
          PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
               Has_Same_Ghost_Boundary_Widths (Lhs_Domain,Rhs_Domain);
#else
  // PADRE not defined
     int temp;
     for ( temp=0; temp < MAX_ARRAY_DIMENSION; temp++ )
        {
       // This works (using the &= operator) and is cute but not as clear as an explicit conditional
       // Return_Has_Same_Ghost_Boundary_Widths &= (Lhs_Domain.InternalGhostCellWidth[temp] == Rhs_Domain.InternalGhostCellWidth[temp]);
          if (Lhs_Domain.InternalGhostCellWidth[temp] != Rhs_Domain.InternalGhostCellWidth[temp])
               Return_Has_Same_Ghost_Boundary_Widths = FALSE;
        }

#if COMPILE_DEBUG_STATEMENTS
  // Temp error checking
     if (!Return_Has_Same_Ghost_Boundary_Widths)
        {
          printf ("Internal Ghost Boundary widths: Lhs (");
          for ( temp=0; temp < MAX_ARRAY_DIMENSION; temp++ )
            {
	      printf(" %d", Lhs_Domain.InternalGhostCellWidth[temp] );
            }
          printf (") Rhs (");
          for ( temp=0; temp < MAX_ARRAY_DIMENSION; temp++ )
             {
	       printf(" %d", Rhs_Domain.InternalGhostCellWidth[temp] );
             }
          printf (")\n");
        }
#endif
  // endif for NOT using PADRE
#endif
  // endif for using P++
#endif

     return Return_Has_Same_Ghost_Boundary_Widths;
   }

bool
Internal_Partitioning_Type::Has_Same_Ghost_Boundary_Widths (
     const Array_Domain_Type & This_Domain,
     const Array_Domain_Type & Lhs_Domain,
     const Array_Domain_Type & Rhs_Domain )
   {
#if defined(PPP)
#if defined(USE_PADRE)
  // APP_ASSERT (distributionPointer != NULL);
     return PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
                 Has_Same_Ghost_Boundary_Widths (This_Domain,Lhs_Domain,Rhs_Domain);
#else
     int temp;
     bool Temp_1_Has_Same_Ghost_Boundary_Widths = FALSE;
     for ( temp=0; temp < MAX_ARRAY_DIMENSION; temp++ )
        {
          Temp_1_Has_Same_Ghost_Boundary_Widths &=
               (This_Domain.InternalGhostCellWidth[temp] == Rhs_Domain.InternalGhostCellWidth[temp]);
        }
     bool Temp_2_Has_Same_Ghost_Boundary_Widths = FALSE;
     for ( temp=0; temp < MAX_ARRAY_DIMENSION; temp++ )
        {
          Temp_2_Has_Same_Ghost_Boundary_Widths &=
	       (Lhs_Domain.InternalGhostCellWidth[temp] == Rhs_Domain.InternalGhostCellWidth[temp]);
        }
     return Temp_1_Has_Same_Ghost_Boundary_Widths && Temp_2_Has_Same_Ghost_Boundary_Widths;
  // PADRE not defined
#endif
#else
     return TRUE;
  // P++ not defined
#endif
   }


#if !defined(USE_PADRE)
SCHED*
Internal_Partitioning_Type::BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Domain_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::BuildCommunicationScheduleUpdateAllGhostBoundaries! \n");
#endif

#if defined(PPP)
     if (X.isNullArray() == TRUE) return NULL;

  // int Number_Of_Dimensions = X.Domain_Dimension;
     APP_ASSERT (X.Domain_Dimension == Array_Domain_Type::computeArrayDimension (X));

#if 1
  // ... (12/11/96,kdb) if X.Array_Conformability_Info isn't null use the
  //  update info to only update the needed ghost cell boundaries ...
     SCHED* Update_All_Ghost_Boundaries = NULL;
// ... (12/12/96,kdb) block parti doesn't allow only one side of ghost
//  boundaries to be updated so turn this off for now and put temporary fix
//  in cases.c that forces an ghost cell update every where if the
//  parallel array size is different or the partitioning is different ...
#if 0
     if (X.Array_Conformability_Info != NULL)
     {

  // ... (12/11/96, kdb) make a fill vec for both the low end and the high
  //  end and compute 2 schedules that will be merged.  Do this so only
  //  the ghost cells that need to be updated are updated. FillVec contains
  //  -ghostCellWidth on the low end and ghostCellWidth on the high end
  //  if the ghost cells should be updated, otherwise it is set to 0. ...

     int fillVecLo[MAX_ARRAY_DIMENSION];
     int fillVecHi[MAX_ARRAY_DIMENSION];
     int Axis;
     for (Axis=0; Axis < MAX_ARRAY_DIMENSION; Axis++)
     {
	fillVecLo[Axis] = X.Array_Conformability_Info->
	   Update_Left_Ghost_Boundary_Width[Axis]?
           -X.InternalGhostCellWidth[Axis]:0;
	fillVecHi[Axis] = X.Array_Conformability_Info->
	   Update_Right_Ghost_Boundary_Width[Axis]?
           X.InternalGhostCellWidth[Axis]:0;
     }

     APP_ASSERT (X.BlockPartiArrayDomain != NULL);
     SCHED* Update_All_Ghost_Boundaries_Lo = 
	ghostFillSpanSched (X.BlockPartiArrayDomain, X.Domain_Dimension, 
			    fillVecLo);
     SCHED* Update_All_Ghost_Boundaries_Hi = 
	ghostFillSpanSched (X.BlockPartiArrayDomain, X.Domain_Dimension, 
			    fillVecHi);
     
     // ... now merge these two schedules before returning ...
     Update_All_Ghost_Boundaries = new SCHED();
     init_Sched (Update_All_Ghost_Boundaries);

     merge_sched (Update_All_Ghost_Boundaries, Update_All_Ghost_Boundaries_Lo);
     merge_sched (Update_All_Ghost_Boundaries, Update_All_Ghost_Boundaries_Hi);
     }
     else
     {
#endif
	// ... the Array_Conformability_Info is null so update all ghost
	//  boundaries ...


  // Parti name used here for consistency with the documentation
  // int fillVec[MAX_ARRAY_DIMENSION];
  // for (int Axis=0; Axis < MAX_ARRAY_DIMENSION; Axis++)
  //    {
  //      fillVec[Axis] = (X.Partitioning_Object_Pointer == NULL) ?
  //                       Internal_Partitioning_Type::DefaultGhostCellWidth[Axis] :
  //                       X.InternalGhostCellWidth[Axis];
  //      printf ("fillVec[%d] = %d \n",Axis,fillVec[Axis]);
  //    }
     APP_ASSERT (X.BlockPartiArrayDomain != NULL);
  // displayDARRAY ( X.BlockPartiArrayDomain );

     Update_All_Ghost_Boundaries = ghostFillAllSched( X.BlockPartiArrayDomain );
  // SCHED* Update_All_Ghost_Boundaries = ghostFillSpanSched( X.BlockPartiArrayDomain );
  //                                                          Number_Of_Dimensions, fillVec);

  // If data is not present on the local processor then the ghostFillAllSched will return a NULL
  // pointer (instead of a valid SCHED that does nothing -- which I would prefer)
  // APP_ASSERT(Update_All_Ghost_Boundaries != NULL);

  // if (Update_All_Ghost_Boundaries != NULL)
  //      displaySCHED (Update_All_Ghost_Boundaries);

// ...(12/12/96,kdb) see not aboce about the problem with block parti ...
#if 0
     }
#endif

     return Update_All_Ghost_Boundaries;
#else
     APP_ASSERT (X.BlockPartiArrayDomain != NULL);
     displayDARRAY ( X.BlockPartiArrayDomain );

  // Right boundary on left processor and left boundary on right processor
     const int fillVec = (Communication_Manager::localProcessNumber() == 0) ? 1 : -1;
     const int X_Axis = 0;
     const int Y_Axis = 1;
     printf ("On processor %d  --  fillVec = %d \n",Communication_Manager::localProcessNumber(),fillVec);
     SCHED* Update_Edge_Ghost_Boundaries = exchSched( X.BlockPartiArrayDomain, Y_Axis, fillVec);

     APP_ASSERT(Update_Edge_Ghost_Boundaries != NULL);

  // displaySCHED (Update_Edge_Ghost_Boundaries);

     return Update_Edge_Ghost_Boundaries;
#endif
#else
     return 0;
#endif
   }
#endif  // PADRE is defined

//====================================================================
#if !defined(USE_PADRE)
SCHED* Internal_Partitioning_Type::BuildCommunicationScheduleSendAllGhostBoundaries 
   ( const Array_Domain_Type & X )
{
#if defined(PPP)
  if (X.isNullArray() == TRUE) return NULL;

  // int Number_Of_Dimensions = X.Domain_Dimension;
  APP_ASSERT (X.Domain_Dimension == 
     Array_Domain_Type::computeArrayDimension (X));

  APP_ASSERT (X.BlockPartiArrayDomain != NULL);

  // First find schedule for all ghost boundary updates and then remove
  // receives
  SCHED* Update_All_Ghost_Boundaries = 
     ghostFillAllSched( X.BlockPartiArrayDomain );

  if (Update_All_Ghost_Boundaries != NULL)
  {
     // ... loop over all processors removing receives ... 
     int maxp = PARTI_numprocs();
     int pnum;
     for (pnum=0; pnum<maxp; pnum++)
     {
        if (Update_All_Ghost_Boundaries->rData[pnum] != NULL)
	   delete Update_All_Ghost_Boundaries->rData[pnum];
	Update_All_Ghost_Boundaries->rMsgSz[pnum] = 0;
     }
  }

  return Update_All_Ghost_Boundaries;
#else
  return 0;
#endif
}
#endif  // PADRE is defined


//====================================================================
//====================================================================

#if defined(PPP)
#if !defined(USE_PADRE)
void
Internal_Partitioning_Type::checkGhostBoundarySchedule (const doubleArray & X , const doubleSerialArray & X_SerialArray, SCHED* sched)
   {
     if (&X);
     if (&X_SerialArray);
     if (&sched);

// ... WARNING: TEMPORARILY TURNED OFF BECAUSE DOESN'T WORK FOR 2D ...
#if 0
   int ndims = X.Array_Descriptor->Domain_Dimension;
   // ... find ghost cell starting positions relative to start of data ...
   int ghost_cell_width[MAX_ARRAY_DIMENSION];
   int left_ghost_cell_start[MAX_ARRAY_DIMENSION];
   int right_ghost_cell_start[MAX_ARRAY_DIMENSION];
   int nd;
   for (nd=0; nd<ndims; nd++)
   {
     ghost_cell_width[nd] = X.Array_Descriptor->InternalGhostCellWidth[nd];
     int dim_size = X_SerialArray.Array_Descriptor->Size[nd];
     if (nd != 0)
        dim_size /= X_SerialArray.Array_Descriptor->Size[nd-1];

     // ... if ghost cell width is less than 1 these will be meaningless
     //  and not used later ...
     left_ghost_cell_start[nd] = 0;
     right_ghost_cell_start[nd] =  dim_size - ghost_cell_width[nd];
   }

   // ... loop over all processors making sure data is sent from correct
   //  interior cells and received into ghost cells ...
   int maxp = PARTI_numprocs();
   int pnum;
   for (pnum=0; pnum<maxp; pnum++)
   {
      if (sched->rData[pnum] != NULL)
      {
         for (nd=0; nd<ndims; nd++)
	 {
	   if(ghost_cell_width[nd]>0)
	   {
	     // ... first check to see if correct number of values will
	     //  be received ...
	     if (ghost_cell_width[nd] != sched->rMsgSz[pnum])
	     {
	       printf("ERROR in receive: ");
	       printf("ghost cell width = %d, number of values received = %d\n",
	          ghost_cell_width[nd],sched->rMsgSz[pnum]);
	     }
	     // ... now make sure the starting position is one of the ghost
	     //  cell starting positions (it's hard to determine which side
	     //  this should go in so accept either for now) ...
	     if (sched->rData[pnum]->startPosn != left_ghost_cell_start[nd] &&
	         sched->rData[pnum]->startPosn != right_ghost_cell_start[nd])
             {
	       printf("ERROR in receive: ");
	       printf("Starting position %d is wrong, should be %d or %d\n",
	          sched->rData[pnum]->startPosn, left_ghost_cell_start[nd],
	          right_ghost_cell_start[nd]);
	     }
	   }
	 }
      }
      if (sched->sData[pnum] != NULL)
      {
         for (nd=0; nd<ndims; nd++)
         {
	   if(ghost_cell_width[nd]>0)
	   {
	     // ... first check to see if correct number of values will
	     //  be received ...
	     if (ghost_cell_width[nd] != sched->sMsgSz[pnum])
	     {
	       printf("ERROR in send: ");
	       printf("ghost cell width = %d, number of values sent = %d\n",
	          ghost_cell_width[nd],sched->sMsgSz[pnum]);
	     }
	     // ... now make sure the starting position is one of the cells
	     //  just inside the ghost cell starting positions (it's hard to 
	     // determine which side this should go in so accept either for now) ...
	     if (sched->sData[pnum]->startPosn != 
	       left_ghost_cell_start[nd] + ghost_cell_width[nd] &&
	         sched->sData[pnum]->startPosn != 
	       right_ghost_cell_start[nd] - ghost_cell_width[nd])
             {
	       printf("ERROR in send: ");
	       printf("Starting position %d is wrong, should be %d or %d\n",
	          sched->sData[pnum]->startPosn, 
	      left_ghost_cell_start[nd]  + ghost_cell_width[nd],
	      right_ghost_cell_start[nd] - ghost_cell_width[nd]);
	     }
	   }
	 }
      }
   }
#endif
}
#endif
#endif
//=====================================================================
#if defined(PPP)
void
Internal_Partitioning_Type::updateGhostBoundaries ( const doubleArray & X , const doubleSerialArray & X_SerialArray )
   {
  // This function supports the use of an optional parameter to specify the SerialArray that is used
  // to hold the boundaries for theupdate process.  This interface supports the use of a parallel array's
  // block parti descriptor to do the communication on an identical array's data.  The usefulness of this
  // is in the regularSectionTransfer where we use the descriptor of the Lhs to get data which is putinto
  // temporary storage of the Rhs on the processor owning the Lhs operand's data.  This interface allows
  // a simpler implementation that updates the ghost boundaries of the temporary storage.  This is important
  // when we mix communication models (having the ghost boundaries properly initialized after the VSG update.
  // The use of PADRE in newer versions of P++ allows for the abstraction of many details (to PADRE) and thus
  // allows PADRE to handel these details more efficiently since PADRE is leveraged across other work as well as P++.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::updateGhostBoundaries(const doubleArray & X) \n");
     X_SerialArray.Test_Consistency("Inside of Internal_Partitioning_Type::updateGhostBoundaries");
#endif

#if defined(USE_PADRE)
     APP_ASSERT (X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer->
          updateGhostBoundaries(X_SerialArray.getDataPointer());
#else
     // ... (12/11/96,kdb) temporarily remove exch schedules to force a new one to 
     //  be built because somehow the incorrect one is found by block parti ...

     remove_exch_scheds();

#if defined(PPP)
  // Temp_Schedule will be NULL if no data is present on the local processor (so no ghost boundaries exist to update)
     SCHED* Temp_Schedule = BuildCommunicationScheduleUpdateAllGhostBoundaries 
	( X.Array_Descriptor.Array_Domain );

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          if (Temp_Schedule == NULL)
               printf ("Temp_Schedule == NULL in Internal_Partitioning_Type::updateGhostBoundaries \n");
        }
#endif
  // Make call to datmove function!
     if (Temp_Schedule != NULL)
        {
          APP_ASSERT(X_SerialArray.Array_Descriptor.Array_Data != NULL);
       // Optimization_Manager::Number_Of_Messages_Sent++;
       // Optimization_Manager::Number_Of_Messages_Recieved++;
          Diagnostic_Manager::incrementNumberOfGhostBoundaryUpdates();

       // ... (12/10/96,kdb) add check to see if the schedule actually
       // specifies the correct ghost cells to update ...
#if COMPILE_DEBUG_STATEMENTS
          Internal_Partitioning_Type::checkGhostBoundarySchedule
             (X, X_SerialArray, Temp_Schedule);
#endif
          dDataMove(X_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,X_SerialArray.Array_Descriptor.Array_Data);
          delete_SCHED (Temp_Schedule);
          Temp_Schedule = NULL;
        }
#endif
#endif // PADRE not defined
   }
#endif

#if defined(PPP)
void
Internal_Partitioning_Type::updateGhostBoundaries ( const floatArray & X , const floatSerialArray & X_SerialArray )
   {
  // See note in equivalent function handling the doubleArray type.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::updateGhostBoundaries(const floatArray & X) \n");
     X_SerialArray.Test_Consistency("Inside of Internal_Partitioning_Type::updateGhostBoundaries");
#endif

#if defined(USE_PADRE)
     APP_ASSERT (X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (X_SerialArray.Array_Descriptor.Array_Data != NULL);
     X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer->updateGhostBoundaries
        ( X_SerialArray.Array_Descriptor.Array_Data );
#else
     // ... (12/11/96,kdb) temporarily remove exch schedules to force a new one to 
     //  be built because somehow the incorrect one is found by block parti ...

     remove_exch_scheds();

#if defined(PPP)
  // Temp_Schedule will be NULL if no data is present on the local processor (so no ghost boundaries exist to update)
     SCHED* Temp_Schedule = BuildCommunicationScheduleUpdateAllGhostBoundaries 
	( X.Array_Descriptor.Array_Domain );

  // Make call to datmove function!
     if (Temp_Schedule != NULL)
        {
          APP_ASSERT(X_SerialArray.Array_Descriptor.Array_Data != NULL);
       // Optimization_Manager::Number_Of_Messages_Sent++;
       // Optimization_Manager::Number_Of_Messages_Recieved++;
          Diagnostic_Manager::incrementNumberOfGhostBoundaryUpdates();

          fDataMove(X_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,X_SerialArray.Array_Descriptor.Array_Data);
          delete_SCHED (Temp_Schedule);
        }
#endif
#endif // PADRE not defined
   }
#endif

#if defined(PPP)
void
Internal_Partitioning_Type::updateGhostBoundaries ( const intArray & X , const intSerialArray & X_SerialArray )
   {
  // See note in equivalent function handling the doubleArray type.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::updateGhostBoundaries(const intArray & X) \n");
     X_SerialArray.Test_Consistency("Inside of Internal_Partitioning_Type::updateGhostBoundaries");
#endif

#if defined(USE_PADRE)
     APP_ASSERT (X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (X_SerialArray.Array_Descriptor.Array_Data != NULL);
     X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer->updateGhostBoundaries
        ( X_SerialArray.Array_Descriptor.Array_Data );
#else
     // ... (12/11/96,kdb) temporarily remove exch schedules to force a new one to 
     //  be built because somehow the incorrect one is found by block parti ...

     remove_exch_scheds();

#if defined(PPP)
  // Temp_Schedule will be NULL if no data is present on the local processor (so no ghost boundaries exist to update)
     SCHED* Temp_Schedule = BuildCommunicationScheduleUpdateAllGhostBoundaries 
	( X.Array_Descriptor.Array_Domain );

  // Make call to datmove function!
     if (Temp_Schedule != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (X_SerialArray.Array_Descriptor.Array_Data == NULL)
             {
               X_SerialArray.view("ERROR in Internal_Partitioning_Type::updateGhostBoundaries: array data pointer == NULL in X_SerialArray");
             }
#endif
          APP_ASSERT(X_SerialArray.Array_Descriptor.Array_Data != NULL);
       // Optimization_Manager::Number_Of_Messages_Sent++;
       // Optimization_Manager::Number_Of_Messages_Recieved++;
          Diagnostic_Manager::incrementNumberOfGhostBoundaryUpdates();

          iDataMove(X_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,X_SerialArray.Array_Descriptor.Array_Data);
          delete_SCHED (Temp_Schedule);
        }
#endif
#endif // PADRE not defined
   }
#endif

#if defined(PPP)
void
Internal_Partitioning_Type::updateGhostBoundaries ( const floatArray & X , const intSerialArray & X_SerialArray )
{
  // See note in equivalent function handling the doubleArray type.

#if COMPILE_DEBUG_STATEMENTS
  if (APP_DEBUG > 0)
    printf ("Inside of Internal_Partitioning_Type::updateGhostBoundaries(const intArray & X) \n");
  X_SerialArray.Test_Consistency("Inside of Internal_Partitioning_Type::updateGhostBoundaries");
#endif

#if defined(USE_PADRE)
     APP_ASSERT (X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (X_SerialArray.Array_Descriptor.Array_Data != NULL);
     X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer->updateGhostBoundaries
        ( X_SerialArray.Array_Descriptor.Array_Data );
#else
     // ... (12/11/96,kdb) temporarily remove exch schedules to force a new one to 
     //  be built because somehow the incorrect one is found by block parti ...

     remove_exch_scheds();

#if defined(PPP)
  // Temp_Schedule will be NULL if no data is present on the local processor (so 
  // no ghost boundaries exist to update)
  SCHED* Temp_Schedule = 
     BuildCommunicationScheduleUpdateAllGhostBoundaries 
	( X.Array_Descriptor.Array_Domain );

  // Make call to datmove function!
  if (Temp_Schedule != NULL)
  {
     APP_ASSERT(X_SerialArray.Array_Descriptor.Array_Data != NULL);
  // Optimization_Manager::Number_Of_Messages_Sent++;
  // Optimization_Manager::Number_Of_Messages_Recieved++;
     Diagnostic_Manager::incrementNumberOfGhostBoundaryUpdates();

     iDataMove(X_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,X_SerialArray.Array_Descriptor.Array_Data);
     delete_SCHED (Temp_Schedule);
  }
#endif
#endif // PADRE not defined
}
#endif

#if defined(PPP)
void
Internal_Partitioning_Type::updateGhostBoundaries ( const doubleArray & X , const intSerialArray & X_SerialArray )
{
  // See note in equivalent function handling the doubleArray type.

#if COMPILE_DEBUG_STATEMENTS
  if (APP_DEBUG > 0)
    printf ("Inside of Internal_Partitioning_Type::updateGhostBoundaries(const intArray & X) \n");
  X_SerialArray.Test_Consistency("Inside of Internal_Partitioning_Type::updateGhostBoundaries");
#endif

#if defined(USE_PADRE)
     APP_ASSERT (X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (X_SerialArray.Array_Descriptor.Array_Data != NULL);
     X.Array_Descriptor.Array_Domain.parallelPADRE_DescriptorPointer->updateGhostBoundaries
        ( X_SerialArray.Array_Descriptor.Array_Data );
#else
     // ... (12/11/96,kdb) temporarily remove exch schedules to force a new one to 
     //  be built because somehow the incorrect one is found by block parti ...

     remove_exch_scheds();

#if defined(PPP)
  // Temp_Schedule will be NULL if no data is present on the local processor (so 
  // no ghost boundaries exist to update)
  SCHED* Temp_Schedule = 
     BuildCommunicationScheduleUpdateAllGhostBoundaries 
	( X.Array_Descriptor.Array_Domain );

  // Make call to datmove function!
  if (Temp_Schedule != NULL)
  {
     APP_ASSERT(X_SerialArray.Array_Descriptor.Array_Data != NULL);
  // Optimization_Manager::Number_Of_Messages_Sent++;
  // Optimization_Manager::Number_Of_Messages_Recieved++;
     Diagnostic_Manager::incrementNumberOfGhostBoundaryUpdates();

     iDataMove(X_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,X_SerialArray.Array_Descriptor.Array_Data);
     delete_SCHED (Temp_Schedule);
  }
#endif
#endif // PADRE not defined
}
#endif

#if !defined(USE_PADRE)
// ****************************************************************************************************
// These functions move a rectangular part of the Rhs (which might be distributed over many processors)
// to the processors owning the specified rectangular part of the Lhs (the operation would typically
// be between unaligned arrays and thus is a basis for the unaligned array operations).
// In cases of no ghost boundaries (width zero) this is an expensive substitute for the
// use of the ghost boundary update.
// ****************************************************************************************************
SCHED*
Internal_Partitioning_Type::BuildCommunicationScheduleRegularSectionTransfer ( 
   const Array_Domain_Type & Lhs, const Array_Domain_Type & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::BuildCommunicationScheduleRegularSectionTransfer(Array_Domain_Type,Array_Domain_Type) \n");
#endif

#if defined(PPP)
     int Max_Dimension = (Lhs.Domain_Dimension > Rhs.Domain_Dimension) ?
                          Lhs.Domain_Dimension : Rhs.Domain_Dimension;

     int Lhs_Dimension_Array [MAX_ARRAY_DIMENSION];
     int Rhs_Dimension_Array [MAX_ARRAY_DIMENSION];
     int Lhs_Base_Array      [MAX_ARRAY_DIMENSION];
     int Rhs_Base_Array      [MAX_ARRAY_DIMENSION];
     int Lhs_Bound_Array     [MAX_ARRAY_DIMENSION];
     int Rhs_Bound_Array     [MAX_ARRAY_DIMENSION];
     int Lhs_Stride_Array    [MAX_ARRAY_DIMENSION];
     int Rhs_Stride_Array    [MAX_ARRAY_DIMENSION];

     Lhs.getRawDataSize (Lhs_Dimension_Array);
     Rhs.getRawDataSize (Rhs_Dimension_Array);

  // This would be more efficient but for now we should initialize the whole array
  // for (int i=0; i < Max_Dimension; i++)
     for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Parti requires that the unused dimensions should have -1 as an entry
          if ( i > Lhs.Domain_Dimension-1 )
               Lhs_Dimension_Array [i] = -1;
            else
               Lhs_Dimension_Array [i] = i;

          if ( i > Rhs.Domain_Dimension-1 )
               Rhs_Dimension_Array [i] = -1;
            else
               Rhs_Dimension_Array [i] = i;

       // We could make this more efficient by just providing pointers to 
       // the Array_Descriptor Base Bound and Stride arrays directly.  But for now we will
       // sperate the two to avoid any potential problems
          Lhs_Base_Array [i] = Lhs.Base[i];
          Rhs_Base_Array [i] = Rhs.Base[i];

          Lhs_Bound_Array [i] = Lhs.Bound[i];
          Rhs_Bound_Array [i] = Rhs.Bound[i];

          Lhs_Stride_Array [i] = Lhs.Stride[i];
          Rhs_Stride_Array [i] = Rhs.Stride[i];
        }

     APP_ASSERT( Lhs.BlockPartiArrayDomain != NULL );
     APP_ASSERT( Rhs.BlockPartiArrayDomain != NULL );

#if 0
     printf ("Lhs.Domain_Dimension = %d \n",Lhs.Domain_Dimension);
     printf ("Rhs.Domain_Dimension = %d \n",Rhs.Domain_Dimension);

     printf ("Max_Dimension = %d \n",Max_Dimension);

     printf ("Lhs_Dimension_Array [0-%d] = ", MAX_ARRAY_DIMENSION-1);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d",Lhs_Dimension_Array [i]);
     printf ("\n");

     printf ("Rhs_Dimension_Array [0-%d] = ", MAX_ARRAY_DIMENSION-1);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d",Rhs_Dimension_Array [i]);
     printf ("\n");

     printf ("Lhs_Base_Array [0-%d] = ", MAX_ARRAY_DIMENSION-1);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d",Lhs_Base_Array [i]);
     printf ("\n");

     printf ("Lhs_Bound_Array [0-%d] = ", MAX_ARRAY_DIMENSION-1);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d",Lhs_Bound_Array [i]);
     printf ("\n");

     printf ("Lhs_Stride_Array [0-%d] = ", MAX_ARRAY_DIMENSION-1);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d",Lhs_Stride_Array [i]);
     printf ("\n");

     printf ("Rhs_Base_Array [0-%d] = ", MAX_ARRAY_DIMENSION-1);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d",Rhs_Base_Array [i]);
     printf ("\n");

     printf ("Rhs_Bound_Array [0-%d] = ", MAX_ARRAY_DIMENSION-1);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d",Rhs_Bound_Array [i]);
     printf ("\n");

     printf ("Rhs_Stride_Array [0-%d] = ", MAX_ARRAY_DIMENSION-1);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d",Rhs_Stride_Array [i]);
     printf ("\n");

     displayDARRAY (Lhs.BlockPartiArrayDomain);
     displayDARRAY (Rhs.BlockPartiArrayDomain);
#endif

  // The Rhs is the source and the Lhs is the destination
  // printf ("Building a Schedule by calling PARTI subArraySched \n");
     SCHED* Temp_Schedule = subArraySched ( Rhs.BlockPartiArrayDomain, 
                                            Lhs.BlockPartiArrayDomain,
                                            Max_Dimension,
                                            Rhs_Dimension_Array, Rhs_Base_Array, Rhs_Bound_Array, Rhs_Stride_Array,
                                            Lhs_Dimension_Array, Lhs_Base_Array, Lhs_Bound_Array, Lhs_Stride_Array );
  // printf ("DONE: Building a Schedule by calling PARTI subArraySched \n");
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          if (Temp_Schedule == NULL)
               printf ("WARNING: Temp_Schedule == NULL (no data to communicate: this is rare but OK) \n");
#endif

#if 0
  // displayDARRAY (Lhs.BlockPartiArrayDomain);
  // displayDARRAY (Rhs.BlockPartiArrayDomain);
#endif

  // Make this part of the diagnostics report
     if (Diagnostic_Manager::getMessagePassingInterpretationReport() > 0)
          displaySCHED (Temp_Schedule);

     return Temp_Schedule;
#else
     return NULL;
#endif
   }

  // PADRE not defined
#endif

//=============================================================================
#if defined(PPP)
void
Internal_Partitioning_Type::regularSectionTransfer ( const doubleArray & Lhs_ParallelArray, 
                                                     const doubleSerialArray & Lhs_SerialArray,
                                                     const doubleArray & Rhs_ParallelArray,
                                                     const doubleSerialArray & Rhs_SerialArray )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::regularSectionTransfer(double,double) \n");
#endif

#if defined(USE_PADRE)
     PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
          transferData(Lhs_ParallelArray.Array_Descriptor.Array_Domain,
                       Rhs_ParallelArray.Array_Descriptor.Array_Domain,
                       Rhs_SerialArray.getDataPointer(),
                       Lhs_SerialArray.getDataPointer());
#else

  // Need to update ghost boundaries since subArraySched does not fill in the ghost boundaries!
  // According to Allan Susman we could merge the communication schedules but that might
  // be a problem in the hashing of the communication schedules.  So in this case if ghost boundaries
  // are present we are forcing more communication taht would be optimial just to get the
  // ghost boundaries of the place were the put the Rhs data (on the processor owning the Lhs)
  // updated propoerly.  If would be much better to have the subArraySched function return the 
  // communication schedule that would update the ghost boundaries as well.  So we might have to
  // modify Block Parti at a later date.

  // Bugfix (1/30/96) ghost boundaries were not updated in the correct SerialArray
  // we need to have the update take place in the Lhs_SerialArray not the Lhs_ParallelArray.SerialArray
  // updateGhostBoundaries (Lhs_ParallelArray);

     SCHED* Temp_Schedule = BuildCommunicationScheduleRegularSectionTransfer 
       ( Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
	 Rhs_ParallelArray.Array_Descriptor.Array_Domain );
  // APP_ASSERT(Temp_Schedule != NULL);

#if COMPILE_DEBUG_STATEMENTS
     bool Receving_Data = FALSE;
     bool Sending_Data  = FALSE;
  // See if we have to send or recieve anything so we can check the pointers using asserts.
     if (Temp_Schedule != NULL)
        {
          for (int i=0; i < PARTI_numprocs(); i++)
             {
               if (Temp_Schedule->rMsgSz[i] > 0)
                    Receving_Data = TRUE;
               if (Temp_Schedule->sMsgSz[i] > 0)
                    Sending_Data  = TRUE;
             }
        }
     if (Receving_Data)
        {
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
     if (Sending_Data)
        {
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
#endif

     if (Temp_Schedule != NULL)
        {
       // Track the number of message sent and recieved
       // Optimization_Manager::Number_Of_Messages_Sent++;
       // Optimization_Manager::Number_Of_Messages_Recieved++;
          Diagnostic_Manager::incrementNumberOfRegularSectionTransfers();

       // Make call to Block PARTI datmove function!
       // The Rhs is the source the Lhs is the destination
          dDataMove(Rhs_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,Lhs_SerialArray.Array_Descriptor.Array_Data);
       // printf ("Inside of Internal_Partitioning_Type::regularSectionTransfer -- Temp_Schedule->referenceCount = %d \n",
       //      Temp_Schedule->referenceCount);
          delete_SCHED (Temp_Schedule);
          Temp_Schedule = NULL;
        }
  // PADRE not defined
#endif
   }
#endif

#if defined(PPP)
void
Internal_Partitioning_Type::regularSectionTransfer ( const floatArray & Lhs_ParallelArray, 
                                                     const floatSerialArray & Lhs_SerialArray,
                                                     const floatArray & Rhs_ParallelArray,
                                                     const floatSerialArray & Rhs_SerialArray )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::regularSectionTransfer(float,float) \n");
#endif

#if defined(PPP)
#if defined(USE_PADRE)

  // This is a static function call!  I think the order of the operands is screwed up!
     PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::transferData
        ( Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
          Rhs_ParallelArray.Array_Descriptor.Array_Domain, 
          ((floatSerialArray &) Rhs_SerialArray).Array_Descriptor.Array_Data,
          ((floatSerialArray &) Lhs_SerialArray).Array_Descriptor.Array_Data );
#else
     SCHED* Temp_Schedule = BuildCommunicationScheduleRegularSectionTransfer ( 
                                 Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
                                 Rhs_ParallelArray.Array_Descriptor.Array_Domain );
  // APP_ASSERT(Temp_Schedule != NULL);

  // Make call to datmove function!
#if COMPILE_DEBUG_STATEMENTS
     bool Receving_Data = FALSE;
     bool Sending_Data  = FALSE;
     if (Temp_Schedule != NULL)
        {
          for (int i=0; i < PARTI_numprocs(); i++)
             {
               if (Temp_Schedule->rMsgSz[i] > 0)
                    Receving_Data = TRUE;
               if (Temp_Schedule->sMsgSz[i] > 0)
                    Sending_Data  = TRUE;
             }
        }
     if (Receving_Data)
        {
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
     if (Sending_Data)
        {
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
#endif

     if (Temp_Schedule != NULL)
        {
       // The Rhs is the source the Lhs is the destination
       // Optimization_Manager::Number_Of_Messages_Sent++;
       // Optimization_Manager::Number_Of_Messages_Recieved++;
          Diagnostic_Manager::incrementNumberOfRegularSectionTransfers();

          fDataMove(Rhs_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,Lhs_SerialArray.Array_Descriptor.Array_Data);
       // printf ("Inside of Internal_Partitioning_Type::regularSectionTransfer -- Temp_Schedule->referenceCount = %d \n",
       //      Temp_Schedule->referenceCount);
          delete_SCHED (Temp_Schedule);
          Temp_Schedule = NULL;
        }

  // Need to update ghost boundaries since subArraySched does not fill in the ghost boundaries!
  // We don't need to have the ghost boundaries filled in the VSG update until we reach the operator=
  // updateGhostBoundaries (Lhs_ParallelArray,Lhs_SerialArray);
#endif
#endif // PADRE not defined
   }
#endif

#if defined(PPP)
void
Internal_Partitioning_Type::regularSectionTransfer ( const intArray & Lhs_ParallelArray, 
                                                     const intSerialArray & Lhs_SerialArray,
                                                     const intArray & Rhs_ParallelArray,
                                                     const intSerialArray & Rhs_SerialArray )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::regularSectionTransfer(intArray,intArray) \n");
#endif

#if defined(PPP)
#if defined(USE_PADRE)
  // This is a static function call!  I think the order of the operands is screwed up!
     PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::transferData
        ( Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
          Rhs_ParallelArray.Array_Descriptor.Array_Domain, 
          Rhs_SerialArray.Array_Descriptor.Array_Data,
          Lhs_SerialArray.Array_Descriptor.Array_Data );
#else
     SCHED* Temp_Schedule = BuildCommunicationScheduleRegularSectionTransfer 
       ( Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
	 Rhs_ParallelArray.Array_Descriptor.Array_Domain );
  // APP_ASSERT(Temp_Schedule != NULL);

  // Make call to datmove function!
#if COMPILE_DEBUG_STATEMENTS
     bool Receving_Data = FALSE;
     bool Sending_Data  = FALSE;
     if (Temp_Schedule != NULL)
        {
          for (int i=0; i < PARTI_numprocs(); i++)
             {
               if (Temp_Schedule->rMsgSz[i] > 0)
                    Receving_Data = TRUE;
               if (Temp_Schedule->sMsgSz[i] > 0)
                    Sending_Data  = TRUE;
             }
        }
     if (Receving_Data)
        {
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
     if (Sending_Data)
        {
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
#endif

     if (Temp_Schedule != NULL)
        {
       // The Rhs is the source the Lhs is the destination
       // Optimization_Manager::Number_Of_Messages_Sent++;
       // Optimization_Manager::Number_Of_Messages_Recieved++;
          Diagnostic_Manager::incrementNumberOfRegularSectionTransfers();

          iDataMove(Rhs_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,Lhs_SerialArray.Array_Descriptor.Array_Data);
       // printf ("Inside of Internal_Partitioning_Type::regularSectionTransfer -- Temp_Schedule->referenceCount = %d \n",
       //      Temp_Schedule->referenceCount);
       // APP_ABORT();

          delete_SCHED (Temp_Schedule);
          Temp_Schedule = NULL;
        }

  // Need to update ghost boundaries since subArraySched does not fill in the ghost boundaries!
  // We don't need to have the ghost boundaries filled in the VSG update until we reach the operator=
  // updateGhostBoundaries (Lhs_ParallelArray,Lhs_SerialArray);
#endif
#endif // PADRE not defined
   }
#endif

#if defined(PPP)
void
Internal_Partitioning_Type::regularSectionTransfer ( const doubleArray & Lhs_ParallelArray,
                                                     const intSerialArray & Lhs_SerialArray,
                                                     const intArray & Rhs_ParallelArray,
                                                     const intSerialArray & Rhs_SerialArray )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::regularSectionTransfer(double,int) \n");
#endif

#if defined(PPP)
#if defined(USE_PADRE)
  // This is a static function call!  I think the order of the operands is screwed up!
     PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::transferData
        ( Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
          Rhs_ParallelArray.Array_Descriptor.Array_Domain, 
          Rhs_SerialArray.Array_Descriptor.Array_Data,
          Lhs_SerialArray.Array_Descriptor.Array_Data );
#else
     SCHED* Temp_Schedule = BuildCommunicationScheduleRegularSectionTransfer 
	( Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
	  Rhs_ParallelArray.Array_Descriptor.Array_Domain );
  // APP_ASSERT(Temp_Schedule != NULL);

  // Make call to datmove function!
     bool Receving_Data = FALSE;
     bool Sending_Data  = FALSE;
#if COMPILE_DEBUG_STATEMENTS
     if (Temp_Schedule != NULL)
        {
          for (int i=0; i < PARTI_numprocs(); i++)
             {
               if (Temp_Schedule->rMsgSz[i] > 0)
                    Receving_Data = TRUE;
               if (Temp_Schedule->sMsgSz[i] > 0)
                    Sending_Data  = TRUE;
             }
        }
     if (Receving_Data)
        {
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
        }
     if (Sending_Data)
        {
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
#endif

     //printf ("ERROR: (Internal_Partitioning_Type::regularSectionTransfer) Sorry - Data Movement between arrays of different types not implemented! \n");
     //APP_ABORT();

     if (Temp_Schedule != NULL)
        {
       // The Rhs is the source the Lhs is the destination
       // Optimization_Manager::Number_Of_Messages_Sent++;
       // Optimization_Manager::Number_Of_Messages_Recieved++;
          Diagnostic_Manager::incrementNumberOfRegularSectionTransfers();

          iDataMove(Rhs_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,Lhs_SerialArray.Array_Descriptor.Array_Data);
          delete_SCHED (Temp_Schedule);
          Temp_Schedule = NULL;
        }
#endif
#endif // PADRE not defined
   }
#endif

#if defined(PPP)
void
Internal_Partitioning_Type::regularSectionTransfer ( const floatArray & Lhs_ParallelArray,
                                                     const intSerialArray & Lhs_SerialArray,
                                                     const intArray & Rhs_ParallelArray,
                                                     const intSerialArray & Rhs_SerialArray )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::regularSectionTransfer(float,int) \n");
#endif

#if defined(PPP)
#if defined(USE_PADRE)
  // This is a static function call!  I think the order of the operands is screwed up!
     PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::transferData
        ( Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
          Rhs_ParallelArray.Array_Descriptor.Array_Domain, 
          Rhs_SerialArray.Array_Descriptor.Array_Data,
          Lhs_SerialArray.Array_Descriptor.Array_Data );
#else
     SCHED* Temp_Schedule = BuildCommunicationScheduleRegularSectionTransfer 
       (Lhs_ParallelArray.Array_Descriptor.Array_Domain, 
	Rhs_ParallelArray.Array_Descriptor.Array_Domain );
  // APP_ASSERT(Temp_Schedule != NULL);

  // Make call to datmove function!
     bool Receving_Data = FALSE;
     bool Sending_Data  = FALSE;
#if COMPILE_DEBUG_STATEMENTS
     if (Temp_Schedule != NULL)
        {
          for (int i=0; i < PARTI_numprocs(); i++)
             {
               if (Temp_Schedule->rMsgSz[i] > 0)
                    Receving_Data = TRUE;
               if (Temp_Schedule->sMsgSz[i] > 0)
                    Sending_Data  = TRUE;
             }
        }
     if (Receving_Data)
        {
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Lhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
     if (Sending_Data)
        {
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray != NULL);
          APP_ASSERT(Rhs_ParallelArray.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
        }
#endif

     //printf ("ERROR: (Internal_Partitioning_Type::regularSectionTransfer) Sorry - Data Movement between arrays of different types not implemented! \n");
     //APP_ABORT();

     if (Temp_Schedule != NULL)
        {
       // The Rhs is the source the Lhs is the destination
       // Optimization_Manager::Number_Of_Messages_Sent++;
       // Optimization_Manager::Number_Of_Messages_Recieved++;
          Diagnostic_Manager::incrementNumberOfRegularSectionTransfers();

          iDataMove(Rhs_SerialArray.Array_Descriptor.Array_Data,Temp_Schedule,Lhs_SerialArray.Array_Descriptor.Array_Data);
          delete_SCHED (Temp_Schedule);
          Temp_Schedule = NULL;
        }

#endif
#endif // PADRE not defined
   }
#endif

//======================================================================

#if !defined(USE_PADRE)
void
Internal_Partitioning_Type::displaySCHED ( SCHED* X )
   {
  // this function prints out the information hiding in the parti SCHED structure
  // which is generated to describe the communication to be done by the <type>DataMove function
  // in the parti library.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > -1)
          printf ("Inside of Internal_Partitioning_Type::displaySCHED() \n");
#endif

  // save the current state and turn off the output of node number prefix to all strings
     bool printfState = Communication_Manager::getPrefixParallelPrintf();
     Communication_Manager::setPrefixParallelPrintf(FALSE);

     APP_ASSERT (X != NULL);

#if defined(PPP)
     int i = 0;
     printf ("rMsgSz[0-%d]:",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          printf (" %d",X->rMsgSz[i]);
        }
     printf ("\n");
     
     printf ("sMsgSz[0-%d]:",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          printf (" %d",X->sMsgSz[i]);
        }
     printf ("\n");
     
     if (X->type == 1)
        {
          printf ("Hash Type (type) == 1 meaning is an 'exch_sched' \n");
        }
       else
        {
          if (X->type == 2)
             {
               printf ("Hash Type (type) == 2 meaning is an 'subarray_sched' \n");
             }
            else
             {
               printf ("Hash Type (type) == %d : AN INVALID TYPE \n",X->type);
             }
        }

     printf ("Hash Bucket (hash) = %d \n",X->hash);

     printf ("rData[0-%d] (+ = VALID POINTER, * = NULL POINTER):",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->rData[i] != NULL)
               printf ("+");
            else
               printf ("*");
        }
     printf ("\n");
     
     printf ("rData[0-%d]->proc:",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->rData[i] != NULL)
               printf (" %d",X->rData[i]->proc);
        }
     printf ("\n");
     
     printf ("rData[0-%d]->numDims:",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->rData[i] != NULL)
               printf (" %d",X->rData[i]->numDims);
        }
     printf ("\n");
     
     printf ("rData[0-%d]->startPosn:",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->rData[i] != NULL)
               printf (" %d",X->rData[i]->startPosn);
        }
     printf ("\n");

     printf ("rData[0-%d]->numelem[0-%d]:",PARTI_numprocs()-1,MAX_ARRAY_DIMENSION-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
//        if (X->rData[i] != NULL)
//             printf ("(%d,%d,%d,%d)",X->rData[i]->numelem[0],X->rData[i]->numelem[1],
//                                     X->rData[i]->numelem[2],X->rData[i]->numelem[3]);
          if (X->rData[i] != NULL)
             {
               printf ("(%d",X->rData[i]->numelem[0]);
               for (int j=1; j < X->rData[i]->numDims; j++)
                  {
                    printf (",%d",X->rData[i]->numelem[j]);
                  }
               printf (")");
             }
        }
     printf ("\n");

     printf ("rData[0-%d]->str[0-%d]:",PARTI_numprocs()-1,MAX_ARRAY_DIMENSION-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
//        if (X->rData[i] != NULL)
//             printf ("(%d,%d,%d,%d)",X->rData[i]->str[0],X->rData[i]->str[1],
//                                     X->rData[i]->str[2],X->rData[i]->str[3]);
          if (X->rData[i] != NULL)
             {
               printf ("(%d",X->rData[i]->str[0]);
               for (int j=1; j < X->rData[i]->numDims; j++)
                  {
                    printf (",%d",X->rData[i]->str[j]);
                  }
               printf (")");
             }
        }
     printf ("\n");

     printf ("sData[0-%d] (+ = VALID POINTER, * = NULL POINTER):",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->sData[i] != NULL)
               printf ("+");
            else
               printf ("*");
        }
     printf ("\n");

     printf ("sData[0-%d]->proc:",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->sData[i] != NULL)
               printf (" %d",X->sData[i]->proc);
        }
     printf ("\n");

     printf ("sData[0-%d]->numDims:",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->sData[i] != NULL)
               printf (" %d",X->sData[i]->numDims);
        }
     printf ("\n");

     printf ("sData[0-%d]->startPosn:",PARTI_numprocs()-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->sData[i] != NULL)
               printf (" %d",X->sData[i]->startPosn);
        }
     printf ("\n");

     printf ("sData[0-%d]->numelem[0-%d]:",PARTI_numprocs()-1,MAX_ARRAY_DIMENSION-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->sData[i] != NULL)
             {
            // printf ("(%d,%d,%d,%d)",X->sData[i]->numelem[0],X->sData[i]->numelem[1],
            //                         X->sData[i]->numelem[2],X->sData[i]->numelem[3]);
               printf ("(%d",X->sData[i]->numelem[0]);
               for (int j=1; j < X->sData[i]->numDims; j++)
                  {
                    printf (",%d",X->sData[i]->numelem[j]);
                  }
               printf (")");
             }
        }
     printf ("\n");

     printf ("sData[0-%d]->str[0-%d]:",PARTI_numprocs()-1,MAX_ARRAY_DIMENSION-1);
     for (i=0; i < PARTI_numprocs(); i++)
        {
          if (X->sData[i] != NULL)
             {
            // printf ("(%d,%d,%d,%d)",X->sData[i]->str[0],X->sData[i]->str[1],
            //                         X->sData[i]->str[2],X->sData[i]->str[3]);
               printf ("(%d",X->sData[i]->str[0]);
               for (int j=1; j < X->sData[i]->numDims; j++)
                  {
                    printf (",%d",X->sData[i]->str[j]);
                  }
               printf (")");
             }
        }
     printf ("\n");

  // reset output of node number prefix to all strings
     Communication_Manager::setPrefixParallelPrintf(printfState);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > -1)
        {
          printf ("At BASE of Internal_Partitioning_Type::displaySCHED() \n");
       // APP_ABORT();
        }
#endif

     printf ("\n");
#endif
   }
#endif

#if 0
void
Internal_Partitioning_Type::displayDefaultPartitioning ()
   {
   }

void
Internal_Partitioning_Type::displayPartitioning ()
   {
   }
#endif

#if !defined(USE_PADRE)
void
Internal_Partitioning_Type::displayDARRAY ( DARRAY* X )
   {
  // This function prints out the information hiding in the parti DARRAY structure

     APP_ASSERT (X != NULL);

#if defined(PPP)
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::displayDARRAY() (referenceCount = %d) \n",
               X->referenceCount);
#endif
#endif

#if defined(PPP)
     int i = 0;
     printf ("Number of Dimensions (nDims) = %d \n",X->nDims);
     printf ("number of internal ghost cells in each dim (ghostCells[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->ghostCells[i]);
        }
     printf ("\n");

     printf ("total size of each dim (dimVecG[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->dimVecG[i]);
        }
     printf ("\n");

     printf ("local size of each dim for central pieces (dimVecL[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->dimVecL[i]);
        }
     printf ("\n");

     printf ("local size of each dim for left most piece (dimVecL_L[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->dimVecL_L[i]);
        }
     printf ("\n");

     printf ("local size of each dim for right most piece (dimVecL_R[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->dimVecL_R[i]);
        }
     printf ("\n");

     printf ("lower global index on my processor (g_index_low[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->g_index_low[i]);
        }
     printf ("\n");

     printf ("upper global index on my processor (g_index_hi[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->g_index_hi[i]);
        }
     printf ("\n");

     printf ("Local size on my processor (local_size[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->local_size[i]);
        }
     printf ("\n");

     printf ("dim of decomp to which each dim aligned defines how array aligned to decomp \n");
     printf ("used with decomp to inialize decompPosn and dimDist -- (decompDim[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->decompDim[i]);
        }
     printf ("\n");

     printf ("coordinate position of processor in the decomposition to which it's bound \n");
     printf ("in the multi-dimensional decomposition space -- (decompPosn[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->decompPosn[i]);
        }
     printf ("\n");

     printf ("type of distribution in each dim (dimDist[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %c",X->dimDist[i]);
        }
     printf ("\n");

     printf ("DECOMP structure inside of DARRAY \n");
     APP_ASSERT(X->decomp != NULL);
     displayDECOMP ( X->decomp );
#endif
   }
#endif


#if !defined(USE_PADRE)
void
Internal_Partitioning_Type::displayDECOMP ( DECOMP* X )
   {
  // This function prints out the information hiding in the parti DARRAY structure

     APP_ASSERT (X != NULL);

#if defined(PPP)
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Partitioning_Type::displayDECOMP() (referenceCount = %d) \n",
               X->referenceCount);
#endif
#endif

#if defined(PPP)
     int i = 0;
     printf ("Number Of Dimensions (nDims)  = %d \n",X->nDims);
     printf ("Number Of Processors (nProcs) = %d \n",X->nProcs);
     printf ("Base Processor (baseProc)     = %d \n",X->baseProc);

     printf ("size of decomposition in each dim (dimVec[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->dimVec[i]);
        }
     printf ("\n");

     printf ("num processors allocated to each dim (dimProc[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %d",X->dimProc[i]);
        }
     printf ("\n");

     printf ("type of distribution in each dim (dimDist[0-%d]):",X->nDims-1);
     for (i=0; i < X->nDims; i++)
        {
          printf (" %c",X->dimDist[i]);
        }
     printf ("\n");
     printf ("\n");
#endif
   }
#endif

// These functions add the input array to the list of arrays associated with that
// partitioning object or the default partitioning object.  the purpose of the
// list is to allow the arrays associated with the partitioning object to be
// repartitioned whenever the partitioning objects change.

void
Internal_Partitioning_Type::AddArrayToPartitioning ( const doubleArray & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
          AssociateObjectWithDefaultDistribution(X);
#else
     if (!DefaultdoubleArrayList.checkElement(X))
          DefaultdoubleArrayList.addElement(X);
#endif
#endif
   }

void
Internal_Partitioning_Type::AddArrayToPartitioning ( Internal_Partitioning_Type & Partition, const doubleArray & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     Partition.distributionPointer->AssociateObjectWithDistribution(X);
#else
     if (!Partition.doubleArrayList.checkElement(X))
        {
       // printf ("In AddArrayToPartitioning: adding an existing reference not already in the list (%p) \n",&X);
          Partition.doubleArrayList.addElement(X);
        }
       else
        {
       // printf ("In AddArrayToPartitioning: (already in the list) \n");
        }

#if 0
  // Error checking for case on not using PADRE
     printf ("Partition.referenceCount              = %d \n",Partition.referenceCount);
     printf ("Partition.intArrayList.getLength()    = %d \n",Partition.intArrayList.getLength());
     printf ("Partition.floatArrayList.getLength()  = %d \n",Partition.floatArrayList.getLength());
     printf ("Partition.doubleArrayList.getLength() = %d \n",Partition.doubleArrayList.getLength());
  // APP_ASSERT (Partition.intArrayList.getLength()+Partition.floatArrayList.getLength()+Partition.doubleArrayList.getLength()+Internal_Partitioning_Type::getReferenceCountBase() == Partition.referenceCount);
#endif
#endif
  // printf ("Exiting in Internal_Partitioning_Type::AddArrayToPartitioning \n");
  // APP_ABORT();
#endif
   }

void
Internal_Partitioning_Type::AddArrayToPartitioning ( const floatArray  & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
          AssociateObjectWithDefaultDistribution(X);
#else
     if (!DefaultfloatArrayList.checkElement(X))
          DefaultfloatArrayList.addElement(X);
#endif
#endif
   }

void
Internal_Partitioning_Type::AddArrayToPartitioning ( Internal_Partitioning_Type & Partition, const floatArray & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     Partition.distributionPointer->AssociateObjectWithDistribution(X);
#else
     if (!Partition.floatArrayList.checkElement(X))
        {
       // printf ("In AddArrayToPartitioning: adding an existing reference not already in the list (%p) \n",&X);
          Partition.floatArrayList.addElement(X);
        }
       else
        {
       // printf ("In AddArrayToPartitioning: (already in the list) \n");
        }
#if 0
  // Error checking for case on not using PADRE
     printf ("Partition.referenceCount              = %d \n",Partition.referenceCount);
     printf ("Partition.intArrayList.getLength()    = %d \n",Partition.intArrayList.getLength());
     printf ("Partition.floatArrayList.getLength()  = %d \n",Partition.floatArrayList.getLength());
     printf ("Partition.doubleArrayList.getLength() = %d \n",Partition.doubleArrayList.getLength());
  // APP_ASSERT (Partition.intArrayList.getLength()+Partition.floatArrayList.getLength()+Partition.doubleArrayList.getLength()+Internal_Partitioning_Type::getReferenceCountBase() == Partition.referenceCount);
#endif

#endif
#endif
   }

void
Internal_Partitioning_Type::AddArrayToPartitioning ( const intArray    & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
          AssociateObjectWithDefaultDistribution(X);
#else
     if (!DefaultintArrayList.checkElement(X))
        DefaultintArrayList.addElement(X);
#endif
#endif
   }

void
Internal_Partitioning_Type::AddArrayToPartitioning ( Internal_Partitioning_Type & Partition, const intArray & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     Partition.distributionPointer->AssociateObjectWithDistribution(X);
#else
     if (!Partition.intArrayList.checkElement(X))
        {
       // printf ("In AddArrayToPartitioning: adding an existing reference not already in the list (%p) \n",&X);
          Partition.intArrayList.addElement(X);
        }
       else
        {
       // printf ("In AddArrayToPartitioning: (already in the list) \n");
        }
#if 0
  // Error checking for case on not using PADRE
     printf ("Partition.referenceCount              = %d \n",Partition.referenceCount);
     printf ("Partition.intArrayList.getLength()    = %d \n",Partition.intArrayList.getLength());
     printf ("Partition.floatArrayList.getLength()  = %d \n",Partition.floatArrayList.getLength());
     printf ("Partition.doubleArrayList.getLength() = %d \n",Partition.doubleArrayList.getLength());
  // APP_ASSERT (Partition.intArrayList.getLength()+Partition.floatArrayList.getLength()+Partition.doubleArrayList.getLength()+Internal_Partitioning_Type::getReferenceCountBase() == Partition.referenceCount);
#endif

#endif
#endif
   }

void
Internal_Partitioning_Type::DeleteArrayToPartitioning ( const doubleArray & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
          UnassociateObjectWithDefaultDistribution(X);
#else
     if (DefaultdoubleArrayList.checkElement( (doubleArray &) X))
        {
       // printf ("In DeleteArrayToPartitioning: removing an existing reference in the DEFAULT list \n");
          DefaultdoubleArrayList.deleteElement( (doubleArray &) X);
        }
       else
        {
       // printf ("In DeleteArrayToPartitioning: Can't find an existing reference in the DEFAULT list \n");
        }

#if 0
  // Error checking for case on not using PADRE
     printf ("Default intArrayList.getLength()    = %d \n",DefaultintArrayList.getLength());
     printf ("Default floatArrayList.getLength()  = %d \n",DefaultfloatArrayList.getLength());
     printf ("Default doubleArrayList.getLength() = %d \n",DefaultdoubleArrayList.getLength());
#endif

#endif
#endif
   }

void
Internal_Partitioning_Type::DeleteArrayToPartitioning ( Internal_Partitioning_Type & Partition, const doubleArray & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
  // Commented out to track down memory leaks and purify FMR
     Partition.distributionPointer->UnassociateObjectWithDistribution(X);
#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Commented out PADRE call to UnassociateObjectWithDistribution(X) \n");
#endif
#else
     if (Partition.doubleArrayList.checkElement( (doubleArray &) X))
        {
       // printf ("In DeleteArrayToPartitioning: removing an existing reference in the list \n");
          Partition.doubleArrayList.deleteElement( (doubleArray &) X);
        }
       else
        {
       // printf ("In DeleteArrayToPartitioning: Can't find an existing reference \n");
        }

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Commented out Non-PADRE call to Partition.doubleArrayList.deleteElement(X) \n");
#endif

#if 0
  // Error checking for case on not using PADRE
     printf ("Partition.referenceCount              = %d \n",Partition.referenceCount);
     printf ("Partition.intArrayList.getLength()    = %d \n",Partition.intArrayList.getLength());
     printf ("Partition.floatArrayList.getLength()  = %d \n",Partition.floatArrayList.getLength());
     printf ("Partition.doubleArrayList.getLength() = %d \n",Partition.doubleArrayList.getLength());

     printf ("Default intArrayList.getLength()    = %d \n",DefaultintArrayList.getLength());
     printf ("Default floatArrayList.getLength()  = %d \n",DefaultfloatArrayList.getLength());
     printf ("Default doubleArrayList.getLength() = %d \n",DefaultdoubleArrayList.getLength());
#endif

#endif
#endif
   }

void
Internal_Partitioning_Type::DeleteArrayToPartitioning ( const floatArray  & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
          UnassociateObjectWithDefaultDistribution(X);
#else
     if (DefaultfloatArrayList.checkElement( (floatArray &) X))
        {
       // printf ("In DeleteArrayToPartitioning: removing an existing reference in the DEFAULT list \n");
          DefaultfloatArrayList.deleteElement( (floatArray &) X);
        }
       else
        {
       // printf ("In DeleteArrayToPartitioning: Can't find an existing reference in the DEFAULT list \n");
        }

#if 0
  // Error checking for case on not using PADRE
     printf ("Default intArrayList.getLength()    = %d \n",DefaultintArrayList.getLength());
     printf ("Default floatArrayList.getLength()  = %d \n",DefaultfloatArrayList.getLength());
     printf ("Default doubleArrayList.getLength() = %d \n",DefaultdoubleArrayList.getLength());
#endif

#endif
#endif
   }

void
Internal_Partitioning_Type::DeleteArrayToPartitioning ( Internal_Partitioning_Type & Partition, const floatArray & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
  // Commented out to track down memory leaks and purify FMR
     Partition.distributionPointer->UnassociateObjectWithDistribution(X);
#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Commented out PADRE call to UnassociateObjectWithDistribution(X) \n");
#endif
#else
     if (Partition.floatArrayList.checkElement( (floatArray &) X))
        {
       // printf ("In DeleteArrayToPartitioning: removing an existing reference in the list \n");
          Partition.floatArrayList.deleteElement( (floatArray &) X);
        }
       else
        {
       // printf ("In DeleteArrayToPartitioning: Can't find an existing reference \n");
        }
#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Commented out Non-PADRE call to Partition.floatArrayList.deleteElement(X) \n");
#endif

#if 0
  // Error checking for case on not using PADRE
     printf ("Partition.referenceCount              = %d \n",Partition.referenceCount);
     printf ("Partition.intArrayList.getLength()    = %d \n",Partition.intArrayList.getLength());
     printf ("Partition.floatArrayList.getLength()  = %d \n",Partition.floatArrayList.getLength());
     printf ("Partition.doubleArrayList.getLength() = %d \n",Partition.doubleArrayList.getLength());

     printf ("Default intArrayList.getLength()    = %d \n",DefaultintArrayList.getLength());
     printf ("Default floatArrayList.getLength()  = %d \n",DefaultfloatArrayList.getLength());
     printf ("Default doubleArrayList.getLength() = %d \n",DefaultdoubleArrayList.getLength());
#endif

#endif
#endif
   }

void
Internal_Partitioning_Type::DeleteArrayToPartitioning ( const intArray    & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
     PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
          UnassociateObjectWithDefaultDistribution(X);
#else
     if (DefaultintArrayList.checkElement( (intArray &) X))
        {
       // printf ("In DeleteArrayToPartitioning: removing an existing reference in the DEFAULT list \n");
          DefaultintArrayList.deleteElement( (intArray &) X);
        }
       else
        {
       // printf ("In DeleteArrayToPartitioning: Can't find an existing reference in the DEFAULT list \n");
        }

#if 0
  // Error checking for case on not using PADRE
     printf ("Default intArrayList.getLength()    = %d \n",DefaultintArrayList.getLength());
     printf ("Default floatArrayList.getLength()  = %d \n",DefaultfloatArrayList.getLength());
     printf ("Default doubleArrayList.getLength() = %d \n",DefaultdoubleArrayList.getLength());
#endif

#endif
#endif
   }

void
Internal_Partitioning_Type::DeleteArrayToPartitioning ( Internal_Partitioning_Type & Partition, const intArray & X )
   {
#if defined(PPP) && !defined(SERIAL_APP)
#if defined(USE_PADRE)
  // Commented out to track down memory leaks and purify FMR
     Partition.distributionPointer->UnassociateObjectWithDistribution(X);
#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Commented out PADRE call to UnassociateObjectWithDistribution(X) \n");
#endif
#else
     if (Partition.intArrayList.checkElement( (intArray &) X))
        {
       // printf ("In DeleteArrayToPartitioning: removing an existing reference in the list \n");
          Partition.intArrayList.deleteElement( (intArray &) X);
        }
       else
        {
       // printf ("In DeleteArrayToPartitioning: Can't find an existing reference \n");
        }
#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Commented out Non-PADRE call to Partition.intArrayList.deleteElement(X) \n");
#endif

#if 0
  // Error checking for case on not using PADRE
     printf ("Partition.referenceCount              = %d \n",Partition.referenceCount);
     printf ("Partition.intArrayList.getLength()    = %d \n",Partition.intArrayList.getLength());
     printf ("Partition.floatArrayList.getLength()  = %d \n",Partition.floatArrayList.getLength());
     printf ("Partition.doubleArrayList.getLength() = %d \n",Partition.doubleArrayList.getLength());

     printf ("Default intArrayList.getLength()    = %d \n",DefaultintArrayList.getLength());
     printf ("Default floatArrayList.getLength()  = %d \n",DefaultfloatArrayList.getLength());
     printf ("Default doubleArrayList.getLength() = %d \n",DefaultdoubleArrayList.getLength());
#endif

#endif
#endif
   }

// ********************************************************************************************
// ********************************************************************************************
// Here we must define the interface for the Partitioning_Type objects!
// This has not been specified yet.  However we should avoid the creation
// of temporary partitions (thus the operations on partition_type objects
// should not follow that of the array class because they don't have similar
// memory requirements).
// ********************************************************************************************
// ********************************************************************************************

Internal_Partitioning_Type*
Partitioning_Type::getInternalPartitioningObject() const
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     return Internal_Partitioning_Object; 
   }

Partitioning_Type::~Partitioning_Type() 
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of destructor for Partitioning_Type! (Internal_Partitioning_Object->referenceCount = %d \n",Internal_Partitioning_Object->referenceCount);
#endif

  // printf ("In Partitioning_Type destructor: getReferenceCount() = %d \n",getReferenceCount());

  // We cannot assert this here since the deletion of a valid Partitioning_Type object
  // can occur when the object goes out of scope.
  // Modified to match correction to the ReferenceCountBase used in a Partitioning_Type
  // APP_ASSERT (referenceCount == getReferenceCountBase()-1);

     APP_ASSERT (Internal_Partitioning_Object != NULL);
     if (Internal_Partitioning_Object != NULL)
        {
          Internal_Partitioning_Object->decrementReferenceCount();
          if (Internal_Partitioning_Object->getReferenceCount() <
              Internal_Partitioning_Object->getReferenceCountBase())
               delete Internal_Partitioning_Object;
          Internal_Partitioning_Object = NULL;
        }

  // se this value to be something that will trigger an error later
     referenceCount = -9999;
   }

#if 1
// This function might be alright if it properly incremented the reference count.
Partitioning_Type::Partitioning_Type ( Internal_Partitioning_Type & X ) 
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of constructor for Partitioning_Type (const Internal_Partitioning_Type & X) --- X.referenceCount = %d \n",
               X.referenceCount);
#endif

     referenceCount = getReferenceCountBase();
     APP_ASSERT (getReferenceCount()   >= 1);
     Internal_Partitioning_Object = &((Internal_Partitioning_Type &) X); 
     APP_ASSERT (Internal_Partitioning_Object != NULL);

  // Now there are two references to the input Internal_Partitioning_Type object
     X.incrementReferenceCount();

     APP_ASSERT (X.getReferenceCount() > 1);
   }
#endif

Partitioning_Type::Partitioning_Type()
   {
     referenceCount = getReferenceCountBase();
     Internal_Partitioning_Object = new Internal_Partitioning_Type(); 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
   }

#if 0
// This is intended to be a generic way to specify a distribution onto
// a large collection of processors.  The map would be the same dimension
// as the Virtual processor space (with for now in PARTI would be 1D).
Partitioning_Type::Partitioning_Type( const doubleArray & Load_Map )
   {
     referenceCount = getReferenceCountBase();
     Internal_Partitioning_Object = new Internal_Partitioning_Type(Load_Map); 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
   }
#endif

Partitioning_Type::Partitioning_Type( int Number_Of_Processors )
   { 
     referenceCount = getReferenceCountBase();
     Internal_Partitioning_Object = new Internal_Partitioning_Type(Number_Of_Processors); 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
   }

Partitioning_Type::Partitioning_Type( const Range & Processor_Subcollection )
   { 
     referenceCount = getReferenceCountBase();
     Internal_Partitioning_Object = new Internal_Partitioning_Type(Processor_Subcollection); 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
   }

#if 0
Partitioning_Type::Partitioning_Type ( const Partitioning_Type & X )
   { 
     referenceCount = getReferenceCountBase();
     Internal_Partitioning_Object = new Internal_Partitioning_Type(*X.Internal_Partitioning_Object); 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
   }
#endif

Partitioning_Type &
Partitioning_Type::operator= ( const Partitioning_Type & X )
   {
     referenceCount = getReferenceCountBase();
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     APP_ASSERT (X.Internal_Partitioning_Object != NULL);

     APP_ASSERT (Internal_Partitioning_Object->getReferenceCount() >= getReferenceCountBase());

  // Delete any existing Internal_Partitioning_Type
     Internal_Partitioning_Object->decrementReferenceCount();
     if (Internal_Partitioning_Object->getReferenceCount() < getReferenceCountBase())
          delete Internal_Partitioning_Object;
     Internal_Partitioning_Object = NULL;

  // Record the new reference to X.Internal_Partitioning_Object
     X.Internal_Partitioning_Object->incrementReferenceCount();
     Internal_Partitioning_Object = X.Internal_Partitioning_Object; 

     APP_ASSERT (Internal_Partitioning_Object != NULL);
     return *this;
   }

// void SpecifyDecompositionAxes ( int Number_Of_Dimensions_To_Split , intArray & ArraySizes );
// void SpecifyDecompositionAxes ( intArray & ArraySizes );

void
Partitioning_Type::SpecifyDecompositionAxes ( int Input_Number_Of_Dimensions_To_Partition )
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Object->SpecifyDecompositionAxes (Input_Number_Of_Dimensions_To_Partition); 
   }

void
Partitioning_Type::SpecifyDefaultInternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER )
   { 
     Internal_Partitioning_Type::SpecifyDefaultInternalGhostBoundaryWidths ( VARIABLE_LIST_MACRO_INTEGER );
   }

void
Partitioning_Type::SpecifyInternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER )
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Object->SpecifyInternalGhostBoundaryWidths ( VARIABLE_LIST_MACRO_INTEGER );
   }

void
Partitioning_Type::SpecifyExternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER )
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Object->SpecifyExternalGhostBoundaryWidths ( VARIABLE_LIST_MACRO_INTEGER );
   }

#if !defined(USE_PADRE)
void
Partitioning_Type::SpecifyDecompositionEmbeddingIntoVirtualProcessorSpace (
          DECOMP* BlockParti_Decomposition_Pointer ,
          int StartingProcessor , int EndingProcessor )
   { 
     Internal_Partitioning_Type::SpecifyDecompositionEmbeddingIntoVirtualProcessorSpace (
          BlockParti_Decomposition_Pointer , StartingProcessor , EndingProcessor ); 
   }
#endif

#if 1
// static function
void
Partitioning_Type::swapDistribution ( 
     const Partitioning_Type & oldDistribution , 
     const Partitioning_Type & newDistribution )
   {
     APP_ASSERT (oldDistribution.Internal_Partitioning_Object != NULL);
     APP_ASSERT (newDistribution.Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Type::swapDistribution ( *oldDistribution.Internal_Partitioning_Object,
                                                    *newDistribution.Internal_Partitioning_Object );

  // Internal_Partitioning_Type *Temp_Internal_Partitioning_Object = 
  //      oldDistribution.Internal_Partitioning_Object;
  // newDistribution.Internal_Partitioning_Object = oldDistribution.Internal_Partitioning_Object;
  // oldDistribution.Internal_Partitioning_Object = Temp_Internal_Partitioning_Object;
   }

void
Internal_Partitioning_Type::swapDistribution ( 
     const Internal_Partitioning_Type & oldDistribution , 
     const Internal_Partitioning_Type & newDistribution )
   {
  // Now reassociate the arrays in the old_Partitioning_Object woth the new one
#if defined(PPP)
#if defined(USE_PADRE)
     APP_ASSERT (oldDistribution.distributionPointer != NULL);
     APP_ASSERT (newDistribution.distributionPointer != NULL);

     PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::swapDistribution
          ( *oldDistribution.distributionPointer, *newDistribution.distributionPointer );

  // printf ("Exiting AFTER PADRE_Descriptor::swapDistribution() \n");
  // APP_ABORT();
#else
     int i = 0;

  // for now lets assume these are of length zero (but this is not always true)
  // the header file contains a description of purpos of these variables
     APP_ASSERT (Internal_Partitioning_Type::DefaultPartitioningObjectList.getLength() == 0);
     APP_ASSERT (oldDistribution.PartitioningObjectList.getLength() == 0);
     APP_ASSERT (newDistribution.PartitioningObjectList.getLength() == 0);

  // Used in the assertion at the base of this function!
     int OldReferenceCountAtStart = oldDistribution.getReferenceCount();

#if 0
  // We don't want to touch the default lists since this is where the array objects
  // are stored which have a default distribution.  The default distributions are not
  // dynamic in the same way.  Specifically an array not built using a partitioning object is
  // assigned a default distribution.  The array objects "partition()" membe function can be used
  // to assign a new partition object to the array but the default partition object can not
  // be changed once arrays objects are associated with it.  If an array object was build using
  // a partition object (or explicitly associated with a specific partition object at a later point)
  // then the partition object can be dynamically changed and the associated array objects will
  // be repartitioned automatically.

     int LengthOfDefaultdoubleArrayList = oldDistribution.DefaultdoubleArrayList.getLength();
     for (i=0; i < LengthOfDefaultdoubleArrayList; i++)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Changing the distribution of DefaultdoubleArray's (Array_ID=%d) (%d of %d in list) \n",
                    oldDistribution.DefaultdoubleArrayList[0].Array_ID(), 1, 
                    oldDistribution.DefaultdoubleArrayList.getLength() );
#endif

          oldDistribution.DefaultdoubleArrayList[0].partition ( newDistribution );
        }

     int LengthOfDefaultfloatArrayList = oldDistribution.DefaultfloatArrayList.getLength();
     for (i=0; i < LengthOfDefaultfloatArrayList; i++)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Changing the distribution of DefaultfloatArray's (Array_ID=%d) (%d of %d in list) \n",
                    oldDistribution.DefaultfloatArrayList[0].Array_ID(), 1, oldDistribution.DefaultfloatArrayList.getLength() );
#endif

          oldDistribution.DefaultfloatArrayList[0].partition ( newDistribution );
        }

     int LengthOfDefaultintArrayList = oldDistribution.DefaultintArrayList.getLength();
     for (i=0; i < LengthOfDefaultintArrayList; i++)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Changing the distribution of DefaultintArray's (Array_ID=%d) (%d of %d in list) \n",
                    oldDistribution.DefaultintArrayList[0].Array_ID(), 1, 
                    oldDistribution.DefaultintArrayList.getLength() );
#endif

          oldDistribution.DefaultintArrayList[0].partition ( newDistribution );
        }
  // end of if 0 (above)
#endif

     int LengthOfdoubleArrayList = oldDistribution.doubleArrayList.getLength();
     for (i=0; i < LengthOfdoubleArrayList; i++)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Changing the distribution of doubleArray's (Array_ID=%d) (%d of %d in list) \n",
                    oldDistribution.doubleArrayList[0].Array_ID(), 1, 
                    oldDistribution.doubleArrayList.getLength() );
#endif

       // printf ("BEFORE PARTITION CALL newDistribution.getReferenceCount() = %d \n",
       //      newDistribution.getReferenceCount());
          oldDistribution.doubleArrayList[0].partition ( newDistribution );
       // printf ("AFTER PARTITION CALL  newDistribution.getReferenceCount() = %d \n",
       //      newDistribution.getReferenceCount());
        }
  // printf ("AFTER LOOP CALL  newDistribution.getReferenceCount() = %d \n",
  //      newDistribution.getReferenceCount());

     int LengthOffloatArrayList = oldDistribution.floatArrayList.getLength();
     for (i=0; i < LengthOffloatArrayList; i++)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Changing the distribution of floatArray's (Array_ID=%d) (%d of %d in list) \n",
                    oldDistribution.floatArrayList[0].Array_ID(), 1, 
                    oldDistribution.floatArrayList.getLength() );
#endif

          oldDistribution.floatArrayList[0].partition ( newDistribution );
        }

     int LengthOfintArrayList = oldDistribution.intArrayList.getLength();
     for (i=0; i < LengthOfintArrayList; i++)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Changing the distribution of intArray's (Array_ID=%d) (%d of %d in list) \n",
                    oldDistribution.intArrayList[i].Array_ID(), 1, 
                    oldDistribution.intArrayList.getLength() );
#endif

          oldDistribution.intArrayList[0].partition ( newDistribution );
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("OldReferenceCountAtStart = %d \n",OldReferenceCountAtStart);
          printf ("oldDistribution.getReferenceCount() = %d \n",oldDistribution.getReferenceCount());
          printf ("newDistribution.getReferenceCount() = %d \n",newDistribution.getReferenceCount());
        }
#endif

  // APP_ASSERT ( OldReferenceCountAtStart == newDistribution.getReferenceCount() );
  // (9/21/2000) Commented out this test to get non-PADRE version of P++ testcode to pass (need to review this)
     printf ("Commented out assertion that old distribution's ref count == new distribution's ref count \n");

#if 0
     if ( (oldDistribution.getReferenceCount() != APP_REFERENCE_COUNT_BASE) || 
          (newDistribution.getReferenceCount() != APP_REFERENCE_COUNT_BASE) )
          APP_ASSERT ( oldDistribution.getReferenceCount() != newDistribution.getReferenceCount() );
#endif

  // PADRE not defined
#endif
  // PPP defined
#endif
   }

void
Partitioning_Type::SpecifyProcessorRange ( const Range & Processor_Range )
   {
  // We have a memory leak here if we don't fix this up!
     Partitioning_Type New_Partitioning_Object (Processor_Range);

     swapDistribution (*this,New_Partitioning_Object);

     Internal_Partitioning_Type *Temp_Partitioning_Object = Internal_Partitioning_Object;
     Internal_Partitioning_Object = New_Partitioning_Object.Internal_Partitioning_Object;
     New_Partitioning_Object.Internal_Partitioning_Object = Temp_Partitioning_Object;
   }
#endif

// void AssociateAplusplusArrayWithPartition ( const doubleArray & X );
// void PartitionAplusplusArray ( const doubleArray & X );
// void PartitionAplusplusArray ( const floatArray  & X );
// void PartitionAplusplusArray ( const intArray    & X );

void
Partitioning_Type::AddArrayToPartitioning ( const doubleArray & X )
   { 
  // Internal_Partitioning_Type::AddArrayToPartitioning (X); 
     Internal_Partitioning_Type::AddArrayToPartitioning (X); 
   }

void
Partitioning_Type::AddArrayToPartitioning ( const Partitioning_Type & Partition, const doubleArray & X )
   { 
     Internal_Partitioning_Type::AddArrayToPartitioning (*Partition.Internal_Partitioning_Object,X); 
   }

void
Partitioning_Type::AddArrayToPartitioning ( const floatArray  & X )
   { 
     Internal_Partitioning_Type::AddArrayToPartitioning (X); 
   }

void
Partitioning_Type::AddArrayToPartitioning ( const Partitioning_Type & Partition, const floatArray  & X )
   { 
     Internal_Partitioning_Type::AddArrayToPartitioning (*Partition.Internal_Partitioning_Object,X); 
   }

void
Partitioning_Type::AddArrayToPartitioning ( const intArray    & X )
   { 
     Internal_Partitioning_Type::AddArrayToPartitioning (X); 
   }

void
Partitioning_Type::AddArrayToPartitioning ( const Partitioning_Type & Partition, const intArray    & X )
   { 
     Internal_Partitioning_Type::AddArrayToPartitioning (*Partition.Internal_Partitioning_Object,X); 
   }

void
Partitioning_Type::DeleteArrayToPartitioning ( const doubleArray & X )
   { 
     Internal_Partitioning_Type::DeleteArrayToPartitioning (X); 
   }

void
Partitioning_Type::DeleteArrayToPartitioning ( const Partitioning_Type & Partition, const doubleArray & X )
   { 
     Internal_Partitioning_Type::DeleteArrayToPartitioning (*Partition.Internal_Partitioning_Object,X); 
   }

void
Partitioning_Type::DeleteArrayToPartitioning ( const floatArray  & X )
   { 
     Internal_Partitioning_Type::DeleteArrayToPartitioning (X); 
   }

void
Partitioning_Type::DeleteArrayToPartitioning ( const Partitioning_Type & Partition, const floatArray  & X )
   { 
     Internal_Partitioning_Type::DeleteArrayToPartitioning (*Partition.Internal_Partitioning_Object,X); 
   }

void
Partitioning_Type::DeleteArrayToPartitioning ( const intArray    & X )
   { 
     Internal_Partitioning_Type::DeleteArrayToPartitioning (X); 
   }

void
Partitioning_Type::DeleteArrayToPartitioning ( const Partitioning_Type & Partition, const intArray    & X )
   { 
     Internal_Partitioning_Type::DeleteArrayToPartitioning (*Partition.Internal_Partitioning_Object,X); 
   }

// Allow user display of relavent partitioning data
void
Partitioning_Type::display ( const char *Label ) const
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Object->display(Label); 
   }

void
Partitioning_Type::displayDefaultValues ( const char *Label )
   { 
     Internal_Partitioning_Type::displayDefaultValues(Label); 
   }

#if defined(PPP) || defined (SERIAL_APP)
intSerialArray
Partitioning_Type::getProcessorSet ()
#else
intArray
Partitioning_Type::getProcessorSet ()
#endif
   {
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     return Internal_Partitioning_Object->getDefaultProcessorSet ();
   }

#if defined(PPP) || defined(SERIAL_APP)
intSerialArray
Partitioning_Type::getDefaultProcessorSet ()
#else
intArray
Partitioning_Type::getDefaultProcessorSet ()
#endif
   {
     return Internal_Partitioning_Type::getDefaultProcessorSet ();
   }

#if !defined(USE_PADRE)
void
Partitioning_Type::Test_Consistency( DARRAY *BlockPartiArrayDomain , const char *Label )
   {
  // Internal_Partitioning_Type::Test_Consistency (BlockPartiArrayDomain,Label);
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Object->Test_Consistency (BlockPartiArrayDomain,Label);
   }
#endif

// void Test_Consistency( DARRAY *BlockPartiArrayDomain , const char *Label ) const;

#if !defined(USE_PADRE)
DARRAY*
Partitioning_Type::Build_BlockPartiArrayDomain ( 
   DECOMP* BlockPartiArrayDecomposition , int *Array_Sizes ,
   int* InternalGhostCellWidth , int* ExternalGhostCellWidth )
   {
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     return Internal_Partitioning_Object->Build_BlockPartiArrayDomain (
                 BlockPartiArrayDecomposition,Array_Sizes,
                 InternalGhostCellWidth,ExternalGhostCellWidth); 
   }
#endif

#if !defined(USE_PADRE)
DARRAY*
Partitioning_Type::Build_BlockPartiArrayDomain ( 
   int *Array_Sizes , int* InternalGhostCellWidth , int* ExternalGhostCellWidth )
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     return Internal_Partitioning_Object->Build_BlockPartiArrayDomain (
                 Array_Sizes, InternalGhostCellWidth, ExternalGhostCellWidth); 
   }
#endif

#if !defined(USE_PADRE)
// Used internally in Build_BlockPartiDecompostion and Build_DefaultBlockPartiDecompostion
DECOMP*
Partitioning_Type::Build_BlockPartiDecompostion ( 
   int *Array_Sizes, char* Local_Distribution_String,
   int Local_Starting_Processor, int Local_Ending_Processor )
   { 
     return Internal_Partitioning_Type::Build_BlockPartiDecompostion (
                 Array_Sizes, Local_Distribution_String,
                 Local_Starting_Processor, Local_Ending_Processor); 
   }
#endif

#if !defined(USE_PADRE)
DECOMP*
Partitioning_Type::Build_BlockPartiDecompostion ( int *Array_Sizes )
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     return Internal_Partitioning_Object->Build_BlockPartiDecompostion (Array_Sizes); 
   }
#endif

#if !defined(USE_PADRE)
DARRAY*
Partitioning_Type::Build_DefaultBlockPartiArrayDomain (
   DECOMP* BlockPartiArrayDecomposition , int *Array_Sizes ,
   const int* InternalGhostCellWidth , 
   const int* ExternalGhostCellWidth )
   { 
     return Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain (
                 BlockPartiArrayDecomposition, Array_Sizes,
                 InternalGhostCellWidth, ExternalGhostCellWidth ); 
   }
#endif

#if !defined(USE_PADRE)
DARRAY*
Partitioning_Type::Build_DefaultBlockPartiArrayDomain (
   int *Array_Sizes , int* InternalGhostCellWidth , int* ExternalGhostCellWidth )
   { 
     return Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain (
                    Array_Sizes, InternalGhostCellWidth, ExternalGhostCellWidth ); 
   }
#endif

void
Partitioning_Type::setDefaultProcessorRange ( const Range & Processor_Range )
   {
     Internal_Partitioning_Type::setDefaultProcessorRange (Processor_Range);
   }

#if !defined(USE_PADRE)
DECOMP*
Partitioning_Type::Build_DefaultBlockPartiDecompostion    ( int *Array_Sizes )
   { 
     return Internal_Partitioning_Type::Build_DefaultBlockPartiDecompostion (Array_Sizes); 
   }
#endif

bool
Partitioning_Type::Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & X_Domain )
   { 
     return Internal_Partitioning_Type::Has_Same_Ghost_Boundary_Widths (X_Domain); 
   }

bool
Partitioning_Type::Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & Lhs_Domain,
                                                    const Array_Domain_Type & Rhs_Domain )
   { 
     return Internal_Partitioning_Type::Has_Same_Ghost_Boundary_Widths (Lhs_Domain,Rhs_Domain); 
   }

bool
Partitioning_Type::Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & This_Domain,
                                                    const Array_Domain_Type & Lhs_Domain,
                                                    const Array_Domain_Type & Rhs_Domain )
   { 
     return Internal_Partitioning_Type::Has_Same_Ghost_Boundary_Widths (This_Domain,Lhs_Domain,Rhs_Domain); 
   }

#if !defined(USE_PADRE)
SCHED*
Partitioning_Type::BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Domain_Type & X )
   { 
     return Internal_Partitioning_Type::BuildCommunicationScheduleUpdateAllGhostBoundaries (X); 
   }
#endif

// These functions updates the ghost boundaries for the distributed array
void
Partitioning_Type::updateGhostBoundaries ( const doubleArray & X )
   { 
#if defined(PPP)
     APP_ASSERT (X.Array_Descriptor.SerialArray != NULL);
     Internal_Partitioning_Type::updateGhostBoundaries (X,*X.Array_Descriptor.SerialArray); 
#endif
   }

void
Partitioning_Type::updateGhostBoundaries ( const floatArray  & X )
   { 
#if defined(PPP)
     APP_ASSERT (X.Array_Descriptor.SerialArray != NULL);
     Internal_Partitioning_Type::updateGhostBoundaries (X,*X.Array_Descriptor.SerialArray); 
#endif
   }

void
Partitioning_Type::updateGhostBoundaries ( const intArray    & X )
   { 
#if defined(PPP)
     APP_ASSERT (X.Array_Descriptor.SerialArray != NULL);
     Internal_Partitioning_Type::updateGhostBoundaries (X,*X.Array_Descriptor.SerialArray); 
#endif
   }

#if !defined(USE_PADRE)
// These functions move a rectangular part of the Rhs (which might be distributed over many processors)
// to the processors owning the specified rectangular part of the Lhs (the operation would typically
// be between unaligned arrays and thus is a basis for the unaligned array operations).
// In cases of no ghost boundaries (width zero) this is an expensive substitute for the
// use of the ghost boundary update.
SCHED*
Partitioning_Type::BuildCommunicationScheduleRegularSectionTransfer (
   const Array_Domain_Type & Lhs, const Array_Domain_Type & Rhs )
   { 
     return Internal_Partitioning_Type::BuildCommunicationScheduleRegularSectionTransfer (Lhs,Rhs); 
   }
#endif

#if defined(PPP)
void
Partitioning_Type::regularSectionTransfer ( const doubleArray & Lhs_Parallel,
                                            const doubleSerialArray & Lhs_SerialArray,
                                            const doubleArray & Rhs_ParallelArray,
                                            const doubleSerialArray & Rhs_SerialArray )
   { 
     Internal_Partitioning_Type::regularSectionTransfer (Lhs_Parallel,Lhs_SerialArray,Rhs_ParallelArray,Rhs_SerialArray); 
   }

void
Partitioning_Type::regularSectionTransfer ( const floatArray & Lhs_Parallel,
                                            const floatSerialArray & Lhs_SerialArray,
                                            const floatArray & Rhs_ParallelArray,
                                            const floatSerialArray & Rhs_SerialArray )
   { 
     Internal_Partitioning_Type::regularSectionTransfer (Lhs_Parallel,Lhs_SerialArray,Rhs_ParallelArray,Rhs_SerialArray); 
   }

void
Partitioning_Type::regularSectionTransfer ( const intArray & Lhs_Parallel,
                                            const intSerialArray & Lhs_SerialArray,
                                            const intArray & Rhs_ParallelArray,
                                            const intSerialArray & Rhs_SerialArray )
   {
     Internal_Partitioning_Type::regularSectionTransfer (Lhs_Parallel,Lhs_SerialArray,Rhs_ParallelArray,Rhs_SerialArray); 
   }

void
Partitioning_Type::regularSectionTransfer ( const doubleArray & Lhs_Parallel,
                                            const intSerialArray & Lhs_SerialArray,
                                            const intArray & Rhs_ParallelArray,
                                            const intSerialArray & Rhs_SerialArray )
   {
     Internal_Partitioning_Type::regularSectionTransfer (Lhs_Parallel,Lhs_SerialArray,Rhs_ParallelArray,Rhs_SerialArray); 
   }

void
Partitioning_Type::regularSectionTransfer ( const floatArray & Lhs_Parallel,
                                            const intSerialArray & Lhs_SerialArray,
                                            const intArray & Rhs_ParallelArray,
                                            const intSerialArray & Rhs_SerialArray )
   {
     Internal_Partitioning_Type::regularSectionTransfer (Lhs_Parallel,Lhs_SerialArray,Rhs_ParallelArray,Rhs_SerialArray); 
   }
#endif

void
Partitioning_Type::partitionAlongAxis ( int Axis, bool Partition_Axis, int GhostBoundaryWidth )
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Object->partitionAlongAxis (Axis,Partition_Axis,GhostBoundaryWidth); 
   }

void
Partitioning_Type::applyPartition ()
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Object->applyPartition (); 
   }

#if !defined(USE_PADRE)
void
Partitioning_Type::displaySCHED  ( SCHED*  X )
   { 
     Internal_Partitioning_Type::displaySCHED (X); 
   }

void
Partitioning_Type::displayDARRAY ( DARRAY* X )
   { 
     Internal_Partitioning_Type::displayDARRAY (X); 
   }

void
Partitioning_Type::displayDECOMP ( DECOMP* X )
   { 
     Internal_Partitioning_Type::displayDECOMP (X); 
   }
#endif

#if 0
void
Partitioning_Type::displayDefaultPartitioning ()
   { 
     Internal_Partitioning_Type::displayDefaultPartitioning (); 
   }

void
Partitioning_Type::displayPartitioning ()
   { 
     APP_ASSERT (Internal_Partitioning_Object != NULL);
     Internal_Partitioning_Object->displayPartitioning (); 
   }
#endif


