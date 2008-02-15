// Allow repeated includes of A++.h without error
#ifndef _APP_PARTITIONING_H
#define _APP_PARTITIONING_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif

// Include the template list class
#if defined(PPP)
#include "arraylist.h"
#include "partlist.h"
#endif

// This class defines an abstract partitioning.  At  present it does so within
// the constraints of the Block Parti restrictions on partitioning (similar to
// HPF partitiong (with I never really liked), but a little more general).

#if !defined(PPP)
// This is a BLOCK-PARTI specific type (C-structure)
// for A++ and SERIAL_APP we have to make it something
#define DECOMP int
#define DARRAY int
#define SCHED int
#endif

// define TEMP_MULTIPROCESSOR_DEBUGGING_CODE

class BaseArray;

class Internal_Partitioning_Type
   {
  // A note about the partitioning restriction due to the use of Block Parti:
  // The partitioning is multidimensional, (unlike the first P++ implementation
  // but very specific sorts of partitioning can no longer be specified.
  // This is because only the starting and ending processors may be specified
  // and that middle processors of the range defined by the starting and ending
  // processors must all have the same number of rows (or columns).  This is not
  // really all that bad unless a very specific partitioning is required (such as will
  // be the case for AMR when the load balancers are completed).  So the use of the Block
  // Parti code may only be a temporary target (which simplifies this second implementation) 
  // and the longer range plan may be to provide a similar layer (since I like the idea of seperating
  // P++ along such seperate layers of implementation) that will permit more specific
  // specification of data partitionings.

  // The pointer to the VirtualProcessorSpace is a data member of the 
  // Communication_Manager so it need not be represented here.

     friend class Partitioning_Type;
     public:
#if defined(PPP)
#if defined(USE_PADRE)
       // We need to use the fully templated version of A++/P++
       // PADRE_Distribution<Array<T>,Array_Descriptor_Type,Array_Domain_Type> 
       // WE need to use base classes that are independent of the double float and int
       // class doubleArray;
       // class doubleArray_Descriptor_Type;
       // class Array_Domain_Type;
       // PADRE_Distribution<BaseArray,doubleArray_Descriptor_Type,Array_Domain_Type> Distribution;
       // PADRE_Distribution<BaseArray,Array_Domain_Type,Array_Domain_Type> Distribution;
          PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *distributionPointer;
#else
       // Specify block distribution (choice of "*" UNDISTRIBUTED "B" BLOCK "C" CYCLIC distribution)
       // However the CYCLIC distribution is not yet implemented in Block Parti
       // The length of the string is the number of dimensions that will be distributed
       // Specify block distribution (choice of "*"-Undistributed or "B"-Block or "C"-Cyclic distribution)
          static char DefaultDistribution_String [MAX_ARRAY_DIMENSION];
          char Distribution_String [MAX_ARRAY_DIMENSION];

       // Default and user specified widths for partitioned array objects in P++
          static int DefaultInternalGhostCellWidth [MAX_ARRAY_DIMENSION];

       // These are the widths used to setup the P++ arrays -- however the P++ arrays can
       // reset there own ghost boundary widths and so these are not the default values for a
       // specific instance of a partitioning object and not the same as the static DefaultGhostCellWidth
       // for the partitioning_type class.
          int LocalGhostCellWidth [MAX_ARRAY_DIMENSION];

       // Default and user specified ArrayDimensionsToAlign
          static int DefaultArrayDimensionsToAlign [MAX_ARRAY_DIMENSION];
          int ArrayDimensionsToAlign [MAX_ARRAY_DIMENSION];

       // Default and user specified ExternalGhostCellWidth
          static int DefaultExternalGhostCellWidth [MAX_ARRAY_DIMENSION];

       // int ExternalGhostCellArrayLeft [MAX_ARRAY_DIMENSION];
       // Default and user specified ExternalGhostCellArrayRight
       // static int DefaultExternalGhostCellArrayRight [MAX_ARRAY_DIMENSION];
       // int ExternalGhostCellArrayRight [MAX_ARRAY_DIMENSION];

       // Default and user specified 
          static int DefaultPartitionControlFlags [MAX_ARRAY_DIMENSION];
          int PartitionControlFlags [MAX_ARRAY_DIMENSION];

       // Default and user specified 
          static int DefaultDecomposition_Dimensions [MAX_ARRAY_DIMENSION];
          int Decomposition_Dimensions [MAX_ARRAY_DIMENSION];

       // Within the restrictions of the Block Parti partitioning only the
       // starting and ending processor may be specified.  Further the range
       // must refer to a contiguous subset of virtual processors in the 
       // virtual processor space.
          int Starting_Processor;
          int Ending_Processor;
          static int DefaultStarting_Processor;
          static int DefaultEnding_Processor;

#if 0
       // We choose to seperate these from the Partitioning Object
       // so that we can have the partitioning objects be indepentent
       // of the size of the P++ array objects.  Thus many P++ arrays
       // can share a single partitioning object and changes to the partitioning
       // objects allowing redistribution of the P++ arrays onto the
       // virtual processor space is a powerful mechanism without 
       // detailed exceptions as would be the case if the array size
       // was a part of the Partitioning Object.

       // Resulting Block Parti structure to describe the distribution
          static DECOMP *DefaultBlockParti_Decomposition_Pointer;
          DECOMP *BlockParti_Decomposition_Pointer;

       // Resulting Block Parti structure for the Distributed array descriptor
          static DARRAY *DefaultBlockPartiArrayDomain;
          DARRAY *BlockPartiArrayDomain;
#endif

       // The total number of dimensions to partition (typically 1, 2, or 3, but could be 1-MAX_ARRAY_DIMENSION)
       // static int DefaultNumber_Of_Dimensions_To_Partition;
       // int Number_Of_Dimensions_To_Partition;

       // Default and user specified lists of P++ Arrays that are using this
       // partition object so that if the partition object changes we can
       // change the distribution of the associated doubleArrays.
          static List_Of_doubleArray DefaultdoubleArrayList;
          static List_Of_floatArray  DefaultfloatArrayList;
          static List_Of_intArray    DefaultintArrayList;

          List_Of_doubleArray        doubleArrayList;
          List_Of_floatArray         floatArrayList;
          List_Of_intArray           intArrayList;

       // Array of pointers to partitioning objects required to support arrays
       // with fewer elements than number of processors represented by the present
       // partitioning objects.  Parti can't handle the distribution of arrays
       // over processor collections in the case where the number of array elements
       // is smaller than the number of processors in the collection.  Actually
       // the situation is a little more complex because of the way that multidimensional 
       // arrays can be partitioned (the number of array elements could exceed the number of
       // processors and still Parti might not be able to handle the distribution).
       // Additionally we must check the number of partitionable dimensions for the partition
       // objects and the dimension of the arrays themselves.

          static List_Of_Partitioning_Type DefaultPartitioningObjectList;
          List_Of_Partitioning_Type PartitioningObjectList;

       // PADRE not defined
#endif
#endif

       // We need to keep track of the parent partitioning object so that we can trace back
       // to the root of a partition object in cases where the reshape function generates 
       // child partition objects to discribe the distribution of arrays reshaped with 
       // leading unit diminsions.  Use bool variable to provide error checking of 
       // NULL pointer.
          bool isRootOfPartitionTree;
          Internal_Partitioning_Type* parentPartitioningObject;

          void setParentPartitioningObject ( Internal_Partitioning_Type* Xptr );

       // Data required for the "new" and "delete" operators!
     private:
        static Internal_Partitioning_Type *Current_Link;
        Internal_Partitioning_Type        *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

     public:
          int referenceCount;

     public:
         void New_Function_Loop ();
         void *operator new    (size_t Size);
         void operator  delete (void* Pointer, size_t Size);

         static void freeMemoryInUse();

         ~Internal_Partitioning_Type();

       // Here we must define the interface for the Partitioning_Type objects!
       // This has not been specified yet.  However we should avoid the creation
       // of temporary partitions (thus the operations on partition_type objects
       // should not follow that of the array class because they don't have similar
       // memory requirements).
          Internal_Partitioning_Type();
       // This is intended to be a generic way to specify a distribution onto
       // a large collection of processors.  The map would be the same dimension
       // as the Virtual processor space (with for now in PARTI would be 1D).
          Internal_Partitioning_Type( const doubleArray & Load_Map );
          Internal_Partitioning_Type( int Number_Of_Processors );
          Internal_Partitioning_Type( const Range & Processor_Subcollection );
#if 0
       // This can be called by mistake and cause reference counting problems!
          Internal_Partitioning_Type ( const Internal_Partitioning_Type & X );
#endif

#if defined(USE_PADRE)
          Internal_Partitioning_Type ( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *X_Pointer );
#endif

          Internal_Partitioning_Type & operator= ( const Internal_Partitioning_Type & X );

       // void SpecifyDecompositionAxes ( int Number_Of_Dimensions_To_Split , intArray & ArraySizes );
       // void SpecifyDecompositionAxes ( intArray & ArraySizes );
          void SpecifyDecompositionAxes ( int Input_Number_Of_Dimensions_To_Partition );

          static void setDefaultProcessorRange ( const Range & Processor_Range );

       // These return an intArray since later any subset of processors will be available
       // on a not contiguous range of processors.
#if defined(PPP) || defined(SERIAL_APP)
          intSerialArray getProcessorSet ();
          static intSerialArray getDefaultProcessorSet ();
#else
          intArray getProcessorSet ();
          static intArray getDefaultProcessorSet ();
#endif

          static void SpecifyDefaultInternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
          void SpecifyInternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
          void SpecifyExternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

          static void swapDistribution ( const Internal_Partitioning_Type & OldDistribution , 
                                         const Internal_Partitioning_Type & newDistribution );

#if !defined(USE_PADRE)
          static void SpecifyDecompositionEmbeddingIntoVirtualProcessorSpace ( 
                    DECOMP* BlockParti_Decomposition_Pointer , 
                    int StartingProcessor , int EndingProcessor );
#endif

       // void AssociateAplusplusArrayWithPartition ( const doubleArray & X );
       // void PartitionAplusplusArray ( const doubleArray & X );
       // void PartitionAplusplusArray ( const floatArray  & X );
       // void PartitionAplusplusArray ( const intArray    & X );


          static void AddArrayToPartitioning ( const doubleArray & X );
       // static void AddArrayToPartitioning ( const doubleArray X );
          static void AddArrayToPartitioning ( Internal_Partitioning_Type & Partition, const doubleArray & X );
          static void AddArrayToPartitioning ( const floatArray  & X );
          static void AddArrayToPartitioning ( Internal_Partitioning_Type & Partition, const floatArray  & X );
          static void AddArrayToPartitioning ( const intArray    & X );
          static void AddArrayToPartitioning ( Internal_Partitioning_Type & Partition, const intArray    & X );

          static void DeleteArrayToPartitioning ( const doubleArray & X );
          static void DeleteArrayToPartitioning ( Internal_Partitioning_Type & Partition, const doubleArray & X );
          static void DeleteArrayToPartitioning ( const floatArray  & X );
          static void DeleteArrayToPartitioning ( Internal_Partitioning_Type & Partition, const floatArray  & X );
          static void DeleteArrayToPartitioning ( const intArray    & X );
          static void DeleteArrayToPartitioning ( Internal_Partitioning_Type & Partition, const intArray    & X );

       // Allow user display of relavent partitioning data
          void display ( const char *Label = "" ) const;
          static void displayDefaultValues ( const char *Label = "" );

#if !defined(USE_PADRE)
       // I think we need both a static and regular member function for error checking
       // (since we have both static and dynamic data)
          static void testPartiConsistency( DARRAY *BlockPartiArrayDomain , const char *Label = "" );
       // We need this to be a static member function so it can be called by the static Partitioning_Type object
          void Test_Consistency( DARRAY *BlockPartiArrayDomain , const char *Label = "" );
          static void staticTestConsistency( DARRAY *BlockPartiArrayDomain , const char *Label = "" );
       // void Test_Consistency( DARRAY *BlockPartiArrayDomain , const char *Label = "" ) const;

          DARRAY* Build_BlockPartiArrayDomain ( DECOMP* BlockPartiArrayDecomposition , int *Array_Sizes , 
                                                    int* InternalGhostCellWidth , int* ExternalGhostCellWidth ); 
          DARRAY* Build_BlockPartiArrayDomain ( int *Array_Sizes , int* InternalGhostCellWidth , 
                                                    int* ExternalGhostCellWidth );

       // Used internally in Build_BlockPartiDecompostion and Build_DefaultBlockPartiDecompostion
          static DECOMP* Build_BlockPartiDecompostion ( int *Array_Sizes, char* Local_Distribution_String, 
                                                        int Local_Starting_Processor, int Local_Ending_Processor );

          DECOMP* Build_BlockPartiDecompostion    ( int *Array_Sizes );

          static DARRAY* Build_DefaultBlockPartiArrayDomain 
	    ( DECOMP* BlockPartiArrayDecomposition , int *Array_Sizes , 
              const int* InternalGhostCellWidth, const int* ExternalGhostCellWidth);
          static DARRAY* Build_DefaultBlockPartiArrayDomain 
	    ( int *Array_Sizes , int* InternalGhostCellWidth , 
	      int* ExternalGhostCellWidth );
          static DECOMP* Build_DefaultBlockPartiDecompostion    ( int *Array_Sizes ); 
#endif

       // static bool Has_Same_Ghost_Boundary_Widths ( Array_Descriptor_Type *This_Descriptor = NULL ,
       //                                                 Array_Descriptor_Type *Lhs_Descriptor  = NULL ,
       //                                                 Array_Descriptor_Type *Rhs_Descriptor  = NULL );
#if 0
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & This_Descriptor,
                                                          const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );

          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );

          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & X_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & This_Descriptor,
                                                          const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );

          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & X );
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & X );

          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & X_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & This_Descriptor,
                                                          const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );

          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & X );
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & X );
#else
#if 0
          template<class T, int Template_dimension>
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<T,Template_dimension> & X_Descriptor );
          template<class T, int Template_dimension>
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<T,Template_dimension> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<T,Template_dimension> & Rhs_Descriptor );
          template<class T, int Template_dimension>
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<T,Template_dimension> & This_Descriptor,
                                                          const Array_Descriptor_Type<T,Template_dimension> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<T,Template_dimension> & Rhs_Descriptor );

          template<class T, int Template_dimension>
          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Descriptor_Type<T,Template_dimension> & X );
          template<class T, int Template_dimension>
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries   ( const Array_Descriptor_Type<T,Template_dimension> & X );
#else
#if !defined(USE_SAGE)
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & X_Domain );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & Lhs_Domain,
                                                          const Array_Domain_Type & Rhs_Domain );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & This_Domain,
                                                          const Array_Domain_Type & Lhs_Domain,
                                                          const Array_Domain_Type & Rhs_Domain );

#if !defined(USE_PADRE)
          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Domain_Type & X );
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries   ( const Array_Domain_Type & X );
#endif
#endif
#endif
#endif

#if defined(PPP)
       // These functions updates the ghost boundaries for the distributed array
          static void updateGhostBoundaries ( const doubleArray & X , const doubleSerialArray & X_SerialArray );
          static void updateGhostBoundaries ( const floatArray  & X , const floatSerialArray  & X_SerialArray );
          static void updateGhostBoundaries ( const intArray    & X , const intSerialArray    & X_SerialArray );
          static void updateGhostBoundaries ( const doubleArray & X , const intSerialArray    & X_SerialArray );
          static void updateGhostBoundaries ( const floatArray  & X , const intSerialArray    & X_SerialArray );

          static void sendGhostBoundaries ( const doubleArray & X , const doubleSerialArray & X_SerialArray );
          static void sendGhostBoundaries ( const floatArray  & X , const floatSerialArray  & X_SerialArray );
          static void sendGhostBoundaries ( const intArray    & X , const intSerialArray    & X_SerialArray );
          static void sendGhostBoundaries ( const doubleArray & X , const intSerialArray    & X_SerialArray );
          static void sendGhostBoundaries ( const floatArray  & X , const intSerialArray    & X_SerialArray );

#if !defined(USE_PADRE)
          static void checkGhostBoundarySchedule ( const doubleArray & X , const doubleSerialArray & X_SerialArray, SCHED* sched );
          static void checkGhostBoundarySchedule ( const floatArray  & X , const floatSerialArray  & X_SerialArray, SCHED* sched );
          static void checkGhostBoundarySchedule ( const intArray    & X , const intSerialArray    & X_SerialArray, SCHED* sched );
          static void checkGhostBoundarySchedule ( const doubleArray & X , const intSerialArray    & X_SerialArray, SCHED* sched );
          static void checkGhostBoundarySchedule ( const floatArray  & X , const intSerialArray    & X_SerialArray, SCHED* sched );
#endif

#endif

       // These functions move a rectangular part of the Rhs (which might be distributed over many processors)
       // to the processors owning the specified rectangular part of the Lhs (the operation would typically
       // be between unaligned arrays and thus is a basis for the unaligned array operations).
       // In cases of no ghost boundaries (width zero) this is an expensive substitute for the
       // use of the ghost boundary update.
#if 0
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Lhs, const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Rhs );
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Lhs, const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Rhs );
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Lhs, const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Rhs );
#else
#if 0
          template<class T, int Template_dimension>
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Descriptor_Type<T,Template_dimension> & Lhs, const Array_Descriptor_Type<T,Template_dimension> & Rhs );
#else
#if !defined(USE_PADRE)
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Domain_Type & Lhs, 
                    const Array_Domain_Type & Rhs );
#endif
#endif
#endif

#if defined(PPP)
          static void regularSectionTransfer ( const doubleArray & Lhs_Parallel, 
                                               const doubleSerialArray & Lhs_SerialArray,
                                               const doubleArray & Rhs_ParallelArray, 
                                               const doubleSerialArray & Rhs_SerialArray );
          static void regularSectionTransfer ( const floatArray & Lhs_Parallel, 
                                               const floatSerialArray & Lhs_SerialArray, 
                                               const floatArray & Rhs_ParallelArray, 
                                               const floatSerialArray & Rhs_SerialArray );
          static void regularSectionTransfer ( const intArray & Lhs_Parallel, 
                                               const intSerialArray & Lhs_SerialArray, 
                                               const intArray & Rhs_ParallelArray, 
                                               const intSerialArray & Rhs_SerialArray );
          static void regularSectionTransfer ( const doubleArray & Lhs_Parallel, 
                                               const intSerialArray & Lhs_SerialArray, 
                                               const intArray & Rhs_ParallelArray, 
                                               const intSerialArray & Rhs_SerialArray );
          static void regularSectionTransfer ( const floatArray & Lhs_Parallel, 
                                               const intSerialArray & Lhs_SerialArray, 
                                               const intArray & Rhs_ParallelArray, 
                                               const intSerialArray & Rhs_SerialArray );
#endif

          int getLocalGhostCellWidth (int i) const
             {
#if defined(PPP)
#if defined(USE_PADRE)
            // printf ("What PADRE function should we call here \n"); APP_ABORT();
               return distributionPointer->getGhostCellWidth(i);
#else
               return LocalGhostCellWidth[i];
#endif
#else
            // use to avoid compiler warning about lack of use of this variable
               if (&i);

               return 0;
#endif
             };

          static int getDefaultInternalGhostCellWidth (int i)
             {
#if defined(PPP)
#if defined(USE_PADRE)
            // printf ("What PADRE function should we call here \n"); APP_ABORT();
            // return Distribution.getDefaultGhostCellWidth(i);
               return PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::getDefaultGhostCellWidth(i);
#else
               return DefaultInternalGhostCellWidth[i];
#endif
#else
            // use to avoid compiler warning about lack of use of this variable
               if (&i);

               return 0;
#endif
             };
          static int getDefaultExternalGhostCellWidth (int i)
             {
#if defined(PPP)
#if defined(USE_PADRE)
            // use to avoid compiler warning about lack of use of this variable
               if (&i);

            // printf ("What PADRE function should we call here \n"); APP_ABORT();
            // PADRE has no concept of external ghost boundary widths
               return 0;
#else
               return DefaultExternalGhostCellWidth[i];
#endif
#else
            // use to avoid compiler warning about lack of use of this variable
               if (&i);

               return 0;
#endif
             };

          static void setDefaultStartingAndEndingProcessorsForDistribution ( int Start, int End )
             {
#if defined(PPP)
#if defined(USE_PADRE)
            // printf ("What PADRE function should we call here \n"); APP_ABORT();
            // PADRE does this internally (I think);
            // printf ("PADRE should have setup DefaultStarting_Processor and DefaultEnding_Processor \n");
            // printf ("Inside of InternalPartitioning_Type::setDefaultStartingAndEndingProcessorsForDistribution() \n");
               PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::setDefaultProcessorRange(Start,End);
            // printf ("Leaving InternalPartitioning_Type::setDefaultStartingAndEndingProcessorsForDistribution() \n");
#else
               DefaultStarting_Processor = Start;
               DefaultEnding_Processor   = End;
#endif
#else
            // use to avoid compiler warning about lack of use of this variable
               if (&Start);
               if (&End);
#endif
             }

          void partitionAlongAxis ( int Axis, bool Partition_Axis, int GhostBoundaryWidth );
          void applyPartition ();

#if !defined(USE_PADRE)
          static void displaySCHED  ( SCHED*  X );
          static void displayDARRAY ( DARRAY* X );
          static void displayDECOMP ( DECOMP* X );
#endif

          static void displayDefaultPartitioning ();
          void displayPartitioning ();

          void incrementReferenceCount () 
               { referenceCount++; };
          void decrementReferenceCount () 
               { referenceCount--; };
          int getReferenceCount () const 
               { return referenceCount; };
          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return APP_REFERENCE_COUNT_BASE;
             }

     private:
       // Used in the constructors!
          void Initialize_Internal_Partitioning_Type( const Range & Processor_Subcollection );
   };

class Partitioning_Type
   {
  // This is the envelope class for the partitioning object.  The Internal_Partitioning_Type
  // class is the letter.  Both classes share the same interface.  The reason for this
  // design is to allow the scope of the partitioning object to be decoupled from
  // the scope of the A++/P++ array object to which it is associated.  This
  // fixes a bug in P++ which was made difficult to understand within the user interface
  // (thus it was considered a bug).

  // friend Partitioning_Type doubleArray::getPartition () const;
  // friend Partitioning_Type floatArray::getPartition () const;
  // friend Partitioning_Type intArray::getPartition () const;
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     private:
       // Bugfix (4/11/96) We want to avoid the auto promotion of an 
       // Internal_Partitioning_Type to a Partitioning_Type.
          Partitioning_Type( Internal_Partitioning_Type & X );

     public:
       // this is the only data in this class
          Internal_Partitioning_Type *Internal_Partitioning_Object;

          Internal_Partitioning_Type* getInternalPartitioningObject() const;

         ~Partitioning_Type();
          Partitioning_Type();

       // Here we must define the interface for the Partitioning_Type objects!
       // This has not been specified yet.  However we should avoid the creation
       // of temporary partitions (thus the operations on partition_type objects
       // should not follow that of the array class because they don't have similar
       // memory requirements).

          int referenceCount;
          void incrementReferenceCount () 
               { referenceCount++; };
          void decrementReferenceCount () 
               { referenceCount--; };
          int getReferenceCount () const 
               { return referenceCount; };
          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return APP_REFERENCE_COUNT_BASE;
             }

       // This is intended to be a generic way to specify a distribution onto
       // a large collection of processors.  The map would be the same dimension
       // as the Virtual processor space (with for now in PARTI would be 1D).
          Partitioning_Type( const doubleArray & Load_Map );
          Partitioning_Type( int Number_Of_Processors );
          Partitioning_Type( const Range & Processor_Subcollection );
          Partitioning_Type ( const Partitioning_Type & X );
          Partitioning_Type & operator= ( const Partitioning_Type & X );

       // void SpecifyDecompositionAxes ( int Number_Of_Dimensions_To_Split , intArray & ArraySizes );
       // void SpecifyDecompositionAxes ( intArray & ArraySizes );
          void SpecifyDecompositionAxes ( int Input_Number_Of_Dimensions_To_Partition );

       // Specific the range of processor used by the default partitioning
          static void setDefaultProcessorRange ( const Range & Processor_Range );

       // These return an intArray since later any subset of processors will be available
#if defined(PPP)
          intSerialArray getProcessorSet ();
          static intSerialArray getDefaultProcessorSet ();
#else
          intArray getProcessorSet ();
          static intArray getDefaultProcessorSet ();
#endif

          static void SpecifyDefaultInternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
          void SpecifyInternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
          void SpecifyExternalGhostBoundaryWidths ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

       // Dynamically redistribute over a new range of processors
          static void swapDistribution ( const Partitioning_Type & OldDistribution , 
                                         const Partitioning_Type & newDistribution );
          void SpecifyProcessorRange ( const Range & Processor_Range );

#if !defined(USE_PADRE)
          static void SpecifyDecompositionEmbeddingIntoVirtualProcessorSpace (
                    DECOMP* BlockParti_Decomposition_Pointer ,
                    int StartingProcessor , int EndingProcessor );
#endif

       // void AssociateAplusplusArrayWithPartition ( const doubleArray & X );
       // void PartitionAplusplusArray ( const doubleArray & X );
       // void PartitionAplusplusArray ( const floatArray  & X );
       // void PartitionAplusplusArray ( const intArray    & X );

          static void AddArrayToPartitioning ( const doubleArray & X );
          static void AddArrayToPartitioning ( const Partitioning_Type & Partition, const doubleArray & X );
          static void AddArrayToPartitioning ( const floatArray  & X );
          static void AddArrayToPartitioning ( const Partitioning_Type & Partition, const floatArray  & X );
          static void AddArrayToPartitioning ( const intArray    & X );
          static void AddArrayToPartitioning ( const Partitioning_Type & Partition, const intArray    & X );

          static void DeleteArrayToPartitioning ( const doubleArray & X );
          static void DeleteArrayToPartitioning ( const Partitioning_Type & Partition, const doubleArray & X );
          static void DeleteArrayToPartitioning ( const floatArray  & X );
          static void DeleteArrayToPartitioning ( const Partitioning_Type & Partition, const floatArray  & X );
          static void DeleteArrayToPartitioning ( const intArray    & X );
          static void DeleteArrayToPartitioning ( const Partitioning_Type & Partition, const intArray    & X );

       // Allow user display of relavent partitioning data
          void display ( const char *Label = "" ) const;
          static void displayDefaultValues ( const char *Label = "" );

#if !defined(USE_PADRE)
          void Test_Consistency( DARRAY *BlockPartiArrayDomain , const char *Label = "" );
       // static void Test_Consistency( DARRAY *BlockPartiArrayDomain , const char *Label = "" );
       // void Test_Consistency( DARRAY *BlockPartiArrayDomain, const char *Label = "" ) const;

          DARRAY* Build_BlockPartiArrayDomain ( DECOMP* BlockPartiArrayDecomposition , int *Array_Sizes ,
                                                    int* InternalGhostCellWidth , int* ExternalGhostCellWidth );
          DARRAY* Build_BlockPartiArrayDomain ( int *Array_Sizes , int* InternalGhostCellWidth ,
                                                    int* ExternalGhostCellWidth );

       // Used internally in Build_BlockPartiDecompostion and Build_DefaultBlockPartiDecompostion
          static DECOMP* Build_BlockPartiDecompostion ( int *Array_Sizes, char* Local_Distribution_String,
                                                        int Local_Starting_Processor, int Local_Ending_Processor );

          DECOMP* Build_BlockPartiDecompostion    ( int *Array_Sizes );

          static DARRAY* Build_DefaultBlockPartiArrayDomain 
	     (DECOMP* BlockPartiArrayDecomposition , int *Array_Sizes ,
              const int* InternalGhostCellWidth, const int* ExternalGhostCellWidth);
          static DARRAY* Build_DefaultBlockPartiArrayDomain (
                              int *Array_Sizes , int* InternalGhostCellWidth , int* ExternalGhostCellWidth );
          static DECOMP* Build_DefaultBlockPartiDecompostion    ( int *Array_Sizes );
#endif

#if 0
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & This_Descriptor,
                                                          const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );

          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );

          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & X_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & This_Descriptor,
                                                          const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );

          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & X );
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries ( const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & X );

          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & X_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & This_Descriptor,
                                                          const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Rhs_Descriptor );

          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & X );
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries ( const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & X );
#else
#if 0
          template<class T, int Template_dimension>
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<T,Template_dimension> & X_Descriptor );
          template<class T, int Template_dimension>
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<T,Template_dimension> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<T,Template_dimension> & Rhs_Descriptor );
          template<class T, int Template_dimension>
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Descriptor_Type<T,Template_dimension> & This_Descriptor,
                                                          const Array_Descriptor_Type<T,Template_dimension> & Lhs_Descriptor,
                                                          const Array_Descriptor_Type<T,Template_dimension> & Rhs_Descriptor );

          template<class T, int Template_dimension>
          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Descriptor_Type<T,Template_dimension> & X );
          template<class T, int Template_dimension>
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries   ( const Array_Descriptor_Type<T,Template_dimension> & X );
#else
#if !defined(USE_SAGE)
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & X_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & Lhs_Descriptor,
                                                          const Array_Domain_Type & Rhs_Descriptor );
          static bool Has_Same_Ghost_Boundary_Widths ( const Array_Domain_Type & This_Descriptor,
                                                          const Array_Domain_Type & Lhs_Descriptor,
                                                          const Array_Domain_Type & Rhs_Descriptor );

#if !defined(USE_PADRE)
          static SCHED* BuildCommunicationScheduleUpdateAllGhostBoundaries ( const Array_Domain_Type & X );
          static SCHED* BuildCommunicationScheduleSendAllGhostBoundaries   ( const Array_Domain_Type & X );
#endif
#endif
#endif
#endif

       // These functions updates the ghost boundaries for the distributed array
          static void updateGhostBoundaries ( const doubleArray & X );
          static void updateGhostBoundaries ( const floatArray  & X );
          static void updateGhostBoundaries ( const intArray    & X );

       // These functions move a rectangular part of the Rhs (which might be distributed over many processors)
       // to the processors owning the specified rectangular part of the Lhs (the operation would typically
       // be between unaligned arrays and thus is a basis for the unaligned array operations).
       // In cases of no ghost boundaries (width zero) this is an expensive substitute for the
       // use of the ghost boundary update.
#if 0
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Lhs, const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & Rhs );
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Lhs, const Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> & Rhs );
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Lhs, const Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> & Rhs );
#else
#if 0
          template<class T, int Template_dimension>
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Descriptor_Type<T,Template_dimension> & Lhs, const Array_Descriptor_Type<T,Template_dimension> & Rhs );
#else
#if !defined(USE_PADRE)
          static SCHED* BuildCommunicationScheduleRegularSectionTransfer (
                    const Array_Domain_Type & Lhs, const Array_Domain_Type & Rhs );
#endif
#endif
#endif

#if defined(PPP)
          static void regularSectionTransfer ( const doubleArray & Lhs_Parallel,
                                               const doubleSerialArray & Lhs_SerialArray,
                                               const doubleArray & Rhs_ParallelArray,
                                               const doubleSerialArray & Rhs_SerialArray );
          static void regularSectionTransfer ( const floatArray & Lhs_Parallel,
                                               const floatSerialArray & Lhs_SerialArray,
                                               const floatArray & Rhs_ParallelArray,
                                               const floatSerialArray & Rhs_SerialArray );
          static void regularSectionTransfer ( const intArray & Lhs_Parallel,
                                               const intSerialArray & Lhs_SerialArray,
                                               const intArray & Rhs_ParallelArray,
                                               const intSerialArray & Rhs_SerialArray );
          static void regularSectionTransfer ( const doubleArray & Lhs_Parallel,
                                               const intSerialArray & Lhs_SerialArray,
                                               const intArray & Rhs_ParallelArray,
                                               const intSerialArray & Rhs_SerialArray );
          static void regularSectionTransfer ( const floatArray & Lhs_Parallel,
                                               const intSerialArray & Lhs_SerialArray,
                                               const intArray & Rhs_ParallelArray,
                                               const intSerialArray & Rhs_SerialArray );
#endif

          void partitionAlongAxis ( int Axis, bool Partition_Axis, int GhostBoundaryWidth );
          void applyPartition ();

          int getGhostBoundaryWidth (int i) const
             {
               APP_ASSERT (Internal_Partitioning_Object != NULL);
               return Internal_Partitioning_Object->getLocalGhostCellWidth(i);
             };

          static int getDefaultGhostBoundaryWidth (int i)
             {
               return Internal_Partitioning_Type::getDefaultInternalGhostCellWidth(i);
             };

#if !defined(USE_PADRE)
          static void displaySCHED  ( SCHED*  X );
          static void displayDARRAY ( DARRAY* X );
          static void displayDECOMP ( DECOMP* X );
#endif

          static void displayDefaultPartitioning ();
          void displayPartitioning ();
   };

  // _APP_PARTITIONING_H
#endif







