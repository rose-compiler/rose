// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma implementation "domain.h"
#endif

#include "A++.h"
extern "C"
   {
/* machine.h is found in MDI/machine.h through a link in A++/inc lude and P++/inc lude */
#include "machine.h"
   }

#define NEW_INDEX_BASE_INITIALIZATION TRUE

#define INLINE_DESCRIPTOR_SUPPORT FALSE
#define FILE_LEVEL_DEBUG 0

// This turns on the calls to the bounds checking function
#define BOUNDS_ERROR_CHECKING TRUE

// It helps to set this to FALSE sometimes for debugging code
// this enambles the A++/P++ operations in the bounds checking function
#define TURN_ON_BOUNDS_CHECKING   TRUE

// This comments out the construction of PADRE objects!
// define TEST_USE_OF_PADRE

#define INITIAL_REFERENCE_COUNT_ARRAY_SIZE 0
#define MAX_ARRAY_ID 1000000

// There is a simple test for P++'s use of PADRE but at the moment
// P++ fails the test on a few parts of the testppp.C (P++ testcode)
#define PADRE_DISTRIBUTION_CONSISTENCY_TEST FALSE

// This is to control the use of O(n^2) tests in the Test_Consistancy function 
// (normally turned off).  The specific test is to check the stack for redundent
// array ids.
#define USE_ORDER_N_SQUARED_TESTS FALSE

// Avoid the accedental case of leaving the n squared test in a final distribution
#if USE_ORDER_N_SQUARED_TESTS && !COMPILE_DEBUG_STATEMENTS
#error "USE_ORDER_N_SQUARED_TESTS is defined when optimized performance is expected"
#endif

// Used to support the Array_ID values! We start at 1 so that we can conside any
// value < 1 as a error! This also allows the idefs in the dependence analysis
// to be negative. This value is also used in the array objects destructor to
// trigger the cleanup of A++ internal memory in use. This prevents warnings from
// Purify.
#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
int APP_Global_Array_ID = 1;
#else
extern int APP_Global_Array_ID;
#endif

// defined in array.C
extern int APP_Global_Array_Base;
















// *****************************************************************************
// *****************************************************************************
// *********************  STATIC VARIABLE INITIALIZATION  **********************
// *****************************************************************************
// *****************************************************************************

// Already decleared in inline_func.h
// int Array_Domain_Type::Memory_Block_Index                = 0;
// const int Array_Domain_Type::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;
// unsigned char *Array_Domain_Type::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#if !defined(PPP)
// See note in header file as to how these are used and why they are important!
int  Array_Domain_Type::Array_Reference_Count_Array_Length = INITIAL_REFERENCE_COUNT_ARRAY_SIZE;
int* Array_Domain_Type::Array_Reference_Count_Array        = NULL;
bool Array_Domain_Type::smartReleaseOfInternalMemory    = FALSE;
bool Array_Domain_Type::exitFromGlobalMemoryRelease     = FALSE;
#endif

// Support for stack operations in the Array_Domain_Type.
// Push and Pop member functions of Array_Domain_Type use these
// to implement the stack operations.  A seperate Stack class was first
// implemented and it could not be a static class because static 
// classes are initialized in a different order for each C++ compiler
// different compilers.
int  Array_Domain_Type::Max_Stack_Size  = 0;
int  Array_Domain_Type::Stack_Depth     = 0;
int *Array_Domain_Type::Top_Of_Stack    = NULL;
int *Array_Domain_Type::Bottom_Of_Stack = NULL;

// we only want to declare these once
int  Array_Domain_Type::IndirectAddressingMap_Index = -1;
int* Array_Domain_Type::IndirectAddressingMap_Data [APP_STATIC_NUMBER_OF_MAPS];
int  Array_Domain_Type::IndirectAddressingMap_Minimum_Base [APP_STATIC_NUMBER_OF_MAPS];
int  Array_Domain_Type::IndirectAddressingMap_Maximum_Bound [APP_STATIC_NUMBER_OF_MAPS];
int  Array_Domain_Type::IndirectAddressingMap_Length [APP_STATIC_NUMBER_OF_MAPS];

// *****************************************************************************
// *****************************************************************************
// **********************   MEMBER FUNCTION DEFINITION  ************************
// *****************************************************************************
// *****************************************************************************

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
AppString::~AppString ()
   {
     delete internalString;
   }

AppString::AppString ()
   {
     internalString = NULL;
   }

AppString::AppString ( const char* c )
   {
     APP_ASSERT (c != NULL);
#if defined(USE_STRING_SPECIFIC_CODE)
     internalString = strdup(c);
#else
     printf ("String functions not available: see use of USE_STRING_SPECIFIC_CODE! \n");
     APP_ABORT();
#endif
   }

AppString::AppString ( const AppString & X )
   {
     APP_ASSERT (X.internalString != NULL);
#if defined(USE_STRING_SPECIFIC_CODE)
     internalString = strdup(X.internalString);
#else
     printf ("String functions not available: see use of USE_STRING_SPECIFIC_CODE! \n");
     APP_ABORT();
#endif
   }

AppString & 
AppString::operator= ( const AppString & X )
   {
     delete internalString;
     APP_ASSERT (X.internalString != NULL);
#if defined(USE_STRING_SPECIFIC_CODE)
     internalString = strdup(X.internalString);
#else
     printf ("String functions not available: see use of USE_STRING_SPECIFIC_CODE! \n");
     APP_ABORT();
#endif
     return *this;
   }

char*
AppString::getInternalString() const
   {
  // Our semantics require the user to delete strings obtained from getInternalString()!
     APP_ASSERT (internalString != NULL);
#if defined(USE_STRING_SPECIFIC_CODE)
     return strdup(internalString);
#else
     printf ("String functions not available: see use of USE_STRING_SPECIFIC_CODE! \n");
     APP_ABORT();
     return NULL;
#endif
   }

void
AppString::setInternalString( const char* c )
   {
     APP_ASSERT (c != NULL);
#if defined(USE_STRING_SPECIFIC_CODE)
     internalString = strdup(c);
#else
     printf ("String functions not available: see use of USE_STRING_SPECIFIC_CODE! \n");
     APP_ABORT();
#endif
   }
#endif

inline
int*
Array_Domain_Type::getDefaultIndexMapData ( int Local_Min_Base, int Local_Max_Bound ) const
   {
  /* This function returns an integer map that can be used to replace
     the constant stride index information that is not readily implemented
     in the MDI loops for indirect addressing.  In the multithreaded
     environment this array represents a shared resource.
  */

  /* The Minimum_Base and Maximum_Bound determine the size of the
     IndirectAddressingStaticMap and the Index_Size tells us what the dimension lengths
     must be (in order to be conformable with the other Index arrays
     in the case where they are multi dimensional).  The Minimum_Base
     also tells us where in the IndirectAddressingStaticMap we are to posision the
     pointer.  The Stride must be set to that of the arrays view which
     we are indexing -- but is constant in each dimension of the
     multidimensional part of the index array. Opps, it is only non
     unitary in the first dimension (if stride > 1)
  */

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("At TOP: IndirectAddressingMap_Index = %d Local_Min_Base = %d  Local_Max_Bound = %d \n",
               IndirectAddressingMap_Index,Local_Min_Base,Local_Max_Bound); 
#endif
     if (IndirectAddressingMap_Index == -1)
        {
       // Initial setup (this is called only once)
          IndirectAddressingMap_Index = 0;
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("SETUP of Map pointers in Array_Domain_Type::getDefaultIndexMapData \n");
#endif
          for (int i=0; i < APP_STATIC_NUMBER_OF_MAPS; i++)
             {
               IndirectAddressingMap_Data          [i] = NULL;
               IndirectAddressingMap_Minimum_Base  [i] = INT_MAX;
               IndirectAddressingMap_Maximum_Bound [i] = INT_MIN;
               IndirectAddressingMap_Length        [i] = 0;
            }
        }
  // if (IndirectAddressingMap_Data[IndirectAddressingMap_Index] == NULL)
  //    {
  //      IndirectAddressingMap_Minimum_Base  [IndirectAddressingMap_Index] = INT_MAX;
  //      IndirectAddressingMap_Maximum_Bound [IndirectAddressingMap_Index] = INT_MIN;
  //      IndirectAddressingMap_Length        [IndirectAddressingMap_Index] = 0;
  //    }

     int i;

  /* allocate memory for IndirectAddressingStaticMap if necessary */

     int NewBase  = (Local_Min_Base  < IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index] ) ?
                     Local_Min_Base : IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index];
     int NewBound = (Local_Max_Bound  > IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index] ) ?
                     Local_Max_Bound : IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index];
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("NewBase = %d  NewBound = %d \n",NewBase,NewBound);
#endif
     if (NewBound - NewBase >= IndirectAddressingMap_Length[IndirectAddressingMap_Index])
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Increment the IndirectAddressingMap_Index \n");
#endif
          IndirectAddressingMap_Index++;
          if (IndirectAddressingMap_Index == APP_STATIC_NUMBER_OF_MAPS)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Reset the IndirectAddressingMap_Index to ZERO! \n");
#endif
               IndirectAddressingMap_Index = 0;
             }
        }

  // if (Local_Min_Base  < IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index] )
  //      IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]  = Local_Min_Base;
  // if (Local_Max_Bound > IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index])
  //      IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index] = Local_Max_Bound;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("IndirectAddressingMap_Minimum_Base[%d]  = %d \n",
               IndirectAddressingMap_Index,IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]);
          printf ("IndirectAddressingMap_Maximum_Bound[%d] = %d \n",
               IndirectAddressingMap_Index,IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index]);
          printf ("IndirectAddressingMap_Length[%d]        = %d \n",
               IndirectAddressingMap_Index,IndirectAddressingMap_Length[IndirectAddressingMap_Index]);
        }
#endif

  // if (IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index] - 
  //     IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index] >= 
  //     IndirectAddressingMap_Length[IndirectAddressingMap_Index])
     if (NewBound - NewBase >= IndirectAddressingMap_Length[IndirectAddressingMap_Index])
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Allocating new memory: IndirectAddressingMap_Index = %d \n",IndirectAddressingMap_Index);
#endif

          if (Local_Min_Base  < IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index] )
               IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]  = Local_Min_Base;
          if (Local_Max_Bound > IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index])
               IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index] = Local_Max_Bound;

          IndirectAddressingMap_Length[IndirectAddressingMap_Index] = 
               (IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index] - 
                IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]) + 1;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("New value for IndirectAddressingMap_Minimum_Base[%d]  = %d \n",
                    IndirectAddressingMap_Index,IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]);
               printf ("New value for IndirectAddressingMap_Maximum_Bound[%d] = %d \n",
                    IndirectAddressingMap_Index,IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index]);
               printf ("New value for IndirectAddressingMap_Length[%d] = %d \n",
                    IndirectAddressingMap_Index,IndirectAddressingMap_Length[IndirectAddressingMap_Index]); 
             }
#endif

          if (IndirectAddressingMap_Data[IndirectAddressingMap_Index] == NULL)
               IndirectAddressingMap_Data[IndirectAddressingMap_Index] = 
                    (int*) malloc ( IndirectAddressingMap_Length[IndirectAddressingMap_Index] * sizeof(int*) );
            else
               IndirectAddressingMap_Data[IndirectAddressingMap_Index] =
                    (int*) realloc ( (char*) IndirectAddressingMap_Data[IndirectAddressingMap_Index] , 
                                     IndirectAddressingMap_Length[IndirectAddressingMap_Index] * sizeof(int*) );

       /* Make sure this vectorizes if possible! */
          for (i=IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]; 
               i <= IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index]; i++)
             {
               IndirectAddressingMap_Data[IndirectAddressingMap_Index]
                                         [i-IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]] = i;
             }
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("Values in IndirectAddressingStaticMap: \n");
               for (i=IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]; 
                    i <= IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index]; i++)
                  {
                 // printf ("%d ",IndirectAddressingMap_Data[IndirectAddressingMap_Index][i]);
                 // printf ("%d ",i);
                    printf ("%d ",IndirectAddressingMap_Data[IndirectAddressingMap_Index]
                                                            [i-IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]]);
                  }
               printf ("\n");
             }
#endif
        }

  // Compute the address of the first position within the IndirectAddressingMap_Data that we require
  // return &(IndirectAddressingMap_Data[IndirectAddressingMap_Index]
  //                                    [-IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]]);
  // int* ReturnValue = &(IndirectAddressingMap_Data[IndirectAddressingMap_Index]
  //                                    [-IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]]);

  // Use pointer arithmetic to compute the offset (rather than indexing the position and taking the address)
  // This also avoids a warning from PURIFY
     int* ReturnValue = IndirectAddressingMap_Data[IndirectAddressingMap_Index] - 
                        IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index];
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("ReturnValue = %p \n",ReturnValue);
#endif
     return ReturnValue;
   }

void
Array_Domain_Type::setupIndirectAddressingSupport() const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::setupIndirectAddressingSupport() \n");
#endif

  // Test to see what sort of indexing is done here
  // We only really need to iterate over the range 0...InternalDimension
  // but we can do this later
  // We can make the indexing more efficient by modifying the
  // IndexDataPointer using the IndexBase.
     for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if ( (usesIndirectAddressing() == TRUE) && (Index_Array[i] != NULL) )
             {
            // For each axis check to see what type of indexing was used (since it could be
            // either intArray Internal_Index or Scalar)
            // An intArray was used for indexing this axis
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("operand uses indirect addressing and uses intArray along axis %d \n",i);
#endif
               APP_ASSERT(Index_Array[i] != NULL);
               APP_ASSERT(Index_Array[i]->getDataPointer() != NULL);
               ((Array_Domain_Type*)(this))->IndexBase        [i] = Index_Array[i]->Array_Descriptor.Array_Domain.Base   [i];
               ((Array_Domain_Type*)(this))->IndexStride      [i] = Index_Array[i]->Array_Descriptor.Array_Domain.Stride [i];
               ((Array_Domain_Type*)(this))->IndexDataPointer [i] = Index_Array[i]->getDataPointer();
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                  {
                    printf ("Test out the intArray used for indexing \n");
                    printf ("Base [%d] = %d \n",i,Base[i]);
                    printf ("Bound[%d] = %d \n",i,Bound[i]);
                    printf ("Index_Array[%d]->Array_Descriptor.Array_Domain.Base   [0] = %d \n",i,
                         Index_Array[i]->Array_Descriptor.Array_Domain.Base   [0]);
                    printf ("Index_Array[%d]->Array_Descriptor.Array_Domain.Stride   [0] = %d \n",i,
                         Index_Array[i]->Array_Descriptor.Array_Domain.Stride   [i]);
                    printf ("IndexDataPointer[%d] = %p \n",i,IndexDataPointer[i]);
                    intArray* IndexArrayPointer = NULL;
                    for (int n=0; n < MAX_ARRAY_DIMENSION; n++)
                         if (Index_Array[n] != NULL)
                              IndexArrayPointer = Index_Array[n];
                    APP_ASSERT (IndexArrayPointer != NULL);
                    for (int j  = IndexArrayPointer->Array_Descriptor.Array_Domain.Base[0];
                             j <= IndexArrayPointer->Array_Descriptor.Array_Domain.Bound[0];
                             j += IndexArrayPointer->Array_Descriptor.Array_Domain.Stride[0])
                       printf ("%d ",IndexDataPointer[i][j]);
                    printf ("\n");
                  }
#endif
             }
            else
             {
            // either an Internal_Index or a scalar was used for indexing this axis
            // An Internal_Index was used for indexing this axis
               if (Base[i] == Bound[i])
                  {
                 // Then we have scalar indexing in use
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("operand uses scalar indexing along axis %d \n",i);
#endif
                    ((Array_Domain_Type*)(this))->IndexBase        [i] = Data_Base[i] + Base[i];
                    ((Array_Domain_Type*)(this))->IndexStride      [i] = 0;
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("operand uses Internal_Index indexing along axis %d \n",i);
#endif
                    ((Array_Domain_Type*)(this))->IndexBase        [i] = getBase(i);
                    ((Array_Domain_Type*)(this))->IndexStride      [i] = getStride(i);
                  }

            // Is this done correctly here?

            // get space for IntegerList
               int localBase  = IndexBase[i];
               int localBound = IndexBase[i] + (getBound(i)-getBase(i));
               ((Array_Domain_Type*)(this))->IndexDataPointer [i] = getDefaultIndexMapData (localBase,localBound);
               APP_ASSERT (IndexDataPointer [i] != NULL);
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                  {
                    printf ("Test out the range from getDefaultIndexMapData \n");
                    printf ("Base [%d] = %d \n",i,Base[i]);
                    printf ("Bound[%d] = %d \n",i,Bound[i]);
                    printf ("getBase  (%d) = %d \n",i,getBase(i));
                    printf ("getBound (%d) = %d \n",i,getBound(i));
                    printf ("getStride(%d) = %d \n",i,getStride(i));
                    printf ("IndexBase       [%d] = %d \n",i,IndexBase[i]);
                    printf ("IndexStride     [%d] = %d \n",i,IndexStride[i]);
                    printf ("IndexDataPointer[%d] = %p \n",i,IndexDataPointer[i]);
                    for (int j=localBase; j <= localBound; j++)
                       printf ("%d ",IndexDataPointer[i][j]);
                    printf ("\n");
                  }
#endif
             }
        }
   }

// *****************************************************************************
// *****************************************************************************
// *********  NEW OPERATOR INITIALIZATION MEMBER FUNCTION DEFINITION  **********
// *****************************************************************************
// *****************************************************************************

void
Array_Domain_Type::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

// *****************************************************************************
// *****************************************************************************
// ****************  MEMORY CLEANUP MEMBER FUNCTION DEFINITION  ****************
// *****************************************************************************
// *****************************************************************************

void
Array_Domain_Type::freeMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Array_Domain_Type::freeMemoryInUse() \n");
#endif 

  // This function is useful in conjuction with the Purify (from Pure Software Inc.)
  // it frees memory allocated for use internally in A++ <type>Array objects.
  // This memory is used internally and is reported as "in use" by Purify
  // if it is not freed up using this function.  This function works with
  // similar functions for each A++ object to free up all of the A++ memory in
  // use internally.

#if !defined(PPP)
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          printf ("Cleaning up the diagnosticInfoArray stuff! \n");
          if (Diagnostic_Manager::diagnosticInfoArray != NULL)
             {
               APP_ASSERT (Array_Reference_Count_Array_Length > 0);
               for (int i=0; i < Array_Reference_Count_Array_Length; i++) 
                  {
                    if (Diagnostic_Manager::diagnosticInfoArray[i] != NULL)
                         delete Diagnostic_Manager::diagnosticInfoArray[i];
                    Diagnostic_Manager::diagnosticInfoArray[i] = NULL;
                  }
             }
        }

  // free memory allocated for reference counting!
     if (Array_Reference_Count_Array != NULL)
          free ((char*) Array_Reference_Count_Array);

  // Reset the size of the reference count array
     Array_Reference_Count_Array_Length = 0;
#endif

  // free memory allocated for memory pools!
     for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++) 
          if (Memory_Block_List [i] != NULL)  
               free ((char*) (Memory_Block_List[i]));
   }

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
void
GlobalMemoryRelease()
   {
  // Call static member functions of A++ objects to free memory in use (internally)
  // This be call AFTER ALL A++ objects have had their destructors called since the
  // A++ destructors must reference the memory that this function will free!

     printf ("\n");
     printf ("\n");
     printf ("\n");
     printf ("Inside of GlobalMemoryRelease -- Freeing up A++ internal memory usage! \n");

  // Turn this off now to about recursive calls to this functions 
  // when we delete the remaining internal array objects
     Diagnostic_Manager::setSmartReleaseOfInternalMemory( OFF );

  // Cleanup previous where mask and history saved in the Where_Statement_Support class
     Where_Statement_Support::cleanup_after_Where();

#if defined (SERIAL_APP)
  // free Serial_A++

  // Cleanup previous where mask and history saved in the Where_Statement_Support class
     Where_Statement_Support::cleanup_after_Serial_Where();

     SerialArray_Domain_Type    ::freeMemoryInUse();
     doubleSerialArray          ::freeMemoryInUse();
     floatSerialArray           ::freeMemoryInUse();
     intSerialArray             ::freeMemoryInUse();

     doubleSerialArray_Descriptor_Type::freeMemoryInUse();
     floatSerialArray_Descriptor_Type ::freeMemoryInUse();
     intSerialArray_Descriptor_Type   ::freeMemoryInUse();

  // Internal_Partitioning_Type::freeMemoryInUse();

#if defined(USE_PADRE)
  // We only have to call this member function the function for cleaning
  // up the memory used within PADRE are called by this function
     PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::freeMemoryInUse();
#endif

  // Cleanup the memory used for the stack of Array_IDs in use
     SerialArray_Domain_Type::Top_Of_Stack    = NULL;
  // delete SerialArray_Domain_Type::Bottom_Of_Stack;
     free (SerialArray_Domain_Type::Bottom_Of_Stack);
     SerialArray_Domain_Type::Bottom_Of_Stack = NULL;

     Array_Conformability_Info_Type::freeMemoryInUse();

#if !defined(USE_PADRE)
     if (Communication_Manager::VirtualProcessorSpace != NULL)
          delete_VPROC (Communication_Manager::VirtualProcessorSpace);
     Communication_Manager::VirtualProcessorSpace = NULL;
#endif

#if defined(USE_PADRE)
  // We only have to call this member function the function for cleaning
  // up the memory used within PADRE are called by this function
  // PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::freeMemoryInUse();
     PADRE::cleanUpAfterPadre();
#else
  // Cleanup memory used withing PARTI (special parti function written by Dan Quinlan)
     cleanup_after_PARTI();
#endif
#endif // end SERIAL_APP

  // free A++ or P++ data

  // Cleanup previous where mask and history saved in the Where_Statement_Support class
  // Where_Statement_Support::cleanup_after_Where();

     Array_Domain_Type::freeMemoryInUse();

     doubleArray          ::freeMemoryInUse();
     floatArray           ::freeMemoryInUse();
     intArray             ::freeMemoryInUse();
 
     doubleArray_Descriptor_Type::freeMemoryInUse();
     floatArray_Descriptor_Type ::freeMemoryInUse();
     intArray_Descriptor_Type   ::freeMemoryInUse();

#if defined(SERIAL_APP)
  // Bugfix (2/14/96) had to move the call to this function from up above.
  // The list within this objectmust remain valid while any P++ objects still exist!
     Internal_Partitioning_Type::freeMemoryInUse();
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)
  // Clean up MDI arrays
     cleanup_after_MDI();
#endif

  // Cleanup the memory used for the stack of Array_IDs in use
     Array_Domain_Type::Top_Of_Stack    = NULL;
  // delete Array_Domain_Type::Bottom_Of_Stack;
     free (Array_Domain_Type::Bottom_Of_Stack);
     Array_Domain_Type::Bottom_Of_Stack = NULL;
  // APP_Top_Of_Stack    = NULL;
  // delete APP_Bottom_Of_Stack;
  // APP_Bottom_Of_Stack = NULL;

  // delete the internall diagnostic information
     Diagnostic_Manager::freeMemoryInUse();

  // delete the MPI communicator
     Optimization_Manager::freeMemoryInUse();

  // There are other objects that might have to be freed!  NOT AFTER cleanup_after_MDI()
  // printf ("There might be some other objects that should be freed (within the MDI layer)! \n");

     printf ("Inside of GlobalMemoryRelease -- Freed up internal memory usage! \n");

     printf ("\n");
     printf ("********************************************************************* \n");
     printf ("WARNING: This MUST be the last A++/P++ related statement in your program! \n");
     printf ("********************************************************************* \n");

  // This mechanism forces an application to exit from this function which permits
  // the use of a return statement with a non-zero value to indicate an error if 
  // this function was not called (so we can be certain that the GlobalMemoryRelease()
  // function is called).  This mechanism needs to be optional.
     if (Diagnostic_Manager::getExitFromGlobalMemoryRelease() == TRUE)
        {
          printf ("NOTE: Exiting from GlobalMemoryRelease() as requested! \n");

       // Exit normally (it is assumed that the user will have specified the return statement
       // in the main program to returna nonzero value).  So then this exit would be correct
       // and if this function (GlobalMemoryRelease()) were not called it would be reported
       // as an error in the $status (which testing programs like automake's "make check" 
       // will look at).
       // exit(0);

       // A++ exit function (called purify if purify is used, etc.)
          APP_Exit();
        }
       else
        {
#if 0
       // Even if we don't use the getExitFromGlobalMemoryRelease() function we want to call the
       // purify exit function at program exit (but only when running with purify, else just exit using the
       // applications exit mechanism)
#if defined(USE_PURIFY)
       // If purify is used (configured and is actually running) then force the exit 
       // status to reflect any errors found when running with purify 
       // (access errors, memory leaks, and memory in use) these bool values 
       // are ORed into the status (ZERO is used as input)
          if (purify_is_running() == TRUE)
             {
               printf ("Exiting ... A++/P++ is configured to use PURIFY and (purify_is_running() == TRUE) \n");
               purify_exit(0);
             }
            else
             {
            // printf ("NOTE: NOT Exiting from GlobalMemoryRelease() ... \n");
             }
#else
       // printf ("NOTE: NOT Exiting from GlobalMemoryRelease() ... \n");
#endif
#endif
        }
   }

// This object must be built first (this can be a problem with static A++ objects)
MemoryManagerType::MemoryManagerType ()
   {
  // Nothing to initialize in constructor!
   }

MemoryManagerType::~MemoryManagerType ()
   {
     GlobalMemoryRelease();
   }
  /* end of APP || ( (SERIAL_APP) && !(PPP) )*/
#endif

// *****************************************************************************
// *****************************************************************************
// ************************  ARRAY ID STACK MANAGEMENT  ************************
// *****************************************************************************
// *****************************************************************************

int
Array_Domain_Type::checkForArrayIDOnStack ( int x )
   {
     int numberOfReferences = 0;

#if USE_ORDER_N_SQUARED_TESTS
  // Search the stack for the existing Array_ID in use (it should not be there!)
  // We might also search for redundent entries -- this is not implemented yet
  // since this would be expensive.
     int* StackEntry = Top_Of_Stack;
     if (StackEntry != NULL)
        {
       // Note Pointer arithmatic
          while (StackEntry > Top_Of_Stack - Stack_Depth)
             {
            // Top_Of_Stack = Bottom_Of_Stack + Stack_Depth;
            // printf ("In while loop: *StackEntry = %d == internalArrayID = %d \n",*StackEntry,internalArrayID);
            // printf ("*StackEntry     = %d \n",*StackEntry);
            // printf ("internalArrayID = %d \n",internalArrayID);
               if (*StackEntry == x)
                  {
                    numberOfReferences++;
                 // printf ("*StackEntry = %d == x = %d numberOfReferences = %d \n",*StackEntry,x,numberOfReferences);
                  }

            // APP_ASSERT (*StackEntry != x);
               StackEntry--;
             }
        }
#else
     if (x);  // use this to avoid compiler warning about non-use of "x"
#endif

     return numberOfReferences;
   }


void
Array_Domain_Type::Push_Array_ID ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if ( (APP_DEBUG > 3) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
          printf ("Inside of Push (x=%d) Stack_Depth = %d Max_Stack_Size = %d \n",x,Stack_Depth,Max_Stack_Size);
#endif

#if USE_ORDER_N_SQUARED_TESTS
  // This is part of an expensive O(n^2) operation so use it saringly as a test
     APP_ASSERT (checkForArrayIDOnStack (x) == 0);
#endif

     Stack_Depth++;
     if (Stack_Depth >= Max_Stack_Size)
        {
       // printf ("Increasing the stack size! \n");
       // Stack size must be increased to acount for more valued push onto it!
          Max_Stack_Size += 100;
       // Old 4.1.3 systems must malloc and can not allow realloc to take a NULL pointer!
          if (Bottom_Of_Stack == NULL)
               Bottom_Of_Stack = (int*) APP_MALLOC ( Max_Stack_Size * sizeof(int) );
            else
               Bottom_Of_Stack = (int*) realloc ( (char*) Bottom_Of_Stack , Max_Stack_Size * sizeof(int) );

       // Pointer arithmatic
          Top_Of_Stack = Bottom_Of_Stack + Stack_Depth;
        }
       else
        {
       // Stack depth sufficient for another value!
          Top_Of_Stack++;
        }

  // printf ("Bottom_Of_Stack = %d Top_Of_Stack = %d \n",Bottom_Of_Stack,Top_Of_Stack);

  // Since this is a new array object is should have an initialize reference count on its
  // raw data.  This is required here because the reference counting mechanism reused the
  // value of zero for one existing reference and no references (this will be fixed soon).
  // resetRawDataReferenceCount();
  // Array_Reference_Count_Array [x] = getRawDataReferenceCountBase();

#if 0
  // Old arrays returning their array id's should be associated with a reference 
  // count equal to Array_Domain_Type::getRawDataReferenceCountBase()-1
#if defined(PPP) || defined(SERIAL_APP)
     APP_ASSERT (SerialArray_Domain_Type::Array_Reference_Count_Array [x] == SerialArray_Domain_Type::getRawDataReferenceCountBase()-1);
#else
     APP_ASSERT (Array_Domain_Type::Array_Reference_Count_Array [x] == Array_Domain_Type::getRawDataReferenceCountBase()-1);
#endif
#endif

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
  // For P++ we only track the serial array objects not the serial array objects
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[x] != NULL);
          delete Diagnostic_Manager::diagnosticInfoArray[x];
          Diagnostic_Manager::diagnosticInfoArray[x] = NULL;
        }
#endif

     *Top_Of_Stack = x;
   }

#if 0
int
Array_Domain_Type::queryNextArrayID ()
   {
  // This function returns what woud be the next value from the stack of Array_IDs

     int Result;
 
     if (Stack_Depth <= 0)
        {
       // Get value from Global_Array_ID (but don't modify the APP_Global_Array_ID!)
          Result = APP_Global_Array_ID;
        }
       else
        {
       // Get value on stack (but don't modify the stack!)
          if (Top_Of_Stack != NULL)
               Result = *Top_Of_Stack;
            else
               Result = -100;
        }

     APP_ASSERT (Result > 0);

     return Result;
   }
#endif

int
Array_Domain_Type::Pop_Array_ID ()
   {
  // This function provides the stack mechanism for the Array_IDs

     int Result;
 
     if (Stack_Depth <= 0)
        {
       // Get value from Global_Array_ID!
          Result = APP_Global_Array_ID++;
        }
       else
        {
       // Get value on stack!
          Result = *Top_Of_Stack;
          Stack_Depth--;
          Top_Of_Stack--;

#if 0
       // reused array id's should be associated with a reference count equal to
       // Array_Domain_Type::getRawDataReferenceCountBase()-1
#if defined(PPP) || defined(SERIAL_APP)
          APP_ASSERT (SerialArray_Domain_Type::Array_Reference_Count_Array [Result] == SerialArray_Domain_Type::getRawDataReferenceCountBase()-1);
#else
          APP_ASSERT (Array_Domain_Type::Array_Reference_Count_Array [Result] == Array_Domain_Type::getRawDataReferenceCountBase()-1);
#endif
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if ( (APP_DEBUG > 3) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
          printf ("Inside of Pop_Array_ID -- Global_Array_ID = %d Result = %d Stack_Depth = %d \n",
               APP_Global_Array_ID,Result,Stack_Depth);
#endif
 
     if (Result >= MAX_ARRAY_ID)
        {
          printf ("WARNING: Inside of Pop_Array_ID  Number (%d) of outstanding array object in excess of %d \n",Result,MAX_ARRAY_ID);
        }
  // APP_ASSERT (Result < MAX_ARRAY_ID);

  // printf ("NOTE: Inside of Pop_Array_ID  new array id = %d \n",Result);

#if defined(PPP) || defined(SERIAL_APP)
  // Since this is a new array object is should have an initialize reference count on its
  // raw data.  This is required here because the reference counting mechanism reused the
  // value of zero for one existing reference and no references (this will be fixed soon).
  // resetRawDataReferenceCount();
  // Array_Reference_Count_Array [Result] = getRawDataReferenceCountBase();
     if (Result >= SerialArray_Domain_Type::Array_Reference_Count_Array_Length)
          SerialArray_Domain_Type::Allocate_Larger_Reference_Count_Array ( Result );

#if defined(SERIAL_APP)
  // The local array associated with a P++ array (a SerialArray) uses the Array_Reference_Count_Array
  // mechanism to provide reference counting on the raw array data associated with an array object.
     SerialArray_Domain_Type::Array_Reference_Count_Array [Result] = 
          SerialArray_Domain_Type::getRawDataReferenceCountBase();
#else
  // For P++ array objects the Array_Reference_Count_Array mechanism is NOT used so set it to ZERO
  // to indicate that it is not used. (Note: SerialArray_Domain_Type::getRawDataReferenceCountBase() - 1 == 0)
     APP_ASSERT (SerialArray_Domain_Type::getRawDataReferenceCountBase() - 1 == 0);
     SerialArray_Domain_Type::Array_Reference_Count_Array [Result] = 
          SerialArray_Domain_Type::getRawDataReferenceCountBase() - 1;
#endif
#else
     if (Result >= Array_Domain_Type::Array_Reference_Count_Array_Length)
          Array_Domain_Type::Allocate_Larger_Reference_Count_Array ( Result );
     Array_Domain_Type::Array_Reference_Count_Array [Result] = Array_Domain_Type::getRawDataReferenceCountBase();
#endif

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
  // For P++ we only track the serial array objects not the serial array objects
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[Result] == NULL);
          Diagnostic_Manager::diagnosticInfoArray[Result] = new DiagnosticInfo(*this);
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[Result] != NULL);
        }
#endif

#if defined(PPP)
  // Bugfix (11/4/2000)
  // P++ arrays should not use the SerialArray_Domain_Type::Array_Reference_Count_Array
  // for tracking reference counts (we need to detect this since it is an error).
     APP_ASSERT (SerialArray_Domain_Type::Array_Reference_Count_Array[Result] == getReferenceCountBase()-1);
#endif

     return Result;
   }

// *****************************************************************************
// *****************************************************************************
// ***********************  MEMBER FUNCTION DEFINITIONS  ***********************
// *****************************************************************************
// *****************************************************************************

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
int
Array_Domain_Type::getNumberOfArraysInUse()
   {
  // This function reports the number of arrays in use within A++/P++
  // it is used in the array object destructors where if internal 
  // debugging is turned ON it optionally permits specialized cleanup of internal
  // memory used in A++/P++ which would otherwise lead to misleading messages from PURIFY.
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Array_Domain_Type::getNumberOfArraysInUse() \n");
#endif 

  // save the current state and turn off the output of node number prefix to all strings
     bool printfState = Communication_Manager::getPrefixParallelPrintf();
     Communication_Manager::setPrefixParallelPrintf(FALSE);

     int ValidArrayCount = 0;

  // Handle case of getNumberOfArraysInUse() being called before any array is allocated (return 0)
     if (Array_Reference_Count_Array != NULL)
        {
          APP_ASSERT (Array_Reference_Count_Array != NULL);

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Array_Reference_Count_Array [0-%d]:",Array_Reference_Count_Array_Length-1);
#endif
#if 0
          int i = 0;
          for (i=0; i < Array_Reference_Count_Array_Length; i++)
             {
            // (DQ 5/1/2000): This is now fixed!
            // What constitues a valid reference count is an issue to be fixed within A++/P++
            // since presently zero means two different things.  Soon this will be fixed so that
            // an existing array object with no external references to its data will have a reference
            // count of 1 (instead of zero as it does now).  This this mechnaism will work properly.
            // So at the moment this function does NOT work properly.
            // if ( (Array_Reference_Count_Array [i] >= APP_REFERENCE_COUNT_BASE) &&
            //      (typeCode == APP_UNDEFINED_ELEMENT_TYPE) )
               if (Array_Reference_Count_Array [i] >= APP_REFERENCE_COUNT_BASE)
                  {
                    ValidArrayCount++;
                  }

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf (" %d",Array_Reference_Count_Array [i]);
#endif
             }
#else
       // For P++ we only track the serial array objects not the paralel array objects ???
          if (Diagnostic_Manager::getTrackArrayData() == TRUE)
             {
               APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
            // This is a more accurate way of counting because it does not include 
            // Array_Domain_Type objects used by PADRE
               int i = 0;
               for (i=0; i < Array_Reference_Count_Array_Length; i++)
                  {
#if COMPILE_DEBUG_STATEMENTS
                 // Error checking
                    if (Array_Reference_Count_Array [i] >= APP_REFERENCE_COUNT_BASE)
                         APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[i] != NULL);
#if 0
                    if (Diagnostic_Manager::diagnosticInfoArray[i] != NULL)
                         APP_ASSERT (Array_Reference_Count_Array [i] >= APP_REFERENCE_COUNT_BASE);
#endif
#endif

                    if ( (Diagnostic_Manager::diagnosticInfoArray[i] != NULL) &&
                         (Diagnostic_Manager::diagnosticInfoArray[i]->getTypeCode() != APP_UNDEFINED_ELEMENT_TYPE) )
                       {
                         ValidArrayCount++;
                       }
                  }
             }
            else
             {
            // This is less accurate since many array domain type objects used by PADRE are counted
               int i = 0;
               for (i=0; i < Array_Reference_Count_Array_Length; i++)
                  {
                    if (Array_Reference_Count_Array [i] >= APP_REFERENCE_COUNT_BASE)
                       {
                         ValidArrayCount++;
                       }
                  }
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("\n");
          printf ("ValidArrayCount = %d \n",ValidArrayCount);
        }
#endif

  // reset output of node number prefix to all strings
     Communication_Manager::setPrefixParallelPrintf(printfState);

     APP_ASSERT (ValidArrayCount >= 0);

     return ValidArrayCount;
   }
#endif /*end of  APP || ( SERIAL_APP) && !(PPP) )*/

// *****************************************************************************
// *****************************************************************************
// ********************  DISPLAY MEMBER FUNCTION DEFINITION  *******************
// *****************************************************************************
// *****************************************************************************

void
Array_Domain_Type::display ( const char *Label ) const
   {
     printf ("Array_Domain_Type::display() -- %s \n",Label);

  // save the current state and turn off the output of node number prefix to all strings
     bool printfState = Communication_Manager::getPrefixParallelPrintf();
     Communication_Manager::setPrefixParallelPrintf(FALSE);

  // We want to remove any dependence upon the diagnostic mechanism
  // printf ("Array_Domain is used in a %sArray (element size = %d) \n",getTypeString(),getTypeSize());
     printf ("Array_Domain is %s \n",(Is_A_View                      == TRUE) ? "VIEW" : "NON-VIEW");
     printf ("Array_Domain is %s \n",(Is_A_Temporary                 == TRUE) ? "TEMPORARY" : "NON-TEMPORARY");
     printf ("Array_Domain is %s \n",(Is_Contiguous_Data             == TRUE) ? "CONTIGUOUS" : "NON-CONTIGUOUS");
     printf ("Array_Domain is %s \n",(Is_A_Null_Array                == TRUE) ? "NULL" : "NON-NULL");
     printf ("Array_Domain Uses_Indirect_Addressing is %s \n",(Uses_Indirect_Addressing          == TRUE) ? "TRUE" : "FALSE");
     printf ("Array_Domain is %s \n",(Is_Built_By_Defered_Evaluation == TRUE) ? "CONSTRUCTED DURING DEFERED EVALUATION" : "NON-DEFERED (Standard Construction)");

     printf ("Domain_Dimension     = %d \n",Domain_Dimension);

#if 0
  // This was added to as part of the debugging which discovered that the use of the
  // BaseArray as a base class adds 4 bytes to the lenght of the array objects and thus
  // tends to effect the size of just about everything (like the intArrays used in the 
  // indirect addressing). This detail of C++ might be compiler dependent.
     printf ("***** Address of THIS this            = %p ***** \n",this);
     printf ("***** Address of Domain_Dimension     = %p ***** \n",&Domain_Dimension);
     printf ("***** Address of Size                 = %p ***** \n",&Size);
     printf ("***** Address of Is_A_View            = %p ***** \n",&Is_A_View);
     printf ("***** Address of Is_A_Null_Array      = %p ***** \n",&Is_A_Null_Array);
     printf ("***** Address of freepointer          = %p ***** \n",&freepointer);
     printf ("***** Address of internalArrayID      = %p ***** \n",&internalArrayID);
     printf ("***** Address of Index_Array          = %p ***** \n",&Index_Array);
     printf ("***** Address of Data_Base            = %p ***** \n",&Data_Base);
     printf ("***** Address of Stride               = %p ***** \n",&Stride);
#endif
     printf ("Constant_Data_Base       = %s \n",(Constant_Data_Base == TRUE) ? "TRUE" : "FALSE");
     printf ("Constant_Unit_Stride     = %s \n",(Constant_Unit_Stride == TRUE) ? "TRUE" : "FALSE");

     printf ("internalArrayID          = %3d \n",internalArrayID);
     printf ("builtUsingExistingData   = %s \n",(builtUsingExistingData == TRUE) ? "TRUE" : "FALSE");
     printf ("View_Offset              = %3d \n",View_Offset);
     printf ("ExpressionTemplateOffset = %3d \n",ExpressionTemplateOffset);

     printf ("Scalar_Offset ========================================= ");
     int i;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",Scalar_Offset[i]);
     printf ("\n");

     printf ("User_Base ============================================= ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",User_Base[i]);
     printf ("\n");

     printf ("Block Size ============================================ ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",Size[i]);
     printf ("\n");

     printf ("Computed - Array Size ================================= ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",getLength(i));
     printf ("\n");

     printf ("(Internal use) Data_Base ============================== ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",Data_Base[i]);
     printf ("\n");

     printf ("Base (of access to data) ============================== ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",Base[i]);
     printf ("\n");

     printf ("getBase (output of getBase member function) =========== ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",getBase(i));
     printf ("\n");

     printf ("getRawBase (output of getRawBase member function) ===== ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",getRawBase(i));
     printf ("\n");

     printf ("Bound (of access to data) ============================= ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",Bound[i]);
     printf ("\n");

     printf ("getBound (output of getBound member function) ========= ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",getBound(i));
     printf ("\n");

     printf ("getRawBound (output of getRawBound member function) === ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",getRawBound(i));
     printf ("\n");

     printf ("Stride (of access to data) ============================ ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",Stride[i]);
     printf ("\n");

     printf ("getStride (output of getStride member function) ======= ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",getStride(i));
     printf ("\n");

     printf ("getRawStride (output of getRawStride member function) = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",getRawStride(i));
     printf ("\n");

// if defined(USE_EXPRESSION_TEMPLATES)
     printf ("***** Expression Template Intermediate Form ***** \n");
     printf ("IndexDataPointer (intermediate form) = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %p ",IndexDataPointer[i]);
     printf ("\n");

     printf ("IndexBase (intermediate form) ======== ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",IndexBase[i]);
     printf ("\n");

     printf ("IndexStride (intermediate form) ====== ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %3d ",IndexStride[i]);
     printf ("\n");
     printf ("************************************************* \n");
// endif

     if (referenceCount > getReferenceCountBase()+1)
        {
          printf ("Domain Reference Count = %d (there is a \
               reference to this descriptor)! \n",referenceCount);
        }
       else
        {
          printf ("Domain Reference Count = %d \n",referenceCount);
        }

     printf ("Indirect addressing arrays \n");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          printf ("     Index_Array [%d] = %s \n",i,(Index_Array [i] == NULL) ? "NULL" : "NON-NULL");
          if (Index_Array [i] != NULL)
             {
               printf ("Index_Array [%d] = %p \n",i,Index_Array[i]);
#if 1
            // The display function has shorter output (so it is easier to work with)
               Index_Array [i]->display("INDIRECT ADDRESSING INTEGER LIST");
#else
            // Long output
               Index_Array [i]->view("INDIRECT ADDRESSING INTEGER LIST");
#endif
             }
        }

#if defined(PPP)
     printf ("Local P++ specific Indirect addressing arrays (intSerialArrays) \n");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          printf ("     Local_Index_Array [%d] = %s \n",i,(Local_Index_Array [i] == NULL) ? "NULL" : "NON-NULL");
          if (Local_Index_Array [i] != NULL)
             {
               printf ("Local_Index_Array [%d] = %p \n",i,Local_Index_Array[i]);
#if 1
            // The display function has shorter output (so it is easier to work with)
               Local_Index_Array [i]->display("LOCAL (P++ specific) INDIRECT ADDRESSING INTEGER LIST");
#else
            // Long output
               Local_Index_Array [i]->view("LOCAL (P++ specific) INDIRECT ADDRESSING INTEGER LIST");
#endif
             }
        }

     printf ("\n");
  // Pointer to a P++ partitioning object
     printf ("Partitioning_Object_Pointer is %s \n",(Partitioning_Object_Pointer == NULL) ? "NULL" : "VALID POINTER");
     printf ("Partitioning_Object_Pointer value = %p \n",Partitioning_Object_Pointer);
  // APP_ASSERT(Partitioning_Object_Pointer != NULL);
     if (Partitioning_Object_Pointer != NULL)
          Partitioning_Object_Pointer->display(Label);
       else
          Internal_Partitioning_Type::displayDefaultValues(Label);

#if defined(USE_PADRE)
  // What PADRE function do we call here?
  // printf ("NEED TO CALL PADRE \n"); 
  // APP_ABORT();
  // parallelDescriptor.display(Label);
     if (parallelPADRE_DescriptorPointer != NULL)
        {
          APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
       // printf ("Call to parallelPADRE_DescriptorPointer->display(Label) commented out! \n");
          parallelPADRE_DescriptorPointer->display(Label);
        }
       else
        {
          printf ("In Array_Domain_Type::display(%s) ##### parallelPADRE_DescriptorPointer == NULL ##### \n",Label);
        }
#else
  // Pointer to Block-Parti parallel decomposition (object)
     printf ("BlockPartiArrayDecomposition is %s \n",(BlockPartiArrayDecomposition == NULL) ? "NULL" : "VALID POINTER");
     if (BlockPartiArrayDecomposition != NULL)
          printf ("BlockPartiArrayDecomposition->referenceCount = %d \n",BlockPartiArrayDecomposition->referenceCount);
  // Pointer to Block-Parti parallel distributed array descriptor
     printf ("BlockPartiArrayDomain is %s \n",(BlockPartiArrayDomain == NULL) ? "NULL" : "VALID POINTER");
     if (BlockPartiArrayDomain != NULL)
          printf ("BlockPartiArrayDomain->referenceCount = %d \n",BlockPartiArrayDomain->referenceCount);

     if (BlockPartiArrayDomain != NULL)
        {
       // These require the BlockPartiArrayDomain internally to determin the partition position
       // isNonPartition requires the Local_Mask to be properly set
          printf ("Is NOT PARTITIONED ==========(requires Local_Mask to be properly set)================ ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %s ",(isNonPartition(i))   ? "TRUE" : "FALSE");
          printf ("\n");

          printf ("Is a LEFT PARTITION =========(uses BlockPartiArrayDomain data)======================= ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %s ",(isLeftPartition(i))   ? "TRUE" : "FALSE");
          printf ("\n");

          printf ("Is a MIDDLE PARTITION =======(uses BlockPartiArrayDomain data)======================= ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %s ",(isMiddlePartition(i))   ? "TRUE" : "FALSE");
          printf ("\n");

          printf ("Is a RIGHT PARTITION ========(uses BlockPartiArrayDomain data)======================= ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %s ",(isRightPartition(i))   ? "TRUE" : "FALSE");
          printf ("\n");

          printf ("PARTI -- ((LOCAL) START INDEX of the range) lalbnd ================================== ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %d ",lalbnd(BlockPartiArrayDomain,i,Base[i],1));
          printf ("\n");

          printf ("PARTI -- ((LOCAL) LAST INDEX of the range) laubnd  ================================== ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %d ",laubnd(BlockPartiArrayDomain,i,Bound[i],1));
          printf ("\n");

          printf ("PARTI -- (Local sizes) laSizes  ===================================================== ");
          int Sizes[MAX_ARRAY_DIMENSION];
          laSizes(BlockPartiArrayDomain,Sizes);
          for (i=0; i < Domain_Dimension; i++)
               printf (" %d ",Sizes[i]);
          printf ("\n");

          printf ("PARTI -- (LOWER bound of the range of global indices stored locally) gLBnd ========== ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %d ",gLBnd(BlockPartiArrayDomain,i));
          printf ("\n");

          printf ("PARTI -- (UPPER bound of the range of global indices stored locally) gUBnd ========== ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %d ",gUBnd(BlockPartiArrayDomain,i));
          printf ("\n");

#if 0
          printf ("PARTI -- (LOCAL INDEX OF THE GLOBAL INDEX (or -1 if not owned)) globalToLocal ======= \n");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %d ",globalToLocal(BlockPartiArrayDomain,Data_Base[i],i));
          printf ("\n");
#else
          printf ("PARTI -- (LOCAL INDEX OF THE GLOBAL INDEX (or -1 if not owned)) globalToLocal ======= \n");
          for (i=0; i < Domain_Dimension; i++)
             {
               int Start = Data_Base[i]+Base[i];
               int End   = Data_Base[i]+Bound[i];
               printf ("          Dimension %d (Global Range: %d-%d): ",i,Start,End);
               for (int j=Start; j <= End; j++)
                    printf ("%d ",globalToLocal(BlockPartiArrayDomain,j,i));
               printf ("\n");
             }
#endif /* end of if 0 else */

#if 0
          printf ("PARTI -- (LOCAL INDEX OF THE GLOBAL INDEX (or -1 if not owned)) globalToLocalWithGhost == ");
          for (i=0; i < Domain_Dimension; i++)
               printf ("%d ",globalToLocalWithGhost(BlockPartiArrayDomain,Data_Base[i],i));
          printf ("\n");
#else
          printf ("PARTI -- (LOCAL INDEX OF THE GLOBAL INDEX (or -1 if not owned)) globalToLocalWithGhost == \n");
          for (i=0; i < Domain_Dimension; i++)
             {
               int Start = (Data_Base[i]+Base[i])-InternalGhostCellWidth[i];
               int End   = (Data_Base[i]+Bound[i])+InternalGhostCellWidth[i];
               printf ("          Dimension %d (Global Range: %d-%d): ",i,Start,End);
               for (int j=Start; j <= End; j++)
                    printf ("%d ",globalToLocalWithGhost(BlockPartiArrayDomain,j,i));
               printf ("\n");
             }
#endif /* end of if 0 else */

#if 0
          printf ("PARTI -- (GLOBAL INDEX OF THE LOCAL INDEX (or -1 if not owned)) localToGlobal =========== ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %d ",localToGlobal(BlockPartiArrayDomain,Data_Base[i],i));
          printf ("\n");
#else
          printf ("PARTI -- (GLOBAL INDEX OF THE LOCAL INDEX (or -1 if not owned)) localToGlobal == \n");
          for (i=0; i < Domain_Dimension; i++)
             {
            // int Start = globalToLocal(BlockPartiArrayDomain,Data_Base[i]+Base[i],i);
            // int End   = globalToLocal(BlockPartiArrayDomain,Data_Base[i]+Bound[i],i);
            // int Start = Data_Base[i]+gLBnd(BlockPartiArrayDomain,i);
            // int End   = Data_Base[i]+gUBnd(BlockPartiArrayDomain,i);
               int Start = lalbnd(BlockPartiArrayDomain,i,Base [i],1);
               int End   = laubnd(BlockPartiArrayDomain,i,Bound[i],1);
               printf ("          Dimension %d (Local Range: %d-%d): ",i,Start,End);
               for (int j=Start; j <= End; j++)
                    printf ("%d ",localToGlobal(BlockPartiArrayDomain,j,i));
               printf ("\n");
             }
#endif /* end of if 0 else */

#if 0
          printf ("PARTI -- (GLOBAL INDEX OF THE LOCAL INDEX (or -1 if not owned)) localToGlobalWithGhost == ");
          for (i=0; i < Domain_Dimension; i++)
               printf (" %d ",localToGlobalWithGhost(BlockPartiArrayDomain,Data_Base[i],i));
          printf ("\n");
#else
          printf ("PARTI -- (GLOBAL INDEX OF THE LOCAL INDEX (or -1 if not owned)) localToGlobalWithGhost == \n");
          for (i=0; i < Domain_Dimension; i++)
             {
            // int Start = globalToLocal(BlockPartiArrayDomain,Data_Base[i]+Base[i],i)  - InternalGhostCellWidth[i];
            // int End   = globalToLocal(BlockPartiArrayDomain,Data_Base[i]+Bound[i],i) + InternalGhostCellWidth[i];
            // int Start = (Data_Base[i]+gLBnd(BlockPartiArrayDomain,i) ) - InternalGhostCellWidth[i];
            // int End   = (Data_Base[i]+gUBnd(BlockPartiArrayDomain,i) ) + InternalGhostCellWidth[i];
               int Start = lalbnd(BlockPartiArrayDomain,i,Base[i],1) - InternalGhostCellWidth[i];
               int End   = laubnd(BlockPartiArrayDomain,i,Bound[i],1) + InternalGhostCellWidth[i];
               printf ("          Dimension %d (Local Range: %d-%d): ",i,Start,End);
               for (int j=Start; j <= End; j++)
                    printf ("%d ",localToGlobalWithGhost(BlockPartiArrayDomain,j,i));
               printf ("\n");
             }
#endif
        }
  // End of if not USE_PADRE
#endif

  // Specialized P++ support for interpretation of message passing
  // This might make more sense in a seperate structure that was started at the
  // beginning of a statements evaluation and ended at the end of a statements
  // evaluation and was passed along through the binary operators in the processing.

  // printf ("\n");
     printf ("************** GHOST BOUNDARY WIDTH INFO ************** \n");
     printf ("InternalGhostCellWidth = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",InternalGhostCellWidth[i]);
     printf ("\n");

     printf ("ExternalGhostCellWidth = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",ExternalGhostCellWidth[i]);
     printf ("\n");
     printf ("************** ************************* ************** \n");

     printf ("Left_Number_Of_Points ========== ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",Left_Number_Of_Points[i]);
     printf ("\n");

     printf ("Right_Number_Of_Points ========= ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",Right_Number_Of_Points[i]);
     printf ("\n");

  // Pointer to Block-Parti parallel decomposition (object)
     printf ("Array_Conformability_Info is %s \n",(Array_Conformability_Info == NULL) ? "NULL" : "VALID POINTER");
     if (Array_Conformability_Info != NULL)
        {
          Array_Conformability_Info->display(Label);
        }

     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          printf ("Global Index used to access elements: (Axis = %d) \n",i);
          Global_Index     [i].display("Global_Index");
        }

     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          printf ("Local Index used to access elements: (Axis = %d) \n",i);
          Local_Mask_Index [i].display("Local_Mask_Index");
        }
#endif /* end of PPP */

  // reset output of node number prefix to all strings
     Communication_Manager::setPrefixParallelPrintf(printfState);

     printf ("\n");
     printf ("\n");
   }

// *********************************************************
// *********************************************************
// *********************  DESTRUCTORS  *********************
// *********************************************************
// *********************************************************

Array_Domain_Type::~Array_Domain_Type ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
        {
          printf ("Call Array_Domain_Type::destructor! %p Array_ID = %d referenceCount = %d typeCode = %d \n",
               this,internalArrayID,referenceCount,typeCode);
          printf ("Number of arrays is use = %d \n",Diagnostic_Manager::getNumberOfArraysInUse());
        }
#endif

  // I think we only want to execute the body of the constructor if the 
  // reference count is less than zero.  This would allow the constructor to
  // be called but not delete anything depending on the reference count.

  // We have to use getReferenceCountBase()-1 as a 
  // lower bound because the delete operator 
  // also decrements the reference count.
     APP_ASSERT(referenceCount >= getReferenceCountBase() - 1);

#if 0
  // The reference count should always be equal to the getReferenceCountBase()
  // (or less for the reason mentioned above) or else we should not be in 
  // this destructor.
  // APP_ASSERT(referenceCount <= getReferenceCountBase());
#endif

  // If we can have the to previous asserts then they should imply that we can have: WRONG!!!
  // APP_ASSERT(referenceCount == getReferenceCountBase());

  // Must cleanup any Index_Array pointers - these are implemented as references so delete
  // must be called even if only the reference count is decremented!

  // Later we can make the execution of the loop dependent upon the conditional: if (Uses_Indirect_Addressing == TRUE)
     int i;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
#if defined(PPP)
          APP_ASSERT ( ((Index_Array[i] != NULL) && (Local_Index_Array[i] != NULL)) || (Index_Array[i] == NULL) );
          APP_ASSERT ( ((Index_Array[i] != NULL) && (Local_Index_Array[i] != NULL)) || (Local_Index_Array[i] == NULL) );
#endif
       // Delete the intArrays used to support indirect addressing (for A++ and P++)
          if (Index_Array[i] != NULL)
             {
               APP_ASSERT (Uses_Indirect_Addressing == TRUE);

            // Added conventional mechanism for reference counting control
            // operator delete no longer decriments the referenceCount.
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 2)
                  {
                    printf ("In Array_Domain_Type::destructor Index_Array[%d] Array_ID = %d referenceCount = %d \n",
                         i,Index_Array[i]->Array_ID(),Index_Array[i]->getReferenceCount());
                    Index_Array[i]->displayReferenceCounts("In Array_Domain_Type::destructor");
                  }
#endif
               Index_Array[i]->decrementReferenceCount();
               if (Index_Array[i]->getReferenceCount() < getReferenceCountBase())
                  {
                 // printf ("Deleting the indirection array (intArray* = %p) \n",Index_Array[i]);
                    delete Index_Array[i];
                  }
               Index_Array[i] = NULL;
             }

#if defined(PPP)
       // Delete the intSerialArrays used to support parallel (P++) indirect addressing
          if (Local_Index_Array[i] != NULL)
             {
               APP_ASSERT (Uses_Indirect_Addressing == TRUE);

            // Added conventional mechanism for reference counting control
            // operator delete no longer decriments the referenceCount.
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 2)
                  {
                    printf ("In Array_Domain_Type::destructor Local_Index_Array[%d] Array_ID = %d referenceCount = %d \n",
                         i,Local_Index_Array[i]->Array_ID(),Local_Index_Array[i]->getReferenceCount());
                    Local_Index_Array[i]->displayReferenceCounts("In Array_Domain_Type::destructor");
                  }
#endif
               Local_Index_Array[i]->decrementReferenceCount();
               if (Local_Index_Array[i]->getReferenceCount() < getReferenceCountBase())
                  {
                 // printf ("Deleting the local indirection array (intSerialArray* = %p) \n",Local_Index_Array[i]);
                    delete Local_Index_Array[i];
                  }
               Local_Index_Array[i] = NULL;
             }
#endif
        }

#if defined(PPP)
  // Pointer to a P++ partitioning object
  // APP_ASSERT(Partitioning_Object_Pointer != NULL);
  // printf ("In ~Array_Domain_Type -- Partitioning_Object_Pointer = %p \n",Partitioning_Object_Pointer);
     if (Partitioning_Object_Pointer != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Inside of ~Array_Domain_Type: Partitioning_Object_Pointer->getReferenceCount() = %d \n",Partitioning_Object_Pointer->getReferenceCount());
#endif

       // Bugfix (9/16/2000) make reference count handling consistant with use elsewhere
          Partitioning_Object_Pointer->decrementReferenceCount();

       // Bugfix (11/14/2000) need to make consistent with code elsewhere
          APP_ASSERT (Partitioning_Object_Pointer->getReferenceCount() >= Internal_Partitioning_Type::getReferenceCountBase()-1);
          if (Partitioning_Object_Pointer->getReferenceCount() < Internal_Partitioning_Type::getReferenceCountBase())
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("In ~Array_Domain_Type deleting Partitioning_Object_Pointer \n");
#endif
               delete Partitioning_Object_Pointer;
             }
          Partitioning_Object_Pointer = NULL;
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Inside of ~Array_Domain_Type: Partitioning_Object_Pointer == NULL \n");
#endif
        }

#if defined(USE_PADRE)
  // Because the PADRE object reference the Array_Domain objects
  // The Array_Descriptor has to delete the PADRE object
  // else the reference counting (greater than 1) on the domain objects
  // will force this descructor to skip over this code
  // and then the PADRE objects is not deleted.  This
  // seems like it might be a problem in the design of P++
  // but is must be throught about before we will know.
  // mostly is is a consequence of PADRE being used inside
  // of objects which PADRE is referencing.  It is not a 
  // problem for PADRE except that this seems to be the
  // most efficient way to use PADRE (and avoid building an
  // excessive number of domain objects).
  // So since the Array_Descriptor deletes the PADRE object 
  // the parallelPADRE_DescriptorPointer should be NULL at this point.
  // APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
  // printf ("DELETE parallelPADRE_DescriptorPointer \n");
     if (parallelPADRE_DescriptorPointer != NULL)
        {
       // printf ("NOTE: Expected parallelPADRE_DescriptorPointer == NULL \n");
          parallelPADRE_DescriptorPointer->decrementReferenceCount();
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("In Array_Domain descructor: parallelPADRE_DescriptorPointer(%p)->getReferenceCount() = %d \n",
                    parallelPADRE_DescriptorPointer,parallelPADRE_DescriptorPointer->getReferenceCount());
#endif
          if (parallelPADRE_DescriptorPointer->getReferenceCount() < parallelPADRE_DescriptorPointer->getReferenceCountBase())
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("     delete parallelPADRE_DescriptorPointer(%p) \n",parallelPADRE_DescriptorPointer);
#endif
               delete parallelPADRE_DescriptorPointer;
             }
          parallelPADRE_DescriptorPointer = NULL;
        }
#else
  // Pointer to Block-Parti parallel decomposition (object)
  // APP_ASSERT(BlockPartiArrayDecomposition != NULL);
     if (BlockPartiArrayDecomposition != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Inside of ~Array_Domain_Type: deleting BlockPartiArrayDecomposition (reference count = %d) \n",
                    BlockPartiArrayDecomposition->referenceCount);
#endif
       // We must handle the reference counting manually since these are BLOCK PARTI objects (in C)!
       // printf ("Inside of ~Array_Domain_Type: BlockPartiArrayDecomposition->referenceCount = %d \n",BlockPartiArrayDecomposition->referenceCount);

       // Note that the decrement of the reference count is a part of the delete_DECOMP function!
          APP_ASSERT( BlockPartiArrayDecomposition->referenceCount >= 0 );
       // printf ("In Array_Domain_Type::~Array_Domain_Type (Array_Domain_Type) --- Calling delete_DECOMP \n");
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               if (BlockPartiArrayDecomposition->referenceCount == 0)
                    printf ("##### BlockPartiArrayDecomposition WILL be deleted! \n");
                 else
                    printf ("##### BlockPartiArrayDecomposition will NOT be deleted! \n");
             }
#endif

          delete_DECOMP ( BlockPartiArrayDecomposition );
          BlockPartiArrayDecomposition = NULL;
        }

  // Pointer to Block-Parti parallel distributed array descriptor
  // APP_ASSERT(BlockPartiArrayDomain != NULL);
     if (BlockPartiArrayDomain != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Inside of ~Array_Domain_Type: deleting BlockPartiArrayDomain (reference count = %d) \n",
                    BlockPartiArrayDomain->referenceCount);
#endif
       // We must handle the reference counting manually since these are BLOCK PARTI objects (in C)!
       // printf ("Inside of ~Array_Domain_Type: BlockPartiArrayDomain->referenceCount = %d \n",BlockPartiArrayDomain->referenceCount);

       // Note that the decrement of the reference count is a part of the delete_DARRAY function!
          APP_ASSERT( BlockPartiArrayDomain->referenceCount >= 0 );
       // printf ("In Array_Domain_Type::~Array_Domain_Type (Array_Domain_Type) --- Calling delete_DARRAY \n");

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               if (BlockPartiArrayDomain->referenceCount == 0)
                    printf ("##### BlockPartiArrayDomain WILL be deleted! \n");
                 else
                    printf ("##### BlockPartiArrayDomain will NOT be deleted! \n");
             }
#endif

          delete_DARRAY ( BlockPartiArrayDomain );
          BlockPartiArrayDomain = NULL;
        }
  // End of if not USE_PADRE
#endif

  // Pointer to Array_Conformability_Info which carries information required for the
  // interpretation of the P++ message passing though the evaluation of the array statement
  // APP_ASSERT(Array_Conformability_Info != NULL);
     if (Array_Conformability_Info != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("Inside of ~Array_Domain_Type: Array_Conformability_Info->getReferenceCount() = %d \n",
                    Array_Conformability_Info->getReferenceCount());
             }
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Array_Conformability_Info->getReferenceCount() >= getReferenceCountBase());
          Array_Conformability_Info->decrementReferenceCount();
          if (Array_Conformability_Info->getReferenceCount() < getReferenceCountBase())
               delete Array_Conformability_Info;
          Array_Conformability_Info = NULL;
        }
  // end of PPP
#endif

#if !defined(PPP)
  // Put the Array ID back for later reuse!
  // The array data has already been deleted at this point and 
  // so the reference count has been decremented PAST the referenceCountBase.
  // printf ("Above Push_Array_ID conditional: Array_Reference_Count_Array [%d] = %d \n",
  //      Array_ID(),Array_Reference_Count_Array [Array_ID()]);

  // Bugfix (21/6/96): Array_Reference_Count_Array had unbounded growth due to Array_ID
  // not being returned to the stack properly.
  // Note that his bug presently casses a memory leak in BUGS/bug88.C 
  // (which is a code fragment from testcode.C and demonstrates the memory leak).

  // This line solves the unbounded array problem but causes a memory leak
  // if (Array_Reference_Count_Array [Array_ID] <= 0)
  // This is the original line which has no memory leak 
  // but causes the unbounded array problem
  // Since array ids are shared between the view and the object
  // which a view is taken we need to make the returning of the 
  // array id dependent upon there being no references to the array data.
  // At this point we also reinitialize the reference count in anticipation of
  // when the same array id will be eventually reused.
  // if ( (Array_Reference_Count_Array [internalArrayID] < getReferenceCountBase()) ||
  //      ( (Array_Reference_Count_Array [internalArrayID] == getReferenceCountBase()) && (builtUsingExistingData == TRUE) ) )

// There are 3 cases to consider:
// 1. When the reference count is less than the reference count base.
// 2. When the reference count is equal to the reference count base 
//    and the array reuses taken from existing data.
// 3. When the array is a temporary and the destructor is called there 
//    is no mechanism to decrement the reference count on the data manually
//    (since the destructor is called automatically).

#if 0
     bool resetReferenceCountAndReturnArrayID =
          (Array_Reference_Count_Array [internalArrayID] < getReferenceCountBase()) ||
          ( (Array_Reference_Count_Array [internalArrayID] == getReferenceCountBase()) && (builtUsingExistingData == TRUE) ) ||
          ( (Array_Reference_Count_Array [internalArrayID] == getReferenceCountBase()) && (isTemporary() == TRUE) );
#else
     bool resetReferenceCountAndReturnArrayID =
          (Array_Reference_Count_Array [internalArrayID] < getReferenceCountBase()) ||
          ( (Array_Reference_Count_Array [internalArrayID] == getReferenceCountBase()) && (builtUsingExistingData == TRUE) );
#endif

#if 0
     printf ("BEFORE reset test: Reset referenceCount in the domain destructor and give back the array ID (referenceCount = %d Array_ID = %d builtUsingExistingData = %s) \n",
               Array_Reference_Count_Array [internalArrayID],
               internalArrayID,
               (builtUsingExistingData == TRUE) ? "TRUE" : "FALSE");

     printf ("resetReferenceCountAndReturnArrayID = %s \n",
          (resetReferenceCountAndReturnArrayID == TRUE) ? "TRUE" : "FALSE");
#endif

     if (resetReferenceCountAndReturnArrayID == TRUE)
        {
          if (Diagnostic_Manager::getReferenceCountingReport() > 0)
             {
               printf ("Reset the referenceCount in the domain destructor and give back the array ID (referenceCount = %d Array_ID = %d) \n",
                    Array_Reference_Count_Array [internalArrayID],internalArrayID);
             }

       // Reinitialize reference count for next use!
          Array_Reference_Count_Array [internalArrayID] = getReferenceCountBase() - 1;

       // Put used Array ID back on stack for later reuse by other array objects!
       // Note that the Push/Pop of array id's triggers the unset/set of the diagnostic 
       // info objects if Diagnostic_Manager::setTrackArrayData() is turned on.
          Push_Array_ID (internalArrayID);
        }
       else
        {
          if (Diagnostic_Manager::getReferenceCountingReport() > 0)
             {
               printf ("SKIP Reset of the referenceCount in the domain destructor and give back the array ID (referenceCount = %d Array_ID = %d) \n",
                    Array_Reference_Count_Array [internalArrayID],internalArrayID);
             }
        }
  // end of !PPP
#endif

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
  // This is a mechanism to release all memory in use after the last
  // A++ array has been deleted.  It is for debugging use only and should be used
  // carfully since the number of A++ arrays decreasing to zero might
  // be acceptable behaviour for an A++ applications and with this mechanism
  // in effect it would delete the internal memory required by A++ after the first
  // cycle to zero.  No other A++ arrays could be build after that and the application
  // would likely die and ugly death (though I would hope that some assertion in A++
  // caught the problem quickly).
     if (smartReleaseOfInternalMemory)
        {
       // Compute the number of arrays that are stored internally
          int numberOfOutstandingArrayObjects = 0;
          numberOfOutstandingArrayObjects     = Where_Statement_Support::numberOfInternalArrays();
          numberOfOutstandingArrayObjects    += intArray   ::numberOfInternalArrays();
          numberOfOutstandingArrayObjects    += floatArray ::numberOfInternalArrays();
          numberOfOutstandingArrayObjects    += doubleArray::numberOfInternalArrays();

#if 0
          printf ("Smart Memory Release: Array_Descriptor.getNumberOfArraysInUse() = %d internal arrays = %d \n",
               getNumberOfArraysInUse(),numberOfOutstandingArrayObjects);
#endif

          if (getNumberOfArraysInUse() <= numberOfOutstandingArrayObjects)
             {
               if ( (Array_Reference_Count_Array [Array_ID()] < getReferenceCountBase()) ||
                    ( (Array_Reference_Count_Array [Array_ID()] == getReferenceCountBase()) &&
                      (builtUsingExistingData == TRUE) ) )
                  {
                 // printf ("CALLING GlobalMemoryRelease() \n");
                    GlobalMemoryRelease();
                  }
                 else
                  {
                 // This could really happen legitimately so we don't want a warning message about it.
                 // printf ("A++/P++ WARNING: reference count too large to call GlobalMemoryRelease() \n");
                  }
             }
        }
#endif

  // printf ("In ~Array_Domain_Type Array_ID on the stack is %d \n",SerialArray_Domain_Type::queryNextArrayID());

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving ~Array_Domain_Type() -- referenceCount = %d \n",referenceCount);
#endif
   }

// *********************************************************
// *********************************************************
// *******  INITIALIZATION SUPPORT FOR CONSTRUCTORS  *******
// *********************************************************
// *********************************************************

#if defined(PPP)
void
Array_Domain_Type::Initialize_Parallel_Parts_Of_Domain ( const Array_Domain_Type & X )
   {
  // I'm not clear on how we want to initialize these so initialize them
  // to some default values for now.
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("Inside of Array_Domain_Type::Initialize_Parallel_Parts_Of_Domain() \n");
        }
#endif

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
  // I have had some conserns that this function was not being called at all times so
  // this is a simple print command to verify it is being called for the construction 
  // of every array.
     printf ("Processing in Array_Domain_Type::Initialize_Parallel_Parts_Of_Domain() \n");
#endif

  // Pointer to a P++ partitioning object
  // test assertion!  if not true then we need to delete the 
  // Partitioning_Object_Pointer object properly!

  // bugfix (8/15/2000) if the current object has an existing partitioning object then reuse it
  // this should fix bug2000_25.C in P++/TESTS
     if (Partitioning_Object_Pointer == NULL)
        {
          APP_ASSERT (Partitioning_Object_Pointer == NULL);
          Partitioning_Object_Pointer = X.Partitioning_Object_Pointer;
          if (Partitioning_Object_Pointer != NULL)
             {
               Partitioning_Object_Pointer->incrementReferenceCount();
             }
        }

#if defined(USE_PADRE)
  // With the modification to have the parallelPADRE_DescriptorPointer
  // initialized in the Allocation function this pointer can be NULL
     APP_ASSERT (X.parallelPADRE_DescriptorPointer != NULL);

  // We want to force a new descriptor to be built -- later we might want
  // to have this use an existing PADRE_Representation object or we might
  // want a new PADRE_Descriptor to be built using the existing 
  // PADRE_Distribution or PADRE_Representation object.  But the correct
  // PADRE_Distribution is already used since the Partitioning object use in X
  // and this domain object are the same.  so the only enhansement would be
  // to have the PADRE_Representation objects shared internally.

#if 1
     if (parallelPADRE_DescriptorPointer != NULL)
        {
       // printf ("(before deleting the existing object) parallelPADRE_DescriptorPointer->getReferenceCount() = %d \n",parallelPADRE_DescriptorPointer->getReferenceCount());

          parallelPADRE_DescriptorPointer->decrementReferenceCount();
          if (parallelPADRE_DescriptorPointer->getReferenceCount() <
              parallelPADRE_DescriptorPointer->getReferenceCountBase())
               delete parallelPADRE_DescriptorPointer;
        }
#else
     printf ("Possible memory leak in Initialize_Parallel_Parts_Of_Domain (commented out delete) \n");
#endif

     parallelPADRE_DescriptorPointer = NULL;
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);

  // printf ("(before new objects is built) this->getReferenceCount() = %d \n",getReferenceCount());

  // We want to make this step as cheap as possible.  We reuse everything except
  // the LocalDescriptor (we pass in the "this" pointer) and the Domain for the 
  // serial array object (which we don't know yet so we have to set that later).
     parallelPADRE_DescriptorPointer =
          new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>
               ( X.parallelPADRE_DescriptorPointer->getGlobalDomain() ,
                 (SerialArray_Domain_Type*) NULL ,
                 X.parallelPADRE_DescriptorPointer->getPADRE_Representation() ,
                 this );
     APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);

  // printf ("(after new objects is built) parallelPADRE_DescriptorPointer(%p)->getReferenceCount() = %d \n",
  //      parallelPADRE_DescriptorPointer,parallelPADRE_DescriptorPointer->getReferenceCount());
  // printf ("(after new objects is built) this->getReferenceCount() = %d \n",getReferenceCount());
#else
  // Pointer to Block-Parti parallel decomposition (object)
     BlockPartiArrayDecomposition = X.BlockPartiArrayDecomposition;
     if (BlockPartiArrayDecomposition != NULL)
        {
          APP_ASSERT (BlockPartiArrayDecomposition->referenceCount >= 0);
          BlockPartiArrayDecomposition->referenceCount++;
       // printf ("In Array_Domain_Type::Initialize_Parallel_Parts_Of_Domain -- BlockPartiArrayDecomposition->referenceCount = %d \n",
       //      BlockPartiArrayDecomposition->referenceCount);
        }

  // Pointer to Block-Parti parallel distributed array descriptor
     BlockPartiArrayDomain = X.BlockPartiArrayDomain;
     if (BlockPartiArrayDomain != NULL)
        {
          APP_ASSERT (BlockPartiArrayDomain->referenceCount >= 0);
          BlockPartiArrayDomain->referenceCount++;
       // printf ("In Array_Domain_Type::Initialize_Parallel_Parts_Of_Domain -- BlockPartiArrayDomain->referenceCount = %d \n",
       //      BlockPartiArrayDomain->referenceCount);
        }
  // End of if not USE_PADRE
#endif

  // Set this to NULL since we assume we will not use Index objects on a temporary (for now)
     Array_Conformability_Info = NULL;

#if 0
  // These have not been initialized yet (so it is a purify UMR error to access them)
     for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          printf ("In Initialize_Parallel_Parts_Of_Domain: Left_Number_Of_Points  [%d] = %d \n",i,Left_Number_Of_Points  [i]);
          printf ("In Initialize_Parallel_Parts_Of_Domain: Right_Number_Of_Points [%d] = %d \n",i,Right_Number_Of_Points [i]);
        }
#endif
   }
// end of PPP
#endif

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// void Array_Domain_Type::Initialize_Domain(
//           int Input_Array_Size_I , int Input_Array_Size_J , 
//           int Input_Array_Size_K , int Input_Array_Size_L ) 
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void
Array_Domain_Type::Initialize_Domain(
   int Number_Of_Valid_Dimensions,
   const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   {
  // This version takes integers within it's interface.  It used to be
  // called by every other Initialization function but since it loses information
  // about the stride (which we would like to have be preserved) the version
  // which takes InternalIndex objects will become the base version of the initializeDomain
  // function (which all others call).

  // So this becomes a much shorter function
     int temp;
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
        {
          printf ("Inside of Array_Domain_Type::Initialize_Domain(Number_Of_Valid_Dimensions=%d,int*) ",
               Number_Of_Valid_Dimensions);
          for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
               printf ("int(%d) ",Integer_List[temp]);
          printf ("\n");
        }
#endif

  // Conditional compilation for new internalIndex based initializeDomain function
     Internal_Index Index_List [MAX_ARRAY_DIMENSION];
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
        {
          int tempBase  = 0;
          int tempBound = Integer_List[temp]-1;
          Index_List[temp] = Internal_Index (tempBase,(tempBound-tempBase)+1);
          Internal_Index_List[temp] = &(Index_List[temp]);
        }

  // Initialize the rest to something
     for (temp = Number_Of_Valid_Dimensions; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          Index_List[temp] = Internal_Index (0,1);
          Internal_Index_List[temp] = &(Index_List[temp]);
        }

  // Now call the new version of the function
     Initialize_Domain(Number_Of_Valid_Dimensions,Internal_Index_List);
   }

void
Array_Domain_Type::Initialize_Domain ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   {
     Initialize_Domain (MAX_ARRAY_DIMENSION,Integer_List);

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency ("Called from constructor Array_Domain_Type(int,int,int,int)");
#endif
   }

int
Array_Domain_Type::computeDimension ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type internalIndexList )
   {
  // Compute the dimension of the array from the number of non unit entries in the Index list

     APP_ASSERT (internalIndexList != NULL);

  // Determine the highest dimension to start with (depending on the number of non-NULL 
  // pointers in the internalIndexList (list of pointers to Index objects))
     int tempDimension   = 0;
     if (internalIndexList[tempDimension] != NULL)
        {
          while ( (tempDimension < MAX_ARRAY_DIMENSION-1) && (internalIndexList[tempDimension+1] != NULL) )
             {
               tempDimension++;
            // printf ("In computeDimension (finding the length of the non-Null pointer list): tempDimension = %d \n",tempDimension);
             }
        }
       else
        {
          tempDimension = 0;
        }

  // printf ("Starting at tempDimension = %d and counting down to zero! \n",tempDimension);

  // Now count down the number of dimensions to exclude trailing dimensions with unit length (length == 1)
     int domainDimension = tempDimension+1;
     while ( (tempDimension >= 0) && (internalIndexList[tempDimension]->getLength() <= 1) )
        {
       // printf ("In computeDimension: tempDimension = %d \n",tempDimension);

          domainDimension--;
          tempDimension--;
        }

  // printf ("In computeDimension (value before test): domainDimension = %d \n",domainDimension);

  // fixup Domain_Dimension for special case of 1D array of size 1
     if ( ((internalIndexList[0] != NULL) && (internalIndexList[0]->getLength() >= 1)) && (domainDimension == 0) )
          domainDimension = 1;

  // printf ("In computeDimension (final value): domainDimension = %d \n",domainDimension);

     APP_ASSERT (domainDimension >= 0);
     APP_ASSERT (domainDimension <= MAX_ARRAY_DIMENSION);

     return domainDimension;
   }

int
Array_Domain_Type::computeDimension ( const Integer_Array_MAX_ARRAY_DIMENSION_Type integerList )
   {
  // Compute the dimension of the array from the number of non unit entries in the integer list

     int domainDimension = MAX_ARRAY_DIMENSION;
     int tempDimension   = MAX_ARRAY_DIMENSION-1;
     APP_ASSERT (integerList != NULL);
     while ( (tempDimension >= 0) && (integerList[tempDimension] <= 1) )
        {
          domainDimension--;
          tempDimension--;
        }

  // fixup Domain_Dimension for special case of 1D array of size 1
     if ( (integerList[0] >= 1) && (domainDimension == 0) )
          domainDimension = 1;

     APP_ASSERT (domainDimension >= 0);
     APP_ASSERT (domainDimension < MAX_ARRAY_DIMENSION);

     return domainDimension;
   }


void
Array_Domain_Type::Initialize_Domain ( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List )
   {
  // Start of new internalIndex based initializeDomain function
  // This is the ONLY initialization function which does the low level 
  // initialization (all others call this function)
     int temp = 0;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("Inside of Array_Domain_Type::Initialize_Domain(Number_Of_Valid_Dimensions=%d,int*) (Array ID=%d) ",
               Number_Of_Valid_Dimensions,Array_ID());
          for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
               printf ("internalIndex(%d - %d by %d) ",Internal_Index_List[temp]->getBase(),
                                                       Internal_Index_List[temp]->getBound(),
                                                       Internal_Index_List[temp]->getStride());
          printf ("\n");
        }
#endif

  /* It is faster to put the initialization of the data members into the body of the 
  // constructor rather than into the preinitialization list (though I think this
  // is compiler dependent)!  Also on the cray we could have an example static descriptor
  // and then initialize the new one using the static one in a loop that would vectorize 
  // and then to the few scalar operations to fixup the new descriptor data that was
  // dependent on the input values (array sizes).
  */

#if defined(APP) || defined(SERIAL_APP)
 // Double check that the MDI data structure is at least the same size
 // printf ("sizeof(array_domain)        = %d \n",sizeof(array_domain));
 // printf ("sizeof(Array_Domain_Type) = %d \n",sizeof(Array_Domain_Type));
    APP_ASSERT ( sizeof(array_domain) == sizeof(Array_Domain_Type) );
#endif

#if defined(PPP)
#if defined(USE_PADRE)
     parallelPADRE_DescriptorPointer = NULL;
#else
  // Pointer to Block-Parti parallel decomposition (object)
  // APP_ASSERT (BlockPartiArrayDomain        == NULL);
  // APP_ASSERT (BlockPartiArrayDecomposition == NULL);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
        {
          if (BlockPartiArrayDomain        != NULL)
               printf ("WARNING: In Initialize_Domain --- BlockPartiArrayDomain        != NULL \n");

          if (BlockPartiArrayDecomposition        != NULL)
               printf ("WARNING: In Initialize_Domain --- BlockPartiArrayDecomposition != NULL \n");
        }
#endif

     BlockPartiArrayDecomposition = NULL;

  // Pointer to Block-Parti parallel distributed array descriptor
     BlockPartiArrayDomain = NULL;
#endif
#endif

     APP_ASSERT (Number_Of_Valid_Dimensions >= 0);

  // If the stride is greater than 1 then there is some freedom in the
  // selection of the size since it could match an existing distribution.
     for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
        {
       // Need to find out the sizes and search for an existing partition!
        }

  // We have to look at ALL the dimensions to figure out if we have a strided view
     bool makeStridedView = FALSE;

     Is_A_Null_Array = (Number_Of_Valid_Dimensions == 0) ? TRUE : FALSE;
     for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
        {
#if defined(PPP)
       // P++ specific stuff
          Local_Mask_Index [temp]       = Global_Index [temp] = *(Internal_Index_List[temp]);
          InternalGhostCellWidth [temp] = Internal_Partitioning_Type::getDefaultInternalGhostCellWidth (temp);
          ExternalGhostCellWidth [temp] = Internal_Partitioning_Type::getDefaultExternalGhostCellWidth (temp);
          Left_Number_Of_Points  [temp] = Right_Number_Of_Points [temp] = 0;
#endif

       // By definition the internal access to the arrays data starts at ZERO
          Base          [temp] = 0;

       // create a local variable since this value is used a couple of places in this loop
          int tempLength = (Internal_Index_List[temp]->getBound() -
                            Internal_Index_List[temp]->getBase()) + 1;

       // This is the offset of the left edge of the array in the user's i n d e x space
          Data_Base     [temp] = Internal_Index_List[temp]->getBase();

       // This is the left edge of the view (initially these are the same 
       // but a view of an array could make it different from the Data_Base)
          User_Base     [temp] = Internal_Index_List[temp]->getBase();
       // printf ("Setup i Array_Domain_Type::Initialize_Domain (): User_Base [%d] = %d \n",temp,User_Base[temp]);

       // By definition the internal access bound is at the right edge of the data
          Bound         [temp] = tempLength - 1;

       // NOTE: The stride of a new array domain does not have to be 1!
          Stride        [temp] = Internal_Index_List[temp]->getStride();

       // if this is a strided array then it must be a view
          if (Stride[temp] > 1)
             {
               makeStridedView = TRUE;
             }

       // A new array object built from a collection of Internal_Index
       // objects can not be using indirect addressing
          Index_Array [temp] = NULL;

#if defined(PPP)
       // Used to support P++ indirect addressing
          Local_Index_Array[temp] = NULL;
#endif

          if (temp == 0)
               Size [0] = tempLength;
            else
               Size [temp] = Size[temp-1] * tempLength;

          APP_ASSERT (tempLength >= 0);
          if (tempLength == 0)
               Is_A_Null_Array = TRUE;

// if defined(USE_EXPRESSION_TEMPLATES)
          IndexBase[temp]        = 0;
          IndexStride[temp]      = 0;
          IndexDataPointer[temp] = NULL;
// endif
        }

  // Initialize the other dimensions to avoid errors in purify (and errors in general!)
     for (temp=Number_Of_Valid_Dimensions; temp < MAX_ARRAY_DIMENSION; temp++)
        {
#if defined(PPP)
          InternalGhostCellWidth [temp] = ExternalGhostCellWidth [temp] = 0;
          Left_Number_Of_Points  [temp] = Right_Number_Of_Points [temp] = 0;
#endif

       // ... (bug fix,kdb, 8/5/96) if APP_Global_Array_Base is
       //  not 0 Base will be non zero and this will be a view ...
       // Base [temp] = Data_Base [temp] = Bound [temp] = APP_Global_Array_Base;
          Data_Base     [temp] = APP_Global_Array_Base;
          User_Base     [temp] = APP_Global_Array_Base;
          Base          [temp] = Bound [temp] = 0;
          Stride        [temp] = 1;
          Index_Array   [temp] = NULL;
          if (temp == 0)
               Size [0] = 0;
            else
               Size [temp] = Size[temp-1];

#if defined(PPP)
       // Used to support P++ indirect addressing
          Local_Index_Array[temp] = NULL;
#endif

// if defined(USE_EXPRESSION_TEMPLATES)
          IndexBase[temp]        = 0;
          IndexStride[temp]      = 0;
          IndexDataPointer[temp] = NULL;
// endif
        }

  // ... Bugfix (8/16/96,kdb) move this section of code here so Bound can be set
  // correctly in case of a null array.  Since Base is already set correctly
  // remove the code that sets it in this loop. ...
  // Bugfix (2/9/96) we have to force null arrays to have Is_Contiguous_Data = FALSE
  // plus the base and bounds are set to specific values
     if (Is_A_Null_Array == TRUE)
        {
          Is_Contiguous_Data   = FALSE;
          Is_A_View            = FALSE;
          Constant_Data_Base   = TRUE;

       // Bugfix (11/6/2000) We want a consistant setting for this when we have a NullArray object
       // This is enforced within the Test_Consistency tests for a domain object.
       // Constant_Unit_Stride = FALSE;
          Constant_Unit_Stride = TRUE;
          for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               Bound [temp] = -1;

#if defined(PPP)
            // Bugfix (11/6/2000)
            // Force all dimensions to be Null_Index mode when the array is a Null array.
            // This is enforced within the Test_Consistency tests for a domain object.
               Local_Mask_Index [temp] = Internal_Index(0,0,1,Null_Index);
               Global_Index [temp]     = Internal_Index(0,0,1,Null_Index);
#endif
             }
        }
       else
        {
       // Most common case is placed first
          if (makeStridedView == FALSE)
             {
               Is_A_View            = FALSE;
               Is_Contiguous_Data   = TRUE;
               Constant_Unit_Stride = TRUE;
             }
            else
             {
            // If we are building a strided view then the data can't be contiguous
               Is_Contiguous_Data   = FALSE;
               Is_A_View            = TRUE;
               Constant_Unit_Stride = FALSE;
             }

       // initialize to true then loop through axes to verify correctness (set to FALSE if incorrect)
          Constant_Data_Base   = TRUE;
          for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               if (Data_Base [temp] != Data_Base [temp-1])
                    Constant_Data_Base   = FALSE;
             }
        }

#if 0
     Domain_Dimension = Number_Of_Valid_Dimensions;

     int tempDimension = Number_Of_Valid_Dimensions-1;
     while ( (tempDimension >= 0) && (Internal_Index_List[tempDimension]->getLength() <= 1) )
        {
          Domain_Dimension--;
          tempDimension--;
        }

  // fixup Domain_Dimension for special case of 1D array of size 1
     if ( (Internal_Index_List[0]->getLength() >= 1) && (Domain_Dimension == 0) )
          Domain_Dimension = 1;

     APP_ASSERT (Domain_Dimension >= 0);
     APP_ASSERT (Domain_Dimension < MAX_ARRAY_DIMENSION);
#else
     Domain_Dimension = computeDimension ( Internal_Index_List );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("In Array_Domain_Type constructor -- Domain_Dimension = %d \n",Domain_Dimension);
#endif

#if defined(PPP)
  // (11/19/2000) assertion added since I think we need to delete
  // any valid pointer (to avoid reference counting problems)
  // APP_ASSERT (Partitioning_Object_Pointer == NULL);

  // Pointer to a P++ partitioning object
     Partitioning_Object_Pointer = NULL;

#if defined(USE_PADRE)
  // What PADRE function do we call here?
  // it is in a later phase of initialization of the local descriptor that
  // we also initialize the local domain object (the domain object in the serial array).
  // So we do nothing here for this case.
#else
  // Pointer to Block-Parti parallel decomposition (object)
     BlockPartiArrayDecomposition = NULL;

  // Pointer to Block-Parti parallel distributed array descriptor
     BlockPartiArrayDomain = NULL;
  // end of if not USE_PADRE
#endif

     Array_Conformability_Info = NULL;
#endif

     Is_A_Temporary  = FALSE;

// if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset = Base[0];
     for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          ExpectedOffset += Base[temp] * Size[temp-1];
        }
     ExpressionTemplateOffset = ExpectedOffset;
     APP_ASSERT (ExpressionTemplateOffset == ExpectedOffset);
// endif

     View_Offset = 0;
  // ... Scalar_Offset won't be 0 in rare case where APP_Global_Array_Base is nonzero ...
     Scalar_Offset[0] = -User_Base[0] * Stride[0];
     for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
        {
       // Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Size[temp-1];
          Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Stride[temp] * Size[temp-1];
       // Scalar_Offset -= Data_Base[temp] * Size[temp-1];
        }

     referenceCount     = getReferenceCountBase();

  // Was this descriptor used to build an array object using the adopt function?
  // This is usually FALSE.
     builtUsingExistingData = FALSE;

  // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
     internalArrayID           = Pop_Array_ID();

     Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;

     Uses_Indirect_Addressing = FALSE;

  // This is used for internal diagnostics
     typeCode = APP_UNDEFINED_ELEMENT_TYPE;

#if !defined(PPP)
     if (internalArrayID >= Array_Reference_Count_Array_Length)
          Allocate_Larger_Reference_Count_Array ( internalArrayID );
#endif

  // Now reset the bases (just as was done in the old version of this function)
     for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
          setBase(Internal_Index_List[temp]->getBase(),temp);

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency ("Called from constructor Array_Domain_Type(int,int,int,int)");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          display("Called from bottom of Array_Domain_Type constructor");
  // APP_ABORT();
#endif
   }

#if defined(APP) || defined(PPP) 
void
Array_Domain_Type::Initialize_Domain (
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List,
     const Internal_Partitioning_Type & Internal_Partition )
   {
  // This is all new code as of 11/17/97 (check for bugs)
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List;
 
     int temp=0;
     for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
        {
       // Internal_Index_List[temp]->display("Index in Array_Domain_Type::Initialize_Domain");
       // Integer_List[temp] = Internal_Index_List[temp]->getLength() - APP_Global_Array_Base;
          Integer_List[temp] = Internal_Index_List[temp]->getLength();
        }

     Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List,Internal_Partition);

     for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
          setBase(Internal_Index_List[temp]->getBase(),temp);

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency ("Called from constructor Array_Domain_Type(int,int,int,int)");
#endif
   }
#endif

//=========================================================
// ... this is APP or PPP in i n c l u d e file ...
#if defined(APP) || defined(PPP) 
void
Array_Domain_Type::Initialize_Domain( 
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List , 
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Initialize_Domain (MAX_ARRAY_DIMENSION,Integer_List,Internal_Partition);
   }

#if 0
void
Array_Domain_Type::Initialize_Domain(
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Initialize_Domain (MAX_ARRAY_DIMENSION,Internal_Index_List,Internal_Partition);
   }
#endif
 // ... end of PPP ...
#endif

//=========================================================
// ... this is PPP in i n c l u d e file ...
#if defined(APP) || defined(PPP)
void
Array_Domain_Type::Initialize_Domain(
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List , 
     const Internal_Partitioning_Type & Internal_Partition )
   {
  // Maybe this function should call the partition function instead of 
  // reproducing some of its functionality here!
#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("Consider calling the partition function before the Initialize_Domain function! \n");
#endif

     Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);

#if defined(PPP)
  // Callers of this function must handle reference counting for the Internal_Partitioning_Type object
     Partitioning_Object_Pointer = &((Internal_Partitioning_Type &) Internal_Partition);
     Partitioning_Object_Pointer->incrementReferenceCount();

  // Bugfix (9/9/2000) all trailing dimensions must have zero ghost boundary widths
  // so we can only reset valid dimensions with nonzero ghost boundary widths
  // The LocalGhostCellWidth may be set to non zero values before being used
  // to build a P++ array object (this is reset from the default values set by 
  // the preevious call to Initialize_Domain (above))
     int temp=0;
     for (temp=0; temp < numberOfDimensions(); temp++)
        {
          InternalGhostCellWidth [temp] = Internal_Partition.getLocalGhostCellWidth (temp);
        }
     for (temp = numberOfDimensions(); temp < MAX_ARRAY_DIMENSION; temp++)
        {
          InternalGhostCellWidth [temp] = 0;
        }

  // Now set the Partitioning_Object_Pointer to point to the input partitioning object
  // Partitioning_Object_Pointer = &((Internal_Partitioning_Type&) Internal_Partition);
#endif

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency ("Called from Array_Domain_Type::Initialize_Domain(int,int,int,int,int)");
#endif
   }
 // ... end of PPP ...
#endif

//=========================================================
// ... this is PPP in i n c l u d e file ...
#if defined(PPP)
void
Array_Domain_Type::Initialize_Domain( ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE )
   {
  // The mechanism to take an exiting distributed array and build a P++ array object is not fully implemented as yet!

  // These macros help us use the input variables so that the 
  // compiler will not warn about unused variables
     INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO
     RANGE_ARGUMENTS_TO_RANGE_LIST_MACRO

     printf ("ERROR: void Array_Domain_Type::Initialize_Domain( ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE ) not implemented \n");
     APP_ABORT();

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency ("Called from Array_Domain_Type::Initialize_Domain(int,int,int,int,int)");
#endif
   }
 // ... end of PPP ...
#endif


// ************************************************************************************
// ************************************************************************************
//        General Initialization of Domain objects (for Indexing operations) 
// ************************************************************************************
// ************************************************************************************

void
Array_Domain_Type::Initialize_Domain (
   const Array_Domain_Type & X ,
   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List )
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
   {
      printf ("Inside of Array_Domain_Type::Initialize_Domain ");
      printf ("( const Array_Domain_Type & X , Internal_Index_List )\n");
   }
#endif

   APP_ASSERT( Internal_Index_List != NULL );
 
   int i = 0;
   for (i=0; i < MAX_ARRAY_DIMENSION; i++)
      {
        Index_Array[i]       = NULL;

#if defined(PPP)
     // Used to support P++ indirect addressing
        Local_Index_Array[i] = NULL;
#endif

// if defined(USE_EXPRESSION_TEMPLATES)
        IndexBase[i]        = 0;
        IndexStride[i]      = 0;
        IndexDataPointer[i] = NULL;
// endif
      }

  // Domain reference counting is currently not used but will be used later!
   referenceCount = getReferenceCountBase();

   Uses_Indirect_Addressing = FALSE;
   Domain_Dimension     = X.Domain_Dimension;

// I think this is the proper constraint (May 11, 1997)
// ... (remove this assert, SerialArray might not live on this processor) ...
//   APP_ASSERT (Domain_Dimension > 0);

#if 0
// It makes sense that these should be set in a function that is supposed to Initialize the Domain
// I'm not sure why they had not been set previously (Dan, 5/31/97)
   View_Offset              = 0;
// if defined(USE_EXPRESSION_TEMPLATES)
   ExpressionTemplateOffset = 0;
// endif
#endif

#if defined(PPP)
     Partitioning_Object_Pointer = NULL;
     Initialize_Parallel_Parts_Of_Domain (X);
#endif

   // If we have a Null Array then set all the bases and bounds to a consistant value.
   // This was suggested by Kristi.  It is tested for in the Test_Consistancy
   // function.  Doing the initialization here means that we only add the overhead
   // of a conditional test to the cost of the normal indexing. (This is really not
   // a big deal).
   if (Is_A_Null_Array == TRUE)
   {
      // We force this to be FALSE in the case of a Null Array (generated by an 
      // Null_Index).
      Is_Contiguous_Data  = FALSE;

      for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
      {
         Base   [i] =  0;
         Bound  [i] = -1;
         // Not certain the stride should be reset (but this is done along 
         // the axis with the Null_Index so we duplicate it here).
         Stride [i] =  1;

#if defined(PPP)
         // I think we have to set the Local_Mask_Index as well.
	 // ... (5/22/98,kdb) fix Global_Index too ...
         Local_Mask_Index [i] = Range (0,-1,1,Null_Index);
         Global_Index [i] = Range (0,-1,1,Null_Index);
#endif
      }
   }

   // This is set so that indexed views will be handled correctly
   Is_A_View          = TRUE;

   // A view is not a temporary 
   // Unless the original array was a temporary then the view is temporary
   // AND if the data is contiguous then the data may be stold within the
   // binary operations.

   // Make sure that X.Is_A_Temporary uses either TRUE or FALSE (1 or 0) for values
   APP_ASSERT ( (X.Is_A_Temporary == TRUE) || (X.Is_A_Temporary == FALSE) );

   // Bugfix (1/26/96) a view should not be a temporary since it could then be
   // deleted within the delete if temporary function if the destructors
   // for the view were called too early as is the case on the SGI.
   // The problem appeared in the P6 = -12 + (P5+P4)(all,all,3) test in
   // the testcode.C program that Kristi wrote.
   // What this means is that a view of a temporary is not a temporary in the A++
   // sense because the view is on the local scope and its destructor is called
   // by the compiler where as an A++ temporary is on the heap and its destructor is
   // called by A++.  If we didn't have it this way then the compiler could delete
   // something allocated within local scope as a local variable.

   Is_A_Temporary = FALSE;

   // This allows the operator()() to be a contiguous data view
   // so that the data can be swaped in this special case which is
   // important for the support of conditional operations on the 
   // multidimensional arrays (which we will move to a little later).
   bool Is_Data_Base_Constant = TRUE;
   int temp;
   for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
      Is_Data_Base_Constant = 
	 (Is_Data_Base_Constant) ? (Data_Base[0] == Data_Base[temp]) : FALSE; 
   Constant_Data_Base = Is_Data_Base_Constant;

   // Bug fix (8/18/94) only equality between the Strides was checked
   // must test for Unit stride (Stride[0] == 1)
   bool Is_Unit_Stride = TRUE;
   for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        Is_Unit_Stride = (Is_Unit_Stride) ? (Stride[temp] == 1) : FALSE;
   Constant_Unit_Stride = Is_Unit_Stride;

   // Bug fix (8/18/94) case of non unit stride view flaged a Contiguous of bases and 
   // bound match -- AN ERROR 
   // Is_Contiguous_Data is automatically FALSE if view is non unit stride ( BUG 35 
   // -- Jeff Saltzman )
   if ( Constant_Unit_Stride && !Uses_Indirect_Addressing)
   {
      bool Is_Contiguous_Data_Test = X.Is_Contiguous_Data;
      for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
         Is_Contiguous_Data_Test = (Is_Contiguous_Data_Test) ? 
            ((Base[temp] == X.Base[temp]) && (Bound[temp] == X.Bound[temp])) : FALSE;
        Is_Contiguous_Data = Is_Contiguous_Data_Test;
   }
   else
   {
      Is_Contiguous_Data = FALSE;
   }

   // A view gets the same Array_ID as that which it is a view of!
   // this is how the dependence is expressed between the view and the actual array 
   // variable.
   internalArrayID = X.internalArrayID;

   // Bug Fix (9/9/93) by dquinlan
   // Views are not anything special under defered evaluation! So set them false! 
   // (WRONG) Actually views are special since the view must have its lifetime 
   // extended too!
   Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;

   // This is a view so these should be the same.
   builtUsingExistingData = X.builtUsingExistingData;

// if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset = Base[0];
     for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          ExpectedOffset += Base[temp] * Size[temp-1];
        }
     ExpressionTemplateOffset = ExpectedOffset;
     APP_ASSERT (ExpressionTemplateOffset == ExpectedOffset);
// endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
       {
         printf ("Leaving Initialize_Domain -- X.Domain_Dimension = ");
         printf ("%d Domain_Dimension = %d \n",
              X.Domain_Dimension,Domain_Dimension);
       }
#endif
   }

#if defined(PPP)
// ******************************************************************************
//     Initialization for when the SerialArray in a P++ array is a NullArray
// ******************************************************************************
void
Array_Domain_Type::resetDomainForNullArray ()
   {
  // This function is called by the array object constructors where in the indexing of an array object
  // the case of a serialArray being a NullArray (because of indexing of the global array).

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Array_Domain_Type::resetDomainForNullArray ");
        }
#endif

  // This is the only information cant needs to be reset in this case
     for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Local_Mask_Index [i] = Internal_Index (0,0,1,Null_Index);

	  Left_Number_Of_Points  [i] = 0;
	  Right_Number_Of_Points [i] = 0;
        }
   }
#endif

// ******************************************************************************
//                     Initialization for indirect addressing 
// ******************************************************************************
void
Array_Domain_Type::Initialize_Domain ( const Array_Domain_Type & X , 
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      {
        printf ("Inside of Array_Domain_Type::Initialize_Domain ");
        printf ("( const Array_Domain_Type & X , Indirect_Index_List )\n");
      }
#endif

// Initialize this for purify
   Uses_Indirect_Addressing = FALSE;

   int temp = 0;
   for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
      {
#if defined(PPP)
     // Used to support P++ indirect addressing
        Local_Index_Array[temp] = NULL;
#endif

     // Initialize this to avoid purify warnings
        Scalar_Offset[temp] = 0;
      }

   APP_ASSERT( Indirect_Index_List != NULL );

   if (X.Uses_Indirect_Addressing)
   {
   // Case of indexing a descriptor previous indexed by indirectaddressing
      Initialize_IndirectAddressing_Domain_Using_IndirectAddressing( X , Indirect_Index_List );
   }
   else
   {
      APP_ASSERT(X.Uses_Indirect_Addressing == FALSE);
   // Case of nonindirectaddressing descriptor accessed buy indirectadressing
      Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing( X , Indirect_Index_List );
   }

   // Domain reference counting is currently not used but will be used later!
   referenceCount = getReferenceCountBase();

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)    
      printf ("Marking descriptor as Uses_Indirect_Addressing = TRUE! \n");
#endif

// Now set this to be true because we should have been handed at least one valid intArray in the indexing list
   Uses_Indirect_Addressing = TRUE;

// Dimension of view built using indirect addressing is always 1D!
   Domain_Dimension = 1;

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)    
      printf ("Domain_Dimension = %d  Uses_Indirect_Addressing = %s \n",
               Domain_Dimension,(Uses_Indirect_Addressing) ? "TRUE" : "FALSE");
#endif
 
#if 0
// It makes sense that these should be set in a function that is supposed to Initialize the Domain
// I'm not sure why they had not been set previously (Dan, 5/31/97)
   View_Offset              = 0;
// if defined(USE_EXPRESSION_TEMPLATES)
   ExpressionTemplateOffset = 0;
// endif
#endif

#if defined(PPP)
   Partitioning_Object_Pointer     = NULL;
#if defined(USE_PADRE)
   parallelPADRE_DescriptorPointer = NULL;
#endif
   Initialize_Parallel_Parts_Of_Domain (X);
#endif

   // This is set so that indexed views will be handled correctly
   Is_A_View          = TRUE;

   // A view is not a temporary in that it's data is not temporary and thus
   // can't be stolen for use in binary operators (actually Array_Data == NULL
   // and all references are made through the Original_Array field).
   // Unless the original array was a temporary then the view is temporary
   // AND if the data is contiguous then the data may be stold within the
   // binary operations.

   // Bugfix (3/17/96) This makes the setting ot this value consistant with the 
   // function for Index object
   Is_A_Temporary = FALSE;

   // This allows the operator()() to be a contiguous data view
   // so that the data can be swaped in this special case which is
   // important for the support of conditional operations on the 
   // multidimensional arrays (which we will move to a little later).

   bool Is_Data_Base_Constant = TRUE;
   for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
      Is_Data_Base_Constant = (Is_Data_Base_Constant) ? 
	(Data_Base[0] == Data_Base[temp]) : FALSE; 
   Constant_Data_Base = Is_Data_Base_Constant;

   // Bug fix (8/18/94) only equality between the Strides was checked
   // must test for Unit stride (Stride[0] == 1)
   bool Is_Unit_Stride = TRUE;
   for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
      Is_Unit_Stride = (Is_Unit_Stride) ? (Stride[temp] == 1) : FALSE;
   Constant_Unit_Stride = Is_Unit_Stride;

   // Bug fix (8/18/94) case of non unit stride view flaged a Contiguous of bases 
   // and bound match -- AN ERROR
   // Is_Contiguous_Data is automatically FALSE if view is non unit stride 
   // ( BUG 35 -- Jeff Saltzman )
   if ( Constant_Unit_Stride && !Uses_Indirect_Addressing )
   {
      bool Is_Contiguous_Data_Test = X.Is_Contiguous_Data;
      for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
         Is_Contiguous_Data_Test = (Is_Contiguous_Data_Test) ? 
           ((Base[temp] == X.Base[temp]) && (Bound[temp] == X.Bound[temp])) : FALSE;
      Is_Contiguous_Data = Is_Contiguous_Data_Test;
   }
   else
   {
      Is_Contiguous_Data = FALSE;
   }

   // A view gets the same Array_ID as that which it is a view of!
   // this is how the dependence is expressed between the view and the actual array 
   // variable.
   internalArrayID = X.internalArrayID;

   // Bug Fix (9/9/93) by dquinlan
   // Views are not anything special under defered evaluation! So set them false! 
   // (WRONG) Actually views are special since the view must have its lifetime 
   // extended too!
   Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;

   // This is a view so these should be the same.
   builtUsingExistingData = X.builtUsingExistingData;

// if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset = Base[0];
     for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          ExpectedOffset += Base[temp] * Size[temp-1];
        }
     ExpressionTemplateOffset = ExpectedOffset;
     APP_ASSERT (ExpressionTemplateOffset == ExpectedOffset);
// endif

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 3)
      {
     // At this point the domain is not completly initialized so we get UMR: Uninitialized memory read errors from PURIFY
     // display ("Called from Array_Domain_Type::Initialize_Domain (Array_Domain_Type,Indirect_Index_List)");
        printf ("Leaving Array_Domain_Type::Initialize_Domain ");
        printf ("( const Array_Domain_Type & X , Indirect_Index_List )\n");
      }
#endif
}

void
Array_Domain_Type::reshape( int Number_Of_Valid_Dimensions , const int* View_Sizes, const int* View_Bases)
   {
  // This function is only called by the reshape member function.
  // We could just call it the reshape function for a domain (that would make more sense!)

     int temp = 0;

     Size [0] = View_Sizes[0]; 
     for (temp=1; temp < Number_Of_Valid_Dimensions; temp++)
        {
          Size [temp] = Size[temp-1] * View_Sizes[temp];

          APP_ASSERT (View_Sizes[temp] >= 0);
          if (View_Sizes[temp] == 0)
               Is_A_Null_Array = TRUE;
        }

  // Initialize the other dimensions to avoid errors in purify (and errors in general!)
     for (temp=Number_Of_Valid_Dimensions; temp < MAX_ARRAY_DIMENSION; temp++)
          Size [temp] = Size[temp-1];

  // ... Since this is a view this is probably false and even if the data
  // is contiguous this will only make the code slightly less efficient ...
     Is_Contiguous_Data = FALSE;

     Is_A_View = TRUE;

  // ... Scalar_Offset won't be 0 in rare case where APP_Global_Array_Base
  // is nonzero and Size has changed so reset this ...
     Scalar_Offset[0] = -User_Base[0] * Stride[0];
     for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Stride[temp] * Size[temp-1];
        }

  // ... fix bases so that view will be in correct place ...
     for (temp=0; temp < Number_Of_Valid_Dimensions; temp++)
        {
          Base[temp] = View_Bases[temp];
          Bound[temp] += Base[temp];
        }
   }

// **********************************************************************
// **********************************************************************
// ********************  CONSTRUCTORS  **********************************
// **********************************************************************
// **********************************************************************

// **********************************************************************
// ***********************  Used in the operator() **********************
// **********************************************************************
Array_Domain_Type::Array_Domain_Type ( 
     const Array_Domain_Type & X , 
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Array_Domain_Type::Array_Domain_Type ");
          printf ("( const Array_Domain_Type & X , Internal_Index** Index_List )");
          printf ("(this = %p)\n",this);
        }
#endif

     if (X.Uses_Indirect_Addressing == TRUE)
        {
          printf ("Sorry, not implemented: Case of Index objects use upon view built ");
          printf ("from indirect addressing not implemented \n");
          APP_ABORT();
        }

  // bug fix (8/19/96,kdb) previous code didn't work with views of views
  // View_Offset = 0;
  // if defined(USE_EXPRESSION_TEMPLATES)
     ExpressionTemplateOffset = X.ExpressionTemplateOffset;
  // endif
     View_Offset = X.View_Offset;
     Preinitialize_Domain ( X );
     int i = 0;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_List[i] != NULL)
               Initialize_View_Of_Target ( i , *(Index_List[i]) , X );
            else
               Initialize_Non_Indexed_Dimension ( i , X );

          if (i==0)
               View_Offset += (User_Base[i] - X.User_Base[i]) * X.Stride[i];
            else
               View_Offset += (User_Base[i] - X.User_Base[i]) * X.Stride[i] * Size[i-1];

// if defined(USE_EXPRESSION_TEMPLATES)
          if (i==0)
               ExpressionTemplateOffset += (Base[i] - X.Base[i]);
            else
               ExpressionTemplateOffset += (Base[i] - X.Base[i]) * Size[i-1];
// endif
        }
  
#if defined(PPP)
     Partitioning_Object_Pointer     = NULL;
#if defined(USE_PADRE)
     parallelPADRE_DescriptorPointer = NULL;
#endif
#endif
     Initialize_Domain ( X , Index_List );

#if 0
     for (i=1; i < MAX_ARRAY_DIMENSION; i++)
          printf ("BEFORE RESET: Scalar_Offset[%d] = %d \n",i,Scalar_Offset[i]);
#endif

  // printf ("BEFORE RESET: View_Offset = %d Stride[0] = %d \n",View_Offset,Stride[0]);
  // printf ("BEFORE RESET: View_Offset = %d Stride[0] = %d User_Base[0] = %d \n",View_Offset,Stride[0],User_Base[0]);

     Scalar_Offset[0] = -User_Base[0] * Stride[0];
     Scalar_Offset[0] += View_Offset;
  // Scalar_Offset[0] += View_Offset * Stride[0];
     for (i=1; i < MAX_ARRAY_DIMENSION; i++)
          Scalar_Offset[i] = Scalar_Offset[i-1] - User_Base[i] * Stride[i] * Size[i-1];
#if 0
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf ("AFTER RESET: Scalar_Offset[%d] = %d \n",i,Scalar_Offset[i]);
#endif

     APP_ASSERT(internalArrayID == X.internalArrayID);

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from Array_Domain_Type (Array_Domain_Type,Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type*) \n");
#endif
   }


Array_Domain_Type &
Array_Domain_Type::displayReferenceCounts (const char* label) const
   {
     printf ("     Inside of Array_Domain_Type::displayReferenceCounts(%s) \n",label);
     printf ("          Array_Domain_Type Reference Count (Array id = %d) = %d \n",Array_ID(),getReferenceCount());

#if defined(PPP)
     if (Array_Conformability_Info != NULL)
        {
          printf ("          Array_Conformability_Info = %p getReferenceCount() = %d \n",
               Array_Conformability_Info,Array_Conformability_Info->getReferenceCount());
        }
       else
        {
          printf ("          Array_Conformability_Info IS NULL \n");
        }
#endif

  // optionally print out the reference count information about the indirection vectors
     if (usesIndirectAddressing() == TRUE)
        {
          int i = 0;
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               if (Index_Array[i] != NULL)
                  {
                    printf ("          indirect addressing intArray [%d] -- Array Reference Count (Array id = %d) = %d \n",
                       i,Index_Array[i]->Array_ID(),Index_Array[i]->getReferenceCount());
                    Index_Array[i]->displayReferenceCounts();
                  }
                 else
                  {
                    printf ("          indirect addressing intArray [%d] == NULL \n",i);
                  }
             }
        }

  // Cast away const here
     return *((Array_Domain_Type*) this);
   }

// ************************************************************************
// Array_Domain_Type constructors for indirect addressing
// ************************************************************************
Array_Domain_Type::
Array_Domain_Type ( const Array_Domain_Type & X , 
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of Array_Domain_Type::Array_Domain_Type ( const Array_Domain_Type & X , Internal_Indirect_Addressing_Index* Indirect_Index_List ) (this = %p)\n",this);
#endif

// *******************************************************************
// This code is derived from the similar Array_Domain_Type for Internal_Index objects
// *******************************************************************
// if defined(USE_EXPRESSION_TEMPLATES)
     ExpressionTemplateOffset = X.ExpressionTemplateOffset;
// endif
     View_Offset = X.View_Offset;
// *******************************************************************

     Preinitialize_Domain ( X );

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
// NOTE: This loop over the number of dimensions is at least somewhat 
//       redundent with the one in the Initialize_Domain function!
     printf ("WARNING: In Array_Domain_Type(Array_Domain_Type,Indirect_Index_List): \n");
     printf ("         Redundent loop over dimensions for indirect addressing! \n");
#endif

     int i = 0;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Indirect_Index_List[i] != NULL)
             {
               APP_ASSERT ( (Indirect_Index_List[i]->intArrayInStorage == FALSE) || 
                            (Indirect_Index_List[i]->IndexInStorage == FALSE) );
               if (Indirect_Index_List[i]->intArrayInStorage == TRUE)
                  {
                 // printf ("In indirect addressing constructor: Axis = %d Indirect_Index_List[%d]->intArrayInStorage == TRUE \n",i,i);
                    Initialize_Indirect_Addressing_View_Of_Target ( i , *(Indirect_Index_List[i]->IndirectionArrayPointer) , X );
                  }
                 else
                  {
                    if (Indirect_Index_List[i]->IndexInStorage == TRUE)
                       {
                      // printf ("In indirect addressing constructor: Axis = %d Indirect_Index_List[%d]->IndexInStorage == TRUE \n",i,i);
                         Initialize_Indirect_Addressing_View_Of_Target ( i , Indirect_Index_List[i]->Index_Data , X );
                       }
                      else
                       {
                      // printf ("In indirect addressing constructor: Axis = %d Indirect_Index_List[%d]->IndexInStorage == FALSE \n",i,i);
                         Initialize_Indirect_Addressing_View_Of_Target ( i , Indirect_Index_List[i]->Scalar , X );
                       }
                  }
             }
            else
             {
            // printf ("In indirect addressing constructor: Axis = %d Non Indexed dimension \n",i,i);
               Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension ( i , X );
             }

       // *******************************************************************
       // This code is derived from the similar Array_Domain_Type for Internal_Index objects
       // *******************************************************************
          if (i==0)
               View_Offset+= (User_Base[i] - X.User_Base[i]) * X.Stride[i];
            else
               View_Offset+= (User_Base[i] - X.User_Base[i]) * X.Stride[i] * Size[i-1];
// if defined(USE_EXPRESSION_TEMPLATES)
          if (i==0)
               ExpressionTemplateOffset = (Base[i] - X.Base[i]);
            else
               ExpressionTemplateOffset = (Base[i] - X.Base[i]) * Size[i-1];
// endif
       // *******************************************************************

       // display ("Axis by axis -- intermediate Domain in constructor Array_Domain_Type (Array_Domain_Type,Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type) \n");
        }

  // This function overwrites some of the date just setup (see note above)
     Initialize_Domain ( X , Indirect_Index_List );

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 2)
        {
          displayReferenceCounts("AFTER Initialize_Domain ( X , Indirect_Index_List ) (for indirect addressing)");
        }
#endif

#if defined(PPP)
     Compute_Local_Index_Arrays (X);
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Before call to Test_Consistency! (this = %p)\n",this);
#endif

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from Array_Domain_Type (Array_Domain_Type,Internal_Indirect_Addressing_Index*)");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Leaving Array_Domain_Type::Array_Domain_Type ( const Array_Domain_Type & X , Internal_Indirect_Addressing_Index* Indirect_Index_List ) (this = %p)\n",this);
#endif

// *******************************************************************
// This code is derived from the similar Array_Domain_Type for Internal_Index objects
// *******************************************************************
   Scalar_Offset[0] = -User_Base[0] * Stride[0];
   Scalar_Offset[0] += View_Offset;
   for (i=1; i < MAX_ARRAY_DIMENSION; i++)
        Scalar_Offset[i] = Scalar_Offset[i-1] - User_Base[i] * Stride[i] * Size[i-1];
// *******************************************************************
// Old code (which was inconsistant with the non-indirect addressing code)
// View_Offset = 0;
// for (i=0; i < MAX_ARRAY_DIMENSION; i++)
//   Scalar_Offset[i] = 0;
// *******************************************************************

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 2)
        {
          displayReferenceCounts("At END of constructor (for indirect addressing)");
        }
#endif

#if 1
// PADRE is currently building a new PADRE descriptor for each array domain
// so we should use that one.  It might be a memory leak to just overwrite the pointer with
// the one form the input "X".

#if defined(USE_PADRE) && defined(PPP)
  // The present implementation of PADRE requires the use of the object we just
  // constructed so we have to initialize the PADRE object AFTER we have initialized the
  // this object (the parallel Array_Domain_Type).
  // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)

#if 0
     if (parallelPADRE_DescriptorPointer != NULL)
        {
          APP_ASSERT (parallelPADRE_DescriptorPointer == X.parallelPADRE_DescriptorPointer);
        }

  // Make sure we don't strand an existing (different PADRE descriptor)
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
#endif

  // Use the PADRE_Descriptor from the source of the view!
     APP_ASSERT (X.parallelPADRE_DescriptorPointer != NULL);
     parallelPADRE_DescriptorPointer = X.parallelPADRE_DescriptorPointer;
     X.parallelPADRE_DescriptorPointer->incrementReferenceCount();
#endif
#endif
#endif
   }

Array_Domain_Type::Array_Domain_Type() 
   {
  // This builds the NULL Array_Domain (used in NULL arrays)!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Array_Domain_Type::Array_Domain_Type() this = %p \n",this);
       // printf ("WARNING: It is a ERROR to use this descriptor constructor except for NULL arrays! \n");
       // APP_ABORT();
        }
#endif

     int Integer_List[MAX_ARRAY_DIMENSION];
  // We have to initialize this to avoid UMR in purify
     Integer_List[0] = 0;
     Initialize_Domain(0,Integer_List);
   }


#ifdef USE_STRING_SPECIFIC_CODE

// this is non-portable code (that we are currently testing)
// include <string.h>

#if 0
char*
my_strdup ( const char* X )
   {
     int length = strlen(X) + 1;;
     char* returnValue = new char[length];
     strcpy (returnValue,X);
     return returnValue;
   }
#endif

void
Array_Domain_Type::parseString ( const char* dataString, int & pos , int dimentionArray[] )
   {
  // This function parses a string to d e f i n e the data that will be stored 
  // in an array object.  Current problem with this function is that it 
  // does not correctly interprete the dimensions of the array in the case
  // of a multidimensional array declaration (as a string).

     static int dimension    = 0;
     static int maxDimension = 0;

  // skip leading white space
     pos += strspn(&(dataString[pos])," ");

     while (dataString[pos] == '{')
        {
          dimension++;
          maxDimension = (dimension > maxDimension) ? dimension : maxDimension;

          pos++;

       // Figure out the array dimensions only increment dimensions
       // that have not previously been incremented
       // Note that the lowest dimension (the highest in the permuted
       // dimensions such as what we compute here) MUST be computed separately
       // (since it is the inner most string).
          if (dimension >= 2)
             {
               dimentionArray[dimension-2]++;
            // reset lower level entries in the dimentionArray
               for (int i=dimension-1; i < MAX_ARRAY_DIMENSION; i++)
                    dimentionArray[i] = 0;
             }

       // printf ("dimension = %d \n",dimension);
       // for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
       //      printf ("dimentionArray[%d] = %d \n",i,dimentionArray[i]);

          parseString (dataString,pos,dimentionArray);
          dimension--;
        }

  // Now read the number string!
  // printf ("&(dataString[pos]) = %s pos = %d  dimension = %d  maxDimension = %d \n",
  //      &(dataString[pos]),pos,dimension,maxDimension);
     char* delimiterListForInnerString = " 0123456789,";
     int length = strspn(&(dataString[pos]),delimiterListForInnerString);
     char* copyString = new char[length+1];
     copyString[length] = '\0';
     strncpy(copyString,&(dataString[pos]),length);
     copyString[length] = '\0';

     if (strlen(copyString) > 0)
        {
       // printf ("copyString = %s \n",copyString);

       // Now parse the inner string to get the values
          char* delimiterListForDataValues = " ,"; 
          char* nextSubString = NULL;
          int counter = 0;
          if ( (nextSubString = strtok(copyString,delimiterListForDataValues)) != NULL )
             {
               counter = 1;
            // printf ("nextSubString = %s \n",nextSubString);
               while ( (nextSubString = strtok(NULL,delimiterListForDataValues)) != NULL )
                  {
                 // printf ("nextSubString = %s \n",nextSubString);
                    counter++;
                  }
               dimentionArray[dimension-1] = counter;
            // printf ("counter = %d dimension = %d dimentionArray[%d] = %d \n",
            //      counter,dimension,dimension,dimentionArray[dimension-1]);
             }
            else
             {
            // printf ("nextSubString = NULL \n");
             }
        }

     pos += strspn(&(dataString[pos])," 0123456789,");

     if (dataString[pos] == '}')
        {
       // printf ("Found a '}' at the end! (pos = %d) \n",pos);
          pos++;
        }
       else
        {
       // printf ("Can NOT find a '}' at the end! (pos = %d) \n",pos);
        }

     if (dataString[pos] == ',')
        {
       // printf ("Found a comma! (pos = %d) \n",pos);
          pos++;
        }
       else
        {
       // printf ("Can NOT find a comma! (pos = %d) \n",pos);
        }
   }
#endif


// Array_Domain_Type::Array_Domain_Type ( const char* dataString )
Array_Domain_Type::Array_Domain_Type ( const AppString & dataString )
   {
  // This builds an array from a string

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Array_Domain_Type::Array_Domain_Type(const char*) this = %p \n",this);
        }
#endif

#ifdef USE_STRING_SPECIFIC_CODE
     char* numberString = strdup(dataString.getInternalString());
#else
     if (&dataString);

     printf ("Using Array initialization from string: \n");
     printf ("   but string functions are not supported unless A++/P++ \n");
     printf ("   is compiled with USE_STRING_SPECIFIC_CODE! \n");
     printf ("   See configure --help for A++/P++ compile options... exiting \n");
     APP_ABORT();
#endif

     int position = 0;

     int permutedDimentionArray[MAX_ARRAY_DIMENSION];
     int i;

  // All arrays are by default 1x1x1x...
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          permutedDimentionArray[i] = 0;

#ifdef USE_STRING_SPECIFIC_CODE
     parseString (numberString,position,permutedDimentionArray);
#endif

  // for (i=0; i < MAX_ARRAY_DIMENSION; i++)
  //      printf ("permutedDimentionArray[%d] = %d \n",i,permutedDimentionArray[i]);

     i = 0;
     while (i < MAX_ARRAY_DIMENSION && permutedDimentionArray[i] != 0)
        {
          i++;
        }

     int numberOfDimensions = i;
  // printf ("numberOfDimensions = %d \n",numberOfDimensions);

     int dimentionArray        [MAX_ARRAY_DIMENSION];
  // Integer_Array_MAX_ARRAY_DIMENSION_Type dimentionArray;
     for (i=0; i < numberOfDimensions; i++)
        {
          dimentionArray[i] = permutedDimentionArray[(numberOfDimensions-i)-1];
       // printf ("dimentionArray[%d] = %d \n",i,dimentionArray[i]);
        }

     int Integer_List[MAX_ARRAY_DIMENSION];
  // We have to initialize this to avoid UMR in purify
  // Integer_List[0] = 0;
  // Initialize_Domain(0,Integer_List);

     for (i=0; i < numberOfDimensions; i++)
        {
          Integer_List[i] = dimentionArray[i];
       // printf ("Integer_List[%d] = %d \n",i,Integer_List[i]);
        }

     Initialize_Domain(numberOfDimensions,Integer_List);
   }

Array_Domain_Type::Array_Domain_Type ( const Internal_Index & I )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);

  // This has to be part of the non-internal debug code path
     for (int temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
          Internal_Index_List[temp] = NULL;
     
     Initialize_Domain (1,Internal_Index_List);
   }
#if (MAX_ARRAY_DIMENSION >= 2)
Array_Domain_Type::Array_Domain_Type ( const Internal_Index & I, const Internal_Index & J )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);

  // This has to be part of the non-internal debug code path
     for (int temp = 2; temp < MAX_ARRAY_DIMENSION; temp++)
          Internal_Index_List[temp] = NULL;
     
     Initialize_Domain (2,Internal_Index_List);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
Array_Domain_Type::Array_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                                           const Internal_Index & K )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);

  // This has to be part of the non-internal debug code path
     for (int temp = 3; temp < MAX_ARRAY_DIMENSION; temp++)
          Internal_Index_List[temp] = NULL;
     
     Initialize_Domain (3,Internal_Index_List);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
Array_Domain_Type::Array_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                                           const Internal_Index & K, const Internal_Index & L )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);

  // This has to be part of the non-internal debug code path
     for (int temp = 4; temp < MAX_ARRAY_DIMENSION; temp++)
          Internal_Index_List[temp] = NULL;
     
     Initialize_Domain (4,Internal_Index_List);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
Array_Domain_Type::Array_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                                           const Internal_Index & K, const Internal_Index & L,
                                           const Internal_Index & M )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Internal_Index_List[4] = &((Internal_Index&) M);

  // This has to be part of the non-internal debug code path
     for (int temp = 5; temp < MAX_ARRAY_DIMENSION; temp++)
          Internal_Index_List[temp] = NULL;
     
     Initialize_Domain (5,Internal_Index_List);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
Array_Domain_Type::Array_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                                           const Internal_Index & K, const Internal_Index & L,
                                           const Internal_Index & M, const Internal_Index & N )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Internal_Index_List[4] = &((Internal_Index&) M);
     Internal_Index_List[5] = &((Internal_Index&) N);

  // This has to be part of the non-internal debug code path
     for (int temp = 6; temp < MAX_ARRAY_DIMENSION; temp++)
          Internal_Index_List[temp] = NULL;
     
     Initialize_Domain (6,Internal_Index_List);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
Array_Domain_Type::Array_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                                           const Internal_Index & K, const Internal_Index & L,
                                           const Internal_Index & M, const Internal_Index & N,
                                           const Internal_Index & O )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Internal_Index_List[4] = &((Internal_Index&) M);
     Internal_Index_List[5] = &((Internal_Index&) N);
     Internal_Index_List[6] = &((Internal_Index&) O);

  // This has to be part of the non-internal debug code path
     for (int temp = 7; temp < MAX_ARRAY_DIMENSION; temp++)
          Internal_Index_List[temp] = NULL;
     
     Initialize_Domain (7,Internal_Index_List);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
Array_Domain_Type::Array_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                                           const Internal_Index & K, const Internal_Index & L,
                                           const Internal_Index & M, const Internal_Index & N,
                                           const Internal_Index & O, const Internal_Index & P )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Internal_Index_List[4] = &((Internal_Index&) M);
     Internal_Index_List[5] = &((Internal_Index&) N);
     Internal_Index_List[6] = &((Internal_Index&) O);
     Internal_Index_List[7] = &((Internal_Index&) P);

  // This has to be part of the non-internal debug code path
     for (int temp = 8; temp < MAX_ARRAY_DIMENSION; temp++)
          Internal_Index_List[temp] = NULL;
     
     Initialize_Domain (8,Internal_Index_List);
   }
#endif


Array_Domain_Type::Array_Domain_Type( 
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   {
     Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);

#if defined(USE_PADRE) && defined(PPP)
  // The present implementation of PADRE requires the use of the object we just
  // constructed so we have to initialize the PADRE object AFTER we have initialized the
  // this object (the parallel Array_Domain_Type).
  // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
  // parallelPADRE_DescriptorPointer = 
  //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
#endif
#endif
   }

Array_Domain_Type::Array_Domain_Type( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List )
   {
     Initialize_Domain (Number_Of_Valid_Dimensions,Internal_Index_List);

#if defined(USE_PADRE) && defined(PPP)
  // The present implementation of PADRE requires the use of the object we just
  // constructed so we have to initialize the PADRE object AFTER we have initialized the
  // this object (the parallel Array_Domain_Type).
  // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
  // parallelPADRE_DescriptorPointer = 
  //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
#endif
#endif
   }

Array_Domain_Type::Array_Domain_Type( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   {
     Initialize_Domain (MAX_ARRAY_DIMENSION,Integer_List);

#if defined(USE_PADRE) && defined(PPP)
  // The present implementation of PADRE requires the use of the object we just
  // constructed so we have to initialize the PADRE object AFTER we have initialized the
  // this object (the parallel Array_Domain_Type).
  // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
  // parallelPADRE_DescriptorPointer = 
  //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
#endif
#endif
   }

Array_Domain_Type::Array_Domain_Type( ARGUMENT_LIST_MACRO_INTEGER )
   {
     INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO
     Initialize_Domain (MAX_ARRAY_DIMENSION,Integer_List);

#if defined(USE_PADRE) && defined(PPP)
  // The present implementation of PADRE requires the use of the object we just
  // constructed so we have to initialize the PADRE object AFTER we have initialized the
  // this object (the parallel Array_Domain_Type).
  // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
  // parallelPADRE_DescriptorPointer = 
  //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
#endif
#endif
   }

#if defined(PPP)
Array_Domain_Type::Array_Domain_Type( ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE )
   {
     Initialize_Domain (VARIABLE_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE);

#if defined(USE_PADRE) && defined(PPP)
  // The present implementation of PADRE requires the use of the object we just
  // constructed so we have to initialize the PADRE object AFTER we have initialized the
  // this object (the parallel Array_Domain_Type).
  // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
  // parallelPADRE_DescriptorPointer =
  //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
#endif
#endif
   }
#endif

#if defined(PPP) || defined(APP)
// Array_Domain_Type::Array_Domain_Type( 
//           int Input_Array_Size_I , int Input_Array_Size_J , 
//           int Input_Array_Size_K , int Input_Array_Size_L ,
//           const Partitioning_Type & Partition ) 
Array_Domain_Type::Array_Domain_Type( 
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List , 
     const Internal_Partitioning_Type & Internal_Partition )
   {
#if 1
     Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List,Internal_Partition);
#else
     Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
     Partitioning_Object_Pointer = &((Internal_Partitioning_Type &) Internal_Partition);
     Partitioning_Object_Pointer->incrementReferenceCount();

  // The LocalGhostCellWidth may be set to non zero values before being used
  // to build a P++ array object
     int temp;
     for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
       // InternalGhostCellWidth [temp] = Internal_Partition.LocalGhostCellWidth [temp];
          InternalGhostCellWidth [temp] = Internal_Partition.getLocalGhostCellWidth (temp);
        }
#endif

#if defined(USE_PADRE) && defined(PPP)
  // The present implementation of PADRE requires the use of the object we just
  // constructed so we have to initialize the PADRE object AFTER we have initialized the
  // this object (the parallel Array_Domain_Type).
  // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
  // parallelPADRE_DescriptorPointer = 
  //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency ("Called from constructor Array_Domain_Type(int,int,int,int)");
#endif
   }
#endif // ... end of PPP ...

#if defined(APP) || defined(PPP)
Array_Domain_Type::Array_Domain_Type( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Initialize_Domain (Number_Of_Valid_Dimensions,Internal_Index_List,Internal_Partition);

#if defined(USE_PADRE) && defined(PPP)
  // The present implementation of PADRE requires the use of the object we just
  // constructed so we have to initialize the PADRE object AFTER we have initialized the
  // this object (the parallel Array_Domain_Type).
  // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
  // parallelPADRE_DescriptorPointer = 
  //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);
#endif
#endif
   }

Array_Domain_Type::Array_Domain_Type ( 
     const Internal_Index & I,
     const Internal_Partitioning_Type & Internal_Partition )
   {
  // I.display("Internal_Index in Array_Domain_Type::Array_Domain_Type(I,Internal_Partition)");
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Initialize_Domain (1,Internal_Index_List,Internal_Partition);
   }
#if (MAX_ARRAY_DIMENSION >= 2)
Array_Domain_Type::Array_Domain_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Initialize_Domain (2,Internal_Index_List,Internal_Partition);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
Array_Domain_Type::Array_Domain_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Initialize_Domain (3,Internal_Index_List,Internal_Partition);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
Array_Domain_Type::Array_Domain_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Initialize_Domain (4,Internal_Index_List,Internal_Partition);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
Array_Domain_Type::Array_Domain_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Internal_Index_List[4] = &((Internal_Index&) M);
     Initialize_Domain (5,Internal_Index_List,Internal_Partition);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
Array_Domain_Type::Array_Domain_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Internal_Index_List[4] = &((Internal_Index&) M);
     Internal_Index_List[5] = &((Internal_Index&) N);
     Initialize_Domain (6,Internal_Index_List,Internal_Partition);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
Array_Domain_Type::Array_Domain_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Internal_Index_List[4] = &((Internal_Index&) M);
     Internal_Index_List[5] = &((Internal_Index&) N);
     Internal_Index_List[6] = &((Internal_Index&) O);
     Initialize_Domain (7,Internal_Index_List,Internal_Partition);
   }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
Array_Domain_Type::Array_Domain_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O, const Internal_Index & P,
     const Internal_Partitioning_Type & Internal_Partition )
   {
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
     Internal_Index_List[0] = &((Internal_Index&) I);
     Internal_Index_List[1] = &((Internal_Index&) J);
     Internal_Index_List[2] = &((Internal_Index&) K);
     Internal_Index_List[3] = &((Internal_Index&) L);
     Internal_Index_List[4] = &((Internal_Index&) M);
     Internal_Index_List[5] = &((Internal_Index&) N);
     Internal_Index_List[6] = &((Internal_Index&) O);
     Internal_Index_List[7] = &((Internal_Index&) P);
     Initialize_Domain (8,Internal_Index_List,Internal_Partition);
   }
#endif
#endif

// *****************************************************************************
// *****************************************************************************
// ***********************  MEMBER FUNCTION DEFINITIONS  ***********************
// *****************************************************************************
// *****************************************************************************

int
Array_Domain_Type::getGhostBoundaryWidth ( int axis ) const
   {
  // Error checking on input value
     APP_ASSERT (axis < MAX_ARRAY_DIMENSION);
     APP_ASSERT (axis >= 0);

#if defined(PPP)
  // We want to report the array objects ghost boundary width which might be 
  // different from that of the partitioning since the partitioning is a guide 
  // and the individual array objects can have their ghost boundaries setup separately
     int width = InternalGhostCellWidth [axis];
#if 0
     int partitionWidth = 0;
     if (Partitioning_Object_Pointer != NULL)
        {
          partitionWidth = Partitioning_Object_Pointer->getLocalGhostCellWidth(axis);
        }
       else
        {
          partitionWidth = Internal_Partitioning_Type::getDefaultInternalGhostCellWidth(axis);
        }
     APP_ASSERT (width = partitionWidth);
#endif
     return width;
#else
  // Avoid compiler warning about unused input variable
     if (&axis);
     return 0;
#endif
   }


void 
Array_Domain_Type::setInternalGhostCellWidth ( 
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   {
#if defined(PPP) 
  // This function modifies both the parallel and the serial Array_Domain
     int temp = 0;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Array_Domain_Type::setInternalGhostCellWidth -- ");
          for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
               printf ("%d ",Integer_List[temp]);
          printf (" \n");
        } 
#endif

  // Modify the parallel descriptor
  // get the old BlockPartiArrayDomain and build a new one
     int Array_Sizes [MAX_ARRAY_DIMENSION];
     getRawDataSize(Array_Sizes);
     int OldGhostCellWidth [MAX_ARRAY_DIMENSION];
     int Default_ExternalGhostCellWidth [MAX_ARRAY_DIMENSION]; 
     for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
       // Initialize arrays used to construction of BlockPartiArrayDomain
          Default_ExternalGhostCellWidth [temp] = 0;

       // Initialize descriptor data with new values
          OldGhostCellWidth      [temp] = InternalGhostCellWidth [temp];
          InternalGhostCellWidth [temp] = Integer_List [temp];

       // We want the array sizes with no ghost boundaries (I think this is what PARTI requires)
       // Array_Sizes            [temp] += Integer_List [temp] - InternalGhostCellWidth [temp];
          Array_Sizes            [temp] -= 0; // 2 * InternalGhostCellWidth [temp];
         // printf ("Inside of Array_Domain_Type::setInternalGhostCellWidth --- Array_Sizes[%d] = %d \n",
         //      temp,Array_Sizes[temp]);
        }
     
#if 0
     APP_ASSERT (BlockPartiArrayDecomposition != NULL);
     DARRAY *New_BlockPartiArrayDomain = NULL;
       // Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain ( 
       //      BlockPartiArrayDecomposition , Array_Sizes ,
       //      Integer_List , Default_ExternalGhostCellWidth );
     if (Partitioning_Object_Pointer != NULL)
        {
          New_BlockPartiArrayDomain =
               Partitioning_Object_Pointer->Build_BlockPartiArrayDomain (
                    BlockPartiArrayDecomposition , Array_Sizes ,
                    InternalGhostCellWidth , Default_ExternalGhostCellWidth );
        }
       else
        {
          New_BlockPartiArrayDomain =
               Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain (
                    BlockPartiArrayDecomposition , Array_Sizes ,
                    InternalGhostCellWidth , ExternalGhostCellWidth );
        }

     APP_ASSERT (New_BlockPartiArrayDomain != NULL);

  // Swap BlockPartiArrayDomains
  // delete BlockPartiArrayDomain;

#if 0
     if (BlockPartiArrayDomain->referenceCount == 0)
          printf ("##### BlockPartiArrayDomain WILL be deleted! \n");
       else
          printf ("##### BlockPartiArrayDomain will NOT be deleted! \n");
#endif

     delete_DARRAY (BlockPartiArrayDomain);
     BlockPartiArrayDomain = New_BlockPartiArrayDomain;
     New_BlockPartiArrayDomain = NULL;
#else
  // delete the existing BlockPartiArrayDomain since the Parallel_Allocation
  // member function of the array object will rebuild it using the ghost boundary
  // data set above.
#if defined(USE_PADRE)
  // What PADRE function do we call here?
  // printf ("NEED TO CALL PADRE \n"); APP_ABORT();

  // printf ("delete parallelPADRE_DescriptorPointer commented out! \n");
  // Added conventional mechanism for reference counting control
  // operator delete no longer decriments the referenceCount.
     parallelPADRE_DescriptorPointer->decrementReferenceCount();
     if (parallelPADRE_DescriptorPointer->getReferenceCount() < 
         parallelPADRE_DescriptorPointer->getReferenceCountBase())
          delete parallelPADRE_DescriptorPointer;
     parallelPADRE_DescriptorPointer = NULL;
#else
     APP_ASSERT (BlockPartiArrayDomain != NULL);
  // delete BlockPartiArrayDomain;

#if 0
     if (BlockPartiArrayDomain->referenceCount == 0)
          printf ("##### BlockPartiArrayDomain WILL be deleted! \n");
       else
          printf ("##### BlockPartiArrayDomain will NOT be deleted! \n");
#endif

     delete_DARRAY (BlockPartiArrayDomain);
     BlockPartiArrayDomain = NULL;
#endif // End of if not USE_PADRE
#endif

#if 0
  // Modify the serial descriptor
     for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          X.Base [temp] += Integer_List[temp] - OldGhostCellWidth[temp];
          X.Size [temp] += (2 * Integer_List[temp]) ;
        }
#endif

  // display ("look at the descriptor from within Array_Domain_Type::setInternalGhostCellWidth");
  // APP_ABORT();
#else
  // If this is not a P++ distributed issue then there is nothing to do!

  // Avoid compiler warning about unused variable
     if (&Integer_List);
#endif
   }

// ... PPP in i n c l u d e ...
#if defined(PPP) 
void
Array_Domain_Type::partition ( const Internal_Partitioning_Type & Internal_Partition )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::partition ( const Internal_Partitioning_Type & Internal_Partition ) \n");
#endif

  // First delete the existing partition (if it exists)
     if (Partitioning_Object_Pointer != NULL)
        {
       // Bugfix (11/14/2000) Make reference counting consistent with code elsewhere
       // if (Partitioning_Object_Pointer->referenceCount-- == Internal_Partitioning_Type::getReferenceCountBase())
          APP_ASSERT (Partitioning_Object_Pointer->getReferenceCount() >= Internal_Partitioning_Type::getReferenceCountBase());
          Partitioning_Object_Pointer->decrementReferenceCount();
          if (Partitioning_Object_Pointer->getReferenceCount() < Internal_Partitioning_Type::getReferenceCountBase())
               delete Partitioning_Object_Pointer;
          Partitioning_Object_Pointer = NULL;
        }

#if defined(USE_PADRE)
  // What PADRE function do we call here?
  // printf ("NEED TO CALL PADRE \n"); APP_ABORT();
  // printf ("####################################################### \n");
  // printf ("Don't we need to build a new PADRE distribution object? \n");
  // printf ("####################################################### \n");
  // printf ("Set parallelPADRE_DescriptorPointer = NULL in Array_Domain::partition() \n");
     if (parallelPADRE_DescriptorPointer != NULL)
        {
#if 0
          printf ("parallelPADRE_DescriptorPointer->getReferenceCount() = %d \n",
               parallelPADRE_DescriptorPointer->getReferenceCount());
          if (parallelPADRE_DescriptorPointer->getReferenceCount() == 
              parallelPADRE_DescriptorPointer->getReferenceCountBase())
               printf ("##### parallelPADRE_DescriptorPointer WILL be deleted! \n");
            else
               printf ("##### parallelPADRE_DescriptorPointer will NOT be deleted! \n");
#endif

       // printf ("In Array_Domain::partition() -- delete parallelPADRE_DescriptorPointer -- commented out! \n");
          parallelPADRE_DescriptorPointer->decrementReferenceCount();
          if (parallelPADRE_DescriptorPointer->getReferenceCount() < 
              parallelPADRE_DescriptorPointer->getReferenceCountBase())
             {
            // printf ("calling delete parallelPADRE_DescriptorPointer \n");
               delete parallelPADRE_DescriptorPointer;
             }
          parallelPADRE_DescriptorPointer = NULL;
        }

  // Now reinitialize the PADRE object pointer with a new PADRE_Descriptor
  // This will use the new PADRE_Distribution object which we provide explicitly
     APP_ASSERT (Internal_Partition.distributionPointer != NULL);
  // parallelPADRE_DescriptorPointer =
  //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> 
  //         ( this , Internal_Partition.distributionPointer );
     APP_ASSERT (parallelPADRE_DescriptorPointer == NULL);

  // printf ("Default distribution reference count = %d \n",
  //      PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
  //           getDefaultDistributionReferenceCount());
#else
  // Now delete the Parti specific data structures related to the old partitioning
     if (BlockPartiArrayDecomposition != NULL)
        {
       // printf ("In Array_Domain_Type::partition (Internal_Partitioning_Type) --- Calling delete_DECOMP \n");
          delete_DECOMP (BlockPartiArrayDecomposition);
          BlockPartiArrayDecomposition = NULL;
        }
     APP_ASSERT (BlockPartiArrayDecomposition == NULL);

     if (BlockPartiArrayDomain != NULL)
        {
       // printf ("In Array_Domain_Type::partition (Internal_Partitioning_Type) --- Calling delete_DARRAY \n");
          delete_DARRAY (BlockPartiArrayDomain);
          BlockPartiArrayDomain = NULL;
        }
     APP_ASSERT (BlockPartiArrayDomain == NULL);
  // End of not USE_PADRE
#endif

  // This function works with the doubleArray::partition member function
     Partitioning_Object_Pointer = &(Internal_Partitioning_Type &) Internal_Partition;
     Partitioning_Object_Pointer->incrementReferenceCount();

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Setting the ghost boundary width! numberOfDimensions() = %d \n",numberOfDimensions());
#endif

  // Array objects keep there own ghost boundaries (though this might not
  // work when using PADRE -- we might need another PADRE_Descriptor object)
  // printf ("Are the new ghost boundaries passed to PADRE? \n");

  // bugfix (12/19/2000) If the ghost boundaries are nonzero then they 
  // are limited to only non unit size axis.
  // for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
     int localNumberOfDimensions = numberOfDimensions();
     int temp = 0;
     for (temp=0; temp < localNumberOfDimensions; temp++)
        {
          InternalGhostCellWidth [temp] = Internal_Partition.getLocalGhostCellWidth (temp);
        }
     for (temp=localNumberOfDimensions; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          InternalGhostCellWidth [temp] = 0;
        }

  // Internal_Partition.display("Call display in Array_Domain::partition()");

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("DONE: Setting the ghost boundary width! \n");
#endif

#if COMPILE_DEBUG_STATEMENTS
  // We can't test this here since the Internal_Partitioning_Type's list of array objects 
  // has not been updated yet.  So the test is done in the doubleArray::partition member function.
  // Test_Consistency ("Called from Array_Domain_Type::partition(Internal_Partitioning_Type)");

  // A test that we can do is:
     APP_ASSERT (Partitioning_Object_Pointer != NULL);
     Test_Distribution_Consistency ("Call Test_Distribution_Consistency in Array_Domain::partition()");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Leaving Array_Domain_Type::partition \n");
#endif
   }
  // ... end of PPP ...
#endif

// ... only PPP  in i n c l u d e  ...
#if defined(PPP) 
#if defined(USE_PADRE)
  // What PADRE function do we call here?
#else
DARRAY* Array_Domain_Type::
Build_BlockPartiArrayDomain ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::Build_BlockPartiArrayDomain \n");
#endif

     DARRAY* Temp_BlockPartiArrayDomain = NULL;
  // APP_ASSERT (Partitioning_Object_Pointer != NULL);

     if (BlockPartiArrayDecomposition == NULL)
        {
          printf ("In Array_Domain_Type::Build_BlockPartiArrayDomain -- no BlockPartiArrayDecomposition available! \n");
          APP_ABORT();
        }

     APP_ASSERT (BlockPartiArrayDecomposition != NULL);

     APP_ASSERT (Is_A_View == FALSE);
     int temp;
     for ( temp=0; temp<MAX_ARRAY_DIMENSION; temp++ ) APP_ASSERT ( Stride[temp] == 1 );

     int Array_Sizes[MAX_ARRAY_DIMENSION];
     for ( temp=0; temp<MAX_ARRAY_DIMENSION; temp++ )
       Array_Sizes[temp] = getLength(temp);

     if (Partitioning_Object_Pointer != NULL)
        {
          Temp_BlockPartiArrayDomain = 
               Partitioning_Object_Pointer->Build_BlockPartiArrayDomain (
                    BlockPartiArrayDecomposition , Array_Sizes , 
                    InternalGhostCellWidth , ExternalGhostCellWidth );
        }
       else
        {
          Temp_BlockPartiArrayDomain = 
               Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain (
                    BlockPartiArrayDecomposition , Array_Sizes , 
                    InternalGhostCellWidth , ExternalGhostCellWidth );
        }

     APP_ASSERT (Temp_BlockPartiArrayDomain != NULL);
     return Temp_BlockPartiArrayDomain;
   }
#endif // End of if not USE_PADRE
#endif // end of PPP

// **********************************************************************************
//                           COPY CONSTRUCTOR
// **********************************************************************************

Array_Domain_Type::Array_Domain_Type(
     const Array_Domain_Type & X ,
     bool AvoidBuildingIndirectAddressingView,
     int Type_Of_Copy )
   {
  // I don't know if it is important to know what type of copy we want this far into the hierarchy.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::Array_Domain_Type (const Array_Domain_Type & X, bool,int) \n");
#endif

     int i = 0;

     switch (Type_Of_Copy)
        {
          case SHALLOWCOPY:
            // Shallow copies are the same as deep copies since there are no pointers
            // break;

          case DEEPCOLAPSEDCOPY:
          case DEEPCOPY:

            // The use of referenceCount in PADRE objects (built in 
            // Initialize_Parallel_Parts_Of_Domain()) forces the referenceCount variable to
            // be initialized here (before calling Initialize_Parallel_Parts_Of_Domain()).
               referenceCount     = getReferenceCountBase();

            // P++ specific code is place here!
#if defined(PPP)
            // Initialize PADRE or Block PARTI pointers to avoid UMR in purify
#if defined(USE_PADRE)
               parallelPADRE_DescriptorPointer = NULL;
#else
               BlockPartiArrayDomain           = NULL;
               BlockPartiArrayDecomposition    = NULL;
#endif

            // Initialize Partitioning Object pointer to avoid UMR in purify
               Partitioning_Object_Pointer = NULL;

            // Copy the parallel specific parts of X to this object!
               Initialize_Parallel_Parts_Of_Domain (X);

            // Should the copy constructor set this to NULL.  As a result of this
            // the Consistancy test at the end of this function had to be disabled.
            // I think this should be NULL since a Copy constructor in P++ should end
            // a statement.  Imagine passing an expresion to a copy constructor.
            // However one could argue that this should be handled in the array objects
            // copy constructor not the descriptor copy constructor.  For now we will leave 
            // it this way and add this question to the internal design list.
               Array_Conformability_Info = NULL;
            // Array_Conformability_Info = X.Array_Conformability_Info;
#endif

               for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                  {
#if defined(PPP)
                    Local_Mask_Index       [i] = X.Local_Mask_Index       [i];
                    Global_Index           [i] = X.Global_Index           [i];
                    Left_Number_Of_Points  [i] = X.Left_Number_Of_Points  [i];
                    Right_Number_Of_Points [i] = X.Right_Number_Of_Points [i];
                    InternalGhostCellWidth [i] = X.InternalGhostCellWidth [i];
                    ExternalGhostCellWidth [i] = X.ExternalGhostCellWidth [i];
#endif
                    Size          [i] = X.Size         [i];
                    Base          [i] = X.Base         [i];
                    Bound         [i] = X.Bound        [i];
                    Stride        [i] = X.Stride       [i];
                    Data_Base     [i] = X.Data_Base    [i];
                    User_Base     [i] = X.User_Base    [i];

                 // printf ("Setup in Array_Domain_Type COPY CONSTRUCTOR (DEEPCOPY): User_Base [%d] = %d \n",i,User_Base[i]);

                    Scalar_Offset [i] = X.Scalar_Offset[i];

                 // printf ("Setup in Array_Domain_Type COPY CONSTRUCTOR (DEEPCOPY): Scalar_Offset[%d] = %d \n",i,Scalar_Offset[i]);
                    Index_Array   [i] = X.Index_Array  [i];
                    if (Index_Array [i] != NULL)
                         Index_Array [i]->incrementReferenceCount();

#if defined(PPP)
                 // Local_Index_Array   [i] = X.Local_Index_Array  [i];
                    Local_Index_Array   [i] = NULL;
                    if (Local_Index_Array [i] != NULL)
                         Local_Index_Array [i]->incrementReferenceCount();
#endif

                    IndexBase        [i] = X.IndexBase        [i];
                    IndexStride      [i] = X.IndexStride      [i];
                    IndexDataPointer [i] = X.IndexDataPointer [i];
                  }

               Domain_Dimension = X.Domain_Dimension;
               Constant_Data_Base   = X.Constant_Data_Base;
               Constant_Unit_Stride = X.Constant_Unit_Stride;

            // Controls use of non FORTRAN 90/HPF Indexing
            // Use_Fortran_Indexing = TRUE;

               Is_A_View          = X.Is_A_View; 
               Is_A_Temporary     = X.Is_A_Temporary; 
               Is_A_Null_Array    = X.Is_A_Null_Array;
               Is_Contiguous_Data = X.Is_Contiguous_Data;

            // Was this descriptor used to build an array object using the adopt function?
            // This is usually FALSE.
               builtUsingExistingData = FALSE;

            // Array_ID           = Global_Array_ID++;
            // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
            // Array_ID           = Stack_Of_Array_IDs.Pop();
            // printf ("Pop_Array_ID called from COPY CONSTRUCTOR! \n");
               internalArrayID           = Pop_Array_ID();
     
            // printf ("In Array_Domain COPY constructor (X input): X.internalArrayID = %d \n",X.internalArrayID);
            // printf ("In Array_Domain COPY constructor: internalArrayID = %d \n",internalArrayID);

#if !defined(PPP)
            // Since this is a new array object is should have an initialize reference count on its
            // raw data.  This is required here because the reference counting mechanism reused the
            // value of zero for one existing reference and no references (this will be fixed soon).
               Array_Domain_Type::resetRawDataReferenceCount();
#endif

               ExpressionTemplateOffset = X.ExpressionTemplateOffset;

               View_Offset        = X.View_Offset;

               Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;

            // used in internal diagnostics
	       typeCode = X.typeCode;

               Uses_Indirect_Addressing = X.Uses_Indirect_Addressing;
               break;

          case DEEPALIGNEDCOPY:
            // Call the operator= since we want to avoid any colapse of the data an rerepresentation through
            // a modified descriptor

            // Need to initialize a few things for the operator= to work properly!
               Is_A_Null_Array = X.Is_A_Null_Array;
#if defined(PPP)
               Partitioning_Object_Pointer = NULL;
#if defined(USE_PADRE)
               parallelPADRE_DescriptorPointer = NULL;
#else
	       BlockPartiArrayDecomposition    = NULL;
               BlockPartiArrayDomain           = NULL;
#endif
#endif

            // Now just call the operator= to do the rest (this will NOT cause the copy of the array to be colapsed)
               (*this) = X;

            // X.display("X in COPY CONSTRUCTOR");

            // Test using the bool operator==
               APP_ASSERT ( (*this) == X );
               break;

          case DEEPCOLAPSEDALIGNEDCOPY:
               printf ("DEEPCOLAPSEDALIGNEDCOPY not supported yet! \n");
               APP_ABORT();
               break;

          default:
               printf ("ERROR: default reached in COPY Constructor (Type_Of_Copy = %d) \n",Type_Of_Copy);
               APP_ABORT();
               break;
        }


#if 0
            // The use of referenceCount in PADRE objects (built in 
            // Initialize_Parallel_Parts_Of_Domain()) forces the referenceCount variable to
            // be initialized here (before calling Initialize_Parallel_Parts_Of_Domain()).
               referenceCount     = getReferenceCountBase();

            // P++ specific code is place here!
#if defined(PPP)
            // Initialize PADRE or Block PARTI pointers to avoid UMR in purify
#if defined(USE_PADRE)
               parallelPADRE_DescriptorPointer = NULL;
#else
               BlockPartiArrayDomain           = NULL;
               BlockPartiArrayDecomposition    = NULL;
#endif

            // Initialize Partitioning Object pointer to avoid UMR in purify
               Partitioning_Object_Pointer = NULL;

            // Copy the parallel specific parts of X to this object!
               Initialize_Parallel_Parts_Of_Domain (X);

            // Should the copy constructor set this to NULL.  As a result of this
            // the Consistancy test at the end of this function had to be disabled.
            // I think this should be NULL since a Copy constructor in P++ should end
            // a statement.  Imagin passing an expresion to a copy constructor.
            // However one could argue that this should be handled in the array objects
            // copy constructor not the descriptor copy constructor.  For now we will leave 
            // it this way and add this question to the internal design list.
               Array_Conformability_Info = NULL;
            // Array_Conformability_Info = X.Array_Conformability_Info;
#endif

               for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                  {
#if defined(PPP)
                    Local_Mask_Index       [i] = X.Local_Mask_Index       [i];
                    Global_Index           [i] = X.Global_Index           [i];
                    Left_Number_Of_Points  [i] = X.Left_Number_Of_Points  [i];
                    Right_Number_Of_Points [i] = X.Right_Number_Of_Points [i];
                    InternalGhostCellWidth [i] = X.InternalGhostCellWidth [i];
                    ExternalGhostCellWidth [i] = X.ExternalGhostCellWidth [i];
#endif
                    Size          [i] = X.Size         [i];
                    Base          [i] = X.Base         [i];
                    Bound         [i] = X.Bound        [i];
                    Stride        [i] = X.Stride       [i];
                    Data_Base     [i] = X.Data_Base    [i]; 
                    User_Base     [i] = X.User_Base    [i]; 
                    Scalar_Offset [i] = X.Scalar_Offset[i];
                    Index_Array   [i] = X.Index_Array  [i];
                    if (Index_Array [i])
                         Index_Array [i]->incrementReferenceCount();

                    IndexBase        [i] = X.IndexBase        [i];
                    IndexStride      [i] = X.IndexStride      [i];
                    IndexDataPointer [i] = X.IndexDataPointer [i];
                  }

               Domain_Dimension = X.Domain_Dimension;
               Constant_Data_Base   = X.Constant_Data_Base;
               Constant_Unit_Stride = X.Constant_Unit_Stride;

            // Controls use of non FORTRAN 90/HPF Indexing
            // Use_Fortran_Indexing = TRUE;

               Is_A_View          = X.Is_A_View; 
               Is_A_Temporary     = X.Is_A_Temporary; 
               Is_A_Null_Array    = X.Is_A_Null_Array;
               Is_Contiguous_Data = X.Is_Contiguous_Data;

            // Was this descriptor used to build an array object using the adopt function?
            // This is usually FALSE.
               builtUsingExistingData = FALSE;

            // Array_ID           = Global_Array_ID++;
            // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
            // Array_ID           = Stack_Of_Array_IDs.Pop();
            // printf ("Pop_Array_ID called from COPY CONSTRUCTOR! \n");
               internalArrayID           = Pop_Array_ID();
     
            // printf ("In Array_Domain COPY constructor (X input): X.internalArrayID = %d \n",X.internalArrayID);
            // printf ("In Array_Domain COPY constructor: internalArrayID = %d \n",internalArrayID);

#if !defined(PPP)
            // Since this is a new array object is should have an initialize reference count on its
            // raw data.  This is required here because the reference counting mechanism reused the
            // value of zero for one existing reference and no references (this will be fixed soon).
               Array_Domain_Type::resetRawDataReferenceCount();
#endif

               ExpressionTemplateOffset = X.ExpressionTemplateOffset;

               View_Offset        = X.View_Offset;

               Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;

               Uses_Indirect_Addressing = X.Uses_Indirect_Addressing;
#endif

  // Often we want the copy to be reduced in the since that a copy of a 
  // view using indirect addressing is contiguous. This is the case for temporaries.
     if (AvoidBuildingIndirectAddressingView)
        {
       // Set the base values of a temporary to the value APP_Global_Array_Base
          int Difference = 0;
          int temp = 0;
          // ... (8/7/98,kdb) use Data_Base of X instead of
	  //  APP_Global_Array_Base ...
	  //int temp_base = APP_Global_Array_Base;
	  int temp_base = X.Data_Base[0];

	  if (Uses_Indirect_Addressing)
             for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
		if (Index_Array [temp])
		   temp_base = Index_Array[temp]->getBase(0);

          Difference = temp_base - Data_Base [0];
          Data_Base [0] += Difference;
          User_Base [0] += Difference;

	  /*
          // ... (8/7/98,kdb) turn off code that resets base of 
	  //  temporary to APP_Global_Array_Base.  Might be able
	  //  to clean out a bunch of code here ...
	  */
	  /*
          for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               Difference = APP_Global_Array_Base - Data_Base [temp];
               Data_Base [temp] += Difference;
               User_Base [temp] += Difference;
	    // ... (10/31/96,kdb) also need to reset Global and Local_Mask_Index
	   //  to be consistent with this ...
             }
	  */

       // Mark the array as a temporary (very important step)
          Is_A_Temporary = TRUE;

       // Fix Scalar_Offset since Data_Base may be changing
       // ... (11/20/98; kdb) View_Offset should be added; usually 0 here
       //  so hard to detect ...
       // Scalar_Offset[0] = -User_Base[0] * Stride[0];
          Scalar_Offset[0] = -User_Base[0] * Stride[0] + View_Offset;
          for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Stride[temp] * Size[temp-1];
             }
  
       // BUG FIX (10/26/95, kdb): If intArray isn't in dimension 0
       // first dimension of X could be a scalar and have length 1
       // temporary should be changed so first dimension is the same
       // size as the intArray
          if (Uses_Indirect_Addressing)
             {
            // Base [0] = APP_Global_Array_Base;
               Base [0] = 0;
	       Stride[0] = 1;

               for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
                  {
                    if (Index_Array [temp]) 
		    {
                         Bound[0] = Index_Array[temp]->elementCount()-1;
                         Size[0] = Bound[0]+1; 
		    }
                  }
               for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
                  Size[temp] = Size[0]; 
             }

       // Since this temporary will be built to be the size of the intArray used to
       // do the indirect addressing it will not be indirectly addressed.
          if (Uses_Indirect_Addressing)
             {
               Uses_Indirect_Addressing = FALSE;
               for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
                  {
                    if (Index_Array [temp]) 
                       {
                      // Added conventional mechanism for reference counting control
                      // operator delete no longer decriments the referenceCount.
                         Index_Array [temp]->decrementReferenceCount();
                         if (Index_Array [temp]->getReferenceCount() < intArray::getReferenceCountBase())
                              delete Index_Array [temp];
                       }
                    Index_Array [temp] = NULL;
                  }

            // If descriptor was a view using mixed intArray and scalar indexing then
            // we have to set the scalar indexed dimensions to be consistant with a 1D array
            // so we set all but the first dimension to be 0 for base and bound and set the 
            // stride to 1.  Thus the resulting temporary is a 1D array (with no indirect addressing).
               if (Size[0] == 0)
	       {
		  Is_A_Null_Array = TRUE;
                  for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
		  {
		     Base   [temp] =  0;
		     Bound  [temp] = -1;
		     Stride [temp] =  1;
		  }
	       }
	       else
	       {
                  for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
                  {
                 // Base   [temp] = APP_Global_Array_Base;
                 // Bound  [temp] = APP_Global_Array_Base;

                 // It seems that this is not required since the non expression 
		 // template version does not see these if the indirect addressing 
		 // is in use.
                 // printf ("Inside of Array_Domain_Type COPY constructor need to fixup the Bases (or Data_Base) \n");
                 // printf ("     to be consistant with the expression template implementation \n");
                    Base   [temp] = 0;
                    Bound  [temp] = 0;
                    Stride [temp] = 1;
                  }
	       }

            // APP_ASSERT (Temporary_Domain->Is_Contiguous_Data == TRUE);
            // Bugfix (1/17/96) In the case of a temporary built from a view obtained
            // via indirect addressing we can't say that the data is contiguous.
            // But as I recall there was a better fix than this (Kristi suggested this fix).
            // I need to understand this problem better.
            // Is_Contiguous_Data = TRUE;
               Is_Contiguous_Data = FALSE;
             }

          // Base of block for "if (AvoidBuildingIndirectAddressingView)"
        }

#if defined(PPP)
  // A temporary must carry along the information accumulated in the 
  // evaluation of the P++ array statement so it can be modified by each
  // operation (if required) and used to interpret the message passing
  // when the operator= is executed.
     if (X.Array_Conformability_Info != NULL)
        {
          Array_Conformability_Info = X.Array_Conformability_Info;
          Array_Conformability_Info->incrementReferenceCount();
        }
#endif

#if !defined(PPP)
  // printf ("Inside of copy constructor descriptor() Array_ID = %d \n",Array_ID);
  // The reference coutning for the raw data is stored in the SerialArrayDomain object.
     if (internalArrayID >= Array_Reference_Count_Array_Length)
          Allocate_Larger_Reference_Count_Array ( internalArrayID );
#endif

#if COMPILE_DEBUG_STATEMENTS
  // We can't test the consistancy here because if an a temporary is input we will
  // have a temporary with a Array_Conformability_Info == NULL which would be detected as an error
  // Question: should the copy constructor always set Is_A_Temporary to FALSE?
  // OR: should the copy constructor always set Array_Conformability_Info = X.Array_Conformability_Info?
#if !defined(PPP)
     Test_Consistency ("Called from Array_Domain_Type COPY CONSTRUCTOR #1");
#endif
#endif
   }



// **********************************************************************************
//                                bool OPERATOR == 
// **********************************************************************************

void
Array_Domain_Type::equalityTest ( bool & returnValue, bool test, const char* description ) const
   {
  // This is a support function for the operator==
     returnValue = returnValue && test;
     if (returnValue == FALSE)
        {
          printf ("Test %s is %s, input returnValue = FALSE \n",description, (test == TRUE) ? "TRUE" : "FALSE");
       // display("FAILED EQUALITY TEST");
          APP_ABORT();
        }
   }

bool
Array_Domain_Type::operator== ( const Array_Domain_Type & X ) const
   {
  // This is the bool equals test operator.  Note that the semantics of this operator
  // is very specific (array_ID's and referenceCounts need not be equal for example).  The output of the copy
  // constructor will only pass the equals test if the DEEPALIGNEDCOPY option is used.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::operator== (const Array_Domain_Type & X) \n");
#endif

     bool returnValue = TRUE;

     int i = 0;

  // We permit future tests to have different semantics which will be specific as input
     int typeOfTest = DEEPALIGNEDCOPY;

     switch (typeOfTest)
        {
          case DEEPALIGNEDCOPY:

            // P++ specific code is place here!
#if defined(PPP)
            // Initialize PADRE or Block PARTI pointers to avoid UMR in purify
#if defined(USE_PADRE)
#if 0
               equalityTest (returnValue,
                    parallelPADRE_DescriptorPointer == X.parallelPADRE_DescriptorPointer,
                   "parallelPADRE_DescriptorPointer");
#endif
#else
               equalityTest (returnValue,
                    BlockPartiArrayDomain           == X.BlockPartiArrayDomain,
                   "BlockPartiArrayDomain");
               equalityTest (returnValue,
                    BlockPartiArrayDecomposition    == X.BlockPartiArrayDecomposition,
                   "BlockPartiArrayDecomposition");
#endif

               equalityTest (returnValue,
                    Partitioning_Object_Pointer == X.Partitioning_Object_Pointer,
                   "Partitioning_Object_Pointer");

            // equalityTest (returnValue, Array_Conformability_Info == X.Array_Conformability_Info, "Array_Conformability_Info");
#endif

            // printf ("View_Offset = %d X.View_Offset = %d \n",View_Offset,X.View_Offset);

               equalityTest (returnValue, View_Offset == X.View_Offset, "View_Offset");

               for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                  {
                    equalityTest (returnValue, Size          [i] == X.Size         [i], "Size");

                 // printf ("Base [i] = %d X.Base [i] = %d \n",Base [i],X.Base [i]);

                    equalityTest (returnValue, Base          [i] == X.Base         [i], "Base");

                 // printf ("Bound [i] = %d X.Bound [i] = %d \n",Bound [i],X.Bound [i]);

                    equalityTest (returnValue, Bound         [i] == X.Bound        [i], "Bound");
                    equalityTest (returnValue, Stride        [i] == X.Stride       [i], "Stride");
                    equalityTest (returnValue, Data_Base     [i] == X.Data_Base    [i], "Data_Base"); 

                 // printf ("User_Base [i] = %d X.User_Base [i] = %d \n",User_Base[i],X.User_Base[i]);

                    equalityTest (returnValue, User_Base     [i] == X.User_Base    [i], "User_Base");

                 // printf ("Scalar_Offset [i] = %d X.Scalar_Offset [i] = %d \n",Scalar_Offset[i],X.Scalar_Offset[i]);

                 // equalityTest (returnValue, Scalar_Offset [i] == X.Scalar_Offset[i], "Scalar_Offset");

                    equalityTest (returnValue, Index_Array      [i] == X.Index_Array      [i], "Index_Array");

                    equalityTest (returnValue, IndexBase        [i] == X.IndexBase        [i], "IndexBase");
                    equalityTest (returnValue, IndexStride      [i] == X.IndexStride      [i], "IndexStride");
                    equalityTest (returnValue, IndexDataPointer [i] == X.IndexDataPointer [i], "IndexDataPointer");

#if defined(PPP)
#if 0
                    Local_Mask_Index  [i].display("Local_Mask_Index");
                    X.Local_Mask_Index[i].display("X.Local_Mask_Index");
                    Global_Index  [i].display("Global_Index");
#endif
                    equalityTest (returnValue,
                         Local_Mask_Index       [i] == X.Local_Mask_Index       [i],
                        "Local_Mask_Index");
                    equalityTest (returnValue,
                         Global_Index           [i] == X.Global_Index           [i],
                        "Global_Index");
#if 0
                    printf ("Left_Number_Of_Points [i] = %d X.Left_Number_Of_Points [i] = %d \n",
                         Left_Number_Of_Points [i],X.Left_Number_Of_Points [i]);
#endif
                    equalityTest (returnValue,
                         Left_Number_Of_Points  [i] == X.Left_Number_Of_Points  [i],
                        "Left_Number_Of_Points");
#if 0
                    printf ("Right_Number_Of_Points [i] = %d X.Right_Number_Of_Points [i] = %d \n",
                         Right_Number_Of_Points [i],X.Right_Number_Of_Points [i]);
#endif
                    equalityTest (returnValue,
                         Right_Number_Of_Points [i] == X.Right_Number_Of_Points [i],
                        "Right_Number_Of_Points");

                    equalityTest (returnValue,
                         InternalGhostCellWidth [i] == X.InternalGhostCellWidth [i],
                        "InternalGhostCellWidth");
                    equalityTest (returnValue,
                         ExternalGhostCellWidth [i] == X.ExternalGhostCellWidth [i],
                        "ExternalGhostCellWidth");
#endif
                  }

               equalityTest (returnValue, Domain_Dimension     == X.Domain_Dimension,     "Domain_Dimension");

            // printf ("Constant_Data_Base   = %s \n",Constant_Data_Base ? "TRUE" : "FALSE");
            // printf ("X.Constant_Data_Base = %s \n",X.Constant_Data_Base ? "TRUE" : "FALSE");

               equalityTest (returnValue, Constant_Data_Base   == X.Constant_Data_Base,   "Constant_Data_Base");

            // printf ("Constant_Unit_Stride   = %s \n",Constant_Unit_Stride ? "TRUE" : "FALSE");
            // printf ("X.Constant_Unit_Stride = %s \n",X.Constant_Unit_Stride ? "TRUE" : "FALSE");

               equalityTest (returnValue, Constant_Unit_Stride == X.Constant_Unit_Stride, "Constant_Unit_Stride");

               equalityTest (returnValue, Is_A_View          == X.Is_A_View,          "Is_A_View"); 
               equalityTest (returnValue, Is_A_Temporary     == X.Is_A_Temporary,     "Is_A_Temporary"); 
               equalityTest (returnValue, Is_A_Null_Array    == X.Is_A_Null_Array,    "Is_A_Null_Array");
               equalityTest (returnValue, Is_Contiguous_Data == X.Is_Contiguous_Data, "Is_Contiguous_Data");

            // Was this descriptor used to build an array object using the adopt function?
            // This is usually FALSE.
               equalityTest (returnValue,
                    builtUsingExistingData == X.builtUsingExistingData, "builtUsingExistingData");

#if 0
            // This is not used but perhaps should be tested since it is set (and so we want it to be checked)
	       printf ("ExpressionTemplateOffset   = %p \n",ExpressionTemplateOffset);
	       printf ("X.ExpressionTemplateOffset = %p \n",X.ExpressionTemplateOffset);
               equalityTest (returnValue,
                    ExpressionTemplateOffset == X.ExpressionTemplateOffset, "ExpressionTemplateOffset");
#endif

               equalityTest (returnValue,
                    View_Offset        == X.View_Offset, "View_Offset");

               equalityTest (returnValue,
                    Is_Built_By_Defered_Evaluation == X.Is_Built_By_Defered_Evaluation,
                   "Is_Built_By_Defered_Evaluation");

            // used in internal diagnostics
	       equalityTest (returnValue, typeCode == X.typeCode, "typeCode");

               equalityTest (returnValue, 
                    Uses_Indirect_Addressing == X.Uses_Indirect_Addressing, "Uses_Indirect_Addressing");
               break;

          case SHALLOWCOPY:
            // Shallow copies are the same as deep copies since there are no pointers

          case DEEPCOLAPSEDCOPY:
          case DEEPCOPY:
          case DEEPCOLAPSEDALIGNEDCOPY:
               printf ("(DEEPCOLAPSEDALIGNEDCOPY && DEEPCOPY && DEEPCOLAPSEDCOPY && SHALLOWCOPY) not supported yet! in operator== (typeOfTest = %d) \n",typeOfTest);
               APP_ABORT();
               break;

          default:
               printf ("ERROR: default reached in operator== (typeOfTest = %d) \n",typeOfTest);
               APP_ABORT();
               break;
        }

     return returnValue;
   }


// ***********************************************************************************
//                        Support for Indirect Addressing
// ***********************************************************************************

#if (defined(SERIAL_APP) || defined(USE_PADRE)) && !defined(PPP)
// This constuctor is used to build a SerialArray_Domain_Type from an Array_Domain_Type
// It is here to support the use of PADRE.  This function is a copy of the COPY CONSTRUCTOR
SerialArray_Domain_Type::SerialArray_Domain_Type
   ( const Array_Domain_Type & X , 
     bool AvoidBuildingIndirectAddressingView,
     int Type_Of_Copy )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 2)
        {
          printf ("Inside of SerialArray_Domain_Type::SerialArray_Domain_Type (const Array_Domain_Type & X) \n");
       // Since the X's data is not setup yet it is a problem to call this function!
       // We need to consider a less complex interface relationship (however this is more a P++ issue than a PADRE issue).
       // X.display("Display Parallel Descriptor");
        } 
#endif

  // Avoid compiler warning about unused input variable
     if (&Type_Of_Copy);

     int i = 0;

  // The use of referenceCount in PADRE objects (built in 
  // Initialize_Parallel_Parts_Of_Domain()) forces the referenceCount variable to
  // be initialized here (before calling Initialize_Parallel_Parts_Of_Domain()).
     referenceCount     = getReferenceCountBase();

  // Remove this soon!
  // The typeCode records the element type associated with the array for which
  // we use this domain object.  This is used only for internal diagnostics.
  // The value 'u' makes the element type as undefined.  This mechanism might be
  // replaced by an alternative mechanism in the future.
  // setTypeCode(APP_UNDEFINED_ELEMENT_TYPE);
     typeCode = APP_UNDEFINED_ELEMENT_TYPE;

#if defined(PPP)
     Partitioning_Object_Pointer = NULL;
     Initialize_Parallel_Parts_Of_Domain (X);
  // APP_ASSERT(BlockPartiArrayDecomposition != NULL);
  // APP_ASSERT( BlockPartiArrayDomain   != NULL );

  // Should the copy constructor set this to NULL.  As a result of this
  // the Consistancy test at the end of this function had to be disabled.
  // I think this should be NULL since a Copy constructor in P++ should end
  // a statement.  Imagin passing an expresion to a copy constructor.
  // However one could argue that this should be handled in the array objects
  // copy constructor not the descriptor copy constructor.  For now we will leave 
  // it this way and add this question to the internal design list.
     Array_Conformability_Info = NULL;
  // Array_Conformability_Info = X.Array_Conformability_Info;
#endif

     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
#if defined(PPP)
          Local_Mask_Index       [i] = X.Local_Mask_Index       [i];
          Global_Index           [i] = X.Global_Index           [i];
          Left_Number_Of_Points  [i] = X.Left_Number_Of_Points  [i];
          Right_Number_Of_Points [i] = X.Right_Number_Of_Points [i];
          InternalGhostCellWidth [i] = X.InternalGhostCellWidth [i];
          ExternalGhostCellWidth [i] = X.ExternalGhostCellWidth [i];

          Local_Index_Array      [i] = NULL;
#endif

          Size          [i] = X.Size         [i];
          Base          [i] = X.Base         [i];
          Bound         [i] = X.Bound        [i];
          Stride        [i] = X.Stride       [i];
          Data_Base     [i] = X.Data_Base    [i]; 
          User_Base     [i] = X.User_Base    [i]; 
          Scalar_Offset [i] = X.Scalar_Offset[i];

       // printf ("Setup in (special) SerialArray_Domain_Type COPY CONSTRUCTOR: Scalar_Offset[%d] = %d \n",i,Scalar_Offset[i]);

#if 1
       // The types of intArrays in this object and the X object (input) are different types
       // so we can't assign pointers to them.
          Index_Array   [i] = NULL;
#else
          Index_Array   [i] = X.Index_Array  [i];
          if (Index_Array [i]) 
               Index_Array [i]->incrementReferenceCount();
#endif

// if defined(USE_EXPRESSION_TEMPLATES)
          IndexBase        [i] = X.IndexBase        [i];
          IndexStride      [i] = X.IndexStride      [i];
          IndexDataPointer [i] = X.IndexDataPointer [i];
// endif
        }

     Domain_Dimension = X.Domain_Dimension;

  // printf ("Domain_Dimension = %d  X.Domain_Dimension = %d \n",Domain_Dimension,X.Domain_Dimension);

     Constant_Data_Base   = X.Constant_Data_Base;
     Constant_Unit_Stride = X.Constant_Unit_Stride;

  // Controls use of non FORTRAN 90/HPF Indexing
  // Use_Fortran_Indexing = TRUE;

     Is_A_View          = X.Is_A_View; 
     Is_A_Temporary     = X.Is_A_Temporary; 
     Is_A_Null_Array    = X.Is_A_Null_Array;
     Is_Contiguous_Data = X.Is_Contiguous_Data;

  // used in internal diagnostics
     typeCode = X.typeCode;

  // Was this descriptor used to build an array object using the adopt function?
  // This is usually FALSE
     builtUsingExistingData = FALSE;

  // Array_ID           = Global_Array_ID++;
  // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
  // Array_ID           = Stack_Of_Array_IDs.Pop();
  // printf ("Pop_Array_ID called from COPY CONSTRUCTOR! \n");
     internalArrayID           = Pop_Array_ID();

// if defined(USE_EXPRESSION_TEMPLATES)
     ExpressionTemplateOffset = X.ExpressionTemplateOffset;
// endif

     View_Offset        = X.View_Offset;

     Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;

     Uses_Indirect_Addressing = X.Uses_Indirect_Addressing;

  // Often we want the copy to be reduced in the sense that a copy of a 
  // view using indirect addressing is contiguous. This is the case for temporaries.
     if (AvoidBuildingIndirectAddressingView)
        {
       // Set the base values of a temporary to the value APP_Global_Array_Base

          int Difference = 0;
          int temp = 0;
          for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               Difference = APP_Global_Array_Base - Data_Base [temp];
               Data_Base [temp] += Difference;

               printf ("Changing the User Base (should we?) \n");

               User_Base [temp] += Difference;

            // printf ("Setup in SerialArray_Domain_Type::SerialArray_Domain_Type (const Array_Domain_Type & X): User_Base [%d] = %d \n",temp,User_Base[temp]);

            // ... (10/31/96,kdb) also need to reset Global and Local_Mask_Index
	    // to be consistent with this ...
             }

       // Mark the array as a temporary (very important step)
          Is_A_Temporary = TRUE;

       // Fix Scalar_Offset since Data_Base may be changing
          Scalar_Offset[0] = -User_Base[0] * Stride[0];
          printf ("At end of (special) SerialArray_Domain_Type COPY CONSTRUCTOR: Scalar_Offset[%d] = %d \n",0,Scalar_Offset[0]);
          for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Stride[temp] * Size[temp-1];
               printf ("At end of (special) SerialArray_Domain_Type COPY CONSTRUCTOR: Scalar_Offset[%d] = %d \n",temp,Scalar_Offset[temp]);
             }
  
       // BUG FIX (10/26/95, kdb): If intArray isn't in dimension 0
       // first dimension of X could be a scalar and have length 1
       // temporary should be changed so first dimension is the same
       // size as the intArray
          if (Uses_Indirect_Addressing)
             {
            // Base [0] = APP_Global_Array_Base;
               Base [0] = 0;

               for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
                  {
                    if (Index_Array [temp]) 
		    {
                         Bound[0] = Index_Array[temp]->elementCount()-1; 
                         Size[0] = Bound[0]+1; 
		    }
                  }
               for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
                  Size[temp] = Size[0]; 
             }

       // Since this temporary will be built to be the size of the intArray used to
       // do the indirect addressing it will not be indirectly addressed.
          if (Uses_Indirect_Addressing)
             {
               Uses_Indirect_Addressing = FALSE;
               for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
                  {
                    if (Index_Array [temp]) 
                       {
                      // Added conventional mechanism for reference counting control
                      // operator delete no longer decriments the referenceCount.
                         Index_Array [temp]->decrementReferenceCount();
                         if (Index_Array [temp]->getReferenceCount() < intArray::getReferenceCountBase())
                              delete Index_Array [temp];
                       }
                    Index_Array [temp] = NULL;
                  }

            // If descriptor was a view using mixed intArray and scalar indexing then
            // we have to set the scalar indexed dimensions to be consistant with a 1D array
            // so we set all but the first dimension to be 0 for base and bound and set the 
            // stride to 1.  Thus the resulting temporary is a 1D array (with no indirect addressing).
               for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
                  {
                 // Base   [temp] = APP_Global_Array_Base;
                 // Bound  [temp] = APP_Global_Array_Base;

                 // It seems that this is not required since the non expression template version
                 // does not see these if the indirect addressing is in use.
                 // printf ("Inside of Array_Domain_Type COPY constructor need to fixup the Bases (or Data_Base) \n");
                 // printf ("     to be consistant with the expression template implementation \n");
                    Base   [temp] = 0;
                    Bound  [temp] = 0;
                    Stride [temp] = 1;
                  }

            // APP_ASSERT (Temporary_Domain->Is_Contiguous_Data == TRUE);
            // Bugfix (1/17/96) In the case of a temporary built from a view obtained
            // via indirect addressing we can't say that the data is contiguous.
            // But as I recall there was a better fix than this (Kristi suggested this fix).
            // I need to understand this problem better.
            // Is_Contiguous_Data = TRUE;
               Is_Contiguous_Data = FALSE;
             }
        }

#if defined(PPP)
       // A temporary must carry along the information accumulated in the 
       // evaluation of the P++ array statement so it can be modified by each
       // operation (if required) and used to interpret the message passing
       // when the operator= is executed.
          if (X.Array_Conformability_Info != NULL)
             {
               Array_Conformability_Info = X.Array_Conformability_Info;
               Array_Conformability_Info->incrementReferenceCount();
             }
#endif

#if !defined(PPP)
  // printf ("Inside of copy constructor descriptor() Array_ID = %d \n",Array_ID);
     if (internalArrayID >= Array_Reference_Count_Array_Length)
          Allocate_Larger_Reference_Count_Array ( internalArrayID );
#endif

#if COMPILE_DEBUG_STATEMENTS
  // We can't test the consistancy here because if an a temporary is input we will
  // have a temporary with a Array_Conformability_Info == NULL which would be detected as an error
  // Question: should the copy constructor always set Is_A_Temporary to FALSE?
  // OR: should the copy constructor always set Array_Conformability_Info = X.Array_Conformability_Info?
#if !defined(PPP)
     Test_Consistency ("Called from Array_Domain_Type COPY CONSTRUCTOR #2");
#endif
#endif
   }

void Array_Domain_Type::normalize()
   {
  // This function translates the base to zero.  It is used as part of the 
  // PADRE where the globalDomain object is stored in the PADRE_Representation
  // object in a translation invariant form (meaning base zero and assumed independent of base).
     setBase(0);
   }
  // if defined(SERIAL_APP)
#endif


// **********************************************************************************
//                           EQUALS OPERATOR
// **********************************************************************************
Array_Domain_Type &
Array_Domain_Type::operator=( const Array_Domain_Type & X )
   {
  // This function is called by the Descriptor operator= and breaks the association of
  // the partitioning to the one in X.

  // This function is also called by the copy constructor when the typeOfCopy == DEEPALIGNEDCOPY

  // At present this function is only called in the optimization of assignment and temporary use
  // in lazy_statement.C!  
  // NOT TRUE! The Expression template option force frequent calls to this function!
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::operator= (const Array_Domain_Type & X) \n");
#endif

     int i = 0;

  // The use of referenceCount in PADRE objects (built in 
  // Initialize_Parallel_Parts_Of_Domain()) forces the referenceCount variable to
  // be initialized here (before calling Initialize_Parallel_Parts_Of_Domain()).
     referenceCount     = getReferenceCountBase();

  // printf ("In Array_Domain_Type::operator=(): Is_A_Null_Array = %s \n",(Is_A_Null_Array) ? "TRUE" : "FALSE");

#if defined(PPP)
  // ... (9/1/97,kdb) this is a memory leak if these aren't null
  // but only check when not a null array because these might be
  // set to non null uninitialized garbage so test will be incorrect ...
     if (!Is_A_Null_Array)
        {
#if defined(USE_PADRE)
       // What function of PADRE should we use?
          if (parallelPADRE_DescriptorPointer != NULL)
             {
            // PADRE requires that the reference counts be 
            // checked before calling the delete operator
               parallelPADRE_DescriptorPointer->decrementReferenceCount();
               if (parallelPADRE_DescriptorPointer->getReferenceCount() > 
                   parallelPADRE_DescriptorPointer->getReferenceCountBase()) 
                   delete parallelPADRE_DescriptorPointer;
               parallelPADRE_DescriptorPointer = NULL;
             }
#else
	  if (BlockPartiArrayDecomposition != NULL) 
             {
            // printf ("In Array_Domain_Type::operator= (Array_Domain_Type) --- Calling delete_DECOMP \n");
	       delete_DECOMP(BlockPartiArrayDecomposition); 
               BlockPartiArrayDecomposition = NULL;
             }
            else
             {
            // printf ("BlockPartiArrayDecomposition == NULL \n");
             }
	  if (BlockPartiArrayDomain != NULL)
             {
            // printf ("In Array_Domain_Type::operator= (Array_Domain_Type) --- Calling delete_DARRAY \n");
	       delete_DARRAY(BlockPartiArrayDomain); 
               BlockPartiArrayDomain = NULL;
             }
            else
             {
            // printf ("BlockPartiArrayDomain == NULL \n");
             }
#endif
        }

  // We don't want to set this to NULL since it has already been initialized!
  // Partitioning_Object_Pointer = NULL;
     Initialize_Parallel_Parts_Of_Domain (X);

#if defined(USE_PADRE)
  // What function of PADRE should we use?
  // printf ("NEED TO CALL PADRE \n"); APP_ABORT();
#else
     APP_ASSERT( BlockPartiArrayDecomposition != NULL );
     APP_ASSERT( BlockPartiArrayDomain != NULL );
#endif

     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Local_Mask_Index       [i] = X.Local_Mask_Index       [i];
          Global_Index           [i] = X.Global_Index           [i];
          Left_Number_Of_Points  [i] = X.Left_Number_Of_Points  [i];
          Right_Number_Of_Points [i] = X.Right_Number_Of_Points [i];
          InternalGhostCellWidth [i] = X.InternalGhostCellWidth [i];
          ExternalGhostCellWidth [i] = X.ExternalGhostCellWidth [i];

       // Local_Index_Array      [i] = X.Local_Index_Array      [i];
          Local_Index_Array      [i] = NULL;
          if (Local_Index_Array [i] != NULL)
               Local_Index_Array [i]->incrementReferenceCount();
        }
#endif

     Domain_Dimension     = X.Domain_Dimension;
     Constant_Data_Base   = X.Constant_Data_Base;
     Constant_Unit_Stride = X.Constant_Unit_Stride;

  // Controls use of non FORTRAN 90/HPF Indexing
  // Use_Fortran_Indexing = TRUE;

     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Size          [i] = X.Size          [i];
          Base          [i] = X.Base          [i];
          Bound         [i] = X.Bound         [i];
          Stride        [i] = X.Stride        [i];
          Data_Base     [i] = X.Data_Base     [i];
          User_Base     [i] = X.User_Base     [i];

       // printf ("Setup in Array_Domain_Type::operator=: User_Base [%d] = %d \n",i,User_Base[i]);

          Scalar_Offset [i] = X.Scalar_Offset [i];

       // printf ("Setup in Array_Domain_Type::operator=: Scalar_Offset[%d] = %d \n",i,Scalar_Offset[i]);

          Index_Array   [i] = X.Index_Array   [i];
          if (Index_Array [i] != NULL) 
               Index_Array [i]->incrementReferenceCount();

// if defined(USE_EXPRESSION_TEMPLATES)
          IndexBase        [i] = X.IndexBase        [i];
          IndexStride      [i] = X.IndexStride      [i];
          IndexDataPointer [i] = X.IndexDataPointer [i];
// endif
        }

     Is_A_View          = X.Is_A_View; 
     Is_A_Temporary     = X.Is_A_Temporary; 
     Is_A_Null_Array    = X.Is_A_Null_Array;
     Is_Contiguous_Data = X.Is_Contiguous_Data;

  // used in internal diagnostics
     typeCode = X.typeCode;

// if defined(USE_EXPRESSION_TEMPLATES)
     ExpressionTemplateOffset = X.ExpressionTemplateOffset;
// endif
     View_Offset        = X.View_Offset;

  // printf ("In Array_Domain_Type::operator=: Scalar_Offset not reinitialized! \n");

  // Array_ID           = Global_Array_ID++;
  // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
  // Array_ID           = Stack_Of_Array_IDs.Pop();

  // Because of the copying of the descriptors in the expression template 
  // implementation we copy the Array_ID as well!
  // Array_ID           = Pop_Array_ID();
  // ... note: this causes problems with the deep copy in the copy
  //  constructor but is fixed for now by saving old Array_ID ...
     internalArrayID           = X.internalArrayID;

  // Was this descriptor used to build an array object using the adopt function?
  // This is usually FALSE.
     builtUsingExistingData = FALSE;

  // Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;
     Is_Built_By_Defered_Evaluation = FALSE;

     Uses_Indirect_Addressing = X.Uses_Indirect_Addressing;

#if !defined(PPP)
  // printf ("Inside of Array_Domain_Type::operator=() internalArrayID = %d \n",internalArrayID);
     if (internalArrayID >= Array_Reference_Count_Array_Length)
          Allocate_Larger_Reference_Count_Array ( internalArrayID );
#endif

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from Array_Domain_Type::operator=");
#endif

     return *this;
   }

// Should be inlined since it is used in a lot of places within A++!
int
Array_Domain_Type::Array_Size () const
   {
  // This function computes the number of elements in the array object!
     int i = 0;   // indexing variable
     int Number_Of_Elements = 1;

  // The case of Indirect_Addressing does not seem to be handled properly
  // so for the moment we will make sure that it is FALSE.
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);

     if (Uses_Indirect_Addressing)
        {
          do {
               if (Index_Array[i])
                    Number_Of_Elements = Index_Array[i]->elementCount();
               i++;
             }
          while ( (Number_Of_Elements <= 1) && (i < MAX_ARRAY_DIMENSION) );
        }
       else
        {
          if (Is_A_Null_Array)
             {
            // A Null Array has ZERO elements!
               Number_Of_Elements = 0;
             }
            else
             {
            // Otherwise the Number of elements is a product of the size of each dimension
            // Number_Of_Elements = (((Bound[0]-Base[0])+1)/Stride[0]) * (((Bound[1]-Base[1])+1)/Stride[1]) * 
            //                      (((Bound[2]-Base[2])+1)/Stride[2]) * (((Bound[3]-Base[3])+1)/Stride[3]);
               for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                    Number_Of_Elements *= ( (Bound[i] - Base[i]) + 1) / Stride[i];
             }
        }

     return Number_Of_Elements;
   }

#if defined(PPP)
bool
Array_Domain_Type::isLeftPartition( int Axis ) const
   {
  // Left_Number_Of_Points and Right_Number_Of_Points start counting at the left and right most
  // ghost boundaries which on small arrays (with the adjacent processor having a number of
  // elements in the partition equal to the ghost boundary width) can mean that a partition might
  // mistakenly be considered to be the leftmost processor.  We use the PARTI descriptor to 
  // resolve this since it assumes the world starts at zero (P++ makes no such assumption).

     int Result = FALSE;

#if defined(USE_PADRE)
  // What function of PADRE should we use?
  // printf ("NEED TO CALL PADRE \n"); APP_ABORT();
     APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     Result = parallelPADRE_DescriptorPointer->isLeftPartition(Axis);
#else
  // printf ("Axis = %d \n",Axis);
     if (!isNonPartition(Axis))
        {
          if ( Axis < Domain_Dimension)
             {
               APP_ASSERT(BlockPartiArrayDomain != NULL);
               Result = (gLBnd(BlockPartiArrayDomain,Axis) == 0);
             }
            else
             {
            // Along higher axes we want to force isLeftPartition == TRUE
               Result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif

     return Result;
   }

bool
Array_Domain_Type::isMiddlePartition( int Axis ) const
   {
  // Left_Number_Of_Points and Right_Number_Of_Points start counting at the left and right most
  // ghost boundaries which on small arrays (with the adjacent processor having a number of
  // elements in the partition equal to the ghost boundary width) can mean that a partition might
  // mistakenly be considered to be the leftmost processor.  We use the PARTI descriptor to 
  // resolve this since it assumes the world starts at zero (P++ makes no such assumption).

     int Result = FALSE;

#if defined(USE_PADRE)
  // What function of PADRE should we use?
  // printf ("NEED TO CALL PADRE \n"); APP_ABORT();
     APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     Result = parallelPADRE_DescriptorPointer->isMiddlePartition(Axis);
#else
  // printf ("Axis = %d \n",Axis);
     if (!isNonPartition(Axis))
        {
          if ( Axis < Domain_Dimension)
             {
               Result = ( (isLeftPartition(Axis) || isRightPartition(Axis)) ) ? FALSE : TRUE;

               if (Result && Is_A_Null_Array)
                  {
                    printf ("Inside of Array_Domain_Type::isMiddlePartition -- Middle_Processor is a NULL_ARRAY \n");
                  }
             }
            else
             {
            // Along higher axes we want to force isMiddlePartition == TRUE
               Result = FALSE;
             }
        }

  // USE_PADRE not defined
#endif

     return Result;
   }

bool
Array_Domain_Type::isRightPartition( int Axis ) const
   {
  // Left_Number_Of_Points and Right_Number_Of_Points start counting at the left and right most
  // ghost boundaries which on small arrays (with the adjacent processor having a number of
  // elements in the partition equal to the ghost boundary width) can mean that a partition might
  // mistakenly be considered to be the leftmost processor.  We use the PARTI descriptor to
  // resolve this since it assumes the world starts at zero (P++ makes no such assumption).

     int Result = FALSE;
#if defined(USE_PADRE)
  // What function of PADRE should we use?
  // printf ("NEED TO CALL PADRE \n"); APP_ABORT();
     APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     Result = parallelPADRE_DescriptorPointer->isRightPartition(Axis);
#else
  // printf ("Axis = %d \n",Axis);
     if (!isNonPartition(Axis))
        {
          if ( Axis < Domain_Dimension)
             {
               APP_ASSERT(BlockPartiArrayDomain != NULL);
               int Array_Size_Data [MAX_ARRAY_DIMENSION];
               getRawDataSize (Array_Size_Data);
               Result = (gUBnd(BlockPartiArrayDomain,Axis) == Array_Size_Data[Axis]-1);
             }
            else
             {
            // Along higher axes we want to force isRightPartition == TRUE
               Result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif

     return Result;
   }

bool
Array_Domain_Type::isNonPartition( int Axis ) const
   {
  // Check to see if local processor has a part of the P++ array object
  // since not all processors have to own a part of every array object.

  // (bug fix 3/5/96 kdb) BlockPartiArrayDomain doesn't
  // reflect views so use Local_Mask_Index instead if any are
  // a Null_Index all should be so check axis 0

#if COMPILE_DEBUG_STATEMENTS
     if (Local_Mask_Index[0].getMode() == Null_Index)
        {
          for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
               APP_ASSERT (Local_Mask_Index[i].getMode() == Null_Index);
        }
#endif

     bool Result = (Local_Mask_Index[Axis].getMode() == Null_Index);
     return Result;
   }


//---------------------------------------------------------------------
int
Array_Domain_Type::findProcNum ( int* indexVals ) const
   {
  // Find number of processor where indexVals lives

     int indexValsNoOffset[MAX_ARRAY_DIMENSION];

     int i;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Bugfix (Brian Miller - 10/5/2000)
       // indexValsNoOffset[i] = (indexVals[i]-Data_Base[i]) * Stride[i];
          indexValsNoOffset[i] = (indexVals[i]-User_Base[i]) * Stride[i] + Base[i];
        }

     int procno = -1;
#if defined(USE_PADRE)
     procno = parallelPADRE_DescriptorPointer->representation->findProcNum(indexValsNoOffset);
#else
     procno = get_proc_num(BlockPartiArrayDomain,indexValsNoOffset);
#endif

     return procno;
   }

//---------------------------------------------------------------------

int*
Array_Domain_Type::setupProcessorList(
   intSerialArray** Index_Arrays, int* global_I_base, int& numberOfSupplements, 
   int** supplementLocationsPtr, int** supplementData, 
   int& numberOfLocal, int** localLocationsPtr, int** localData) const
   {
   // ... Find last dimension that is indirectly addressed and use that for
   //  control since all will be the same length. ...
   int indirdim = 0;
   int nd; 
   for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++) 
      if (Index_Arrays[nd] != NULL) indirdim = nd;

   // ... Index_Arrays must be 1d so only use length of first dimension. ...
   int length = Index_Arrays[indirdim]->getLength(0);
   int* processorList = new int[length];

   int* supplementLocations = new int[length];
   int* localLocations      = new int[length];
   *supplementLocationsPtr  = supplementLocations;
   *localLocationsPtr       = localLocations;

   for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
   {
     supplementData[nd]      = new int[length];
     APP_ASSERT (supplementData[nd] != NULL);
     localData[nd]           = new int[length];
     APP_ASSERT (localData[nd] != NULL);
   }
   numberOfSupplements = 0;
   numberOfLocal= 0;


   int proc_num = Communication_Manager::My_Process_Number;

   int indexVals[MAX_ARRAY_DIMENSION];
   for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = getRawBase(nd);
   //for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = 0;

   int index_Base[MAX_ARRAY_DIMENSION];
   for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++) 
      if (Index_Arrays[nd] != NULL) 
         index_Base[nd] = Index_Arrays[nd]->getDataBase(0);
      else
         index_Base[nd] = 0;

   // ... loop through Index_Arrays in each dimension finding off processor 
   //  points and then storing the associated processor number ...  
   int cntr = 0;
   int cntrl = 0;
   int pnum;
   int i; 
   for (i=0; i<length; i++)
   {
     for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
        if (Index_Arrays[nd] != NULL) 
	   indexVals[nd] = (*Index_Arrays[nd])(i+index_Base[nd]);
     pnum = findProcNum(indexVals);

     // ... supplementLocations and localLocations contain the position
     //  of the Index_Array value globally (i.e. the position it has
     //  in the parallel Index_Array attached to the parallel domain) ...
     if (pnum != proc_num)
     {
	// need to subtract base for location
	supplementLocations[cntr] = i-global_I_base[indirdim]+
	   index_Base[indirdim];

        for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
 	  supplementData[nd][cntr] = indexVals[nd];

	processorList[cntr++] = pnum;
     }
     else
     {
	localLocations[cntrl] = i-global_I_base[indirdim]+index_Base[indirdim];
        for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
	  localData[nd][cntrl] = indexVals[nd];

	cntrl++;
     }
   }
   numberOfSupplements = cntr;
   numberOfLocal = cntrl;
   return processorList;
   }

//---------------------------------------------------------------------

int*
Array_Domain_Type::setupProcessorList
   (int** I_A_list, int list_size, 
    int& numberOfSupplements, int** supplementLocationsPtr, int** supplementData, 
    int& numberOfLocal, int** localLocationsPtr, int** localData) const
{
   int length = list_size;
   int* processorList = new int[length];

   int* supplementLocations = new int[length];
   int* localLocations      = new int[length];
   *supplementLocationsPtr  = supplementLocations;
   *localLocationsPtr       = localLocations;

   int nd;
   for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
   {
     supplementData[nd]      = new int[length];
     localData[nd]           = new int[length];
   }
   numberOfSupplements = 0;
   numberOfLocal= 0;

   int proc_num = Communication_Manager::My_Process_Number;

   int cntr = 0;
   int cntrl = 0;
   int i; 
   int indexVals[MAX_ARRAY_DIMENSION];
   for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = getRawBase(nd);
   //for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = 0;

   // ... loop through I_A finding off processor points and then
   //  storing the associated processor number ...
   int pnum;
   for (i=0; i<length; i++)
   {
     for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) 
       if (I_A_list[nd] != NULL) 
         indexVals[nd] = I_A_list[nd][i];
     pnum = findProcNum(indexVals);

     if (pnum != proc_num)
     {
	supplementLocations[cntr] = i;
        for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) 
	  supplementData[nd][cntr] = indexVals[nd];
	processorList[cntr++] = pnum;
     }
     else
     {
	localLocations[cntrl] = i;
        for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) 
	  localData[nd][cntrl] = indexVals[nd];
	cntrl++;
     }
   }
   numberOfSupplements = cntr;
   numberOfLocal = cntrl;
   return processorList;
}
//---------------------------------------------------------------------
int*
Array_Domain_Type::setupProcessorListOnPosition(
   intSerialArray** Index_Arrays, int* global_I_base, 
   int& numberOfSupplements, int** supplementLocationsPtr, int** supplementData, 
   int& numberOfLocal, int** localLocationsPtr, int** localData) const
   {

   //  ... Use last indirectly addressed dimension for control since all
   //    will be the same length and distributed the same. ...
   int indirdim = 0;
   int nd;
   for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++) 
      if (Index_Arrays[nd] != NULL) indirdim = nd;

   // ... Index_Arrays are 1d so use first dimension. ...
   int length = Index_Arrays[indirdim]->getLength(0);
   int* processorList = new int[length];

   int* supplementLocations = new int[length];
   int* localLocations      = new int[length];
   *supplementLocationsPtr  = supplementLocations; 
   *localLocationsPtr       =  localLocations; 

   for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
   {
      supplementData[nd]    = new int[length];
      localData[nd]         = new int[length];
   }
   numberOfSupplements = 0;
   numberOfLocal= 0;

   int proc_num = Communication_Manager::My_Process_Number;

   int cntr = 0;
   int cntrl = 0;
   int i; 
   int indexVals[MAX_ARRAY_DIMENSION];
   for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = getRawBase(nd);
   //for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = 0;

   // ... loop through Index_Arrays finding off processor points and then
   //  storing the associated processor number ...
   int pnum;
   int offset_Base[MAX_ARRAY_DIMENSION];
   int index_Base[MAX_ARRAY_DIMENSION];
   for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++) 
   {
      if (Index_Arrays[nd] != NULL) 
      {
         index_Base[nd]  = Index_Arrays[nd]->getBase(0);
         offset_Base[nd] = Data_Base[nd]+index_Base[nd];
      }
   }
   for (i=0; i<length; i++)
   {
     for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) 
        if (Index_Arrays[nd] != NULL) indexVals[nd] = i+ offset_Base[nd];

     pnum = findProcNum(indexVals);
     if (pnum != proc_num)
     {
	supplementLocations[cntr] = i-global_I_base[indirdim]+
	   index_Base[indirdim];

        for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
	{
           if (Index_Arrays[nd] != NULL) 
	      supplementData[nd][cntr] = (*Index_Arrays[nd])(i+index_Base[nd]);
           else
	      supplementData[nd][cntr] = 0;
	}
	processorList[cntr++] = pnum;
     }
     else
     {
	localLocations[cntrl] = i-global_I_base[indirdim]+index_Base[indirdim];
        for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
           if (Index_Arrays[nd] != NULL) 
	      localData[nd][cntrl] = (*Index_Arrays[nd])(i+index_Base[nd]);
           else
	      localData[nd][cntrl] = 0;
	cntrl++;
     }
   }
   numberOfSupplements = cntr;
   numberOfLocal = cntrl;
   return processorList;
}
//---------------------------------------------------------------------
int*
Array_Domain_Type::setupProcessorListOnPosition(
   int* I_A_list, int list_size, int I_A_base, 
   int& numberOfSupplements, int** supplementLocationsPtr, int** supplementData, 
   int& numberOfLocal, int** localLocationsPtr, int** localData) const
   {

   // ... this routine only can be called by a 1d array but a view
   //  will have Domain_Dimension 2 and so assert won't work ...
   //APP_ASSERT (Domain_Dimension == 1);

   int length = list_size;
   int* processorList = new int[length];

   int* supplementLocations = new int[length];
   int* localLocations      = new int[length];
   *supplementLocationsPtr  = supplementLocations; 
   *localLocationsPtr       = localLocations;      

   int nd;
   for (nd = 0; nd < MAX_ARRAY_DIMENSION; nd++)
   {
      supplementData[nd]      = new int[length];
      localData[nd]           = new int[length];
   }
   numberOfSupplements = 0;
   numberOfLocal= 0;

   int proc_num = Communication_Manager::My_Process_Number;

   int cntr = 0;
   int cntrl = 0;
   int indexVals[MAX_ARRAY_DIMENSION];
   for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = getRawBase(nd);
   //for (nd=1;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = 0;

   // ... loop through I_A_list finding off processor points and then
   //  storing the associated processor number ...
   int indirdim = 0;
   int pnum;
   int i; 
   for (i=0; i<length; i++)
   {
     indexVals[0] = I_A_list[i] * Stride[0] + User_Base[0];
     pnum = findProcNum(indexVals);

     if (pnum != proc_num)
     {
	supplementLocations[cntr] = i+I_A_base;
	supplementData[0][cntr] = I_A_list[i];
        for (nd=1;nd<MAX_ARRAY_DIMENSION;nd++)
	  supplementData[nd][cntr] = 0;
	  /*supplementData[nd][cntr] = indexVals[nd];*/

	processorList[cntr++] = pnum;
     }
     else
     {
	localLocations[cntrl] = i+I_A_base;
	localData[0][cntrl] = I_A_list[i];
        for (nd=1;nd<MAX_ARRAY_DIMENSION;nd++)
	  localData[nd][cntrl] = 0;
	  /*localData[nd][cntrl] = indexVals[nd];*/

        cntrl++;	
     }
   }
   numberOfSupplements = cntr;
   numberOfLocal = cntrl;
   return processorList;
}
//---------------------------------------------------------------------

#endif

//---------------------------------------------------------------------
int
Array_Domain_Type::getRawDataSize( int Axis ) const
   {
  // This function is useful in accessing the size along each axis of the actual 
  // raw data.  It is mostly used in conjunction with explicit indexing
  // of the raw data obtained from the getDataPointer member function.

     int Raw_Data_Size = 0;

#if COMPILE_DEBUG_STATEMENTS
     APP_ASSERT ( (Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION) );
     if (Size[Axis] == 0)
          for (int temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
               APP_ASSERT(Size[temp] == 0);
#endif

#if 0
     if (Is_A_Null_Array)
          Raw_Data_Size = 0;
       else
        {
          if (Axis == 0)
               Raw_Data_Size = Size[0];
            else
            // this data is stored in a premultiplied form to simplify the MDI layer
               Raw_Data_Size = Size[Axis] / Size[Axis-1];
        }
#else
  // (7/28/2000) We want the original size independent of the view - but the null array could be a view
  // in which case we don't want to treat it differently just because it is a null array view.
     if (Axis == 0)
        {
          Raw_Data_Size = Size[0];
        }
       else
        {
       // this data is stored in a premultiplied form to simplify the MDI layer
       // Since the Size is multiplied with each dimension it is only zero if all are zero!
          if (Size[Axis] == 0)
               Raw_Data_Size = 0;
            else
               Raw_Data_Size = Size[Axis] / Size[Axis-1];
        }
#endif          
     return Raw_Data_Size;
   }

void
Array_Domain_Type::getRawDataSize( int* Data_Size ) const
   {
  // It is useful to have a way of getting the size of the array data
  // (especially in the case of a view) since computing it directly from 
  // the base and bound of the descriptor would be incorrect in the case of
  // a view.  This sort of operation comes up over and over again and so we
  // provide a member function for it.  The input is assumed to be a size
  // consistant with the number of dimensions of the A++/P++ arrays.

     int temp;
#if COMPILE_DEBUG_STATEMENTS
     if (Size[0] == 0)
          for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
               APP_ASSERT(Size[temp] == 0);
#endif

#if 0
     if (Is_A_Null_Array)
        {
       // In case of Size[0] == 0 we have to avoid dividing by ZERO
          for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
               Data_Size [temp] = 0;
        }
       else
        {
          Data_Size [0] = Size[0];
          for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
               Data_Size [temp] = Size[temp] / Size[temp-1];
        }
#else
  // (7/28/2000) We want the original size independent of the view - but the null array could be a view
  // in which case we don't want to treat it differently just because it is a null array view.
  // Since the Size is multiplied with each dimension it is only zero if all are zero!
     if (Size[0] == 0)
        {
       // In case of Size[MAX_ARRAY_DIMENSION-1] == 0 we have to avoid dividing by ZERO
          for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
               Data_Size [temp] = 0;
        }
       else
        {
          Data_Size [0] = Size[0];
          for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
               Data_Size [temp] = Size[temp] / Size[temp-1];
        }
#endif
   }

bool
Array_Domain_Type::isSameBase ( const Array_Domain_Type & X ) const
   {
  // This function is a dimension independent test of equality
  // between the bases of each axis of two array descriptors.

     bool Return_Value = TRUE;
     int temp = 0;
     do {
          Return_Value = getRawBase(temp) == X.getRawBase(temp);
          temp++;
        }
     while ( (temp < MAX_ARRAY_DIMENSION) && (Return_Value == TRUE) );

     return Return_Value;
   }

bool
Array_Domain_Type::isSameBound ( const Array_Domain_Type & X ) const
   {
  // This function is a dimension independent test of equality
  // between the bounds of each axis of two array descriptors.

     bool Return_Value = TRUE;
     int temp = 0;
     do {
          Return_Value = getRawBound(temp) == X.getRawBound(temp);
          temp++;
        }
     while ( (temp < MAX_ARRAY_DIMENSION) && (Return_Value == TRUE) );

     return Return_Value;
   }

bool
Array_Domain_Type::isSameStride ( const Array_Domain_Type & X ) const
   {
  // This function is a dimension independent test of equality
  // between the stride of each axis of two array descriptors.

     bool Return_Value = TRUE;
     int temp = 0;
     do {
          Return_Value = getRawStride(temp) == X.getRawStride(temp);
          temp++;
        }
     while ( (temp < MAX_ARRAY_DIMENSION) && (Return_Value == TRUE) );

     return Return_Value;
   }

bool
Array_Domain_Type::isSameLength ( const Array_Domain_Type & X ) const
   {
  // This function is a dimension independent test of equality
  // between the length of each axis of two array descriptors.

     bool Return_Value = TRUE;
     int temp = 0;
     do {
          Return_Value = getLength(temp) == X.getLength(temp);
          temp++;
        }
     while ( (temp < MAX_ARRAY_DIMENSION) && (Return_Value == TRUE) );

     return Return_Value;
   }

#if defined(APP) || defined(PPP)
bool
Array_Domain_Type::isSameGhostBoundaryWidth ( const Array_Domain_Type & X ) const
   {
  // This function is a dimension independent test of equality
  // between the ghost boundary widths of each axis of two array descriptors.

     bool Return_Value = TRUE;
#if defined(PPP)
     int temp = 0;
     do {
          Return_Value = InternalGhostCellWidth[temp] == X.InternalGhostCellWidth[temp];
          temp++;
        }
     while ( (temp < MAX_ARRAY_DIMENSION) && (Return_Value == TRUE) );
#endif

     return Return_Value;
   }

bool
Array_Domain_Type::isSameDistribution ( const Array_Domain_Type & X ) const
   {
     bool Return_Value = TRUE;

#if defined(PPP)
     int temp = 0;
     if ( Partitioning_Object_Pointer == X.Partitioning_Object_Pointer)
        {
       // If they use the same partition then they have the same 
       // distribution only if they are the same size
          do {
               Return_Value = Size[temp] == X.Size[temp];
               temp++;
             }
          while ( (temp < MAX_ARRAY_DIMENSION) && (Return_Value == TRUE) );
        }
     else
        Return_Value = FALSE;
#endif

     return Return_Value;
   }
#endif

bool
Array_Domain_Type::isSimilar ( const Array_Domain_Type & X ) const
   {
  // This function is a dimension independent test of equality
  // between the bases of each axis of two array descriptors.

     bool Return_Value = isSameBase(X) && isSameBound(X) && isSameStride(X);
     return Return_Value;
   }

int
Array_Domain_Type::computeArrayDimension ( int* Array_Sizes )
   {
#if 1
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
        {
          printf ("Inside of Array_Domain_Type::computeArrayDimension --- \n");
          for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
               printf (" %d",Array_Sizes[i]);
          printf ("\n");
        }

     int temp;
     for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          APP_ASSERT(Array_Sizes[temp] >= 0);
        }
#endif

     int Array_Dimension = MAX_ARRAY_DIMENSION;
     bool Done = FALSE;
     int j = MAX_ARRAY_DIMENSION-1;
     while ( (!Done) && (j >= 0) )
        {
          if (Array_Sizes[j] <= ((j > 0) ? 1 : 0))
               Array_Dimension--;
            else
               Done = TRUE;
          j--;
        }
     APP_ASSERT (Array_Dimension >= 0);
     APP_ASSERT (Array_Dimension <= MAX_ARRAY_DIMENSION);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of Array_Domain_Type::computeArrayDimension (int*) -- Array_Dimension = %d \n",
               Array_Dimension);
#endif

     return Array_Dimension;
#else
     // return computeArrayDimension (Array_Sizes[0],Array_Sizes[1],Array_Sizes[2],Array_Sizes[3]);
     return computeArrayDimension (ARRAY_TO_LIST_MACRO(Array_Sizes));
#endif
   }

int
Array_Domain_Type::computeArrayDimension ( const Array_Domain_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of Array_Domain_Type::computeArrayDimension () \n");
#endif

     int Array_Sizes[MAX_ARRAY_DIMENSION];

     if (X.Is_A_Null_Array == TRUE)
          return 0;

  // Remember that Sizes in the Array_descriptor_Type are stored already multiplied by 
  // the lower dimension sizes to simplify the internal address arithmitic.
     Array_Sizes[0] = X.Size[0];
     int temp;
     for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
          Array_Sizes[temp] = X.Size[temp] / X.Size[temp-1];

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Calling Array_Domain_Type::computeArrayDimension () \n");
#endif
     return computeArrayDimension (Array_Sizes);
   }

#if !defined(PPP)
void
Array_Domain_Type::Allocate_Larger_Reference_Count_Array ( int New_Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Inside of Array_Domain_Type::Allocate_Larger_Reference_Count_Array (%d) current size = %d \n",
               New_Size,Array_Reference_Count_Array_Length);
#endif

     const int referenceCountArrayChunkSize = 100;

  // Make the array size a little larger to account for growth!
  // New_Size += 300;
     New_Size += referenceCountArrayChunkSize;

  // Note that on 4.1.3 systems realloc does not work when the pointer is NULL (a bug) so we
  // have to allocate some memory using malloc to initialize the pointer!
     if ( Array_Reference_Count_Array == NULL )
        {
          if ( Array_Reference_Count_Array_Length != 0 )
             {
               printf ("ERROR: Array_Reference_Count_Array_Length != 0 at first use of malloc! \n");
               APP_ABORT();
             }

          Array_Reference_Count_Array = (int*) APP_MALLOC ( New_Size * sizeof(int) );
          APP_ASSERT (Array_Reference_Count_Array != NULL);
	  if (Diagnostic_Manager::getTrackArrayData() == TRUE)
             {
            // printf ("tracking array data (initial allocation)! \n");
               Diagnostic_Manager::diagnosticInfoArray = 
                    (DiagnosticInfo**) APP_MALLOC ( New_Size * sizeof(DiagnosticInfo*) );
               APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
             }
        }
       else
        {
          Array_Reference_Count_Array = (int*) realloc ( (char*) Array_Reference_Count_Array , 
                                                         New_Size * sizeof(int) );
          APP_ASSERT (Array_Reference_Count_Array != NULL);
	  if (Diagnostic_Manager::getTrackArrayData() == TRUE)
             {
            // printf ("tracking array data (realloc)! \n");
               Diagnostic_Manager::diagnosticInfoArray =
                    (DiagnosticInfo**) realloc ( (char*) Diagnostic_Manager::diagnosticInfoArray, 
                                                 New_Size * sizeof(DiagnosticInfo*) );
               APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
             }
        }


     APP_ASSERT (Array_Reference_Count_Array             != NULL);
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
        }

  // Zero the new elements of the array!
  // This is where the reference count array is initialized
     int i;
     int initialReferenceCountValue = getReferenceCountBase() - 1;
     for (i=Array_Reference_Count_Array_Length; i < New_Size; i++)
        {
       // initialize to starting values
          Array_Reference_Count_Array [i] = initialReferenceCountValue;
        }
     
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
          for (i=Array_Reference_Count_Array_Length; i < New_Size; i++)
             {
            // initialize new pointers to NULL
	       Diagnostic_Manager::diagnosticInfoArray[i] = NULL;
             }
        }

     Array_Reference_Count_Array_Length = New_Size;
   }
#endif

#if defined(PPP)
//-------------------------------------------------------------------------
// This function is specific for use with P++ indirect addressing!
void
Array_Domain_Type::Compute_Local_Index_Arrays ( const Array_Domain_Type & originalDomain )
   {
  // By initializing the Local_Array_Index initially to the serial part of the Index_Array
  // we can test the P++ indirect addressing on a single process and THEN move onto the
  // proper initialization of the Local_Index_Array using the valid serial portion of the parallel
  // indirect addressing vector (parallel array).  This pemits an incremental approach to the
  // support of P++ indirect addressing.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Array_Domain_Type::Compute_Local_Index_Arrays()");
          printf ("     (internalArrayID = %d)\n",internalArrayID);
        }
#endif

  // Avoid compiler warning about unused input variable
     if (&originalDomain);

  // Initialize the Local_Index_Array pointers for each intArray used in the indirect addressing
     int i;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {   
          if (Index_Array[i] == NULL)
             {
               Local_Index_Array[i] = NULL;
             }
            else
             {
               Local_Index_Array[i] = Index_Array[i]->Array_Descriptor.SerialArray;
               Index_Array[i]->Array_Descriptor.SerialArray->incrementReferenceCount();
               APP_ASSERT (Local_Index_Array[i]->Array_Descriptor.Array_Domain.Is_A_Temporary == FALSE);
             }
        }
   }

//-------------------------------------------------------------------------
#if 0
// I think this function is no longer used!
// This function is specific for use with P++ indirect addressing!
void
Array_Domain_Type::Compute_Local_Index_Arrays ( const Array_Domain_Type & Original_Domain )
   {
     int i; // Index value used below

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::Compute_Local_Index_Arrays() (internalArrayID = %d)\n",internalArrayID);
#endif

  // P++ indirect addressing must perform some magic here!
  // Instead of disabling indirect addressing in A++ we can make it work by doing the
  // following:
  //   1. Copy distributed intArray to intSerialArray (replicated on every processor)
  //   2. Record the intArray associated with a specific intSerialArray
  //   3. Cache intSerialArray and record Array_ID in list
  //   4. Force any change to an intArray to check for an associated intSerialArray and
  //      remove the intSerialArray from the cache if it is found
  //   5. Bind the intSerialArray to the doubleArray or floatArray
  //   6. remove the values that are out of bounds of the local A++ array
  //      associated with the P++ Array and reform the smaller intSerialArray

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::Compute_Local_Index_Arrays() -- Caching of local intArray usage not implemented yet! \n");
#endif

  // The whole point is to cache the Local_Index_Arrays for reuse -- though we have to figure out
  // if they have been changed so maybe this is expensive anyway.  This later work has not
  // been implemented yet.

     intArray *Disabled_Where_Statement = NULL;
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
             {
               printf ("Where statement in use so we temporarily disable \n");
               printf ("  it so we can use A++ to process the intArray indexing \n");
             }
#endif
          Disabled_Where_Statement = Where_Statement_Support::Where_Statement_Mask;
          Where_Statement_Support::Where_Statement_Mask = NULL;
        }

  // printf ("Initialize the Replicated_Index_intArray array! \n");
     intSerialArray *Replicated_Index_intArray[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Put P++ intArray into the cache
       // Indirect_Addressing_Cache_Domain *Cache_Domain = Indirect_Addressing_Cache.put (X);
       // Distrbuted_Array_To_Replicated_Array_Conversion (X);

       // printf ("Initialize the Replicated_Index_intArray[%d] \n",i);
          if (Index_Array[i] == NULL)
             {
               Replicated_Index_intArray[i] = NULL;
             }
            else
             {
            // For now we handle the case of replicating the P++ intArray onto only a single processor
            // so we can debug indirect addressing on a single processor first.
            // printf ("Conversion of P++ intArray to intSerialArray only handled for single processor case! \n");
               Replicated_Index_intArray[i] = 
		  Index_Array[i]->Array_Descriptor.SerialArray;
               Index_Array[i]->Array_Descriptor.SerialArray->
		  incrementReferenceCount();
            // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");
               APP_ASSERT(Replicated_Index_intArray[i]->Array_Descriptor.
			  Array_Domain.Is_A_Temporary == FALSE);
             }

       // printf ("Replicated_Index_intArray[%d] = %p \n",i,Replicated_Index_intArray[i]);
        }


  // printf ("Initialize the Valid_Local_Entries_Mask! \n");
  // Build a mask which marks where the entries in the Replicated_intArray are valid for the local
  // serial Array (i.e. those entries that are in bounds).  I think we need all the dimension
  // information to do this!!!!
     intSerialArray Valid_Local_Entries_Mask;

  // APP_DEBUG = 5;
  // printf ("SET Valid_Local_Entries_Mask = 1! \n");
  // Valid_Local_Entries_Mask = 1;
  // printf ("DONE with SET Valid_Local_Entries_Mask = 1! \n");
  // APP_DEBUG = 0;
  // Valid_Local_Entries_Mask.view("Valid_Local_Entries_Mask");
  // printf ("sum(Valid_Local_Entries_Mask) = %d \n",sum(Valid_Local_Entries_Mask));
  // Valid_Local_Entries_Mask.view("Valid_Local_Entries_Mask");
  // printf ("Exiting after test ... \n");
  // APP_ABORT();

     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // printf ("Initialize the Valid_Local_Entries_Mask i = %d \n",i);
          if (Replicated_Index_intArray[i] != NULL)
             {
               int Local_Base  = Original_Domain.Data_Base[i] + Original_Domain.Base[i];
               int Local_Bound = Original_Domain.Data_Base[i] + Original_Domain.Bound[i];

            // printf ("Local_Base = %d  Local_Bound = %d \n",Local_Base,Local_Bound);
               if (Valid_Local_Entries_Mask.Array_Descriptor.Array_Domain.
		   Is_A_Null_Array)
                  {
                 // printf ("FIRST assignment of Valid_Local_Entries_Mask \n");
                    Valid_Local_Entries_Mask = ((*Replicated_Index_intArray[i]) >= Local_Base) &&
                                               ((*Replicated_Index_intArray[i]) <= Local_Bound);
                  }
                 else
                  {
                 // printf ("LATER assignment of Valid_Local_Entries_Mask \n");
                    Valid_Local_Entries_Mask = ( ((*Replicated_Index_intArray[i]) >= Local_Base) &&
                                                 ((*Replicated_Index_intArray[i]) <= Local_Bound) ) &&
                                                 Valid_Local_Entries_Mask;
                  }

               if ( sum(Valid_Local_Entries_Mask) != Replicated_Index_intArray[i]->getLength(0) )
                  {
                    printf ("ERROR CHECKING: sum(Valid_Local_Entries_Mask) != Replicated_Index_intArray[%d]->getLength(0) \n",i);
                    printf ("sum(Valid_Local_Entries_Mask) = %d \n",sum(Valid_Local_Entries_Mask));
                    printf ("Replicated_Index_intArray[%d]->getLength(0) = %d \n",i,
                              Replicated_Index_intArray[i]->getLength(0));
                 // display("THIS Domain");
                 // Original_Domain.display("ORIGINAL Domain");
                 // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");
                    Valid_Local_Entries_Mask.view("Valid_Local_Entries_Mask");
                    APP_ABORT();
                  }
             }
        }

  // Valid_Local_Entries_Mask.display("Inside of Array_Domain_Type::Compute_Local_Index_Arrays -- Valid_Entries_Mask");

  // printf ("Initialize the Valid_Local_Entries! \n");
  // Build a set of  i n d e x  values that are associated with where the Valid_Entries_Mask is TRUE
     intSerialArray Valid_Local_Entries;
     Valid_Local_Entries = Valid_Local_Entries_Mask.indexMap();

  // Valid_Local_Entries.display("Valid_Local_Entries");

  // printf ("Initialize the Local_Index_Array array! \n");
  // Now initialize the Local_Index_Array pointers for each intArray used in the indirect addressing
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // printf ("Initialize the Local_Index_Array array[%d] \n",i);
          if (Index_Array[i] == NULL)
             {
               Local_Index_Array[i] = NULL;
             }
            else
             {
               APP_ASSERT (Replicated_Index_intArray[i] != NULL);
            // printf ("BEFORE -- build new intSerialArray ((*Replicated_Index_intArray[i]) (Valid_Local_Entries)) \n");
               Local_Index_Array[i] = new intSerialArray ((*Replicated_Index_intArray[i]) (Valid_Local_Entries));
            // printf ("AFTER -- build new intSerialArray ((*Replicated_Index_intArray[i]) (Valid_Local_Entries)) \n");

            // error checking
               if ( sum ( (*Index_Array[i]->Array_Descriptor.SerialArray) != 
		    (*Local_Index_Array[i]) ) != 0 )
                  {
                    printf ("ERROR: On a single processor the Local_Index_Array should match the Index_Array[i]->Array_Descriptor.SerialArray \n");
                    APP_ABORT();
                  }
             }
        }

  // Reenable the where statement if it was in use at the invocation of this function
     if (Disabled_Where_Statement != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Where statement was disable so now RESET it! \n");
#endif
          Where_Statement_Support::Where_Statement_Mask = Disabled_Where_Statement;
        }
   }
#endif
//----------------------------------------------------------------------
#endif

#if 0
#if defined(PPP)
void
Array_Domain_Type::Update_Parallel_Information_Using_Old_Domain ( const Array_Domain_Type & Old_Array_Domain )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of Array_Domain_Type::Update_Parallel_Information_Using_Old_Domain! \n");
#endif
  // Copy relevant data from old descriptor (this should be a function)

  // printf ("In Array_Domain_Type::Update_Parallel_Information_Using_Old_Domain - INCREMENT REFERENCE COUNTS OF THESE OBJECTS!!! \n");
     Partitioning_Object_Pointer  = Old_Array_Domain.Partitioning_Object_Pointer;
     if ( Partitioning_Object_Pointer != NULL) 
          Partitioning_Object_Pointer->incrementReferenceCount();

#if defined(USE_PADRE)
  // What PADRE function do we call here?
     printf ("NEED TO CALL PADRE \n"); APP_ABORT();
#else
#if 1
     BlockPartiArrayDecomposition = Old_Array_Domain.BlockPartiArrayDecomposition;
     if ( BlockPartiArrayDecomposition != NULL) 
          BlockPartiArrayDecomposition->referenceCount++;

     BlockPartiArrayDomain    = Old_Array_Domain.BlockPartiArrayDomain;
     if ( BlockPartiArrayDomain != NULL) 
          BlockPartiArrayDomain->referenceCount++;
#else
     BlockPartiArrayDecomposition = NULL;
     BlockPartiArrayDomain    = NULL;
#endif
#endif // End of USE_PADRE not defined

     int temp;
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          InternalGhostCellWidth [temp] = Old_Array_Domain.InternalGhostCellWidth [temp];
          ExternalGhostCellWidth [temp] = Old_Array_Domain.ExternalGhostCellWidth [temp]; 
        }
   }
#endif
#endif

// ***************************************************************************
// This function checks the values of data stored inside the descriptor against
// data that can be computed a different way (often more expensively) or it checks
// to make sure that the values are within some sutible range. (i.e values that
// should never be negative are verified to be >= 0 etc.)  In the case of the 
// Block-Parti data we have some data stored redundently and so we can do a
// lot of checking to make sure that the two sets of data are consistant with one
// another.  This catches a LOT of errors and avoids any deviation of the PARTI data
// from the A++/P++ descriptor data.  It is hard to explain how useful this
// consistency checking is -- it automates so much error checking that is is nearly
// impossible to have A++/P++ make an error without it being caught internally by this
// automated error checking.  The calls to these functions (every object has a similar 
// function) can be compiled out by specifying the appropriate compile option
// so they cost nothing in final runs of the code and help a lot in the development.
// ***************************************************************************
void
Array_Domain_Type::Test_Preliminary_Consistency( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since Array_Domain_Type::Test_Preliminary_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of Array_Domain_Type::Test_Preliminary_Consistency! (Label = %s) (Domain_Dimension = %d) \n",
               Label,Domain_Dimension);
#endif

     int temp = 0;
#if !defined(PPP)
  // APP_ASSERT(internalArrayID < Array_Reference_Count_Array_Length);
     if (internalArrayID >= Array_Reference_Count_Array_Length)
        {
          printf ("ERROR: internalArrayID >= Array_Reference_Count_Array_Length in Array_Domain_Type::Test_Preliminary_Consistency \n");
          APP_ABORT();
        }
#endif

  // The base is always the base into the raw memory and so can not be negative
  // so we might as well test it since it has would have caught a P++ bug I just fixed
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
       // Bugfix (3/17/96)
       // I can't see why the case of Uses_Indirect_Addressing == TRUE should change
       // the general rule that the Base should be >= 0.  So I have modified the test.
       // I think that having made the mixed intArray scalar indexing consistant with 
       // that of Index object scalar indexing will simplify this problem.
       // APP_ASSERT(Uses_Indirect_Addressing || (Base[temp] >= 0));
          if ( Base[temp] < 0 )
             {
               display("ERROR in Array_Domain_Type::Test_Preliminary_Consistency");
             }
          APP_ASSERT ( Base[temp] >= 0 );
        }

     if (Uses_Indirect_Addressing == TRUE)
        {
          bool Valid_Pointer_To_intArray_Found = FALSE;
          for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               if (Index_Array [temp] != NULL)
                  {
                    Valid_Pointer_To_intArray_Found = TRUE;
                    Index_Array [temp]->Test_Consistency (Label);
                  }
             }
          APP_ASSERT (Valid_Pointer_To_intArray_Found == TRUE);
        }

#if 0
  // If we test this variable (used for default initialization) and the user
  // application does not reference it (indirectly through a member function using
  // it as a default parameter) then the use gets an undefined symbol at link time.
     APP_Unit_Range.Test_Consistency(Label);
#endif

  // Since I just fixed a bug related to this we add it as a test
     if (Is_A_Null_Array == TRUE)
        {
          bool Confirmed_Null_Array = FALSE;
          for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
               if (Base[temp] == Bound[temp]+1)
                    Confirmed_Null_Array = TRUE;
          if (Confirmed_Null_Array == FALSE)
               display("ERROR in Array_Domain_Type::Test_Consistancy()");
          APP_ASSERT (Confirmed_Null_Array == TRUE);
        }

#if 1
#if !defined(PPP)
  // check for illegal values in the reference counts
  // if (Array_Reference_Count_Array [internalArrayID] < 0)
  // *********************************************************************************
  // Later we have to remove the -1 adjustment to the getReferenceCountBase() function
  // *********************************************************************************
     if (Array_Reference_Count_Array [internalArrayID] < getReferenceCountBase()-1)
        {
          printf ("ERROR: Array_Reference_Count_Array [internalArrayID] < getReferenceCountBase()-1 --- Array_Reference_Count_Array [internalArrayID] = %d \n",
                Array_Reference_Count_Array [internalArrayID]); 
          display("ERROR: Array_Reference_Count_Array [internalArrayID] < getReferenceCountBase()-1");
        }
     APP_ASSERT(Array_Reference_Count_Array [internalArrayID] >= getReferenceCountBase()-1);
     APP_ASSERT(Array_Reference_Count_Array [internalArrayID] < 100000);

  // Was this descriptor used to build an array object using the adopt function?
  // This is usually FALSE.
     if (builtUsingExistingData == TRUE)
        {
       // If we have used the adopt function with some existing data
       // then the reference count for the data reflects this by being 
       // greater than or equal to one instead of zero.
       // APP_ASSERT(Array_Reference_Count_Array [internalArrayID] >= 1);
          APP_ASSERT(Array_Reference_Count_Array [internalArrayID] >= getReferenceCountBase()+1);
        }
#endif
#else
     printf ("WARNING: referenceCount error testing turned off in Array_Domain_Type::Test_Preliminary_Consistency \n");
#endif

  // This is the reference count for the domain object (not the array data)
     if (referenceCount < getReferenceCountBase())
          printf ("referenceCount = %d \n",referenceCount);
     APP_ASSERT(referenceCount >= getReferenceCountBase());

#if defined(PPP)
  // Temp range checking on these variables (for debugging only)
#if 0
  // This is commented out so we can run larger problems
     int Arbitrary_Bound = 100;
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          APP_ASSERT(Left_Number_Of_Points [temp] >= 0 && Left_Number_Of_Points [temp] < Arbitrary_Bound);
          APP_ASSERT(Right_Number_Of_Points[temp] >= 0 && Right_Number_Of_Points[temp] < Arbitrary_Bound);
        }
#endif

     int Small_Arbitrary_Bound = 10;
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          if ( InternalGhostCellWidth[temp] > Small_Arbitrary_Bound )
             {
               printf ("P++ presently setup for ghost boundary width limit of %d to simplify debugging \n",
                    Small_Arbitrary_Bound);
               APP_ABORT();
             }
          APP_ASSERT(InternalGhostCellWidth[temp] >= 0 && InternalGhostCellWidth[temp] <= Small_Arbitrary_Bound);
          APP_ASSERT(ExternalGhostCellWidth[temp] == 0);
        }

     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          APP_ASSERT(InternalGhostCellWidth[temp] >= 0 && InternalGhostCellWidth[temp] <= Small_Arbitrary_Bound);
          APP_ASSERT(ExternalGhostCellWidth[temp] == 0);
        }

     if (Communication_Manager::Number_Of_Processors == 1)
        {
       // This is the case of P++ on a single processor!
          for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
             {
#if COMPILE_DEBUG_STATEMENTS
               if ( (Left_Number_Of_Points [temp] != 0) || (Right_Number_Of_Points [temp] != 0) )
                    printf ("ERROR  ---  Left_Number_Of_Points [%d] = %d Right_Number_Of_Points[%d] = %d \n",
                         temp,Left_Number_Of_Points [temp],temp,Right_Number_Of_Points [temp]);
#endif
               APP_ASSERT( Left_Number_Of_Points [temp] == 0 );
               APP_ASSERT( Right_Number_Of_Points[temp] == 0 );
             }

          // ... these won't be set correctly if indirect addressing
	  //  is used but it doesn't seem to matter ...
          if (!Uses_Indirect_Addressing)
	     {
               for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
                  {
#if 0
                    printf ("In Array_Domain_Type::Test_Preliminary_Consistency: temp = %d Global_Index[%d].getLength() = %d  Local_Mask_Index[%d].getLength() = %d \n",
                         temp,temp,Global_Index[temp].getLength(),temp,Local_Mask_Index[temp].getLength());
                    Global_Index[temp].display("Global_Index in Test_Preliminary_Consistency");
                    Local_Mask_Index[temp].display("Local_Mask_Index in Test_Preliminary_Consistency");
#endif
                    APP_ASSERT (Global_Index[temp].getLength() == Local_Mask_Index[temp].getLength() );
                  }
	     }
        }

  // Mode could be Null_Index or Index_Triplet so just make sure it is not an All_Index
  // There is no test for an Internal_Index except a consistancy test
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          Global_Index     [temp].Test_Consistency(Label);
          Local_Mask_Index [temp].Test_Consistency(Label);
        }

     if ( Array_Conformability_Info != NULL )
        {
          Array_Conformability_Info->Test_Consistency(Label);
        }
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Leaving Array_Domain_Type::Test_Preliminary_Consistency! (Label = %s) \n",Label);
#endif
   }

void
Array_Domain_Type::Test_Distribution_Consistency ( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since Array_Domain_Type::Test_Distribution_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
        {
          printf ("Inside of Array_Domain_Type::Test_Distribution_Consistency! (Label = %s) \n",Label);
        }
#endif

#if defined(PPP)
#if defined(USE_PADRE)
  // What PADRE function do we call here?
  // printf ("NEED TO CALL PADRE \n");
  // APP_ABORT();
  // printf ("WARNING: Ingnoring PADRE specific code in Array_Domain_Type::Test_Consistency! \n");
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
  // parallelPADRE_DescriptorPointer->testConsistency(Label);
     if (parallelPADRE_DescriptorPointer != NULL)
        {
          APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
          parallelPADRE_DescriptorPointer->testConsistency(Label);

       // Make sure that the PADRE_Distribution that the Partition object uses
       // is the same as the one that is used by the PADRE_Descriptor
          PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>
               *PartitionObjectPADRE_DistributionPointer = NULL;
          if (Partitioning_Object_Pointer != NULL)
             {
               PartitionObjectPADRE_DistributionPointer =
                    Partitioning_Object_Pointer->distributionPointer;
             }
            else
             {
            // Need to find the PADRE_Distribution used by the default distribution
               PartitionObjectPADRE_DistributionPointer =
                    PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>
                         ::getDefaultDistributionPointer();
             }

#if PADRE_DISTRIBUTION_CONSISTENCY_TEST
       // It is not clear if this is a valid test -- it seems that it should be
       // but there are many special cases where it fails!
          PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>
               *ArrayDomainObjectPADRE_DistributionPointer = NULL;
          APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
          ArrayDomainObjectPADRE_DistributionPointer = 
               parallelPADRE_DescriptorPointer->getPADRE_Distribution();

          if (PartitionObjectPADRE_DistributionPointer != 
              ArrayDomainObjectPADRE_DistributionPointer)
             {

               printf ("Array_Domain_Type::Test_Distribution_Consistency: Failed test PartitionObjectPADRE_DistributionPointer == ArrayDomainObjectPADRE_DistributionPointer! \n");
               printf ("PartitionObjectPADRE_DistributionPointer = %p \n",
                    PartitionObjectPADRE_DistributionPointer);
               printf ("ArrayDomainObjectPADRE_DistributionPointer = %p \n",
                    ArrayDomainObjectPADRE_DistributionPointer);
             }
       // APP_ASSERT (PartitionObjectPADRE_DistributionPointer == 
       //             ArrayDomainObjectPADRE_DistributionPointer);
#endif
        }
#else
  // We don't have a specific test for the non-PADRE case but I think we should
  // implement one that is similar.

     if (BlockPartiArrayDomain != NULL)
        {
          if (Partitioning_Object_Pointer != NULL)
             {
               Partitioning_Object_Pointer->Test_Consistency(BlockPartiArrayDomain,Label);
             }
            else
             {
               Internal_Partitioning_Type::staticTestConsistency(BlockPartiArrayDomain,Label);
             }
        }
       else
        {
       // printf ("In Array_Domain_Type::Test_Distribution_Consistency(): BlockPartiArrayDomain == NULL \n");
        }
#endif
#endif
   }

void
Array_Domain_Type::Test_Consistency( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since Array_Domain_Type::Test_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if 0
#if defined(PPP)
#   error "PPP IS defined"
#else
#   error "PPP is NOT defined"
#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
        {
       // printf ("Inside of Array_Domain_Type::Test_Consistency! (Label = %s) (Domain_Dimension = %d) \n", Label,Domain_Dimension);
          printf ("Inside of Array_Domain_Type::Test_Consistency! (Label = %s) \n",Label);
          printf ("Domain_Dimension = %d \n",Domain_Dimension);
        }
#endif

     Test_Preliminary_Consistency (Label);

#if 0
  // Make sure that the Domain_Dimension value matches what we can compute from the stored 
  // values of the array sizes in the descriptor.  This just checks consistency which is what
  // this function is all about.
     if ( Domain_Dimension != computeArrayDimension (*this) )
        {
          printf ("Domain_Dimension (=%d) != computeArrayDimension (*this) (=%d) \n",
               Domain_Dimension,computeArrayDimension (*this));
          APP_ABORT();
        }

     APP_ASSERT( Domain_Dimension == computeArrayDimension (*this) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of Array_Domain_Type::Test_Consistency!  PASSED Description Check \n");
#endif

     APP_ASSERT ( referenceCount >= getReferenceCountBase() );
     APP_ASSERT ( internalArrayID > 0 );

     APP_ASSERT ( typeCode > 0 );
     APP_ASSERT ( (typeCode == APP_INT_ELEMENT_TYPE)    || (typeCode == APP_FLOAT_ELEMENT_TYPE) ||
                  (typeCode == APP_DOUBLE_ELEMENT_TYPE) || (typeCode == APP_UNDEFINED_ELEMENT_TYPE) );

#if USE_ORDER_N_SQUARED_TESTS
  // This is an expensive O(n^2) operation so use it saringly as a test
  // Search the stack for the existing Array_ID in use (it should not be there!)
  // We might also search for redundent entries -- this is not implemented yet
  // since this would be expensive.
     APP_ASSERT (checkForArrayIDOnStack (internalArrayID) == 0);

     int* StackEntry = Top_Of_Stack;
     if (StackEntry != NULL)
        {
       // Note Pointer arithmetic
          while (StackEntry > Top_Of_Stack - Stack_Depth)
             {
            // Top_Of_Stack = Bottom_Of_Stack + Stack_Depth;
            // printf ("In while loop: *StackEntry = %d == internalArrayID = %d \n",*StackEntry,internalArrayID);
            // printf ("*StackEntry     = %d \n",*StackEntry);
               APP_ASSERT (checkForArrayIDOnStack (*StackEntry) == 1);
               StackEntry--;
             }
        }
#endif

  // Reference this variable so that purify can check that it is initialized!
     APP_ASSERT ( (ExpressionTemplateOffset        ==    0) || (ExpressionTemplateOffset         !=    0) );
     APP_ASSERT ( (Is_A_View                       == TRUE) || (Is_A_View                       == FALSE) );
     APP_ASSERT ( (Is_A_Temporary                  == TRUE) || (Is_A_Temporary                  == FALSE) );
     APP_ASSERT ( (Is_Contiguous_Data              == TRUE) || (Is_Contiguous_Data              == FALSE) );
     APP_ASSERT ( (Is_Built_By_Defered_Evaluation  == TRUE) || (Is_Built_By_Defered_Evaluation  == FALSE) );
     APP_ASSERT ( (Uses_Indirect_Addressing        == TRUE) || (Uses_Indirect_Addressing        == FALSE) );
     APP_ASSERT ( (Constant_Data_Base              == TRUE) || (Constant_Data_Base              == FALSE) );
     APP_ASSERT ( (Constant_Unit_Stride            == TRUE) || (Constant_Unit_Stride            == FALSE) );
     APP_ASSERT ( (Is_A_Null_Array                 == TRUE) || (Is_A_Null_Array                 == FALSE) );

     APP_ASSERT ( (Domain_Dimension >= 0) && (Domain_Dimension <= MAX_ARRAY_DIMENSION) );

     int i;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          APP_ASSERT (Is_A_View || (Base[i] == 0) );
        }

     if (Is_A_Null_Array == TRUE)
        {
           APP_ASSERT(Is_Contiguous_Data == FALSE);

        // (8/10/2000) We want a consistant setting for this when we have a NullArray object
        // temporarily disabled to pass P++ test_Ppp_execution
        // printf ("In Array_Domain_Type::Test_Consistency(): Constant_Unit_Stride == %s \n",(Constant_Unit_Stride == TRUE) ? "TRUE" : "FALSE");
           APP_ASSERT ( Constant_Unit_Stride == TRUE );

           for (i=0; i < MAX_ARRAY_DIMENSION; i++)
              {
                APP_ASSERT(Base  [i] ==  0);
                APP_ASSERT(Bound [i] == -1);
              }
        }

  // If we are using indirect addressing then there must be a valid intArray in use
  // however if not then all the pointers to the intArrays should be NULL
     if (Uses_Indirect_Addressing == TRUE)
        {
           APP_ASSERT(Is_Contiguous_Data == FALSE);
           bool IndexArrayInUse = FALSE;
           for (i=0; i < MAX_ARRAY_DIMENSION; i++)
              {
                if (Index_Array[i] != NULL)
                   {
                     IndexArrayInUse = TRUE;
                   }
              }
           APP_ASSERT(IndexArrayInUse == TRUE);

        // Call the Test_Consistency function on the indirect addressing vectors!
           for (i=0; i < MAX_ARRAY_DIMENSION; i++)
              {
                if (Index_Array[i] != NULL)
                   {
                     Index_Array[i]->Test_Consistency("Indirect Addressing Array");
                   }
              }
        }
       else
        {
           for (i=0; i < MAX_ARRAY_DIMENSION; i++)
              {
                APP_ASSERT(Index_Array[i] == NULL);
              }
        }

  // See if this is true in general!
  // APP_ASSERT(User_Base[0] == Data_Base[0] + Base[0]);

#if defined(PPP)
  // printf ("In Array_Domain_Type::Test_Consistency(): Checking SerialArray_Domain_Type::Array_Reference_Count_Array[%d] = %d \n",
  //      internalArrayID,SerialArray_Domain_Type::Array_Reference_Count_Array[internalArrayID]);

  // Bugfix (11/4/2000)
  // P++ arrays should not use the SerialArray_Domain_Type::Array_Reference_Count_Array
  // for tracking reference counts (we need to detect this since it is an error).
     APP_ASSERT (SerialArray_Domain_Type::Array_Reference_Count_Array[internalArrayID] == getReferenceCountBase()-1);

  // Bugfix (9/23/2000) This was not previously looping over all i and was using the value of i that
  // previous loops had set i to be (on the DEC this was MAX_ARRAY_DIMENSION)
  // NOTE: The out of bounds array access is NOT caught by purify!!!
  // printf ("Value for i in Test_Consistency() = %d \n",i);
  // printf ("Index_Array[i]       = %p \n",Index_Array[i]);
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          APP_ASSERT ( ((Index_Array[i] != NULL) && (Local_Index_Array[i] != NULL)) || (Index_Array[i] == NULL) );
          APP_ASSERT ( ((Index_Array[i] != NULL) && (Local_Index_Array[i] != NULL)) || (Local_Index_Array[i] == NULL) );
        }

  // Test added (8/8/2000)
     if (Is_A_Null_Array == TRUE)
        {
           for (i=0; i < MAX_ARRAY_DIMENSION; i++)
              {
                if (Local_Mask_Index [i].getMode() != Null_Index)
                     display("Called from Array_Domain_Type::Test_Consistency");

                APP_ASSERT(Local_Mask_Index [i].getMode() == Null_Index);
                APP_ASSERT (Global_Index    [i].getMode() == Null_Index);
              }

        // We want a consistant setting for this when we have a NullArray object
        // APP_ASSERT ( Constant_Unit_Stride == FALSE );
           APP_ASSERT ( Constant_Unit_Stride == TRUE );
        }
       else
        {
           bool stridedView = FALSE;
           for (i=0; i < MAX_ARRAY_DIMENSION; i++)
              {
             // In the global descriptor the local serial array might be a 
             // NullArray (i.e. data is partitioned onto other processors)
                if ( Local_Mask_Index [i].getMode() != Null_Index )
                   {
                     if ( Local_Mask_Index [i].getStride() > 1 )
                        {
                          stridedView = TRUE;
                          APP_ASSERT ( Constant_Unit_Stride == FALSE );
                        }
                     if (Global_Index [i].getMode() == Null_Index)
                          display("Called from Array_Domain_Type::Test_Consistency");

                     APP_ASSERT (Global_Index [i].getMode() != Null_Index);
                     APP_ASSERT (Global_Index [i].getStride() == Local_Mask_Index [i].getStride());
                   }
                  else
                   {
                  // Case of serialArray being a NullArray (so check the Global_Index)
                     APP_ASSERT (Global_Index [i].getMode() != Null_Index);
                     if ( Global_Index [i].getStride() > 1 )
                        {
                          stridedView = TRUE;
                          APP_ASSERT ( Constant_Unit_Stride == FALSE );
                        }
                   }
              }
           if ( stridedView == TRUE )
              {
                APP_ASSERT ( Constant_Unit_Stride == FALSE );
              }
             else
              {
                APP_ASSERT ( Constant_Unit_Stride == TRUE );
              }
        }

     Test_Distribution_Consistency (Label);

#if defined(USE_PADRE)
  // What PADRE function do we call here?
  // printf ("NEED TO CALL PADRE \n"); 
  // APP_ABORT();
  // printf ("WARNING: Ingnoring PADRE specific code in Array_Domain_Type::Test_Consistency! \n");
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
  // parallelPADRE_DescriptorPointer->testConsistency(Label);
     if (parallelPADRE_DescriptorPointer != NULL)
        {
          APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
          parallelPADRE_DescriptorPointer->testConsistency(Label);
        }

  // Test commented out on (12/4/2000)
  // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
#else
     if ( BlockPartiArrayDomain != NULL )
        {
          APP_ASSERT( BlockPartiArrayDomain != NULL );
          APP_ASSERT( BlockPartiArrayDecomposition != NULL );

       // Make sure that the dimension of the PARTI descriptor matches that of the P++ descriptor!
	  if (!Uses_Indirect_Addressing)
	  {
             if ( BlockPartiArrayDomain->nDims != Domain_Dimension )
             {
               printf ("ERROR: Domain_Dimension = %d \n",Domain_Dimension);
               printf ("ERROR: BlockPartiArrayDomain->nDims = %d \n",BlockPartiArrayDomain->nDims);
             }
             APP_ASSERT ( BlockPartiArrayDomain->nDims == Domain_Dimension );
       //    APP_ASSERT ( BlockPartiArrayDomain->nDims == Array_Domain_Type::computeArrayDimension(*this) );
	  }
 
       // For now we make it an error if the distribution is not done on all processors!
       // BLOCK-PARTI returns a -1 if the array has no distribution on a processor

       // These Block Parti values are checked since as a test of the BlockParti descriptor.
       // The base and bound for block parti assume the default base of an array is ZERO
       // P++ makes up for this internally to provide greater flexibility of the P++ array objects.
          for (i=0; i < Domain_Dimension; i++)
             {
               if (gLBnd(BlockPartiArrayDomain,i) == -1)
                  {
                // Data not partitioned on this processor (so verify same result for gUBnd)
                   APP_ASSERT (gUBnd(BlockPartiArrayDomain,i) == -1);
                  }
                else
                  {
                 // Data is present on this processor
                    APP_ASSERT (gLBnd(BlockPartiArrayDomain,i) >= 0);
                    APP_ASSERT (gUBnd(BlockPartiArrayDomain,i) >= 0);
                  }
             }
        }
  // End of USE_PADRE not defined
#endif

  // If an array is a temporary then it contians the information about how the 
  // parallel operations was done so that the message passing can be interpreted
  // at either the operator= or the replace operator (operations that specify the end of
  // an array statement).
     if (Is_A_Temporary == TRUE)
        {
       // ... WARNING: turn this off for now ...
#if 0
          if (Array_Conformability_Info == NULL)
             {
               printf ("Is_A_Temporary == TRUE  but  Array_Conformability_Info == NULL \n");
               APP_ABORT();
             }
          APP_ASSERT (Array_Conformability_Info != NULL);
#endif
        }
       else
        {
       // A non temporary can have a valid Array_Conformability_Info pointer as in:
       //   A(I) = (B+C)(I);
       // Here the temporary B+C is a view and not an A++ temporary (in the A++ sense) but it
       // would have a valid Array_Conformability_Info pointer.
#if 0
          if (Array_Conformability_Info != NULL)
             {
               printf ("Is_A_Temporary == FALSE  but  Array_Conformability_Info != NULL \n");
               APP_ABORT();
             }
          APP_ASSERT (Array_Conformability_Info == NULL);
#endif
        }

  // Bugfix (9/9/2000) all trailing dimensions must have zero ghost boundary widths
     for (i = numberOfDimensions(); i < MAX_ARRAY_DIMENSION; i++)
        {
          APP_ASSERT (InternalGhostCellWidth[i] == 0);
        }

  // These should always be set to ZERO since they are not used
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
          APP_ASSERT (ExternalGhostCellWidth[i] == 0);
        }

#if 0
  // In the transpose function in testppp this code generates an error by calling the Array_ID()
  // doubleArray member function when the array objects Array_Domain is NULL!
  // So for now we just turn this off.
     if ( (Is_A_View == FALSE) && (Is_A_Temporary == FALSE) )
        {
          bool Found_Array_ID_In_Array_List = FALSE;
          if (Partitioning_Object_Pointer == NULL)
             {
               if (Internal_Partitioning_Type::DefaultdoubleArrayList.getLength() > 0)
                  {
                    for (i=0; i < Internal_Partitioning_Type::DefaultdoubleArrayList.getLength(); i++)
                         if (Internal_Partitioning_Type::DefaultdoubleArrayList[i].Array_ID() == internalArrayID)
                              Found_Array_ID_In_Array_List = TRUE;
                  }
               if ( (Internal_Partitioning_Type::DefaultfloatArrayList.getLength() > 0) && !Found_Array_ID_In_Array_List )
                  {
                    for (i=0; i < Internal_Partitioning_Type::DefaultfloatArrayList.getLength(); i++)
                         if (Internal_Partitioning_Type::DefaultfloatArrayList[i].Array_ID() == internalArrayID)
                              Found_Array_ID_In_Array_List = TRUE;
                  }
               if ( (Internal_Partitioning_Type::DefaultintArrayList.getLength() > 0) && !Found_Array_ID_In_Array_List )
                  {
                    for (i=0; i < Internal_Partitioning_Type::DefaultintArrayList.getLength(); i++)
                         if (Internal_Partitioning_Type::DefaultintArrayList[i].Array_ID() == internalArrayID)
                              Found_Array_ID_In_Array_List = TRUE;
                  }
             }
            else
             {
               if (Partitioning_Object_Pointer->doubleArrayList.getLength() > 0)
                  {
                    for (i=0; i < Partitioning_Object_Pointer->doubleArrayList.getLength(); i++)
                         if (Partitioning_Object_Pointer->doubleArrayList[i].Array_ID() == internalArrayID)
                              Found_Array_ID_In_Array_List = TRUE;
                  }
               if ( (Partitioning_Object_Pointer->floatArrayList.getLength() > 0) && !Found_Array_ID_In_Array_List )
                  {
                    for (i=0; i < Partitioning_Object_Pointer->floatArrayList.getLength(); i++)
                         if (Partitioning_Object_Pointer->floatArrayList[i].Array_ID() == internalArrayID)
                              Found_Array_ID_In_Array_List = TRUE;
                  }
               if ( (Partitioning_Object_Pointer->intArrayList.getLength() > 0) && !Found_Array_ID_In_Array_List )
                  {
                    for (i=0; i < Partitioning_Object_Pointer->intArrayList.getLength(); i++)
                         if (Partitioning_Object_Pointer->intArrayList[i].Array_ID() == internalArrayID)
                              Found_Array_ID_In_Array_List = TRUE;
                  }
             }

          if (Found_Array_ID_In_Array_List == FALSE)
             {
               printf ("Error Found_Array_ID_In_Array_List == FALSE \n");
               display(Label);
             }
          APP_ASSERT (Found_Array_ID_In_Array_List == TRUE);
        }
#endif

  // if (Array_Conformability_Info != NULL)
  //    {
  //      printf ("##################################################################### \n");
  //      printf ("################# Array_Conformability_Info == NULL ################# \n");
  //      printf ("##################################################################### \n");
  //      APP_ABORT();
  //    }
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Leaving Array_Domain_Type::Test_Consistency! (Label = %s) \n",Label);
#endif
   }

// *********************************************************************************************
// **********************   APPENDED error_checking.C  *****************************************
// *********************************************************************************************

// This turns on the calls to the bounds checking function
#define BOUNDS_ERROR_CHECKING TRUE

// It helps to set this to FALSE sometimes for debugging code
// this enambles the A++/P++ operations in the bounds checking function
#define TURN_ON_BOUNDS_CHECKING   TRUE

// ***************************************************************************************
//     Array_Domain constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

// ****************************************************************************
// ****************************************************************************
// **************  ERROR CHECKING FUNCTION FOR INDEXING OPERATORS  ************
// ****************************************************************************
// ****************************************************************************

void
Array_Domain_Type::Error_Checking_For_Index_Operators (
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::Error_Checking_For_Index_Operators \n");
#endif

  // This array is used when the values of the input Index objects or intArray objects
  // can be simplified to be integers (used in mixed indexing)
     int Integer_List [MAX_ARRAY_DIMENSION];

     int i = 0;

  // ... code change (8/20/96, kdb) new scalar indexing changes
  //   meaning of correct range for indexing. The range now must
  //   be between User_Base and User_Base + Count ...
  // Not sure if this is the correct defaults
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          Integer_List[i] = User_Base[i];
          //Integer_List[i] = Base[i] + Data_Base[i];;

     APP_ASSERT(Internal_Index_List != NULL);

  // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
  // objects can be either an intArray or a scalar (integer) so we have to process the list

     bool Indirect_Addressing_In_Use = FALSE;
     bool Index_Object_In_Use        = FALSE;
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Internal_Index_List[i] != NULL)
               Index_Object_In_Use = TRUE;
        }

     if ( Index_Object_In_Use && Indirect_Addressing_In_Use )
        {
          printf ("ERROR: Can't mix Index objects with intArray object (indirect addressing) indexing! \n");
          APP_ABORT();
        }

  // ... code change (8/20/96, kdb) new scalar indexing changes ...
  // Compute Local_Base and Local_Bound
     int Local_Base  [MAX_ARRAY_DIMENSION];
     int Local_Bound [MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Local_Base [i] = User_Base[i];
          Local_Bound[i] = User_Base[i]+(Bound[i]-Base[i])/Stride[i];
          //Local_Base [i] = Data_Base[i]+Base[i];
          //Local_Bound[i] = Data_Base[i]+Bound[i];
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             printf ("Local_Base[%d] = %d Local_Bound[%d] = %d \n",i,Local_Base[i],i,Local_Bound[i]);
#endif
        }

 // Optionally turn off the bounds checking since it adds to the output when debugging!
#if TURN_ON_BOUNDS_CHECKING
  // ****************************************************************************
  // Now find the min and max access values of the array operation (then check
  // them against the local base and local bound to determine if the access is
  // out of bounds).
  // ****************************************************************************

  // Bug fix (9/23/94) this broke the interpolation in overture for the case of
  // nozero bases of the Target and Source (for a 2D grid containing 2 components and
  // having nonzero base in the 3rd axis) (reference A++/BUGS/bug44.C).
  // The fix is to use *Parm0_int instead of *Parm0_int+Data_Base[0] along all axes.

     int Min[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        Min[i] = (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Base : Local_Base[i] : Integer_List[i];
       // Min[i] = (intArrayList[i]) ? min(*intArrayList[i]) : (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Base : Local_Base[i] : Integer_List[i];

     int Max[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          Max[i] = (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Bound : Local_Bound[i] : Integer_List[i];
       // Max[i] = (intArrayList[i]) ? max(*intArrayList[i]) : (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Bound : Local_Bound[i] : Integer_List[i];

  // Bugfix (3/30/95) added test for Null_Array since P++ requires that we allow indexing
  // of the global indexspace outside of a given partition
     if (Is_A_Null_Array == FALSE)
        {
       // Check bounds!
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               if ( (Min[i] < Local_Base[i]) || (Max[i] > Local_Bound[i]) )
                  {
                    printf ("\n");
                    printf ("**************************************************************** \n");
                    printf ("ERROR: (Array:Error_Checking_For_Index_Operators) INDEXING OUT OF BOUNDS                      \n");
                    printf ("**************************************************************** \n");
                    printf ("ERROR: in Array Indexing using: \n");
                    int j;
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                         printf ("     (Min[%d] = %d,Max[%d] = %d) \n",j,Min[j],j,Max[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("Original Array object base and Bounds are: \n");
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                         printf ("     (Local_Base[%d] = %d,Local_Bound[%d] = %d) \n",j,Local_Base[j],j,Local_Bound[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("\n");
                    APP_ABORT();
                  }
             }
        }
#else
     printf ("WARNING: bounds checking of indirect addressing turned off! \n");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving Array_Domain_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }

void
Array_Domain_Type::Error_Checking_For_Index_Operators (
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::Error_Checking_For_Index_Operators(Indirect_Index_List) \n");
#endif

  // This array is used when the values of the input Index objects or intArray objects
  // can be simplified to be integers (used in mixed indexing)
     int Integer_List [MAX_ARRAY_DIMENSION];
     Internal_Index* Index_List [MAX_ARRAY_DIMENSION];

     int i = 0;

  // ... code change (8/20/96, kdb) new scalar indexing changes ...
  // Not sure if this is the correct defaults
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          Integer_List[i] = User_Base[i];
        //Integer_List[i] = Base[i] + Data_Base[i];

     APP_ASSERT( Indirect_Index_List != NULL );

     intArray* intArrayList[MAX_ARRAY_DIMENSION];
     for (i=0; i<MAX_ARRAY_DIMENSION; i++)
        {
          Index_List  [i] = NULL;
          intArrayList[i] = NULL;
        }

  // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
  // objects can be either an intArray or a scalar (integer) so we have to process the list
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Indirect_Index_List[i] != NULL) 
             { 
               if (Indirect_Index_List[i]->intArrayInStorage)
                    intArrayList[i] = Indirect_Index_List[i]->IndirectionArrayPointer;
                 else
                  {
                    if (Indirect_Index_List[i]->IndexInStorage)
                         Index_List[i]   = &(Indirect_Index_List[i]->Index_Data);
                      else
                         Integer_List[i] = Indirect_Index_List[i]->Scalar;
                  }
             }
        }

     int Size_Of_Parameter [MAX_ARRAY_DIMENSION];
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Error checking for conformable indirect addressing
       // Size_Of_Parameter[i] = (intArrayList[i] != NULL) ? intArrayList[i]->elementCount() : 1;
          if (intArrayList[i] != NULL)
             {
               Size_Of_Parameter[i] = intArrayList[i]->elementCount();
            // printf ("Case of intArrayList[%d] != NULL Size_Of_Parameter[%d] = %d \n",i,i,Size_Of_Parameter[i]);
             }
            else
             {
               if (Index_List[i] != NULL)
                  {
                    Size_Of_Parameter[i] = Index_List [i]->length();
                 // printf ("Case of Index_List[%d] != NULL Size_Of_Parameter[%d] = %d \n",i,i,Size_Of_Parameter[i]);
                  }
                 else
                  {
                    Size_Of_Parameter[i] = 1;
                 // printf ("Case of SCALAR != NULL Size_Of_Parameter[%d] = %d \n",i,Size_Of_Parameter[i]);
                  }
             }

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Size_Of_Parameter[%d] = %d \n",i,Size_Of_Parameter[i]);
#endif
        }

     int Max_Size = 0;
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Max_Size = (Size_Of_Parameter[i] > Max_Size) ? Size_Of_Parameter[i] : Max_Size;
        }

     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Zero should be an acceptable value since in the case of a NULL array used as an indirection array
       // We allow the mixing of scalars and intArray indexing (indirect addressing)
          if ( (Size_Of_Parameter[i] != Max_Size) && (Size_Of_Parameter[i] != 1) )
             {
               printf ("\n");
               printf ("******************************************************************** \n");
               printf ("ERROR - intArrays used for indirect addressing must be the same size \n");
               printf ("******************************************************************** \n");
               printf ("MORE INFO: Inside of indexing operator error checking -- non conformable indirect addressing Max_Size = %d  Size_Of_Parameter list - \n",Max_Size);
               int j;
               for (j = 0; j < MAX_ARRAY_DIMENSION; j++)
                    printf ("%d ",Size_Of_Parameter[i]);
               printf ("\n");
               printf ("******************************************************************** \n");
               printf ("\n");
               APP_ABORT();
             }
        }

  // ... code change (8/20/96, kdb) new scalar indexing changes ...
  // Compute Local_Base and Local_Bound
     int Local_Base  [MAX_ARRAY_DIMENSION];
     int Local_Bound [MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Local_Base [i] = User_Base[i];
          Local_Bound[i] = User_Base[i]+(Bound[i]-Base[i])/Stride[i];
          //Local_Base [i] = Data_Base[i]+Base[i];
          //Local_Bound[i] = Data_Base[i]+Bound[i];
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             printf ("Local_Base[%d] = %d Local_Bound[%d] = %d \n",i,Local_Base[i],i,Local_Bound[i]);
#endif
        }

 // Optionally turn off the bounds checking since it adds to the output when debugging!
#if TURN_ON_BOUNDS_CHECKING
     bool intArrayTemporaryList[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          intArrayTemporaryList[i] = (intArrayList[i]) ? intArrayList[i]->Array_Descriptor.Array_Domain.Is_A_Temporary : FALSE;
        }

  // Set Array destriptor as a non-temporary!
  // Otherwise the temporaries will be deleted by the min max functions!
  // Bug fix (12/14/94) We have to set the Is_A_Temporary flag consistently in A++ and P++
  // so in P++ we have to change the flag status for both P++ and the internal A++ array.
  // and then reset them both in the case of P++.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (intArrayTemporaryList[i] == TRUE)
             {
               intArrayList[i]->Array_Descriptor.Array_Domain.Is_A_Temporary = FALSE;
#if defined(PPP)
               intArrayList[i]->Array_Descriptor.SerialArray->
		  Array_Descriptor.Array_Domain.Is_A_Temporary = FALSE;
#endif
             }
        }
     
  // ****************************************************************************
  // Now find the min and max access values of the array operation (then check
  // them against the local base and local bound to determine if the access is
  // out of bounds).
  // ****************************************************************************

  // Bug fix (9/23/94) this broke the interpolation in overture for the case of
  // nozero bases of the Target and Source (for a 2D grid containing 2 components and
  // having nonzero base in the 3rd axis) (reference A++/BUGS/bug44.C).
  // The fix is to use *Parm0_int instead of *Parm0_int+Data_Base[0] along all axes.

#if COMPILE_DEBUG_STATEMENTS
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          if (intArrayList[i] != NULL)
               intArrayList[i]->Test_Consistency("Array_Domain_Type::Error_Checking_For_Index_Operators (intArrayList[i])");
#endif

     int Min[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Min[i] = (intArrayList[i]) ? min(*intArrayList[i]) : Integer_List[i];
          if (intArrayList[i] != NULL)
               Min[i] = min(*intArrayList[i]);
            else
             {
               if (Index_List[i] != NULL)
                    Min[i] = Index_List [i]->Base;
                 else
                    Min[i] = Integer_List[i];
             }
        }


     int Max[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Max[i] = (intArrayList[i]) ? max(*intArrayList[i]) : Integer_List[i];
          if (intArrayList[i] != NULL)
               Max[i] = max(*intArrayList[i]);
            else
             {
               if (Index_List[i] != NULL)
                    Max[i] = Index_List [i]->Bound;
                 else
                    Max[i] = Integer_List[i];
             }
        }

  // Reset Array destriptor as a temporary!
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (intArrayTemporaryList[i] == TRUE)
             {
               intArrayList[i]->Array_Descriptor.Array_Domain.Is_A_Temporary = TRUE;
#if defined(PPP)
               intArrayList[i]->Array_Descriptor.SerialArray->
		  Array_Descriptor.Array_Domain.Is_A_Temporary = TRUE;
#endif
             }
        }

  // Bugfix (3/30/95) added test for Null_Array since P++ requires that we allow indexing
  // of the global indexspace outside of a given partition
     if (Is_A_Null_Array == FALSE)
        {
       // Check bounds!
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               if ( (Min[i] < Local_Base[i]) || (Max[i] > Local_Bound[i]) )
                  {
                    printf ("\n");
                    printf ("**************************************************************** \n");
                    printf ("     ERROR: (indirect addressing?) INDEXING OUT OF BOUNDS                      \n");
                    printf ("**************************************************************** \n");
                    printf ("ERROR: in Array Indexing using: \n");
                    int j;
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                         printf ("     (Min[%d] = %d,Max[%d] = %d) \n",j,Min[j],j,Max[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("Original Array object base and Bounds are: \n");
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                         printf ("     (Local_Base[%d] = %d,Local_Bound[%d] = %d) \n",j,Local_Base[j],j,Local_Bound[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("\n");
                    APP_ABORT();
                  }
             }
        }
#else
     printf ("WARNING: bounds checking of indirect addressing turned off! \n");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving Array_Domain_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }

// **************************************************************************
// This function does the bounds checking for the scalar indexing of A++
// array objects.  Its purpose is to localize all the error checking for
// scalar indexing.
// **************************************************************************
// Old prototype
// void Array::Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const
void
Array_Domain_Type::Error_Checking_For_Scalar_Index_Operators ( 
     const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const
   {
  // Set maximum dimension of indexing
     int Problem_Dimension = MAX_ARRAY_DIMENSION;

  // Lower the dimension as indicated by the input

     int i;
     for (i = MAX_ARRAY_DIMENSION-1; i >= 0; i--)
          if (Integer_List[i] == NULL) 
               Problem_Dimension--;

  // No support for the scalar indexing of views obtained through indirect addressing
     if (Uses_Indirect_Addressing)
        {
       // printf ("Error Checking for scalar Indexing operations on views built through indirect addressing not implemented yet! \n");
       // APP_ABORT();
          APP_ASSERT (Problem_Dimension == 1);
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               if (Index_Array[i] != NULL)
                    Index_Array[i]->Array_Descriptor.Array_Domain.Error_Checking_For_Scalar_Index_Operators(Integer_List);
             } 
        }
       else
        {
	  // ... first check dimensionality ...
	  if (Domain_Dimension > Problem_Dimension)
	  {
	    for (i=Problem_Dimension;i<Domain_Dimension;i++)
	    {
	      if (Base[i] != Bound[i])
	      {
	        printf ("ERROR: Not enough subscripts, \n");
	        printf ("  Domain_Dimension = %d, \n",
			Domain_Dimension);
                printf ("  Number of subscripts is %d.\n",
		        Problem_Dimension);
                APP_ABORT();
	      }
	    }
	  }
	  else if (Domain_Dimension < Problem_Dimension)
	  {
	    for (i=Domain_Dimension;i<Problem_Dimension;i++)
	    {
  // ... code change (8/20/96, kdb) new scalar indexing changes ...
	      //if ((Base[i] != Bound[i]) && 
	      //  ((Base[i]+Data_Base[i]) != *Integer_List[i]))
	      if ((Base[i] != Bound[i]) && 
		  ((User_Base[i]) != *Integer_List[i]))
              {
		printf ("ERROR: too many subscripts, \n");
                printf ("  Number of subscripts is %d.\n",
		        Problem_Dimension);
	        printf ("  Domain_Dimension = %d, \n",
			Domain_Dimension);
                APP_ABORT();
	      }
	    }
	  }
          for (i=0; i < Problem_Dimension; i++)
             {
  // ... code change (8/20/96, kdb) new scalar indexing changes ...
               //if ( (*Integer_List[i] < Base[i]  + Data_Base[i]) ||
               //   (*Integer_List[i] > Bound[i] + Data_Base[i]) )
               if ( (*Integer_List[i] < User_Base[i]) ||
                    (*Integer_List[i] > 
			User_Base[i]+ (Bound[i]-Base[i])/Stride[i]) )
                  {
                    printf ("ERROR: Scalar Index for dimension(%d) = %d out of bounds! \n",i,*Integer_List[i]);
                    printf ("********************************************************* \n");
                    printf ("Scalar values used in indexing: \n");
                    int j;
                    for (j=0; j < Problem_Dimension; j++)
                       {
                         printf ("     Scalar Index (along dimension %d) = %d \n",j,*Integer_List[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("Original Array object base and Bounds are: \n");
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                        //printf ("    (Base[%d] = %d,Bound[%d] = %d)\n",
			//j,Base[j]+Data_Base[j],j,Bound[j]+Data_Base[j]);
                         printf ("     (Base[%d] = %d,Bound[%d] = %d)\n",
			   j,User_Base[j],j,
			   ((Bound[j]-Base[j])/Stride[j])+Data_Base[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("\n");
                    APP_ABORT();
                  }
             }
        }
   }


// *************************************************************************************************
// ********************  APPEND index_operator.C  **************************************************
// *************************************************************************************************

// ***************************************************************************************
//     Array_Domain constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

#if HPF_INDEXING
#error HPF FORTRAN indexing permanently disabled
#endif

// defined in array.C
extern int APP_Global_Array_Base;

void
Array_Domain_Type::Initialize_View_Of_Target (
   int Axis , const Internal_Index & X , const Array_Domain_Type & Target_Domain )
   {
  /* This function Initialized the Size, Base, Bound, and Stride of a view of a
  // target array object.   It is sufficiently complex that reused below that we isolate it
  // out as a seperate function.  It does the interpretation of the different
  // index modes (Index_Triplet , Null_Index, All_Index).  An index object
  // is an "all" index object by default, it is an Index_Triplet is the
  // base and bound are provided (or a Range object), and it is
  // a Null_Index if is is specially build that way (only P++ uses the Null_Index
  // mode).  P++ uses the Null_Index mode to define array operations on
  // processors that have no part of the distributed data on that processor.
  */

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of Array_Domain_Type::Initialize_View_Of_Target ( int, Internal_Index, Array_Domain_Type ) \n");

  // We don't want to print this out all the time
     if (APP_DEBUG > FILE_LEVEL_DEBUG+5)
        {
          X.display("Inside of Array_Domain_Type::Initialize_View_Of_Target: X Internal_Index object");
          Target_Domain.display("Inside of Array_Domain_Type::Initialize_View_Of_Target: Target_Domain");
        }
#endif

     if (Target_Domain.Uses_Indirect_Addressing)
        {
          printf ("Bug in Internal_Index operations on views built through indirect addressing (not fixed yet!) \n");
          APP_ABORT();
        }

#if defined(PPP)
  // In P++ we save the global Index that was used since it helps drive the message 
  // passing. The Local_Index is not set here (it is set in the 
  // Parallel_Conformability_Enforcement function).  I think this test does not allow
  // for some reasonable cases -- thus is is wrong to do this test.
  // It is here because it is effective in most cases ans thus helped allow me to 
  // debug the code
  // APP_ASSERT (Target_Domain.Global_Index[Axis].getMode()     == Index_Triplet);
  // APP_ASSERT (Target_Domain.Local_Mask_Index[Axis].getMode() == Index_Triplet);

  // if (Target_Domain.Global_Index [Axis].Index_Mode == All_Index)
  //      Global_Index [Axis] = X;
  //   else
  //      Global_Index [Axis] = Target_Domain.Global_Index [Axis](X);

     Global_Index [Axis]     = Target_Domain.Global_Index [Axis](X);
  // ... this doesn't work because Local_Index_Mask needs to know 
  // global info for correct numbering ...
  // Local_Mask_Index [Axis] = Target_Domain.Local_Mask_Index [Axis](x);

     int Temp_Base;
     int Temp_Bound;
     int Temp_Count;

  // ... NOTE: there must be an easier way to compute the first value
  // secified by Global_Index that is larger than or equal to the
  // Target_Domain.Local_Mask_Index[Axis].Base! ...
     if (Global_Index[Axis].Base >= Target_Domain.Local_Mask_Index[Axis].Base)
        {
          Temp_Base = Global_Index[Axis].Base;
        }
       else
        {
          Temp_Base = Target_Domain.Local_Mask_Index[Axis].Base +
	              (Global_Index[Axis].Stride -
	              ((Target_Domain.Local_Mask_Index[Axis].Base -
	              Global_Index[Axis].Base)%Global_Index[Axis].Stride)) % 
                      Global_Index[Axis].Stride;
       /*
       // Temp_Base = Target_Domain.Local_Mask_Index[Axis].Base +
       //    (X.Stride- ((Target_Domain.Local_Mask_Index[Axis].Base -
       //    Global_Index[Axis].Base)% X.Stride)) % X.Stride;
       */

       /*
       // Temp_Base = Target_Domain.Local_Mask_Index[Axis].Base +
       //   (Target_Domain.Local_Mask_Index[Axis].Stride-
       //   ((Target_Domain.Local_Mask_Index[Axis].Base -
       //    Global_Index[Axis].Base)%
       //    Target_Domain.Local_Mask_Index[Axis].Stride)) %
       //   Target_Domain.Local_Mask_Index[Axis].Stride;
       */
        }

  // ... bug fix (9/5/96,kdb) make sure that Temp_Base is contained locally
  // and is in the range specified by X before going to the trouble of finding
  // Temp_Bound.  Otherwise tthe Local_Mask_Index will be NULL ...
     if ((Temp_Base <= Target_Domain.Local_Mask_Index[Axis].Bound) &&
         (Temp_Base <= Global_Index[Axis].Bound))
        {
          if (Global_Index[Axis].Bound <= Target_Domain.Local_Mask_Index[Axis].Bound)
             {
               Temp_Bound = Global_Index[Axis].Bound;
             }
            else
             {
            // ... divide and multiple to find the Bound less than or equal to
            //  the Target_Domain Bound ...
               Temp_Bound = Temp_Base + ((Target_Domain.Local_Mask_Index[Axis].Bound - Temp_Base) /
                            Global_Index[Axis].Stride) * Global_Index[Axis].Stride;
	    /*
            // Target_Domain.Local_Mask_Index[Axis].Stride) *
            // Target_Domain.Local_Mask_Index[Axis].Stride;
	    */
	     }
          Temp_Count = (Temp_Bound-Temp_Base)/Global_Index[Axis].Stride + 1;
        }
       else
        {
	  Temp_Base = 0;
	  Temp_Bound = -1;
	  Temp_Count = 0;
        }

     Local_Mask_Index [Axis] = Internal_Index (Temp_Base,Temp_Count,Global_Index[Axis].Stride);


#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG+5)
          Global_Index [Axis].display ("Global_Index [Axis] in Array_Domain_Type::Initialize_View_Of_Target");
#endif

     APP_ASSERT (Global_Index [Axis].length() <= Target_Domain.Global_Index [Axis].length());

  // Mode could be Null_Index so just make sure it is not and All_Index
     APP_ASSERT (Local_Mask_Index[Axis].getMode() != All_Index);
     APP_ASSERT (Global_Index[Axis].getMode()     != All_Index);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          Global_Index [Axis].display("Global_Index [Axis]");
          Local_Mask_Index [Axis].display("Local_Mask_Index [Axis]");
        }
#endif

     Left_Number_Of_Points  [Axis] = Local_Mask_Index [Axis].getBase() - Global_Index [Axis].getBase();
     Right_Number_Of_Points [Axis] = Global_Index [Axis].getBound()    - Local_Mask_Index [Axis].getBound();

     if (Local_Mask_Index [Axis].Index_Mode == Null_Index)
        {
          Left_Number_Of_Points  [Axis] = Target_Domain.Left_Number_Of_Points  [Axis]; 
          Right_Number_Of_Points [Axis] = Target_Domain.Right_Number_Of_Points [Axis];
        }
       else
        {
          Left_Number_Of_Points  [Axis] = Local_Mask_Index [Axis].getBase() - Global_Index [Axis].getBase();
          Right_Number_Of_Points [Axis] = Global_Index [Axis].getBound()    - Local_Mask_Index [Axis].getBound();
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW \n");
          printf ("In Initialize_View_Of_Target: Left_Number_Of_Points  [%d] = %d \n",Axis,Left_Number_Of_Points[Axis]);
          printf ("In Initialize_View_Of_Target: Right_Number_Of_Points [%d] = %d \n",Axis,Right_Number_Of_Points[Axis]);
          printf ("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW \n");
        }
#endif

     APP_ASSERT( Left_Number_Of_Points  [Axis] >= 0 );
     APP_ASSERT( Right_Number_Of_Points [Axis] >= 0 );

  // There may be some redundent initialization here
     InternalGhostCellWidth [Axis] = Target_Domain.InternalGhostCellWidth [Axis];
     ExternalGhostCellWidth [Axis] = Target_Domain.ExternalGhostCellWidth [Axis];
#endif

  // Used in Initialize_Non_Indexed_Dimension but set in Initialize_Domain!
     Uses_Indirect_Addressing = FALSE;

     Size      [Axis] = Target_Domain.Size[Axis];

  // Bug fix (11/11/94) Data base is same between view and original descriptor so that
  // the orginal geometry can be computed (i.e. it is preserved in the view)
     Data_Base [Axis] = Target_Domain.Data_Base[Axis];

     if (X.Index_Mode == Index_Triplet)
        {
       // Most common case!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > FILE_LEVEL_DEBUG)
               printf ("X.Index_Mode == Index_Triplet \n");
#endif
       // Bug fix (8/11/94)
       // Base   [Axis] = X.Base  - Target_Domain.Data_Base[Axis];
       // Bound  [Axis] = X.Bound - Target_Domain.Data_Base[Axis];
          User_Base[Axis] = X.Base;

       // printf ("Setup in Array_Domain_Type::Initialize_View_Of_Target ( int, Internal_Index, Array_Domain_Type ): User_Base[%d] = %d \n",Axis,User_Base[Axis]);

       // Note that the strides multiply!
          Stride [Axis] = X.Stride * Target_Domain.Stride[Axis];

       // This is the reason we require the User_Base for computing 
       // views of views (the strides > 1) make this more complex
          Base   [Axis] = (X.Base  - Target_Domain.User_Base[Axis]) * Target_Domain.Stride[Axis] + Target_Domain.Base[Axis];
          Bound  [Axis] = Base[Axis] + (X.Count - 1) * Stride[Axis];

       // For example -- if we  i n d e x  a stride 2 view with a
       // stride 2  i n d e x  then we require stride 4 access!
       // Most of the time Target_Domain.Stride [Axis] == 1!

          APP_ASSERT (Stride[Axis] >= 1);
        }
       else 
        {
          if (X.Index_Mode == All_Index)
             {
            // All_Index is the default mode for an Index and it means access all of the 
            // dimension of any array it is applied to in the construction of a view 
            // through indexing.
            // 2nd most common case!
               User_Base   [Axis] = Target_Domain.User_Base[Axis];
               Base        [Axis] = Target_Domain.Base[Axis];
               Bound       [Axis] = Target_Domain.Bound[Axis];
               Stride      [Axis] = Target_Domain.Stride[Axis];

            // Modify the Geometry base to reflect the indexed view
            // Geometry_Base [Axis] = Target_Domain.Geometry_Base[Axis];
             }
            else
             {
               if (X.Index_Mode == Null_Index)
                  {
                    User_Base [Axis]   =  Data_Base[Axis];
                    Base      [Axis]   =  0;
                    Bound     [Axis]   = -1;
                    Stride    [Axis]   =  1;
                    Is_A_Null_Array = TRUE;

                 // We force this to be FALSE in the case of a Null Array (generated by an Null_Index).
                    Is_Contiguous_Data  = FALSE;

                 // Modify the Geometry base to reflect the indexed view
                 // Geometry_Base [Axis] = Target_Domain.Geometry_Base[Axis];
                  }
#if COMPILE_DEBUG_STATEMENTS
                 else
                  {
                    printf ("ERROR: Inside of Array_Domain_Type::Initialize_View_Of_Target ");
                    printf  ("-- Index_Mode unknown! \n");
                    APP_ABORT();
                  }
#endif
             }
        }
   }

// ************************************************************************************
// ************************************************************************************
//             Initialization of descriptors for Non specified dimensions
// ************************************************************************************
// ************************************************************************************
void
Array_Domain_Type::Initialize_Non_Indexed_Dimension ( int Axis , const Array_Domain_Type & Target_Domain )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of Array_Domain_Type::Initialize_Non_Indexed_Dimension ");
          printf ("( Array_Domain_Type & Target_Domain , ");
          printf ("int (Axis = %d) ) \n",Axis);
       }
#endif

     APP_ASSERT (Uses_Indirect_Addressing == FALSE);

     Data_Base     [Axis] = Target_Domain.Data_Base[Axis];
     User_Base     [Axis] = Target_Domain.User_Base[Axis];

  // This forces non indexed dimension (last dimensions) 
  // to be equivalent to indexing with an ALL Index object.
     Base          [Axis] = Target_Domain.Base[Axis];
     Bound         [Axis] = Target_Domain.Bound [Axis];

     Size          [Axis] = Target_Domain.Size[Axis];
     Stride        [Axis] = Target_Domain.Stride[Axis];

#if defined(PPP)
     Local_Mask_Index[Axis] = Target_Domain.Local_Mask_Index[Axis];
     Global_Index[Axis]     = Target_Domain.Global_Index[Axis];
     APP_ASSERT (Target_Domain.Global_Index[Axis].getMode()     != All_Index);
     APP_ASSERT (Target_Domain.Global_Index[Axis].getMode()     != All_Index);
     APP_ASSERT (Local_Mask_Index[Axis].getMode()   != All_Index);
     APP_ASSERT (Global_Index[Axis].getMode()       != All_Index);
     Left_Number_Of_Points  [Axis] = Target_Domain.Left_Number_Of_Points  [Axis];
     Right_Number_Of_Points [Axis] = Target_Domain.Right_Number_Of_Points [Axis];

  // There may be some redundent initialization here
     InternalGhostCellWidth [Axis] = Target_Domain.InternalGhostCellWidth [Axis];
     ExternalGhostCellWidth [Axis] = Target_Domain.ExternalGhostCellWidth [Axis];
#endif

#if 0
     printf ("In Array_Domain_Type::Initialize_Non_Indexed_Dimension(): Scalar_Offset[%d] = %d \n",Axis,Scalar_Offset[Axis]);
     if ( Axis > 0 )
          printf ("Scalar_Offset[%d] = %d \n",Axis-1,Scalar_Offset[Axis-1]);
     printf ("User_Base[%d]     = %d \n",Axis,User_Base[Axis]);
     printf ("Stride[%d]        = %d \n",Axis,Stride[Axis]);
     printf ("Size[%d]          = %d \n",Axis,Size[Axis]);

     if (Axis==0)
          Scalar_Offset[0] = View_Offset - User_Base[0] * Stride[0];
       else
          Scalar_Offset[Axis] = Scalar_Offset[Axis-1] - User_Base[Axis] * Stride[Axis] * Size[Axis-1];
#endif
   }

// ************************************************************************************
// ************************************************************************************
//                Initialization of descriptors for Indirect Addressing
// ************************************************************************************
// The scalar values are setup differently for Indirect addressing so we require a 
// special function for this
// ************************************************************************************
// ************************************************************************************
void 
Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target (
   int Axis , int x , const Array_Domain_Type & Target_Domain )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target ( int, int=%d, Array_Domain_Type ) \n",x);
#endif

     Size      [Axis] = Target_Domain.Size[Axis];
  // Bugfix (3/16/96) must match behavior of Internal_Index indexing (non indirect addressing)
  // Base      [Axis] = x;
     Base      [Axis] = x - Target_Domain.Data_Base[Axis];
     Bound     [Axis] = Base [Axis];
     Stride    [Axis] = Target_Domain.Stride[Axis];
     Data_Base [Axis] = Target_Domain.Data_Base[Axis];
     User_Base [Axis] = Target_Domain.User_Base[Axis];

#if defined(PPP)
  // In P++ we save the global Index that was used since it helps drive the message passing
  // The Local_Index is not set here (it is set in the Parallel_Conformability_Enforcement function)
     if (Target_Domain.Global_Index [Axis].Index_Mode == All_Index)
          Global_Index [Axis] = x;
       else
          Global_Index [Axis] = Target_Domain.Global_Index [Axis](x);

     APP_ASSERT (Target_Domain.Global_Index[Axis].getMode()     == Index_Triplet);

  // I don't know if this is a good test to have here!
     APP_ASSERT (Target_Domain.Local_Mask_Index[Axis].getMode() == Index_Triplet);
     if (!Uses_Indirect_Addressing)
        APP_ASSERT (Local_Mask_Index[Axis].getMode() == Index_Triplet);
     APP_ASSERT (Global_Index[Axis].getMode()     == Index_Triplet);

     //printf ("Since indirect addressing is not debugged the following may be incorrect! \n");
     //APP_ABORT();

     Local_Mask_Index[Axis] = Target_Domain.Local_Mask_Index[Axis];
  // Global_Index[Axis]     = Target_Domain.Global_Index[Axis];
     if (!Uses_Indirect_Addressing)
        APP_ASSERT (Target_Domain.Local_Mask_Index[Axis].getMode() == Index_Triplet);
     APP_ASSERT (Target_Domain.Global_Index[Axis].getMode()     == Index_Triplet);
     APP_ASSERT (Local_Mask_Index[Axis].getMode()   == Index_Triplet);
     APP_ASSERT (Global_Index[Axis].getMode()       == Index_Triplet);
     Left_Number_Of_Points  [Axis] = Target_Domain.Left_Number_Of_Points  [Axis];
     Right_Number_Of_Points [Axis] = Target_Domain.Right_Number_Of_Points [Axis];

  // There may be some redundent initialization here
     InternalGhostCellWidth [Axis] = Target_Domain.InternalGhostCellWidth [Axis];
     ExternalGhostCellWidth [Axis] = Target_Domain.ExternalGhostCellWidth [Axis];
#endif
   }

// ************************************************************************************
// The scalar values are setup differently for Indirect addressing so we require a
// special function for this
// ************************************************************************************
// ************************************************************************************
void
Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target (
int Axis , const Internal_Index & X , const Array_Domain_Type & Target_Domain )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target ( int, Internal_Index, Array_Domain_Type ) \n");
#endif

     if (X.Index_Mode != Null_Index)
     {
        Size      [Axis] = Target_Domain.Size[Axis];
      //Base      [Axis] = X.Base - Target_Domain.Data_Base[Axis];
        Base      [Axis] = (X.Base - Target_Domain.User_Base[Axis]) *
			    Target_Domain.Stride[Axis] + Target_Domain.Base[Axis];
      //Stride    [Axis] = X.Stride;
        Stride    [Axis] = X.Stride * Target_Domain.Stride[Axis];
      //Bound     [Axis] = X.Bound;
        Bound     [Axis] = Base[Axis] + (X.Count - 1) * Stride[Axis];
        Data_Base [Axis] = Target_Domain.Data_Base[Axis];
      //User_Base [Axis] = Target_Domain.User_Base[Axis];
        User_Base [Axis] = X.Base;
     }
     else
     {
        Size      [Axis] = Target_Domain.Size[Axis];
        Base      [Axis] = 0;
        Stride    [Axis] = 1;
        Bound     [Axis] = -1;
        Data_Base [Axis] = Target_Domain.Data_Base[Axis];
        User_Base [Axis] = 0;
     }

#if defined(PPP)
  // In P++ we save the global Index that was used since it helps drive the message passing
  // The Local_Index is not set here (it is set in the Parallel_Conformability_Enforcement function)
     if (Target_Domain.Global_Index [Axis].Index_Mode == All_Index)
          Global_Index [Axis] = X;
       else
          Global_Index [Axis] = Target_Domain.Global_Index [Axis](X);

     APP_ASSERT (Target_Domain.Global_Index[Axis].getMode()     == Index_Triplet);

  // I don't know if this is a good test to have here!
     APP_ASSERT (Target_Domain.Local_Mask_Index[Axis].getMode() == Index_Triplet);
     if (!Uses_Indirect_Addressing)
        APP_ASSERT (Local_Mask_Index[Axis].getMode() == Index_Triplet);
     APP_ASSERT (Global_Index[Axis].getMode()     == Index_Triplet);

     //printf ("Since indirect addressing is not debugged the following may be incorrect (Initialize_Indirect_Addressing_View_Of_Target(int,Internal_Index,Array_Domain_Type))! \n");
     //APP_ABORT();

     Local_Mask_Index[Axis] = Target_Domain.Local_Mask_Index[Axis];
  // Global_Index[Axis]     = Target_Domain.Global_Index[Axis];
     APP_ASSERT (Target_Domain.Local_Mask_Index[Axis].getMode() == Index_Triplet);
     APP_ASSERT (Target_Domain.Global_Index[Axis].getMode()     == Index_Triplet);
     APP_ASSERT (Local_Mask_Index[Axis].getMode()   == Index_Triplet);
     APP_ASSERT (Global_Index[Axis].getMode()       == Index_Triplet);
     Left_Number_Of_Points  [Axis] = Target_Domain.Left_Number_Of_Points  [Axis];
     Right_Number_Of_Points [Axis] = Target_Domain.Right_Number_Of_Points [Axis];

  // There may be some redundent initialization here
     InternalGhostCellWidth [Axis] = Target_Domain.InternalGhostCellWidth [Axis];
     ExternalGhostCellWidth [Axis] = Target_Domain.ExternalGhostCellWidth [Axis];
#endif
   }

// ************************************************************************************
// ************************************************************************************
//                Initialization of descriptors for Indirect Addressing
// ************************************************************************************
// ************************************************************************************
void
Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target (
   int Axis , const intArray & X , const Array_Domain_Type & Target_Domain )
   {
  /* This function Initialized the Size, Base, Bound, and Stride of a view of a 
  // target array object.   It sets the Base to ZERO always, and then Bound to the size of
  // the intArray used to index the Array -1, so that the size of subsequent
  // temporaries which compute their size based on the Base and Bound info will be
  // correct.
  */
  /* Only the first dimension is allowed to represent the size of the array's view
     The other dimensions carry the information required for it to be interpreted correctly by the 
     MDI functions.
  */
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG+5)
        {
          printf ("Inside of Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target ( int, intArray, Array_Domain_Type ) \n");
          X.display("X intArray Index object");
        }
#endif
  // Used in Initialize_Non_Indexed_Dimension but set in Initialize_Domain!
     Uses_Indirect_Addressing = TRUE;

     Size      [Axis] = Target_Domain.Size[Axis]; 

  // The Data_Base is set to APP_Global_Array_Base not the Base.
  // Base      [Axis] = APP_Global_Array_Base;
     Base      [Axis] = 0;
     Bound     [Axis] = Base [Axis];
     Stride    [Axis] = Target_Domain.Stride[Axis];

     if (Target_Domain.Uses_Indirect_Addressing)
        {
       // If indexed with a intArray we assume Geometry_Base [Axis] = 0
       // We have to assume something and we can not afford to search for the
       // minimum of the intArray
          Data_Base     [Axis] = APP_Global_Array_Base;
          User_Base     [Axis] = APP_Global_Array_Base;
        }
       else
        {
       // Bug fix (9/27/94) make consistant with scalar and Index object indexing!
          Data_Base     [Axis] = Target_Domain.Data_Base[Axis];
          User_Base     [Axis] = Target_Domain.User_Base[Axis];
        }

  // We can pick any axis since the intArray objects are conformable
     if (Axis == 0)
          Bound [Axis] += X.elementCount()-1;

#if defined(PPP)
  // printf ("Since indirect addressing is not debugged the following may be incorrect! \n");
  // WARNING: this is not correct but it might not be used anyways in indirect addressing
     Local_Mask_Index[Axis] = Target_Domain.Local_Mask_Index[Axis];
     Global_Index[Axis]     = Target_Domain.Global_Index[Axis];
  // special case for Axis 0 for indirect addressing to make consistant with base and bound
     if (Axis == 0)
	Global_Index[0] = Index(0,Bound[0]+1);
     APP_ASSERT (Target_Domain.Local_Mask_Index[Axis].getMode() == Index_Triplet);
     APP_ASSERT (Target_Domain.Global_Index[Axis].getMode()     == Index_Triplet);
     APP_ASSERT (Local_Mask_Index[Axis].getMode()   == Index_Triplet);
     APP_ASSERT (Global_Index[Axis].getMode()       == Index_Triplet);
     Left_Number_Of_Points  [Axis] = Target_Domain.Left_Number_Of_Points  [Axis];
     Right_Number_Of_Points [Axis] = Target_Domain.Right_Number_Of_Points [Axis];

  // There may be some redundent initialization here
     InternalGhostCellWidth [Axis] = Target_Domain.InternalGhostCellWidth [Axis];
     ExternalGhostCellWidth [Axis] = Target_Domain.ExternalGhostCellWidth [Axis];
#endif
   }

// ************************************************************************************
// ************************************************************************************
//             Initialization of descriptors for Non specified dimensions
// ************************************************************************************
// ************************************************************************************
void
Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension(
   int Axis , const Array_Domain_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Inside of Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension ( Array_Domain_Type & X , int (Axis = %d) ) \n",Axis);
#endif

     APP_ASSERT (Uses_Indirect_Addressing == TRUE);

     Data_Base     [Axis] = X.Data_Base[Axis];
     User_Base     [Axis] = X.User_Base[Axis];

  // Bugfix (10/3/2000) Fix for base of non accessed dimension in indirect addressing
  // Base          [Axis] = X.Data_Base[Axis];
     Base          [Axis] = X.User_Base[Axis];

     Bound         [Axis] = Base [Axis];

     Size          [Axis] = X.Size[Axis];
     Stride        [Axis] = X.Stride[Axis];

#if defined(PPP)
     Local_Mask_Index[Axis] = X.Local_Mask_Index[Axis];
     Global_Index[Axis]     = X.Global_Index[Axis];
     APP_ASSERT (X.Local_Mask_Index[Axis].getMode() == Index_Triplet);
     APP_ASSERT (X.Global_Index[Axis].getMode()     == Index_Triplet);
     APP_ASSERT (Local_Mask_Index[Axis].getMode()   == Index_Triplet);
     APP_ASSERT (Global_Index[Axis].getMode()       == Index_Triplet);
     Left_Number_Of_Points  [Axis] = X.Left_Number_Of_Points  [Axis];
     Right_Number_Of_Points [Axis] = X.Right_Number_Of_Points [Axis];

  // There may be some redundent initialization here
     InternalGhostCellWidth [Axis] = X.InternalGhostCellWidth [Axis];
     ExternalGhostCellWidth [Axis] = X.ExternalGhostCellWidth [Axis];
#endif
   }

// ************************************************************************************
// ************************************************************************************
//             General Initialization of descriptors 
// ************************************************************************************
// ************************************************************************************
void
Array_Domain_Type::Preinitialize_Domain ( const Array_Domain_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)    
          printf ("Inside of Array_Domain_Type::Preinitialize_Domain ( const Array_Domain_Type & X )\n");
#endif

  // Take this out soon!
  // The typeCode records the element type associated with the array for which
  // we use this domain object.  This is used only for internal diagnostics.
  // The value 'u' makes the element type as undefined.  This mechanism might be
  // replaced by an alternative mechanism in the future.
  // setTypeCode(APP_UNDEFINED_ELEMENT_TYPE);
     typeCode = APP_UNDEFINED_ELEMENT_TYPE;

  // Make sure that only the values of TRUE and FALSE (1 and 0) are used
     APP_ASSERT ( (X.Is_A_Null_Array == TRUE) || (X.Is_A_Null_Array == FALSE) );
     Is_A_Null_Array    = X.Is_A_Null_Array;
   }

// ************************************************************************************
// ************************************************************************************
//             General Initialization of descriptors 
// ************************************************************************************
// ************************************************************************************
void
Array_Domain_Type::Initialize_IndirectAddressing_Domain_Using_IndirectAddressing(
   const Array_Domain_Type & X , 
   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Inside of Array_Domain_Type::Initialize_IndirectAddressing_Domain_Using_IndirectAddressing ( const Array_Domain_Type & X , ALL PARMETERS )\n");
#endif

     printf ("Inside of Array_Domain_Type::Initialize_IndirectAddressing_Domain_Using_IndirectAddressing \n");
     printf ("Sorry, not implemented: Indexing of views built from indirect addressing temporarily disabled \n");
     APP_ABORT();

     int i = 0;
     intArray* intArrayList[MAX_ARRAY_DIMENSION];
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List;
     Internal_Index* Index_List[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Index_List   [i] = NULL;
          intArrayList [i] = NULL;

// if defined(USE_EXPRESSION_TEMPLATES)
          IndexBase[i]        = 0;
          IndexStride[i]      = 0;
          IndexDataPointer[i] = NULL;
// endif
        }

  // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
  // objects can be either an intArray or a scalar (integer) so we have to process the list.
  // Or it can be an Internal_Index (added (3/15/96)).
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Indirect_Index_List[i] != NULL)
             {
               if (Indirect_Index_List[i]->intArrayInStorage)
                  {
                    intArrayList[i] = Indirect_Index_List[i]->IndirectionArrayPointer;
                  }
                 else
                  {
                    if (Indirect_Index_List[i]->IndexInStorage)
                       {
                      // printf ("Case of Index and intArray indexing not handled yet! \n");
                      // APP_ABORT();
                         Index_List[i] = &(Indirect_Index_List[i]->Index_Data);
                       }
                      else
                       {
                         Integer_List[i] = Indirect_Index_List[i]->Scalar;
                       }
                  }
             }
        }

     APP_ASSERT (X.Uses_Indirect_Addressing == TRUE);

  // **********************************************************************
  // Case of intArray indexing of an A++ object (a view) already indexed 
  // by indirect addressing.
  // **********************************************************************
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > FILE_LEVEL_DEBUG)    
               printf ("X.Uses_Indirect_Addressing == TRUE (i=%d) \n",i);
#endif
       // Case of Target being an array previously indexed using indirect addressing
          if (X.Index_Array [i] != NULL)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > FILE_LEVEL_DEBUG)    
                    printf ("   X.Index_Array [i] is a valid pointer = %p \n",X.Index_Array [i]);
#endif
            // Index_Array [i] = new intArray;
               if ( Domain_Dimension == 1 )
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > FILE_LEVEL_DEBUG)    
                         printf ("Domain_Dimension == 1! \n");
#endif

                 // Only the first intArray is meaningful here since it is reused 
                 // to INDEX the X.Index_Array in each dimension.
                    if (intArrayList[0] != NULL)
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > FILE_LEVEL_DEBUG)    
                              printf ("intArrayList[0] is a valid pointer! \n");
#endif
                      // Bug fix (4/17/94) Must force a copy since recursive views 
                      // of indirect addressing can not be readily supported!
                      // actually we only want the size of the Parm0_intArray!
                      // the copying is redundent and inefficient
                         Index_Array[i] = new intArray (*intArrayList[0]);
                         *Index_Array[i] = (*X.Index_Array[i]) (*intArrayList[0]);
                       }
                      else
                       {
                         if (Index_List[0] != NULL)
                            {
                              Index_Array [i] = new intArray;
                              Index_Array [i]->reference ( (*(X.Index_Array[i])) (*(Index_List[0])) );
                            }
                           else
                            {
                           // printf ("ERROR: Not a valid dimension accessed in Initialize Domain! \n");
                           // APP_ABORT();
                              Index_Array [i] = new intArray;
                              Index_Array [i]->reference ( (*(X.Index_Array[i])) (Integer_List[0]) );
                            }
                       }
                  }
#if COMPILE_DEBUG_STATEMENTS
               if ( Domain_Dimension > 1 )
                  {
                    printf ("SORRY not implemented (view of indirect addressing view (Domain_Dimension > 1))! \n");
                    APP_ABORT();
                  }
#endif
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > FILE_LEVEL_DEBUG)    
                    printf ("   X.Index_Array [i] is NULL = %p \n",X.Index_Array [i]);
#endif
               Index_Array [i] = NULL;
             }
        }
   }

// ************************************************************************************
// ************************************************************************************
//             General Initialization of descriptors 
// ************************************************************************************
// ************************************************************************************
void
Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing (
   const Array_Domain_Type & X ,
   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
   {
  // This function only needs to handle the case of the intArrays not the Index or Scalar indexing
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Inside of Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing ( const Array_Domain_Type & X , Indirect_Index_List )\n");
#endif

     int i = 0;
     intArray* intArrayList[MAX_ARRAY_DIMENSION];
     Internal_Index* Index_List[MAX_ARRAY_DIMENSION];
     for (i=0; i<MAX_ARRAY_DIMENSION; i++)
        {
          intArrayList [i] = NULL;
          Index_List   [i] = NULL;

// if defined(USE_EXPRESSION_TEMPLATES)
          IndexBase[i]        = 0;
          IndexStride[i]      = 0;
          IndexDataPointer[i] = NULL;
// endif
        }

  // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
  // objects can be either an intArray or a scalar (integer) so we have to process the list
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Indirect_Index_List[i] != NULL)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > FILE_LEVEL_DEBUG)    
                    printf ("Indirect_Index_List[%d] != NULL \n",i);
#endif
               if (Indirect_Index_List[i]->intArrayInStorage)
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("Indirect_Index_List[i]->intArrayInStorage == TRUE \n");
                    APP_ASSERT (Indirect_Index_List[i]->IndirectionArrayPointer != NULL);
                 // Test the integer array object being used as an indirection vector!
                    Indirect_Index_List[i]->IndirectionArrayPointer->Test_Consistency
                         ("Inside of Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing(Array_Domain_Type,Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type)");
#endif

                    intArrayList[i] = Indirect_Index_List[i]->IndirectionArrayPointer;
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("Indirect_Index_List[i]->intArrayInStorage == FALSE \n");
#endif
                    if (Indirect_Index_List[i]->IndexInStorage)
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                              printf ("Indirect_Index_List[i]->IndexInStorage == FALSE \n");
#endif
                         Index_List [i] = &(Indirect_Index_List[i]->Index_Data);
                       }
                      else
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                              printf ("Indirect_Index_List[i]->Scalar = %d \n",Indirect_Index_List[i]->Scalar);
#endif
                      // We don't have to worry about this case since it has 
                      // nothing to do with indirect addressing (I think)
                      // This is a REACHABLE case!
                      // Integer_List[i] = Indirect_Index_List[i]->Scalar;
                      // printf ("This case should be unreachable in Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing() \n");
                      // APP_ABORT();
                       }
                  }
             }
            else
             {
            // printf ("Indirect_Index_List[%d] == NULL \n",i);
             }
        }

     APP_ASSERT (X.Uses_Indirect_Addressing == FALSE);

     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          Index_Array [i] = NULL;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)    
          printf ("X.Uses_Indirect_Addressing == FALSE \n");
#endif

     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          APP_ASSERT ( (intArrayList[i] == NULL) || (Index_List [i] == NULL) );
          if (intArrayList[i] != NULL)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > FILE_LEVEL_DEBUG)    
                    printf ("Build intArrayList[%d] using intArray \n",i);
#endif
               if (intArrayList[i]->Array_Descriptor.Array_Domain.Uses_Indirect_Addressing)
                  {
                 // If the intArray being used for indirect addressing is an array which uses
                 // indirect addressing thenwe are forced to make a copy of the resulting array because
                 // the MDI layer can not handle such nested indirect addressing complexity.
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 2)
                         printf ("intArrayList[i]->Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE! \n");
#endif
                    APP_ASSERT (Index_Array [i] == NULL);
                    Index_Array [i] = new intArray (*(intArrayList[i]));
                    APP_ASSERT (Index_Array [i] != NULL);
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 2)
                         printf ("intArrayList[i]->Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == FALSE! \n");
#endif
                    intArrayList[i]->incrementReferenceCount();
                    APP_ASSERT (Index_Array [i] == NULL);
                    Index_Array [i] = intArrayList[i];
                    APP_ASSERT (Index_Array [i] != NULL);
                  }

#if COMPILE_DEBUG_STATEMENTS
               if (Diagnostic_Manager::getReferenceCountingReport() > 0)
                  {
                 // This mechanism outputs reports which allow us to trace the reference counts
                    Index_Array [i]->displayReferenceCounts("In Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing(): indirection vector (intArray)");
                  }
               Index_Array [i]->Test_Consistency ("Testing intArray indexing in Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing");
#endif

            // How take care of cases where the input indirection arrays where
            // temporaries (the results if expressions)
               APP_ASSERT (intArrayList[i] != NULL);
            // if (intArrayList[i]->Array_Descriptor.Array_Domain.Is_A_Temporary)
               if (intArrayList[i]->isTemporary())
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > FILE_LEVEL_DEBUG)
                         printf ("intArrayList[i]->isTemporary() == TRUE! \n");
#endif
                 // Added conventional mechanism for reference counting control
                 // operator delete no longer decriments the referenceCount.
                    intArrayList [i]->decrementReferenceCount();
                    if (intArrayList [i]->getReferenceCount() < intArray::getReferenceCountBase())
                         delete intArrayList[i];
                 // Bugfix (12/15/94) to allow processing of intArray_index variables
                 // APP_ASSERT(intArrayList[i]->Array_Domain != NULL);
                    intArrayList[i]->Array_Descriptor.Array_Domain.Is_A_Temporary = FALSE;
#if defined(PPP)
                    APP_ASSERT(intArrayList[i]->Array_Descriptor.SerialArray != NULL);
                 // APP_ASSERT(intArrayList[i]->Array_Descriptor.SerialArray->
                 //            Array_Descriptor.Array_Domain != NULL);
                    intArrayList[i]->Array_Descriptor.SerialArray->
		       Array_Descriptor.Array_Domain.Is_A_Temporary = FALSE;
#endif
                  }

#if COMPILE_DEBUG_STATEMENTS
               if (Diagnostic_Manager::getReferenceCountingReport() > 0)
                  {
                 // This mechanism outputs reports which allow us to trace the reference counts
                    Index_Array [i]->displayReferenceCounts("Index_Array [i] in Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing");
                  }
#endif
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing ( const Array_Domain_Type & X , Indirect_Index_List )\n");
#endif
   }


Array_Domain_Type*
Array_Domain_Type::Vectorizing_Domain ( const Array_Domain_Type & X )
   {
  // Build a descriptor to fill in!
     Array_Domain_Type* Return_Domain = new Array_Domain_Type;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of Array_Domain_Type::Vectorizing_Domain ( const Array_Domain_Type & X ) (this = %p)\n",Return_Domain);
#endif

  // BTNG APP_ASSERT(MAX_ARRAY_DIMENSION <= 4);

#if defined(USE_PADRE) && defined(PPP)
#if !defined(TEST_USE_OF_PADRE)
  // Use the PADRE_Descriptor from the source of the view!
  // APP_ASSERT (X.parallelPADRE_DescriptorPointer != NULL);
  // parallelPADRE_DescriptorPointer = X.parallelPADRE_DescriptorPointer;
  // X.parallelPADRE_DescriptorPointer->incrementReferenceCount();
#endif
#endif

     if (X.Is_Contiguous_Data)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > FILE_LEVEL_DEBUG)
               printf ("X.Is_Contiguous_Data == TRUE \n");
#endif

          int i = 0;
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               Return_Domain->Size[i]          = X.Size[3];
               Return_Domain->Data_Base[i]     = APP_Global_Array_Base;
            // Return_Domain->Geometry_Base[i] = APP_Global_Array_Base;
               Return_Domain->Base[i]          = 0;
               Return_Domain->Bound[i]         = 0;
               Return_Domain->Stride[i]        = 1;
               Return_Domain->Index_Array[i]   = NULL;
#if defined(PPP)
               Return_Domain->Local_Index_Array[i] = NULL;
#endif
             }

       // Specific for 0th dimension
          Return_Domain->Bound[0] = X.Size[MAX_ARRAY_DIMENSION-1]-1;

          Return_Domain->Is_A_View                      = TRUE;
          Return_Domain->Is_A_Temporary                 = FALSE;
          Return_Domain->Is_Contiguous_Data             = TRUE;
          Return_Domain->Is_A_Null_Array                = FALSE;
          Return_Domain->Is_Built_By_Defered_Evaluation = FALSE;

       // Used in Initialize_Non_Indexed_Dimension but set in Initialize_Domain!
          Return_Domain->Uses_Indirect_Addressing = FALSE;
          Return_Domain->internalArrayID                 = X.internalArrayID;
          Return_Domain->referenceCount           = getReferenceCountBase();
          Return_Domain->Domain_Dimension         = 1;
          Return_Domain->Constant_Data_Base       = TRUE;
          Return_Domain->Constant_Unit_Stride     = TRUE;
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > FILE_LEVEL_DEBUG)
               printf ("X.Is_Contiguous_Data == FALSE \n");
#endif

          if (X.Uses_Indirect_Addressing)
             {
               printf ("\n");
               printf ("\n");
               printf ("*************************************************************** \n");
               printf ("ERROR: Sorry not implemented -- linearized view (vectorization) of indirect addressed view! \n");
               printf ("Views formed by indirect addressing are already 1 dimensional (so use of operator()() should not be required!) \n");
               printf ("*************************************************************** \n");
               APP_ABORT();
             }

          Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List_1;
          Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List_2;

          int Dimension [MAX_ARRAY_DIMENSION];
          Internal_Index Index_List [MAX_ARRAY_DIMENSION];
          Range          Range_List [MAX_ARRAY_DIMENSION];

          int Total_Size = 0;
          int i = 0;
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               Dimension[i] = X.getLength(i);
               Total_Size *= Dimension[i];

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > FILE_LEVEL_DEBUG)
                    printf ("Total_Size = %d  Dimension[i] = %d \n",Total_Size,Dimension[i]);
#endif

               Return_Domain->Size[i] = X.Size[i];
               if (X.Is_A_View)
                  {
                 // If indexed with a intArray we assume Geometry_Base [Axis] = 0
                 // We have to assume something and we can not afford to search for the
                 // minimum of the intArray
                    Return_Domain->Data_Base[i] = APP_Global_Array_Base;
                 // Return_Domain->Geometry_Base[i] = APP_Global_Array_Base;
                  }
                 else
                  {
                    Return_Domain->Data_Base[i] = X.Data_Base[i];
                 // Return_Domain->Geometry_Base[i] = X.Geometry_Base[i];
                  }

               Return_Domain->Base[i]        = 0;
               Return_Domain->Bound[i]       = 0;
               Return_Domain->Stride[i]      = X.Stride[i];
               Return_Domain->Index_Array[i] = NULL;

#if defined(PPP)
               Return_Domain->Local_Index_Array[i] = NULL;
#endif

            // Build Mask to be size of A++ object before the view was taken
               Range_List[i]          = Range (X.Data_Base[i],X.Data_Base[i]+(X.Size[i]-1));
            // Internal_Index_List_1[i] = &((Internal_Index) Range_List[i]);
               Internal_Index_List_1[i] = (Internal_Index*) &(Range_List[i]);

               Index_List[i] = Internal_Index (X.Base[0],Dimension[i],X.Stride[0]);
            // Internal_Index_List_2[i] = &((Internal_Index) Index_List[i]);
               Internal_Index_List_2[i] = (Internal_Index*) &(Index_List[i]);
             }

       // intArray Mask ( ((Range*)Internal_Index_List_1) );
          intArray Mask ( Internal_Index_List_1 );

       // Build Map into interior
          intArray *Map = new intArray (Total_Size);

          Return_Domain->Index_Array[0] = Map;
          Return_Domain->Bound[0]       = Total_Size - 1;

          Return_Domain->Is_A_View                      = TRUE;
          Return_Domain->Is_A_Temporary                 = FALSE;
          Return_Domain->Is_Contiguous_Data             = FALSE;
          Return_Domain->Is_A_Null_Array                = FALSE;
          Return_Domain->Is_Built_By_Defered_Evaluation = FALSE;

       // Used in Initialize_Non_Indexed_Dimension but set in Initialize_Domain!
          Return_Domain->Uses_Indirect_Addressing = TRUE;

          Return_Domain->internalArrayID = X.internalArrayID;

          Return_Domain->referenceCount = getReferenceCountBase();

          Return_Domain->Domain_Dimension = 1;
          Return_Domain->Constant_Data_Base   = FALSE;
          Return_Domain->Constant_Unit_Stride = FALSE;

       // This is a view so these should be the same.
          Return_Domain->builtUsingExistingData = X.builtUsingExistingData;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > FILE_LEVEL_DEBUG+5)
             {
               for (int j=0; j < MAX_ARRAY_DIMENSION; j++)
                  {
                    Range_List[j].display("Range");
                    Index_List[j].display("Index");
                  }
             }
#endif

       // Initialize the Mask
          Mask          = 0;

       // Set view of Mask to non-zero value (like 1)
       // Mask(I,J,K,L) = 1;
          Mask(Internal_Index_List_2) = 1;

#if !defined(USE_EXPRESSION_TEMPLATES)
       // Build intArray to access view where Mask is non-zero
          *Map = Mask.indexMap();
#else
          printf ("Inside of Array_Domain_Type::Vectorizing_Domain call to \n");
          printf ("indexMap commented out for expression template implementation \n");
          APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > FILE_LEVEL_DEBUG+5)
             {
               Mask.display("Mask");
               Map->display("Map");
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     Return_Domain->Test_Consistency ("Called from Array_Domain_Type::Vectorizing_Domain (Array_Domain_Type)");
#endif
     return Return_Domain;
   }


void
Array_Domain_Type::setBase ( int New_Base_For_All_Axes )
   {
#if COMPILE_DEBUG_STATEMENTS
  // Enforce bound on Alt_Base depending on INT_MAX and INT_MIN
     static int Max_Base = INT_MAX;
     static int Min_Base = INT_MIN;

  // error checking!
     APP_ASSERT((New_Base_For_All_Axes > Min_Base) && (New_Base_For_All_Axes < Max_Base));
#endif

  // NEW VERSION (copied from the orignal version in typeArray::setBase(int))
     int temp       = 0;  // iteration variable
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          APP_ASSERT (isView() || (Base[temp] == 0) );
          int Difference = New_Base_For_All_Axes - (Data_Base[temp] + Base[temp]);

          Data_Base [temp] += Difference;
          User_Base [temp] += Difference;

          if (temp==0)
               Scalar_Offset[0] = View_Offset - User_Base[0] * Stride[0];
            else
               Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Stride[temp] * Size[temp-1];

#if defined(PPP)
       // Now  the Global_Index and Local_Mask_Index Index objects
          Global_Index [temp]     += Difference;
          Local_Mask_Index [temp] += Difference;

       // printf ("getRawBase(temp) = %d \n",getRawBase(temp));
       // Global_Index [temp].display("Global_Index [temp]");
       // Local_Mask_Index [temp].display("Local_Mask_Index [temp]");

       // Error checking that works for only the 1 processor case
          if (Communication_Manager::Number_Of_Processors == 1)
             {
               APP_ASSERT (Global_Index [temp].getBase() == getRawBase(temp));
               APP_ASSERT (Local_Mask_Index [temp].getBase() == getRawBase(temp));
             }
#endif
        }
   }

void
Array_Domain_Type::setBase( int New_Base , int Axis )
   {
#if COMPILE_DEBUG_STATEMENTS
  // Enforce bound on Alt_Base depending on INT_MAX and INT_MIN
     static int Max_Base = INT_MAX;
     static int Min_Base = INT_MIN;

  // error checking!
     APP_ASSERT((New_Base > Min_Base) && (New_Base < Max_Base));
  // APP_ASSERT(Array_Descriptor != NULL);
#endif

  // This should help make sure the paramerters were input in the correct order
     APP_ASSERT (Axis >= 0);
     APP_ASSERT (Axis < MAX_ARRAY_DIMENSION);

     int i = 0;
     int Temp_Scalar_Offset = 0;
     int Difference = New_Base - (Data_Base[Axis] + Base[Axis]);

     Temp_Scalar_Offset = Difference * Stride[Axis];
     if (Axis > 0)
          Temp_Scalar_Offset *= Size[Axis-1];

     APP_ASSERT (isView() || (Base[Axis] == 0) );

     Data_Base [Axis] = New_Base - Base[Axis];
     User_Base [Axis] += Difference;

     for (i = Axis; i < MAX_ARRAY_DIMENSION; i++)
          Scalar_Offset[i] -= Temp_Scalar_Offset;

#if defined(PPP)
  // Now  the Global_Index and Local_Mask_Index Index objects
     Global_Index [Axis]     += Difference;
     Local_Mask_Index [Axis] += Difference;

  // Error checking specific to one processor only!
     if (Communication_Manager::Number_Of_Processors == 1)
        {
          APP_ASSERT (Global_Index [Axis].getBase() == getRawBase(Axis));
          APP_ASSERT (Local_Mask_Index [Axis].getBase() == getRawBase(Axis));
        }
#endif

#if 0
// I don't know why this is commented out (6/14/2000)
// though it seems that the reason is that data set by these macros are
// translation independent.
#if defined(PPP)
     SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
#else
     POINTER_LIST_INITIALIZATION_MACRO;
#endif
#endif
   }


#if defined(USE_EXPRESSION_TEMPLATES)
int
Array_Domain_Type::conformsTo ( const Array_Domain_Type & X ) const
   {
  // This function is not implemented 

     printf ("Sorry, not implemented! \n");
     APP_ABORT();

     return TRUE;
   }
#endif

#if defined(SERIAL_APP)
void
Array_Domain_Type::adjustSerialDomainForStridedAllocation ( int axis,  const Array_Domain_Type & globalDomain )
   {
  // This function is called by the Array_Descriptor_Type::Allocate_Parallel_Array() function
     APP_ASSERT( (globalDomain.Stride [axis] > 1) && (isNullArray() == FALSE) );

  // Use Range::operator()(Range) to help simplify the computation of the 
  // base and bound for the view formed when the stride is greater then 1.
     Range localSerialRange (getBase(axis),getBound(axis),getStride(axis));
     Range globalRange (globalDomain.getRawBase(axis),globalDomain.getRawBound(axis),globalDomain.getRawStride(axis));

#if 0
     localSerialRange.display("localSerialRange");
     globalRange.display("globalRange");
#endif

     APP_ASSERT (globalDomain.getRawStride(axis) == globalDomain.Stride [axis]);
     APP_ASSERT (globalRange.getMode()      == Index_Triplet);
     APP_ASSERT (localSerialRange.getMode() == Index_Triplet);

     Range intersectionRange = localSerialRange(globalRange);

     if (intersectionRange.getMode() == Null_Index)
        {
       // This is the case for the middle processor (when partitioned over 3 processors) when:
       //     <type>Array X(Range(6,8,2));
#if 0
          intersectionRange.display("NULL INDEX: intersectionRange");
#endif
       // Compute new values for Serial array domain (before assignment)
          int newStride   = 1;
          int newUserBase = Base[axis];
          int newDataBase = Data_Base[axis]; // Does not change (I think)
          int newBase     = Base[axis];
          int newBound    = newBase-1;
#if 0
          printf ("NULL INTERSECTION: Axis = %d newStride = %d newUserBase = %d newDataBase = %d newBase = %d newBound = %d \n",
               axis,newStride,newUserBase,newDataBase,newBase,newBound);
#endif
       // Set the stride to that of the global domain
          Stride    [axis] = newStride;
          User_Base [axis] = newUserBase;
          Base      [axis] = newBase;
          Bound     [axis] = newBound;
          Data_Base [axis] = newDataBase;

          Is_A_Null_Array = TRUE;
        }
       else
        {
          APP_ASSERT (intersectionRange.getMode() == Index_Triplet);
#if 0
          intersectionRange.display("VALID INDEX: intersectionRange");
#endif
       // Compute new values for Serial array domain (before assignment)
          int newStride   = globalDomain.Stride [axis];
       // int newUserBase = intersectionRange.getBase();
          int newUserBase = globalRange.getBase() + ((intersectionRange.getBase() - globalRange.getBase()) / globalRange.getStride());
          int newDataBase = getDataBase(axis); // Does not change (I think)
          int newBase     = intersectionRange.getBase()  - (localSerialRange.getBase() * localSerialRange.getStride());
          int newBound    = (intersectionRange.getBound() - intersectionRange.getBase()) + newBase;
#if 0
          printf ("VALID INTERSECTION: Axis = %d newStride = %d newUserBase = %d newDataBase = %d newBase = %d newBound = %d \n",
               axis,newStride,newUserBase,newDataBase,newBase,newBound);
#endif
       // Set the stride to that of the global domain
          Stride    [axis] = newStride;
       // User_Base [axis] = newUserBase * newStride;
          User_Base [axis] = newUserBase;
          Base      [axis] = newBase;
          Bound     [axis] = newBound;
          Data_Base [axis] = newDataBase;
        }

     int finalBase   = Base  [axis];
     int finalBound  = Bound [axis];
     int finalStride = Stride[axis];
#if 0
     printf ("Stride > 1: globalDomain.getBase(%d) = %d  getBase(%d) = %d  globalDomain.getBound(%d) = %d getBound(%d) = %d \n",
          axis,globalDomain.getBase(axis),axis,getBase(axis),axis,globalDomain.getBound(axis),axis,getBound(axis));
     printf ("Base[%d] = %d Bound[%d] = %d Data_Base[%d] = %d User_Base[%d] = %d Stride[%d] = %d \n",
          axis,Base [axis],axis,Bound[axis],axis,Data_Base[axis],axis,User_Base[axis],axis,Stride[axis]);
     printf ("globalDomain: Base[%d] = %d Bound[%d] = %d Data_Base[%d] = %d User_Base[%d] = %d Stride[%d] = %d \n",
          axis,globalDomain.Base [axis],axis,globalDomain.Bound[axis],axis,globalDomain.Data_Base[axis],
          axis,globalDomain.User_Base[axis],axis,globalDomain.Stride[axis]);
     printf ("Axis %d finalBase = %d finalBound = %d finalStride = %d \n",
          axis,finalBase,finalBound,finalStride);
#endif
     int finalLengthUnitStride    = (finalBound - finalBase) + 1;
     int finalLengthNonUnitStride = ((finalBound - finalBase) / finalStride) + 1;
#if 0
     printf ("Axis %d finalLengthUnitStride = %d finalLengthNonUnitStride = %d \n",
          axis,finalLengthUnitStride,finalLengthNonUnitStride);
#endif

  // if this is a strided array then this must be false
     Constant_Unit_Stride = FALSE;
  // globalDomain.display("globalDomain in Array_Domain_Type::adjustSerialDomainForStridedAllocation");
     APP_ASSERT (Constant_Unit_Stride == globalDomain.Constant_Unit_Stride);

     if (axis == 0)
        {
          View_Offset = 0;
          Scalar_Offset[axis] = -User_Base[axis] * Stride[axis];
        }
       else
        {
          Scalar_Offset[axis] = Scalar_Offset[axis-1] - User_Base[axis] * Stride[axis] * Size[axis-1];
        }

#if 0
     printf ("Reset Scalar_Offset[%d] = %d \n",axis,Scalar_Offset[axis]);
#endif

     if( Communication_Manager::numberOfProcessors() == 1 )
        {
          APP_ASSERT (globalDomain.Global_Index[axis].getLength() == finalLengthNonUnitStride);
        }
   }
#endif

#if 0
int
Array_Domain_Type::getUnitStrideBound ( int j )
   {
     int tempLocalBound = 0;
     if (Is_A_Left_Partition == TRUE)
          tempLocalBound = getLocalBound(j);
       else
          tempLocalBound = getBase(j) + (getBound(j) - getBase(j)) * Stride[j];

     return tempLocalBound;
   }
#endif


#if defined(PPP)

#if defined(PPP)
bool
Array_Domain_Type::isLeftNullArray( SerialArray_Domain_Type & serialArrayDomain , int Axis ) const
   {
  // A Null array can be part of the left edge or the right edge of a distributed axis
  // This function is helpful in determining which it is.

     int result = FALSE;

#if defined(USE_PADRE)
     APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     result = parallelPADRE_DescriptorPointer->isLeftNullArray(serialArrayDomain,Axis);
#else
  // printf ("In isLeftNullArray(): Axis = %d \n",Axis);

     if ( serialArrayDomain.isNullArray() == TRUE )
        {
       // printf ("In Array_Domain::isLeftNullArray(axis): Domain_Dimension = %d \n",Domain_Dimension);
          if ( Axis < Domain_Dimension)
             {
               APP_ASSERT(BlockPartiArrayDomain != NULL);
            // printf ("     gUBnd(BlockPartiArrayDomain,Axis) = %d \n",gUBnd(BlockPartiArrayDomain,Axis));
            // printf ("     lalbnd(BlockPartiArrayDomain,Axis,Base[Axis],1) = %d \n",lalbnd(BlockPartiArrayDomain,Axis,Base[Axis],1));

               result = (gUBnd(BlockPartiArrayDomain,Axis) < lalbnd(BlockPartiArrayDomain,Axis,Base[Axis],1));
             }
            else
             {
            // Along higher axes we want to force isLeftPartition == TRUE
               result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif

     return result;
   }

bool
Array_Domain_Type::isRightNullArray( SerialArray_Domain_Type & serialArrayDomain , int Axis ) const
   {
  // A Null array can be part of the left edge or the right edge of a distributed axis
  // This function is helpful in determining which it is.

     int result = FALSE;

#if defined(USE_PADRE)
     APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
     result = parallelPADRE_DescriptorPointer->isRightNullArray(serialArrayDomain,Axis);
#else
  // printf ("In isRightNullArray(): Axis = %d \n",Axis);
     if ( serialArrayDomain.isNullArray() == TRUE )
        {
          if ( Axis < Domain_Dimension)
             {
               result = !isLeftNullArray(serialArrayDomain,Axis);
             }
            else
             {
            // Along higher axes we want to force isRightPartition == TRUE
               result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif

     return result;
   }
#endif


void
Array_Domain_Type::postAllocationSupport ( SerialArray_Domain_Type & serialArrayDomain )
   {
  // Support function for allocation of P++ array objects
  // This function modifies the serialArray descriptor to 
  // make it a proper view of the serial array excluding 
  // the ghost boundaries

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Domain_Type::postAllocationSupport ( SerialArray_Domain_Type & X ) (this = %p)\n",this);
#endif

     bool Generated_A_Null_Array              = FALSE;
     int SerialArray_Cannot_Have_Contiguous_Data = FALSE;

  // Need to set the upper bound on the iteration to MAX_ARRAY_DIMENSION
  // since all the variables for each dimension must be initialized.
     int j;
     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
#if 0
          printf ("(after setBase): getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
               j,serialArrayDomain.getBase(j),j,getBase(j),j,serialArrayDomain.getBound(j),j,getBound(j));
	  printf ("(after setBase): SerialArray->getRawBase(%d) = %d  getRawBase(%d) = %d  SerialArray->getRawBound(%d) = %d getRawBound(%d) = %d \n",
		  j,serialArrayDomain.getRawBase(j),j,getRawBase(j),j,serialArrayDomain.getRawBound(j),j,getRawBound(j));
#endif

       // If this is not true then things get screwed up
       // APP_ASSERT (serialArrayDomain.getBound(j) >= serialArrayDomain.getBase(j));

       // This function is used tohide the use of A++/P++ specific range objects and 
       // provide an interface that both the PADRE and nonPADRE versions can call.
       // void adjustSerialDomainForStridedAllocation ( int axis,  const Array_Domain_Type & globalDomain );
          if ( (Stride [j] > 1) && (serialArrayDomain.isNullArray() == FALSE) )
             {
            // printf ("##### Calling Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");
               serialArrayDomain.adjustSerialDomainForStridedAllocation(j,*this);
            // printf ("##### DONE: Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");
             }

       // This could be implemented without the function call overhead
       // represented here but this simplifies the initial implementation for now

       // The left side uses the bases which are relative to a unit stride (a relatively simple case)!
          int Left_Size_Size  = (serialArrayDomain.getBase(j) - getBase(j)) * Stride[j];

#if 0
	  printf ("In postAllocationSupport: getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
		  j,serialArrayDomain.getBase(j),j,getBase(j),j,serialArrayDomain.getBound(j),j,getBound(j));
	  printf ("In postAllocationSupport: getLocalStride(%d) = %d  getStride(%d) = %d \n",
		  j,serialArrayDomain.getStride(j),j,getStride(j));

	  printf ("In postAllocationSupport: serialArrayDomain.getRawBase(%d) = %d  getRawBase(%d) = %d  serialArrayDomain.getRawBound(%d) = %d getRawBound(%d) = %d \n",
		  j,serialArrayDomain.getRawBase(j),j,getRawBase(j),j,serialArrayDomain.getRawBound(j),j,getRawBound(j));
	  printf ("In postAllocationSupport: serialArrayDomain.getRawStride(%d) = %d  getRawStride(%d) = %d \n",
		  j,serialArrayDomain.getRawStride(j),j,getRawStride(j));

          printf ("In postAllocationSupport: getBase(%d) = %d getLocalBase(%d) = %d getLocalBound(%d) = %d Array_Domain.Stride[%d] = %d \n",
               j,getBase(j),j,serialArrayDomain.getBase(j),j,serialArrayDomain.getBound(j),j,Stride[j]);
#endif

          int tempGlobalBound_A = getBase(j) + (getBound(j) - getBase(j)) * Stride[j];

       // int tempLocalUnitStrideBase = getBase(j) + (serialArrayDomain.getBase(j) - getBase(j)) * Stride[j];

       // This variable is set in code just below
          int Right_Size_Size = 0;

#if 0
          printf ("tempGlobalBound_A       = %d \n",tempGlobalBound_A);
#endif

       // The bound is the funky case since the bases are relative to stride 1 and 
       // the bounds are relative to stride.

       // int tempLocalBound = tempLocalUnitStrideBase + (serialArrayDomain.getBound(j) - serialArrayDomain.getBase(j)) * Stride[j];

          if(serialArrayDomain.isNullArray() == TRUE)
             {
#if 0
               printf ("found a null array (locally) so reset tempLocalBound to getLocalBase(%d) = %d \n",j,serialArrayDomain.getBase(j));
#endif

            // This variable is set in code just below
               int tempLocalBound = 0;

               int nullArrayOnLeftEdge = isLeftNullArray(serialArrayDomain,j);

               if (nullArrayOnLeftEdge == TRUE)
                  {
#if 0
                    printf ("nullArrayOnLeftEdge == TRUE --- Set tempLocalBound to base! \n");
#endif
                 // tempLocalBound  = getLocalBase(j);
                    tempLocalBound  = getBase(j);
                 // gUBnd(Array_Domain.BlockPartiArrayDomain,j);
	       
                  }
                 else
                  {
#if 0
                    printf ("nullArrayOnLeftEdge == FALSE --- Set tempLocalBound to bound! \n");
#endif
                 // tempLocalBound  = getBound(j);
                 // tempLocalBound  = getBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
                    tempLocalBound  = getBound(j);
                  }

               int rightEdgeSize = tempGlobalBound_A - tempLocalBound;
#if 0
               printf ("tempGlobalBound_A = %d tempLocalBound = %d rightEdgeSize = %d \n",
                    tempGlobalBound_A,tempLocalBound,rightEdgeSize);
            // printf ("gUBnd(Array_Domain.BlockPartiArrayDomain,j) = %d \n",gUBnd(Array_Domain.BlockPartiArrayDomain,j));
#endif

               Right_Size_Size = (rightEdgeSize == 0) ? 0 : rightEdgeSize + 1;
             }
            else
             {
#if 0
               printf ("NOT A NULL ARRAY (locally) \n");
#endif
            // If this is not true then things get screwed up
               APP_ASSERT (serialArrayDomain.getBound(j) >= serialArrayDomain.getBase(j));

               int tempLocalUnitStrideBase = getBase(j) + (serialArrayDomain.getBase(j) - getBase(j)) * Stride[j];
               int tempLocalBound = tempLocalUnitStrideBase + (serialArrayDomain.getBound(j) - serialArrayDomain.getBase(j)) * Stride[j];
               Right_Size_Size = (tempGlobalBound_A - tempLocalBound);

#if 0
               printf ("tempLocalUnitStrideBase = %d \n",tempLocalUnitStrideBase);
               printf ("tempGlobalBound_A = %d tempLocalBound = %d \n",tempGlobalBound_A,tempLocalBound);
#endif
             }

       // printf ("In postAllocationSupport: first instance: j = %d tempLocalBound = %d tempGlobalBound_A = %d \n",j,tempLocalBound,tempGlobalBound_A);

       // bugfix (8/15/2000) account for stride
       // int Right_Size_Size = (tempGlobalBound_A - tempLocalBound) / Array_Domain.Stride[j];
       // int Right_Size_Size = (tempGlobalBound_A - tempLocalBound);

       // Do these depend upon the stride in anyway?
          Left_Number_Of_Points [j] = (Left_Size_Size  >= 0) ? Left_Size_Size  : 0;
          Right_Number_Of_Points[j] = (Right_Size_Size >= 0) ? Right_Size_Size : 0;

#if 0
          printf ("In postAllocationSupport: Left_Size_Size = %d Right_Size_Size = %d \n",Left_Size_Size,Right_Size_Size);
       // printf ("In postAllocationSupport: tempLocalBound = %d tempGlobalBound_A = %d \n",tempLocalBound,tempGlobalBound_A);
	  printf ("In postAllocationSupport: Left_Number_Of_Points [%d] = %d Right_Number_Of_Points[%d] = %d \n",
               j,Left_Number_Of_Points [j],
               j,Right_Number_Of_Points[j]);
#endif

       // Comment out the Global Index since it should already be set properly
       // and this will just screw it up! We have to set it (I think)
#if 0
	  printf ("BEFORE RESET: Array_Domain.Global_Index [%d] = (%d,%d,%d,%s) \n",j,
               Global_Index [j].getBase(),
               Global_Index [j].getBound(),
               Global_Index [j].getStride(),
               Global_Index[j].getModeString());
#endif
       // Array_Domain.Global_Index [j] = Index (getBase(j),(getBound(j)-getBase(j))+1,1);
       // Use the range object instead
          Global_Index [j] = Range (getRawBase(j),getRawBound(j),getRawStride(j));
#if 0
	  printf ("AFTER RESET: Array_Domain.Global_Index [%d] = (%d,%d,%d,%s) \n",j,
               Global_Index [j].getBase(),
               Global_Index [j].getBound(),
               Global_Index [j].getStride(),
               Global_Index[j].getModeString());
#endif

          APP_ASSERT (Global_Index [j].getMode() != Null_Index);

          int ghostBoundaryWidth = InternalGhostCellWidth[j];
          APP_ASSERT (ghostBoundaryWidth >= 0);

       // If the data is not contiguous on the global level then it could not 
       // be locally contiguous (unless the partitioning was just right --but 
       // we don't worry about that case) if this processor is on the left or 
       // right and the ghost bundaries are of non-zero width
       // then the left and right processors can not have:
          serialArrayDomain.Is_Contiguous_Data = Is_Contiguous_Data;

       // Additionally if we have ghost boundaries then these are always hidden 
       // in a view and so the data can't be contiguous
          if (InternalGhostCellWidth [j] > 0)
               SerialArray_Cannot_Have_Contiguous_Data = TRUE;

       // In this later case since we have to modify the parallel domain object to be consistant
          if (SerialArray_Cannot_Have_Contiguous_Data == TRUE)
             {
            // Note: we are changing the parallel array descriptor (not done much in this function)
               Is_Contiguous_Data                               = FALSE;
               serialArrayDomain.Is_Contiguous_Data = FALSE;
             }

          int Local_Stride = serialArrayDomain.Stride [j];

          if (j < Domain_Dimension)
             {
            // Bugfix (17/10/96)
            // The Local_Mask_Index is set to NULL in the case of repartitioning
            // an array distributed first across one processor and then across 2 processors.
            // The second processor is a NullArray and then it has a valid local array (non nullarray).
            // But the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
            // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
            // setup.  The problem is that it is either not setup or sometimes setup incorrectly.
            // So we have to set it to a non-nullArray as a default.
               if (Local_Mask_Index[j].getMode() == Null_Index)
                  {
                 // This resets the Local_Mask_Index to be consistant with what it usually is going into 
                 // this function. This provides a definition of Local_Mask_Index consistant with
                 // the global range of the array. We cannot let the Local_Mask_Index be a Null_Index
                 // since it would not allow the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
                 // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
                 // setup.
                 // Array_Domain.Local_Mask_Index[j].display("INSIDE OF ALLOCATE PARALLEL ARRAY -- CASE OF NULL_INDEX");
                 // Array_Domain.Local_Mask_Index[j] = Range (Array_Domain.Base[j],Array_Domain.Bound[j]);
                    Local_Mask_Index[j] = Global_Index[j];
                  }

               APP_ASSERT (Local_Mask_Index[j].getMode() != Null_Index);
               APP_ASSERT (Local_Mask_Index[j].getMode() != All_Index);

               bool Is_A_Left_Partition   = isLeftPartition  (j);
               bool Is_A_Right_Partition  = isRightPartition (j);
               bool Is_A_Middle_Partition = isMiddlePartition(j);

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                  {
                    printf ("Is_A_NonPartition     = %s \n",(isNonPartition(j)     == TRUE) ? "TRUE" : "FALSE");
                    printf ("Is_A_Left_Partition   = %s \n",(Is_A_Left_Partition   == TRUE) ? "TRUE" : "FALSE");
                    printf ("Is_A_Right_Partition  = %s \n",(Is_A_Right_Partition  == TRUE) ? "TRUE" : "FALSE");
                    printf ("Is_A_Middle_Partition = %s \n",(Is_A_Middle_Partition == TRUE) ? "TRUE" : "FALSE");
                  }
#endif

            // If the parallel array descriptor is a view then we have to
            // adjust the view we build on the local partition!
            // Bases are relative to unit stride 
               int base_offset  = getBase(j) - serialArrayDomain.getBase(j);

            // Bounds are relative to strides (non-unit strides)
            // int bound_offset = getLocalBound(j) - getBound(j);
            // Convert the bound to a unit stride bound so it can be compared to the value of getBound(j)
#if 0
               int tempLocalBound = 0;
               if (Is_A_Left_Partition == TRUE)
                    tempLocalBound = serialArrayDomain.getBound(j);
                 else
                    tempLocalBound = serialArrayDomain.getBase(j) + (serialArrayDomain.getBound(j) - serialArrayDomain.getBase(j)) * Stride[j];
#else
   //          int tempLocalBound  = getBase(j) + (serialArrayDomain.getBound(j) - getBase(j)) * Stride[j];
   //          int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Stride[j];

            // I think these are already computed above!
               int tempLocalBound = serialArrayDomain.getBase(j) + (serialArrayDomain.getBound(j) - serialArrayDomain.getBase(j)) * Stride[j];
               int tempGlobalBound_B = getBase(j) + (getBound(j) - getBase(j)) * Stride[j];

            // printf ("In allocate.C: 2nd instance: j = %d tempLocalBound = %d tempGlobalBound_B = %d \n",j,tempLocalBound,tempGlobalBound_B);
#endif

            // Now that we have a unit stride bound we can do the subtraction
               int bound_offset = tempLocalBound - tempGlobalBound_B;

               int Original_Base_Offset_Of_View  = (Is_A_View) ? (base_offset>0 ? base_offset : 0) : 0;
               int Original_Bound_Offset_Of_View = (Is_A_View) ? (bound_offset>0 ? bound_offset : 0) : 0;

            // Need to account for the stride
               int Count = ((serialArrayDomain.getBound(j) - serialArrayDomain.getBase(j))+1) -
                           (Original_Base_Offset_Of_View + Original_Bound_Offset_Of_View);
            // Only could the left end once in adjusting for stride!
            // Count = 1 + (Count-1) * Array_Domain.Stride[j];

#if 0
               printf ("In postAllocationSupport: Excluding ghost boundaries axis = %d Count = %d ghostBoundaryWidth = %d \n",
                    j,Count,ghostBoundaryWidth);
#endif

#if 0
               printf ("Is_A_View = %s \n",(Is_A_View == TRUE) ? "TRUE" : "FALSE");

               printf ("(compute Count): getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
               j,serialArrayDomain.getBase(j),j,getBase(j),j,serialArrayDomain.getBound(j),j,getBound(j));

	       printf ("base_offset = %d  tempGlobalBound_B = %d tempLocalBound = %d bound_offset = %d \n",
                    base_offset,tempGlobalBound_B,tempLocalBound,bound_offset);
               printf ("SerialArray->Array_Descriptor.Array_Domain.getBase(%d) = %d \n",
                  j,serialArrayDomain.getBase(j));
               printf ("SerialArray->Array_Descriptor.Array_Domain.getBound(%d) = %d \n",
                  j,serialArrayDomain.getBound(j));
               printf ("Original_Base_Offset_Of_View  = %d \n",Original_Base_Offset_Of_View);
               printf ("Original_Bound_Offset_Of_View = %d \n",Original_Bound_Offset_Of_View);
               printf ("Initial Count = %d \n",Count);
#endif

            /*
            // ... bug fix, 4/8/96, kdb, at the end the local base and
            // bound won't include ghost cells on the left and right
            // processors respectively so adjust Count ...
            */

               int Start_Offset = 0;

	       if (!Is_A_View)
                  {
                 /*
                 // if this is a view, the ghost boundary cells are
                 // already removed by Original_Base_Offset_Of_View and
                 // Original_Bound_Offset_Of_View
                 */
                    if (Is_A_Left_Partition)
                       {
                         Count        -= ghostBoundaryWidth;
                         Start_Offset += ghostBoundaryWidth;
                       }

                    if (Is_A_Right_Partition)
                         Count -= ghostBoundaryWidth;
                  }

#if 0
               printf ("In postAllocationSupport: Final Count = %d \n",Count);
#endif

               if (Count > 0)
                  {
                 // This should have already been set properly (so why reset it?)
                 // Actually it is not set properly all the time at least so we have to reset it.
#if 0
                    printf ("BEFORE 1st RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Local_Mask_Index [j].getBase(),
                         Local_Mask_Index [j].getBound(),
                         Local_Mask_Index [j].getStride());
#endif
                 // This should be relative to stride 1 base/bound data since the local mask is relative to the local data directly (unit stride)
                 // Array_Domain.Local_Mask_Index[j] = Index(getLocalBase(j) + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);
		    int tempLocalBase = getBase(j) + (serialArrayDomain.getBase(j) - getBase(j)) * Local_Stride;
                    Local_Mask_Index[j] = Index(tempLocalBase + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);

#if 0
                    printf ("AFTER 1st RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Local_Mask_Index [j].getBase(),
                         Local_Mask_Index [j].getBound(),
                         Local_Mask_Index [j].getStride());
#endif

                 // Make the SerialArray a view of the valid portion of the local partition
                    serialArrayDomain.Is_A_View = TRUE;

                 /*
                 // ... (bug fix, 5/21/96,kdb) bases and bounds need to
                 // be adjusted by Original_Base_Offset_Of_View and
                 // Original_Bound_Offset_Of_View no matter what processor ...
                 */

                    serialArrayDomain.Base [j] += Original_Base_Offset_Of_View;
                    serialArrayDomain.Bound[j] -= Original_Bound_Offset_Of_View;
                 // ... bug fix (8/26/96, kdb) User_Base must reflect the view ...
                    serialArrayDomain.User_Base[j] += Original_Base_Offset_Of_View;

                    if (!Is_A_View)
                       {
                         if (Is_A_Left_Partition)
                            {
                              serialArrayDomain.Base[j] += ghostBoundaryWidth;
                           // ... bug fix (8/26/96, kdb) User_Base must reflect the ghost cell ...
                              serialArrayDomain.User_Base[j] += ghostBoundaryWidth;
                            }

                         if (Is_A_Right_Partition)
                              serialArrayDomain.Bound[j] -= ghostBoundaryWidth;
                       }

                 // Bugfix (10/19/95) if we modify the base and bound in the 
                 // descriptor then the data is no longer contiguous
                 // meaning that it is no longer binary conformable
                    if (ghostBoundaryWidth > 0)
                         serialArrayDomain.Is_Contiguous_Data = FALSE;
                  }
                 else
                  {
                    Generated_A_Null_Array = TRUE;
                  }
             }
            else // j >= Domain_Dimension
             {
               int Count = (serialArrayDomain.getBound(j) - serialArrayDomain.getBase(j)) + 1;
               if (Count > 0)
                  {
#if 0
                    printf ("BEFORE 2nd RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Local_Mask_Index [j].getBase(),
                         Local_Mask_Index [j].getBound(),
                         Local_Mask_Index [j].getStride());
#endif
                 // (6/28/2000) part of fix to permit allocation of strided array data
                 // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,1);
                 // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,Local_Stride);
		    int tempLocalBase = getBase(j) + (serialArrayDomain.getBase(j) - getBase(j)) * Local_Stride;
                    Local_Mask_Index[j] = Index(tempLocalBase, Count, Local_Stride);

#if 0
                    printf ("AFTER 2nd RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Local_Mask_Index [j].getBase(),
                         Local_Mask_Index [j].getBound(),
                         Local_Mask_Index [j].getStride());
#endif
                  }
                 else
                  {
                 // build a null internalIndex for this case
                    Local_Mask_Index[j] = Index (0,0,1);
                  }
             }

       // ... add correct view offset and Scalar Offset ...
          if (j==0)
             {
            // bugfix (7/16/2000) The View_Offset is not computed with a stride!
               serialArrayDomain.View_Offset = serialArrayDomain.Base[0];
            // The Scalar_Offset is computed with a stride!
               serialArrayDomain.Scalar_Offset[0] = - serialArrayDomain.User_Base[0] * serialArrayDomain.Stride[0];
             }
            else
             {
            // The View_Offset is not computed with a stride!
               serialArrayDomain.View_Offset += serialArrayDomain.Base[j] * serialArrayDomain.Size[j-1];
               serialArrayDomain.Scalar_Offset[j] =
                    serialArrayDomain.Scalar_Offset[j-1] - serialArrayDomain.User_Base[j] * serialArrayDomain.Stride[j] * serialArrayDomain.Size[j-1];
             }

       // Error checking for same strides in parallel and serial array
          APP_ASSERT ( serialArrayDomain.isNullArray() || (Stride [j] == serialArrayDomain.Stride [j]) );

#if 0
          for (int temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               printf ("Check values: SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
                    temp,serialArrayDomain.Scalar_Offset[temp]);
             }
#endif

#if 0
       // New test (7/2/2000)
       // In general this test is not always valid (I think it was a mistake to add it - but
       // we might modify it in the future so I will leave it here for now (commented out))
       // This test is not working where the array on one processor is a NullArray (need to fix this test)
          if (serialArrayDomain.isNullArray() == FALSE)
             {
            // recompute the partition information for this axis
               bool Is_A_Left_Partition   = isLeftPartition  (j);
               bool Is_A_Right_Partition  = isRightPartition (j);
               bool Is_A_Middle_Partition = isMiddlePartition(j);

            // Error checking on left and right edges of the partition
               if (Is_A_Left_Partition)
                  {
                    if ( getRawBase(j) != serialArrayDomain.getRawBase(j) )
                       {
                      // display("ERROR: getRawBase(j) != SerialArray->getRawBase(j)");
		         printf ("getRawBase(%d) = %d  SerialArray->getRawBase(%d) = %d \n",
                              j,getRawBase(j),j,serialArrayDomain.getRawBase(j));
                       }
                    APP_ASSERT( getRawBase(j) == serialArrayDomain.getRawBase(j) );
                  }

               if (Is_A_Right_Partition)
                  {
                    if ( getRawBound(j) != serialArrayDomain.getRawBound(j) )
                       {
                      // display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
                      // SerialArray->Array_Descriptor.display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
		         printf ("getRawBound(%d) = %d  SerialArray->getRawBound(%d) = %d \n",
                              j,getRawBound(j),j,serialArrayDomain.getRawBound(j));
                       }
                    APP_ASSERT( getRawBound(j) == serialArrayDomain.getRawBound(j) );
                  }
             }
#endif
        } // end of j loop


     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
          if (j == 0)
             {
            // Start out true
               serialArrayDomain.Constant_Data_Base = TRUE;
#if 0
               printf ("Initial setting: SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = %s \n",
                    serialArrayDomain.Constant_Data_Base ? "TRUE" : "FALSE");
#endif
             }
            else
             {
            // Start out true
               serialArrayDomain.Constant_Data_Base = 
                    serialArrayDomain.Constant_Data_Base && (serialArrayDomain.Data_Base[j] ==  serialArrayDomain.Data_Base[j-1]);
#if 0
               printf ("axis = %d Constant_Data_Base = %s \n",
                    j,serialArrayDomain.Constant_Data_Base ? "TRUE" : "FALSE");
#endif
             }
        }

#if 0
     printf ("Final value: SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = %s \n",
          serialArrayDomain.Constant_Data_Base ? "TRUE" : "FALSE");

     printf ("SerialArray->Array_Descriptor.Array_Domain.View_Offset = %d \n",
          serialArrayDomain.View_Offset);
#endif

  // ... add View_Offset to Scalar_Offset (we can't do this inside the
  // loop above because View_Offset is a sum over all dimensions). Also
  // set View_Pointers now. ...
     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
          serialArrayDomain.Scalar_Offset[j] +=
               serialArrayDomain.View_Offset;
       // printf ("SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
       //      j,SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j]);
        }

  /* SERIAL_POINTER_LIST_INITIALIZATION_MACRO; */
  /* DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO; */

  // if we have generated a null view of a valid serial array then we have 
  // to make the descriptor conform to some specific rules
  // 1. A Null_Array has to have the Is_Contiguous_Data flag FALSE
  // 2. A Null_Array has to have the Base and Bound 0 and -1 (repectively)
  //    for ALL dimensions
  // 3. The Local_Mask_Index in the Parallel Descriptor must be a Null Index

     if (Generated_A_Null_Array == TRUE)
        {
       // We could call a function here to set the domain to represent a Null Array
          serialArrayDomain.Is_Contiguous_Data   = FALSE;
          serialArrayDomain.Is_A_View            = TRUE;
          serialArrayDomain.Is_A_Null_Array      = TRUE;

          serialArrayDomain.Constant_Unit_Stride = TRUE;

       // Call this specialized function for this purpose (it is used elsewhere as well)
          resetDomainForNullArray();

          int k;
          for (k=0; k < MAX_ARRAY_DIMENSION; k++)
             {
#if 0
            // (12/14/2000) set in resetDomainForNullArray()
            // Modify the parallel domain
               Local_Mask_Index           [k] = Index (0,0,1,Null_Index);

            // Added (12/14/2000)
               Left_Number_Of_Points      [k] = 0;
               Right_Number_Of_Points     [k] = 0;
#endif
            // Modify the serial domain
               serialArrayDomain.Base     [k] =  0;
               serialArrayDomain.Bound    [k] = -1;
               serialArrayDomain.User_Base[k] =  0;

            // (12/13/2000) Added test: set in resetDomainForNullArray()
               APP_ASSERT (getLeftNumberOfPoints(k) == 0);
               APP_ASSERT (getRightNumberOfPoints(k) == 0);
             }
        }
   }

#endif










int Array_Domain_Type::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

Array_Domain_Type* Array_Domain_Type::Current_Link                      = NULL;

int Array_Domain_Type::Memory_Block_Index                = 0;

const int Array_Domain_Type::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *Array_Domain_Type::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *Array_Domain_Type::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call Array_Domain_Type::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(Array_Domain_Type))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from Array_Domain_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In Array_Domain_Type::operator new: Calling malloc because Size(%d) != sizeof(Array_Domain_Type)(%d) \n",Size,sizeof(Array_Domain_Type));

          return malloc(Size);
        }
       else
        {
       // printf ("In Array_Domain_Type::operator new: Using the pool mechanism Size(%d) == sizeof(Array_Domain_Type)(%d) \n",Size,sizeof(Array_Domain_Type));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (Array_Domain_Type*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(Array_Domain_Type) );
#else
               Current_Link = (Array_Domain_Type*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(Array_Domain_Type) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     Array_Domain_Type* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from Array_Domain_Type::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void Array_Domain_Type::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In Array_Domain_Type::operator delete: Size(%d)  sizeof(Array_Domain_Type)(%d) \n",sizeOfObject,sizeof(Array_Domain_Type));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(Array_Domain_Type))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from Array_Domain_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In Array_Domain_Type::operator delete: Calling global delete (free) because Size(%d) != sizeof(Array_Domain_Type)(%d) \n",sizeOfObject,sizeof(Array_Domain_Type));
             }
#endif

          free(Pointer);
        }
       else
        {
          Array_Domain_Type *New_Link = (Array_Domain_Type*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In Array_Domain_Type::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(Array_Domain_Type)(%d) \n",Pointer,sizeOfObject,sizeof(Array_Domain_Type));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In Array_Domain_Type::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving Array_Domain_Type::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }






















