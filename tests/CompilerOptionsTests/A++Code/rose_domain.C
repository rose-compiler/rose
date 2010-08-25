// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU

#endif

#include "A++.h"


/* machine.h is found in MDI/machine.h through a link in A++/inc lude and P++/inc lude */
#include "machine.h"


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

#endif

// Used to support the Array_ID values! We start at 1 so that we can conside any
// value < 1 as a error! This also allows the idefs in the dependence analysis
// to be negative. This value is also used in the array objects destructor to
// trigger the cleanup of A++ internal memory in use. This prevents warnings from
// Purify.
#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
int APP_Global_Array_ID=(1); 
#else

#endif

// defined in array.C

















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
 int Array_Domain_Type::Array_Reference_Count_Array_Length=(0); 
int * Array_Domain_Type::Array_Reference_Count_Array=(((int * )0)); 
bool Array_Domain_Type::smartReleaseOfInternalMemory=(false); 
bool Array_Domain_Type::exitFromGlobalMemoryRelease=(false); 
#endif

                   // Support for stack operations in the Array_Domain_Type.
                   // Push and Pop member functions of Array_Domain_Type use these
                   // to implement the stack operations.  A seperate Stack class was first
                   // implemented and it could not be a static class because static 
                   // classes are initialized in a different order for each C++ compiler
                   // different compilers.
 int Array_Domain_Type::Max_Stack_Size=(0); 
 int Array_Domain_Type::Stack_Depth=(0); 
int * Array_Domain_Type::Top_Of_Stack=(((int * )0)); 
int * Array_Domain_Type::Bottom_Of_Stack=(((int * )0)); 

                   // we only want to declare these once
 int Array_Domain_Type::IndirectAddressingMap_Index=(-1); 
int * Array_Domain_Type::IndirectAddressingMap_Data[100]; 
 int Array_Domain_Type::IndirectAddressingMap_Minimum_Base[100]; 
 int Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[100]; 
 int Array_Domain_Type::IndirectAddressingMap_Length[100]; 

                    // *****************************************************************************
                    // *****************************************************************************
                    // **********************   MEMBER FUNCTION DEFINITION  ************************
                    // *****************************************************************************
                    // *****************************************************************************

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
AppString::~AppString()
   { 
     delete this -> internalString; 
} 

AppString::AppString()
   { 
     this -> internalString = (char * )0; 
} 

AppString::AppString(const char * c)
   { 
     if (!(c != (const char * )0)){ APP_Assertion_Support("domain.C",133); } 
#if defined(USE_STRING_SPECIFIC_CODE)
     this -> internalString = strdup(c); 
} 
#else


#endif


AppString::AppString(const AppString & X)
   { 
     if (!(X.internalString != (char * )0)){ APP_Assertion_Support("domain.C",144); } 
#if defined(USE_STRING_SPECIFIC_CODE)
     this -> internalString = strdup((X.internalString)); 
} 
#else


#endif



AppString & AppString::operator=(const AppString & X)
   { 
     delete this -> internalString; 
     if (!(X.internalString != (char * )0)){ APP_Assertion_Support("domain.C",157); } 
#if defined(USE_STRING_SPECIFIC_CODE)
     this -> internalString = strdup((X.internalString)); 
#else


#endif
     return *this;
} 


char * AppString::getInternalString() const
   { 
   // Our semantics require the user to delete strings obtained from getInternalString()!
     if (!(this -> internalString != (char * )0)){ APP_Assertion_Support("domain.C",171); } 
#if defined(USE_STRING_SPECIFIC_CODE)
     return strdup(this -> internalString);
} 
#else



#endif



void AppString::setInternalString(const char * c)
   { 
     if (!(c != (const char * )0)){ APP_Assertion_Support("domain.C",184); } 
#if defined(USE_STRING_SPECIFIC_CODE)
     this -> internalString = strdup(c); 
} 
#else


#endif

#endif



inline int * Array_Domain_Type::getDefaultIndexMapData(int Local_Min_Base,int Local_Max_Bound) const
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
          { printf(("At TOP: IndirectAddressingMap_Index = %d Local_Min_Base = %d  Local_Max_Bound = %d \n"),
          Array_Domain_Type::IndirectAddressingMap_Index,Local_Min_Base,Local_Max_Bound); } 
#endif
     if (Array_Domain_Type::IndirectAddressingMap_Index == -1)
        { 
        // Initial setup (this is called only once)
          Array_Domain_Type::IndirectAddressingMap_Index = 0; 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               { printf(("SETUP of Map pointers in Array_Domain_Type::getDefaultIndexMapData \n")); } 
#endif
          for (int i=0; i < 100; i++)
             { 
               Array_Domain_Type::IndirectAddressingMap_Data[i] = (int * )0; 
               Array_Domain_Type::IndirectAddressingMap_Minimum_Base[i] = 2147483647; 
               Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[i] = -1; 
               Array_Domain_Type::IndirectAddressingMap_Length[i] = 0; 
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

     int NewBase=(Local_Min_Base < Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index])?Local_Min_Base:(Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index]); 

     int NewBound=(Local_Max_Bound > Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index])?Local_Max_Bound:(Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index]); 

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("NewBase = %d  NewBound = %d \n"),NewBase,NewBound); } 
#endif
     if (NewBound - NewBase >= Array_Domain_Type::IndirectAddressingMap_Length[Array_Domain_Type::IndirectAddressingMap_Index])
        { 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               { printf(("Increment the IndirectAddressingMap_Index \n")); } 
#endif
          Array_Domain_Type::IndirectAddressingMap_Index++; 
          if (Array_Domain_Type::IndirectAddressingMap_Index == 100)
             { 
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    { printf(("Reset the IndirectAddressingMap_Index to ZERO! \n")); } 
#endif
               Array_Domain_Type::IndirectAddressingMap_Index = 0; 
          } 
     } 

               // if (Local_Min_Base  < IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index] )
               //      IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]  = Local_Min_Base;
               // if (Local_Max_Bound > IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index])
               //      IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index] = Local_Max_Bound;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        { 
          printf(("IndirectAddressingMap_Minimum_Base[%d]  = %d \n"),Array_Domain_Type::IndirectAddressingMap_Index,
          Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index]); 
          printf(("IndirectAddressingMap_Maximum_Bound[%d] = %d \n"),Array_Domain_Type::IndirectAddressingMap_Index,
          Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index]); 
          printf(("IndirectAddressingMap_Length[%d]        = %d \n"),Array_Domain_Type::IndirectAddressingMap_Index,
          Array_Domain_Type::IndirectAddressingMap_Length[Array_Domain_Type::IndirectAddressingMap_Index]); 
     } 
#endif

          // if (IndirectAddressingMap_Maximum_Bound[IndirectAddressingMap_Index] - 
          //     IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index] >= 
          //     IndirectAddressingMap_Length[IndirectAddressingMap_Index])
     if (NewBound - NewBase >= Array_Domain_Type::IndirectAddressingMap_Length[Array_Domain_Type::IndirectAddressingMap_Index])
        { 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               { printf(("Allocating new memory: IndirectAddressingMap_Index = %d \n"),
               Array_Domain_Type::IndirectAddressingMap_Index); } 
#endif

          if (Local_Min_Base < Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index])
               { Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index]
                = Local_Min_Base; } if (Local_Max_Bound > Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index])
               { Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index]
                = Local_Max_Bound; } 
          Array_Domain_Type::IndirectAddressingMap_Length[Array_Domain_Type::IndirectAddressingMap_Index]
           = (Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index]
           - Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index])
           + 1; 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             { 
               printf(("New value for IndirectAddressingMap_Minimum_Base[%d]  = %d \n"),
               Array_Domain_Type::IndirectAddressingMap_Index,Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index]); 
               printf(("New value for IndirectAddressingMap_Maximum_Bound[%d] = %d \n"),
               Array_Domain_Type::IndirectAddressingMap_Index,Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index]); 
               printf(("New value for IndirectAddressingMap_Length[%d] = %d \n"),Array_Domain_Type::IndirectAddressingMap_Index,
               Array_Domain_Type::IndirectAddressingMap_Length[Array_Domain_Type::IndirectAddressingMap_Index]); 
          } 
#endif

          if (Array_Domain_Type::IndirectAddressingMap_Data[Array_Domain_Type::IndirectAddressingMap_Index]
           == (int * )0){ Array_Domain_Type::IndirectAddressingMap_Data[Array_Domain_Type::IndirectAddressingMap_Index]
           = (int * )malloc((Array_Domain_Type::IndirectAddressingMap_Length[Array_Domain_Type::IndirectAddressingMap_Index]
           * 4)); 
          } 
          else { Array_Domain_Type::IndirectAddressingMap_Data[Array_Domain_Type::IndirectAddressingMap_Index]
           = (int * )realloc(((char * )Array_Domain_Type::IndirectAddressingMap_Data[Array_Domain_Type::IndirectAddressingMap_Index]),
          (Array_Domain_Type::IndirectAddressingMap_Length[Array_Domain_Type::IndirectAddressingMap_Index]
           * 4)); } 
               /* Make sure this vectorizes if possible! */
          for (i = Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index]; i
           <= Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index]; i++)
             { 
               (Array_Domain_Type::IndirectAddressingMap_Data[Array_Domain_Type::IndirectAddressingMap_Index])[(i
                - Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index])]
                = i; 
          } 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             { 
               printf(("Values in IndirectAddressingStaticMap: \n")); 
               for (i = Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index]; i
                <= Array_Domain_Type::IndirectAddressingMap_Maximum_Bound[Array_Domain_Type::IndirectAddressingMap_Index]; i++)
                  { 
                  // printf ("%d ",IndirectAddressingMap_Data[IndirectAddressingMap_Index][i]);
                  // printf ("%d ",i);
                    printf(("%d "),(Array_Domain_Type::IndirectAddressingMap_Data[Array_Domain_Type::IndirectAddressingMap_Index])[(i
                     - Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index])]); 
               } 
               printf(("\n")); 
          } 
     } 
#endif


               // Compute the address of the first position within the IndirectAddressingMap_Data that we require
               // return &(IndirectAddressingMap_Data[IndirectAddressingMap_Index]
               //                                    [-IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]]);
               // int* ReturnValue = &(IndirectAddressingMap_Data[IndirectAddressingMap_Index]
               //                                    [-IndirectAddressingMap_Minimum_Base[IndirectAddressingMap_Index]]);

               // Use pointer arithmetic to compute the offset (rather than indexing the position and taking the address)
               // This also avoids a warning from PURIFY
     int * ReturnValue=Array_Domain_Type::IndirectAddressingMap_Data[Array_Domain_Type::IndirectAddressingMap_Index]
      - Array_Domain_Type::IndirectAddressingMap_Minimum_Base[Array_Domain_Type::IndirectAddressingMap_Index]; 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("ReturnValue = %p \n"),ReturnValue); } 
#endif
     return ReturnValue;
} 


void Array_Domain_Type::setupIndirectAddressingSupport() const
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::setupIndirectAddressingSupport() \n")); } 
#endif

          // Test to see what sort of indexing is done here
          // We only really need to iterate over the range 0...InternalDimension
          // but we can do this later
          // We can make the indexing more efficient by modifying the
          // IndexDataPointer using the IndexBase.
     for (int i=0; i < 6; i++)
        { 
          if (((this -> usesIndirectAddressing)() == 1) && (this -> Index_Array)[i] != (intArray * )0)
             { 
             // For each axis check to see what type of indexing was used (since it could be
             // either intArray Internal_Index or Scalar)
             // An intArray was used for indexing this axis
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    { printf(("operand uses indirect addressing and uses intArray along axis %d \n"),
                    i); } 
#endif
               if (!((this -> Index_Array)[i] != (intArray * )0)){ APP_Assertion_Support("domain.C",
               393); 
               } 
               if (!(((*(this -> Index_Array)[i]).getDataPointer)() != (int * )0)){ APP_Assertion_Support("domain.C",
               394); 
               } 
               (((Array_Domain_Type & )(*this)).IndexBase)[i] = ((((*(this -> Index_Array)[i]).Array_Descriptor).Array_Domain).Base)[i]; 
               (((Array_Domain_Type & )(*this)).IndexStride)[i] = ((((*(this -> Index_Array)[i]).Array_Descriptor).Array_Domain).Stride)[i]; 
               (((Array_Domain_Type & )(*this)).IndexDataPointer)[i] = ((*(this -> Index_Array)[i]).getDataPointer)(); 
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                  { 
                    printf(("Test out the intArray used for indexing \n")); 
                    printf(("Base [%d] = %d \n"),i,(this -> Base)[i]); 
                    printf(("Bound[%d] = %d \n"),i,(this -> Bound)[i]); 
                    printf(("Index_Array[%d]->Array_Descriptor.Array_Domain.Base   [0] = %d \n"),
                    i,((((*(this -> Index_Array)[i]).Array_Descriptor).Array_Domain).Base)[0]); 
                    printf(("Index_Array[%d]->Array_Descriptor.Array_Domain.Stride   [0] = %d \n"),
                    i,((((*(this -> Index_Array)[i]).Array_Descriptor).Array_Domain).Stride)[i]); 
                    printf(("IndexDataPointer[%d] = %p \n"),i,(this -> IndexDataPointer)[i]); 
                    class intArray * IndexArrayPointer=((intArray * )0); 
                    for (int n=0; n < 6; n++)
                         { if ((this -> Index_Array)[n] != (intArray * )0)
                              { IndexArrayPointer = (this -> Index_Array)[n]; } 
                    } if (!(IndexArrayPointer != (intArray * )0)){ APP_Assertion_Support("domain.C",
                    413); 
                    } 
                    for (int j=(((IndexArrayPointer -> Array_Descriptor).Array_Domain).Base)[0]; j
                     <= (((IndexArrayPointer -> Array_Descriptor).Array_Domain).Bound)[0]; j
                     += (((IndexArrayPointer -> Array_Descriptor).Array_Domain).Stride)[0])
                       { printf(("%d "),((this -> IndexDataPointer)[i])[j]); } 
                    printf(("\n")); 
               } 
          } 
          else 
#endif

             { 
             // either an Internal_Index or a scalar was used for indexing this axis
             // An Internal_Index was used for indexing this axis
               if ((this -> Base)[i] == (this -> Bound)[i])
                  { 
                  // Then we have scalar indexing in use
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         { printf(("operand uses scalar indexing along axis %d \n"),i); } 
#endif
                    (((Array_Domain_Type & )(*this)).IndexBase)[i] = (this -> Data_Base)[i]
                     + (this -> Base)[i]; (((Array_Domain_Type & )(*this)).IndexStride)[i]
                     = 0; 
               } 
               else { 
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         { printf(("operand uses Internal_Index indexing along axis %d \n"),
                         i); } 
#endif
                    (((Array_Domain_Type & )(*this)).IndexBase)[i] = (this -> getBase)(i); 
                    (((Array_Domain_Type & )(*this)).IndexStride)[i] = (this -> getStride)(i); 
               } 

                    // Is this done correctly here?

                    // get space for IntegerList
               int localBase=(this -> IndexBase)[i]; 
               int localBound=(this -> IndexBase)[i] + ((this -> getBound)(i) - (this
                -> getBase)(i)); (((Array_Domain_Type & )(*this)).IndexDataPointer)[i]
                = (this -> getDefaultIndexMapData)(localBase,localBound); if (!((this
                -> IndexDataPointer)[i] != (int * )0)){ APP_Assertion_Support("domain.C",
               452); } 
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                  { 
                    printf(("Test out the range from getDefaultIndexMapData \n")); 
                    printf(("Base [%d] = %d \n"),i,(this -> Base)[i]); 
                    printf(("Bound[%d] = %d \n"),i,(this -> Bound)[i]); 
                    printf(("getBase  (%d) = %d \n"),i,this -> getBase(i)); 
                    printf(("getBound (%d) = %d \n"),i,this -> getBound(i)); 
                    printf(("getStride(%d) = %d \n"),i,this -> getStride(i)); 
                    printf(("IndexBase       [%d] = %d \n"),i,(this -> IndexBase)[i]); 
                    printf(("IndexStride     [%d] = %d \n"),i,(this -> IndexStride)[i]); 
                    printf(("IndexDataPointer[%d] = %p \n"),i,(this -> IndexDataPointer)[i]); 
                    for (int j=localBase; j <= localBound; j++)
                       { printf(("%d "),((this -> IndexDataPointer)[i])[j]); } 
                    printf(("\n")); 
               } 
          } 
     } 
} 
#endif




                    // *****************************************************************************
                    // *****************************************************************************
                    // *********  NEW OPERATOR INITIALIZATION MEMBER FUNCTION DEFINITION  **********
                    // *****************************************************************************
                    // *****************************************************************************


void Array_Domain_Type::New_Function_Loop()
   { 
   // Initialize the free list of pointers!
     for (int i=0; i < Array_Domain_Type::CLASS_ALLOCATION_POOL_SIZE - 1; i++)
        { 
          (Array_Domain_Type::Current_Link[i]).freepointer = Array_Domain_Type::Current_Link
           + (i + 1); 
     } 
} 
          // *****************************************************************************
          // *****************************************************************************
          // ****************  MEMORY CLEANUP MEMBER FUNCTION DEFINITION  ****************
          // *****************************************************************************
          // *****************************************************************************


void Array_Domain_Type::freeMemoryInUse()
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        { printf(("Inside of Array_Domain_Type::freeMemoryInUse() \n")); } 
#endif 

        // This function is useful in conjuction with the Purify (from Pure Software Inc.)
        // it frees memory allocated for use internally in A++ <type>Array objects.
        // This memory is used internally and is reported as "in use" by Purify
        // if it is not freed up using this function.  This function works with
        // similar functions for each A++ object to free up all of the A++ memory in
        // use internally.

#if !defined(PPP)
     if (Diagnostic_Manager::getTrackArrayData() == 1)
        { 
          printf(("Cleaning up the diagnosticInfoArray stuff! \n")); 
          if (Diagnostic_Manager::diagnosticInfoArray != (DiagnosticInfo * * )0)
             { 
               if (!(Array_Domain_Type::Array_Reference_Count_Array_Length > 0)){ APP_Assertion_Support("domain.C",
               517); 
               } 
               for (int i=0; i < Array_Domain_Type::Array_Reference_Count_Array_Length; i++)
                  { 
                    if (Diagnostic_Manager::diagnosticInfoArray[i] != (DiagnosticInfo * )0)
                         { :: delete Diagnostic_Manager::diagnosticInfoArray[i]; } 
                    Diagnostic_Manager::diagnosticInfoArray[i] = (DiagnosticInfo * )0; 
               } 
          } 
     } 

                    // free memory allocated for reference counting!
     if (Array_Domain_Type::Array_Reference_Count_Array != (int * )0)
          { free(((char * )Array_Domain_Type::Array_Reference_Count_Array)); } 

          // Reset the size of the reference count array
     Array_Domain_Type::Array_Reference_Count_Array_Length = 0; 
#endif

     // free memory allocated for memory pools!
     for (int i=0; i < Array_Domain_Type::Max_Number_Of_Memory_Blocks - 1; i++)
          { if (Array_Domain_Type::Memory_Block_List[i] != (unsigned char * )0)
               { free(((char * )Array_Domain_Type::Memory_Block_List[i])); } 
     } 
} 
#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )

void GlobalMemoryRelease()
   { 
   // Call static member functions of A++ objects to free memory in use (internally)
   // This be call AFTER ALL A++ objects have had their destructors called since the
   // A++ destructors must reference the memory that this function will free!

     printf(("\n")); 
     printf(("\n")); 
     printf(("\n")); 
     printf(("Inside of GlobalMemoryRelease -- Freeing up A++ internal memory usage! \n")); 

     // Turn this off now to about recursive calls to this functions 
     // when we delete the remaining internal array objects
     Diagnostic_Manager::setSmartReleaseOfInternalMemory(OFF); 

     // Cleanup previous where mask and history saved in the Where_Statement_Support class
     Where_Statement_Support::cleanup_after_Where(); 

#if defined (SERIAL_APP)
     // free Serial_A++

     // Cleanup previous where mask and history saved in the Where_Statement_Support class











     // Internal_Partitioning_Type::freeMemoryInUse();

#if defined(USE_PADRE)
     // We only have to call this member function the function for cleaning
     // up the memory used within PADRE are called by this function

#endif

     // Cleanup the memory used for the stack of Array_IDs in use

     // delete SerialArray_Domain_Type::Bottom_Of_Stack;





#if !defined(USE_PADRE)



#endif

#if defined(USE_PADRE)
     // We only have to call this member function the function for cleaning
     // up the memory used within PADRE are called by this function
     // PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::freeMemoryInUse();

#else
     // Cleanup memory used withing PARTI (special parti function written by Dan Quinlan)

#endif
#endif // end SERIAL_APP

     // free A++ or P++ data

     // Cleanup previous where mask and history saved in the Where_Statement_Support class
     // Where_Statement_Support::cleanup_after_Where();

     Array_Domain_Type::freeMemoryInUse(); 

     doubleArray::freeMemoryInUse(); 
     floatArray::freeMemoryInUse(); 
     intArray::freeMemoryInUse(); 

     doubleArray_Descriptor_Type::freeMemoryInUse(); 
     floatArray_Descriptor_Type::freeMemoryInUse(); 
     intArray_Descriptor_Type::freeMemoryInUse(); 

#if defined(SERIAL_APP)
     // Bugfix (2/14/96) had to move the call to this function from up above.
     // The list within this objectmust remain valid while any P++ objects still exist!

#endif

#if !defined(USE_EXPRESSION_TEMPLATES)
     // Clean up MDI arrays
     cleanup_after_MDI(); 
#endif

     // Cleanup the memory used for the stack of Array_IDs in use
     Array_Domain_Type::Top_Of_Stack = (int * )0; 
     // delete Array_Domain_Type::Bottom_Of_Stack;
     free(Array_Domain_Type::Bottom_Of_Stack); 
     Array_Domain_Type::Bottom_Of_Stack = (int * )0; 
     // APP_Top_Of_Stack    = NULL;
     // delete APP_Bottom_Of_Stack;
     // APP_Bottom_Of_Stack = NULL;

     // delete the internall diagnostic information
     Diagnostic_Manager::freeMemoryInUse(); 

     // delete the MPI communicator
     Optimization_Manager::freeMemoryInUse(); 

     // There are other objects that might have to be freed!  NOT AFTER cleanup_after_MDI()
     // printf ("There might be some other objects that should be freed (within the MDI layer)! \n");

     printf(("Inside of GlobalMemoryRelease -- Freed up internal memory usage! \n")); 

     printf(("\n")); 
     printf(("********************************************************************* \n")); 
     printf(("WARNING: This MUST be the last A++/P++ related statement in your program! \n")); 
     printf(("********************************************************************* \n")); 

     // This mechanism forces an application to exit from this function which permits
     // the use of a return statement with a non-zero value to indicate an error if 
     // this function was not called (so we can be certain that the GlobalMemoryRelease()
     // function is called).  This mechanism needs to be optional.
     if (Diagnostic_Manager::getExitFromGlobalMemoryRelease() == 1)
        { 
          printf(("NOTE: Exiting from GlobalMemoryRelease() as requested! \n")); 

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
        { } 
} 
#if 0
        // Even if we don't use the getExitFromGlobalMemoryRelease() function we want to call the
        // purify exit function at program exit (but only when running with purify, else just exit using the
        // applications exit mechanism)
#if defined(USE_PURIFY)
        // If purify is used (configured and is actually running) then force the exit 
        // status to reflect any errors found when running with purify 
        // (access errors, memory leaks, and memory in use) these bool values 
        // are ORed into the status (ZERO is used as input)







        // printf ("NOTE: NOT Exiting from GlobalMemoryRelease() ... \n");

#else
        // printf ("NOTE: NOT Exiting from GlobalMemoryRelease() ... \n");
#endif
#endif



        // This object must be built first (this can be a problem with static A++ objects)
MemoryManagerType::MemoryManagerType()
   { } 
   // Nothing to initialize in constructor!


MemoryManagerType::~MemoryManagerType()
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


int Array_Domain_Type::checkForArrayIDOnStack(int x)
   { 
     int numberOfReferences=0; 

#if USE_ORDER_N_SQUARED_TESTS
     // Search the stack for the existing Array_ID in use (it should not be there!)
     // We might also search for redundent entries -- this is not implemented yet
     // since this would be expensive.



     // Note Pointer arithmatic


     // Top_Of_Stack = Bottom_Of_Stack + Stack_Depth;
     // printf ("In while loop: *StackEntry = %d == internalArrayID = %d \n",*StackEntry,internalArrayID);
     // printf ("*StackEntry     = %d \n",*StackEntry);
     // printf ("internalArrayID = %d \n",internalArrayID);



     // printf ("*StackEntry = %d == x = %d numberOfReferences = %d \n",*StackEntry,x,numberOfReferences);


     // APP_ASSERT (*StackEntry != x);



#else
     if (x){ }      // use this to avoid compiler warning about non-use of "x"
#endif

     return numberOfReferences;
} 



void Array_Domain_Type::Push_Array_ID(int x)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3 || Diagnostic_Manager::getReferenceCountingReport() > 0)
          { printf(("Inside of Push (x=%d) Stack_Depth = %d Max_Stack_Size = %d \n"),
          x,Array_Domain_Type::Stack_Depth,Array_Domain_Type::Max_Stack_Size); } 
#endif

#if USE_ORDER_N_SQUARED_TESTS
          // This is part of an expensive O(n^2) operation so use it saringly as a test

#endif

     Array_Domain_Type::Stack_Depth++; 
     if (Array_Domain_Type::Stack_Depth >= Array_Domain_Type::Max_Stack_Size)
        { 
        // printf ("Increasing the stack size! \n");
        // Stack size must be increased to acount for more valued push onto it!
          Array_Domain_Type::Max_Stack_Size += 100; 
          // Old 4.1.3 systems must malloc and can not allow realloc to take a NULL pointer!
          if (Array_Domain_Type::Bottom_Of_Stack == (int * )0)
               { Array_Domain_Type::Bottom_Of_Stack = (int * )valloc((Array_Domain_Type::Max_Stack_Size
                * 4)); } 
          else { Array_Domain_Type::Bottom_Of_Stack = (int * )realloc(((char * )Array_Domain_Type::Bottom_Of_Stack),
          (Array_Domain_Type::Max_Stack_Size * 4)); 
          }           // Pointer arithmatic
          Array_Domain_Type::Top_Of_Stack = Array_Domain_Type::Bottom_Of_Stack + Array_Domain_Type::Stack_Depth; 
     } 
     else 
        { 
        // Stack depth sufficient for another value!
          Array_Domain_Type::Top_Of_Stack++; 
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

#else

#endif
#endif

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
          // For P++ we only track the serial array objects not the serial array objects
     if (Diagnostic_Manager::getTrackArrayData() == 1)
        { 
          if (!(Diagnostic_Manager::diagnosticInfoArray != (DiagnosticInfo * * )0)){ APP_Assertion_Support("domain.C",
          817); 
          } 
          if (!(Diagnostic_Manager::diagnosticInfoArray[x] != (DiagnosticInfo * )0)){ APP_Assertion_Support("domain.C",
          818); 
          } 
          :: delete Diagnostic_Manager::diagnosticInfoArray[x]; 
          Diagnostic_Manager::diagnosticInfoArray[x] = (DiagnosticInfo * )0; 
     } 
#endif

     *Array_Domain_Type::Top_Of_Stack = x; 
} 

#if 0



     // This function returns what woud be the next value from the stack of Array_IDs





     // Get value from Global_Array_ID (but don't modify the APP_Global_Array_ID!)




     // Get value on stack (but don't modify the stack!)










#endif


int Array_Domain_Type::Pop_Array_ID()
   { 
   // This function provides the stack mechanism for the Array_IDs

     int Result; 

     if (Array_Domain_Type::Stack_Depth <= 0)
        { 
        // Get value from Global_Array_ID!
          Result = APP_Global_Array_ID++; 
     } 
     else 
        { 
        // Get value on stack!
          Result = *Array_Domain_Type::Top_Of_Stack; 
          Array_Domain_Type::Stack_Depth--; 
          Array_Domain_Type::Top_Of_Stack--; 
     } 
#if 0
          // reused array id's should be associated with a reference count equal to
          // Array_Domain_Type::getRawDataReferenceCountBase()-1
#if defined(PPP) || defined(SERIAL_APP)

#else

#endif
#endif


#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3 || Diagnostic_Manager::getReferenceCountingReport() > 0)
          { printf(("Inside of Pop_Array_ID -- Global_Array_ID = %d Result = %d Stack_Depth = %d \n"),
          APP_Global_Array_ID,Result,Array_Domain_Type::Stack_Depth); } 
#endif

     if (Result >= 1000000)
        { 
          printf(("WARNING: Inside of Pop_Array_ID  Number (%d) of outstanding array object in excess of %d \n"),
          Result,1000000); 
     }           // APP_ASSERT (Result < MAX_ARRAY_ID);

          // printf ("NOTE: Inside of Pop_Array_ID  new array id = %d \n",Result);

#if defined(PPP) || defined(SERIAL_APP)
          // Since this is a new array object is should have an initialize reference count on its
          // raw data.  This is required here because the reference counting mechanism reused the
          // value of zero for one existing reference and no references (this will be fixed soon).
          // resetRawDataReferenceCount();
          // Array_Reference_Count_Array [Result] = getRawDataReferenceCountBase();



#if defined(SERIAL_APP)
          // The local array associated with a P++ array (a SerialArray) uses the Array_Reference_Count_Array
          // mechanism to provide reference counting on the raw array data associated with an array object.


#else
          // For P++ array objects the Array_Reference_Count_Array mechanism is NOT used so set it to ZERO
          // to indicate that it is not used. (Note: SerialArray_Domain_Type::getRawDataReferenceCountBase() - 1 == 0)



#endif
#else
     if (Result >= Array_Domain_Type::Array_Reference_Count_Array_Length)
          { Array_Domain_Type::Allocate_Larger_Reference_Count_Array(Result); } 
     Array_Domain_Type::Array_Reference_Count_Array[Result] = Array_Domain_Type::getRawDataReferenceCountBase(); 
#endif

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
     // For P++ we only track the serial array objects not the serial array objects
     if (Diagnostic_Manager::getTrackArrayData() == 1)
        { 
          if (!(Diagnostic_Manager::diagnosticInfoArray != (DiagnosticInfo * * )0)){ APP_Assertion_Support("domain.C",
          930); 
          } 
          if (!(Diagnostic_Manager::diagnosticInfoArray[Result] == (DiagnosticInfo * )0)){ APP_Assertion_Support("domain.C",
          931); 
          } 
          Diagnostic_Manager::diagnosticInfoArray[Result] = new DiagnosticInfo((*this)); 
          if (!(Diagnostic_Manager::diagnosticInfoArray[Result] != (DiagnosticInfo * )0)){ APP_Assertion_Support("domain.C",
          933); 
          } 
     } 
#endif

#if defined(PPP)
          // Bugfix (11/4/2000)
          // P++ arrays should not use the SerialArray_Domain_Type::Array_Reference_Count_Array
          // for tracking reference counts (we need to detect this since it is an error).

#endif

     return Result;
} 

     // *****************************************************************************
     // *****************************************************************************
     // ***********************  MEMBER FUNCTION DEFINITIONS  ***********************
     // *****************************************************************************
     // *****************************************************************************

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )

int Array_Domain_Type::getNumberOfArraysInUse()
   { 
   // This function reports the number of arrays in use within A++/P++
   // it is used in the array object destructors where if internal 
   // debugging is turned ON it optionally permits specialized cleanup of internal
   // memory used in A++/P++ which would otherwise lead to misleading messages from PURIFY.
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        { printf(("Inside of Array_Domain_Type::getNumberOfArraysInUse() \n")); } 
#endif 

        // save the current state and turn off the output of node number prefix to all strings
     bool printfState=Communication_Manager::getPrefixParallelPrintf(); 
     Communication_Manager::setPrefixParallelPrintf(false); 

     int ValidArrayCount=0; 

     // Handle case of getNumberOfArraysInUse() being called before any array is allocated (return 0)
     if (Array_Domain_Type::Array_Reference_Count_Array != (int * )0)
        { 
          if (!(Array_Domain_Type::Array_Reference_Count_Array != (int * )0)){ APP_Assertion_Support("domain.C",
          975); 
          } 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               { printf(("Array_Reference_Count_Array [0-%d]:"),Array_Domain_Type::Array_Reference_Count_Array_Length
                - 1); } 
#endif
#if 0



               // (DQ 5/1/2000): This is now fixed!
               // What constitues a valid reference count is an issue to be fixed within A++/P++
               // since presently zero means two different things.  Soon this will be fixed so that
               // an existing array object with no external references to its data will have a reference
               // count of 1 (instead of zero as it does now).  This this mechnaism will work properly.
               // So at the moment this function does NOT work properly.
               // if ( (Array_Reference_Count_Array [i] >= APP_REFERENCE_COUNT_BASE) &&
               //      (typeCode == APP_UNDEFINED_ELEMENT_TYPE) )





#if COMPILE_DEBUG_STATEMENTS


#endif

#else
               // For P++ we only track the serial array objects not the paralel array objects ???
          if (Diagnostic_Manager::getTrackArrayData() == 1)
             { 
               if (!(Diagnostic_Manager::diagnosticInfoArray != (DiagnosticInfo * * )0)){ APP_Assertion_Support("domain.C",
               1007); 
               }                // This is a more accurate way of counting because it does not include 
               // Array_Domain_Type objects used by PADRE
               int i=0; 
               for (i = 0; i < Array_Domain_Type::Array_Reference_Count_Array_Length; i++)
                  { 
#if COMPILE_DEBUG_STATEMENTS
                  // Error checking
                    if (Array_Domain_Type::Array_Reference_Count_Array[i] >= 1)
                         { if (!(Diagnostic_Manager::diagnosticInfoArray[i] != (DiagnosticInfo * )0)){ APP_Assertion_Support("domain.C",
                         1016); 
                         } 
                    } 
#if 0


#endif
#endif

                    if (Diagnostic_Manager::diagnosticInfoArray[i] != (DiagnosticInfo * )0
                     && ((*Diagnostic_Manager::diagnosticInfoArray[i]).getTypeCode)()
                     != 7000){ 
                         ValidArrayCount++; 
                    } 
               } 
          } 
          else 
             { 
             // This is less accurate since many array domain type objects used by PADRE are counted
               int i=0; 
               for (i = 0; i < Array_Domain_Type::Array_Reference_Count_Array_Length; i++)
                  { 
                    if (Array_Domain_Type::Array_Reference_Count_Array[i] >= 1)
                       { 
                         ValidArrayCount++; 
                    } 
               } 
          } 
     } 
#endif


#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        { 
          printf(("\n")); 
          printf(("ValidArrayCount = %d \n"),ValidArrayCount); 
     } 
#endif

          // reset output of node number prefix to all strings
     Communication_Manager::setPrefixParallelPrintf(printfState); 

     if (!(ValidArrayCount >= 0)){ APP_Assertion_Support("domain.C",1056); } 

     return ValidArrayCount;
} 
#endif /*end of  APP || ( SERIAL_APP) && !(PPP) )*/

     // *****************************************************************************
     // *****************************************************************************
     // ********************  DISPLAY MEMBER FUNCTION DEFINITION  *******************
     // *****************************************************************************
     // *****************************************************************************


void Array_Domain_Type::display(const char * Label) const
   { 
     printf(("Array_Domain_Type::display() -- %s \n"),Label); 

     // save the current state and turn off the output of node number prefix to all strings
     bool printfState=Communication_Manager::getPrefixParallelPrintf(); 
     Communication_Manager::setPrefixParallelPrintf(false); 

     // We want to remove any dependence upon the diagnostic mechanism
     // printf ("Array_Domain is used in a %sArray (element size = %d) \n",getTypeString(),getTypeSize());
     printf(("Array_Domain is %s \n"),(this -> Is_A_View == 1)?"VIEW":"NON-VIEW"); 
     printf(("Array_Domain is %s \n"),(this -> Is_A_Temporary == 1)?"TEMPORARY":"NON-TEMPORARY"); 
     printf(("Array_Domain is %s \n"),(this -> Is_Contiguous_Data == 1)?"CONTIGUOUS":"NON-CONTIGUOUS"); 
     printf(("Array_Domain is %s \n"),(this -> Is_A_Null_Array == 1)?"NULL":"NON-NULL"); 
     printf(("Array_Domain Uses_Indirect_Addressing is %s \n"),(this -> Uses_Indirect_Addressing
      == 1)?"TRUE":"FALSE"); printf(("Array_Domain is %s \n"),(this -> Is_Built_By_Defered_Evaluation
      == 1)?"CONSTRUCTED DURING DEFERED EVALUATION":"NON-DEFERED (Standard Construction)"); 
     printf(("Domain_Dimension     = %d \n"),this -> Domain_Dimension); 

#if 0
     // This was added to as part of the debugging which discovered that the use of the
     // BaseArray as a base class adds 4 bytes to the lenght of the array objects and thus
     // tends to effect the size of just about everything (like the intArrays used in the 
     // indirect addressing). This detail of C++ might be compiler dependent.










#endif
     printf(("Constant_Data_Base       = %s \n"),(this -> Constant_Data_Base == 1)?"TRUE":"FALSE"); 
     printf(("Constant_Unit_Stride     = %s \n"),(this -> Constant_Unit_Stride == 1)?"TRUE":"FALSE"); 

     printf(("internalArrayID          = %3d \n"),this -> internalArrayID); 
     printf(("builtUsingExistingData   = %s \n"),(this -> builtUsingExistingData == 1)?"TRUE":"FALSE"); 
     printf(("View_Offset              = %3d \n"),this -> View_Offset); 
     printf(("ExpressionTemplateOffset = %3d \n"),this -> ExpressionTemplateOffset); 

     printf(("Scalar_Offset ========================================= ")); 
     int i; 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> Scalar_Offset)[i]); } 
     printf(("\n")); 

     printf(("User_Base ============================================= ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> User_Base)[i]); } 
     printf(("\n")); 

     printf(("Block Size ============================================ ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> Size)[i]); } 
     printf(("\n")); 

     printf(("Computed - Array Size ================================= ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),this -> getLength(i)); } 
     printf(("\n")); 

     printf(("(Internal use) Data_Base ============================== ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> Data_Base)[i]); } 
     printf(("\n")); 

     printf(("Base (of access to data) ============================== ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> Base)[i]); } 
     printf(("\n")); 

     printf(("getBase (output of getBase member function) =========== ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),this -> getBase(i)); } 
     printf(("\n")); 

     printf(("getRawBase (output of getRawBase member function) ===== ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),this -> getRawBase(i)); } 
     printf(("\n")); 

     printf(("Bound (of access to data) ============================= ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> Bound)[i]); } 
     printf(("\n")); 

     printf(("getBound (output of getBound member function) ========= ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),this -> getBound(i)); } 
     printf(("\n")); 

     printf(("getRawBound (output of getRawBound member function) === ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),this -> getRawBound(i)); } 
     printf(("\n")); 

     printf(("Stride (of access to data) ============================ ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> Stride)[i]); } 
     printf(("\n")); 

     printf(("getStride (output of getStride member function) ======= ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),this -> getStride(i)); } 
     printf(("\n")); 

     printf(("getRawStride (output of getRawStride member function) = ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),this -> getRawStride(i)); } 
     printf(("\n")); 

     // if defined(USE_EXPRESSION_TEMPLATES)
     printf(("***** Expression Template Intermediate Form ***** \n")); 
     printf(("IndexDataPointer (intermediate form) = ")); 
     for (i = 0; i < 6; i++)
          { printf((" %p "),(this -> IndexDataPointer)[i]); } 
     printf(("\n")); 

     printf(("IndexBase (intermediate form) ======== ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> IndexBase)[i]); } 
     printf(("\n")); 

     printf(("IndexStride (intermediate form) ====== ")); 
     for (i = 0; i < 6; i++)
          { printf((" %3d "),(this -> IndexStride)[i]); } 
     printf(("\n")); 
     printf(("************************************************* \n")); 
     // endif

     if (this -> referenceCount > Array_Domain_Type::getReferenceCountBase() + 1)
        { 
          printf(("Domain Reference Count = %d (there is a                reference to this descriptor)! \n"),
          this -> referenceCount); 
     } 
     else 
        { 
          printf(("Domain Reference Count = %d \n"),this -> referenceCount); 
     } 

     printf(("Indirect addressing arrays \n")); 
     for (i = 0; i < 6; i++)
        { 
          printf(("     Index_Array [%d] = %s \n"),i,((this -> Index_Array)[i] == (intArray * )0)?"NULL":"NON-NULL"); 
          if ((this -> Index_Array)[i] != (intArray * )0)
             { 
               printf(("Index_Array [%d] = %p \n"),i,(this -> Index_Array)[i]); 
#if 1
               // The display function has shorter output (so it is easier to work with)
               (*(this -> Index_Array)[i]).display(("INDIRECT ADDRESSING INTEGER LIST")); 
          } 
     } 
#else
               // Long output

#endif



#if defined(PPP)







#if 1
               // The display function has shorter output (so it is easier to work with)

#else
               // Long output

#endif




               // Pointer to a P++ partitioning object


               // APP_ASSERT(Partitioning_Object_Pointer != NULL);





#if defined(USE_PADRE)
               // What PADRE function do we call here?
               // printf ("NEED TO CALL PADRE \n"); 
               // APP_ABORT();
               // parallelDescriptor.display(Label);



               // printf ("Call to parallelPADRE_DescriptorPointer->display(Label) commented out! \n");






#else
               // Pointer to Block-Parti parallel decomposition (object)



               // Pointer to Block-Parti parallel distributed array descriptor






               // These require the BlockPartiArrayDomain internally to determin the partition position
               // isNonPartition requires the Local_Mask to be properly set















































#if 0




#else










#endif /* end of if 0 else */

#if 0




#else










#endif /* end of if 0 else */

#if 0




#else



               // int Start = globalToLocal(BlockPartiArrayDomain,Data_Base[i]+Base[i],i);
               // int End   = globalToLocal(BlockPartiArrayDomain,Data_Base[i]+Bound[i],i);
               // int Start = Data_Base[i]+gLBnd(BlockPartiArrayDomain,i);
               // int End   = Data_Base[i]+gUBnd(BlockPartiArrayDomain,i);







#endif /* end of if 0 else */

#if 0




#else



               // int Start = globalToLocal(BlockPartiArrayDomain,Data_Base[i]+Base[i],i)  - InternalGhostCellWidth[i];
               // int End   = globalToLocal(BlockPartiArrayDomain,Data_Base[i]+Bound[i],i) + InternalGhostCellWidth[i];
               // int Start = (Data_Base[i]+gLBnd(BlockPartiArrayDomain,i) ) - InternalGhostCellWidth[i];
               // int End   = (Data_Base[i]+gUBnd(BlockPartiArrayDomain,i) ) + InternalGhostCellWidth[i];







#endif

               // End of if not USE_PADRE
#endif

               // Specialized P++ support for interpretation of message passing
               // This might make more sense in a seperate structure that was started at the
               // beginning of a statements evaluation and ended at the end of a statements
               // evaluation and was passed along through the binary operators in the processing.

               // printf ("\n");






















               // Pointer to Block-Parti parallel decomposition (object)

















#endif /* end of PPP */

               // reset output of node number prefix to all strings
     Communication_Manager::setPrefixParallelPrintf(printfState); 

     printf(("\n")); 
     printf(("\n")); 
} 

     // *********************************************************
     // *********************************************************
     // *********************  DESTRUCTORS  *********************
     // *********************************************************
     // *********************************************************

Array_Domain_Type::~Array_Domain_Type()
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0 || Diagnostic_Manager::getReferenceCountingReport() > 0)
        { 
          printf(("Call Array_Domain_Type::destructor! %p Array_ID = %d referenceCount = %d typeCode = %d \n"),
          this,this -> internalArrayID,this -> referenceCount,this -> typeCode); 
          printf(("Number of arrays is use = %d \n"),Diagnostic_Manager::getNumberOfArraysInUse()); 
     } 
#endif

          // I think we only want to execute the body of the constructor if the 
          // reference count is less than zero.  This would allow the constructor to
          // be called but not delete anything depending on the reference count.

          // We have to use getReferenceCountBase()-1 as a 
          // lower bound because the delete operator 
          // also decrements the reference count.
     if (!(this -> referenceCount >= Array_Domain_Type::getReferenceCountBase() - 1)){ APP_Assertion_Support("domain.C",
     1495); 
     } 
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
     for (i = 0; i < 6; i++)
        { 
#if defined(PPP)


#endif
        // Delete the intArrays used to support indirect addressing (for A++ and P++)
          if ((this -> Index_Array)[i] != (intArray * )0)
             { 
               if (!(this -> Uses_Indirect_Addressing == 1)){ APP_Assertion_Support("domain.C",
               1521); 
               }                // Added conventional mechanism for reference counting control
               // operator delete no longer decriments the referenceCount.
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 2)
                  { 
                    printf(("In Array_Domain_Type::destructor Index_Array[%d] Array_ID = %d referenceCount = %d \n"),
                    i,(*(this -> Index_Array)[i]).Array_ID(),(*(this -> Index_Array)[i]).getReferenceCount()); 
                    (*(this -> Index_Array)[i]).displayReferenceCounts(("In Array_Domain_Type::destructor")); 
               } 
#endif
               (*(this -> Index_Array)[i]).decrementReferenceCount(); 
               if (((*(this -> Index_Array)[i]).getReferenceCount)() < Array_Domain_Type::getReferenceCountBase())
                  { 
                  // printf ("Deleting the indirection array (intArray* = %p) \n",Index_Array[i]);
                    delete (this -> Index_Array)[i]; 
               } 
               (this -> Index_Array)[i] = (intArray * )0; 
          } 
     } 
#if defined(PPP)
               // Delete the intSerialArrays used to support parallel (P++) indirect addressing




               // Added conventional mechanism for reference counting control
               // operator delete no longer decriments the referenceCount.
#if COMPILE_DEBUG_STATEMENTS






#endif



               // printf ("Deleting the local indirection array (intSerialArray* = %p) \n",Local_Index_Array[i]);




#endif


#if defined(PPP)
               // Pointer to a P++ partitioning object
               // APP_ASSERT(Partitioning_Object_Pointer != NULL);
               // printf ("In ~Array_Domain_Type -- Partitioning_Object_Pointer = %p \n",Partitioning_Object_Pointer);


#if COMPILE_DEBUG_STATEMENTS


#endif

               // Bugfix (9/16/2000) make reference count handling consistant with use elsewhere


               // Bugfix (11/14/2000) need to make consistent with code elsewhere



#if COMPILE_DEBUG_STATEMENTS


#endif






#if COMPILE_DEBUG_STATEMENTS


#endif


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


               // printf ("NOTE: Expected parallelPADRE_DescriptorPointer == NULL \n");

#if COMPILE_DEBUG_STATEMENTS



#endif


#if COMPILE_DEBUG_STATEMENTS


#endif




#else
               // Pointer to Block-Parti parallel decomposition (object)
               // APP_ASSERT(BlockPartiArrayDecomposition != NULL);


#if COMPILE_DEBUG_STATEMENTS



#endif
               // We must handle the reference counting manually since these are BLOCK PARTI objects (in C)!
               // printf ("Inside of ~Array_Domain_Type: BlockPartiArrayDecomposition->referenceCount = %d \n",BlockPartiArrayDecomposition->referenceCount);

               // Note that the decrement of the reference count is a part of the delete_DECOMP function!

               // printf ("In Array_Domain_Type::~Array_Domain_Type (Array_Domain_Type) --- Calling delete_DECOMP \n");
#if COMPILE_DEBUG_STATEMENTS







#endif





               // Pointer to Block-Parti parallel distributed array descriptor
               // APP_ASSERT(BlockPartiArrayDomain != NULL);


#if COMPILE_DEBUG_STATEMENTS



#endif
               // We must handle the reference counting manually since these are BLOCK PARTI objects (in C)!
               // printf ("Inside of ~Array_Domain_Type: BlockPartiArrayDomain->referenceCount = %d \n",BlockPartiArrayDomain->referenceCount);

               // Note that the decrement of the reference count is a part of the delete_DARRAY function!

               // printf ("In Array_Domain_Type::~Array_Domain_Type (Array_Domain_Type) --- Calling delete_DARRAY \n");

#if COMPILE_DEBUG_STATEMENTS







#endif




               // End of if not USE_PADRE
#endif

               // Pointer to Array_Conformability_Info which carries information required for the
               // interpretation of the P++ message passing though the evaluation of the array statement
               // APP_ASSERT(Array_Conformability_Info != NULL);


#if COMPILE_DEBUG_STATEMENTS





#endif
               // Added conventional mechanism for reference counting control
               // operator delete no longer decriments the referenceCount.






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




#else
     bool resetReferenceCountAndReturnArrayID=Array_Domain_Type::Array_Reference_Count_Array[(this
      -> internalArrayID)] < Array_Domain_Type::getReferenceCountBase() || (Array_Domain_Type::Array_Reference_Count_Array[(this
      -> internalArrayID)] == Array_Domain_Type::getReferenceCountBase()) && (this -> builtUsingExistingData
      == 1); 
#endif

#if 0







#endif

     if (resetReferenceCountAndReturnArrayID == 1)
        { 
          if (Diagnostic_Manager::getReferenceCountingReport() > 0)
             { 
               printf(("Reset the referenceCount in the domain destructor and give back the array ID (referenceCount = %d Array_ID = %d) \n"),
               Array_Domain_Type::Array_Reference_Count_Array[(this -> internalArrayID)],
               this -> internalArrayID); 
          } 
               // Reinitialize reference count for next use!
          Array_Domain_Type::Array_Reference_Count_Array[(this -> internalArrayID)] = Array_Domain_Type::getReferenceCountBase()
           - 1; 
          // Put used Array ID back on stack for later reuse by other array objects!
          // Note that the Push/Pop of array id's triggers the unset/set of the diagnostic 
          // info objects if Diagnostic_Manager::setTrackArrayData() is turned on.
          Array_Domain_Type::Push_Array_ID(this -> internalArrayID); 
     } 


     else if (Diagnostic_Manager::getReferenceCountingReport() > 0)
             { 
               printf(("SKIP Reset of the referenceCount in the domain destructor and give back the array ID (referenceCount = %d Array_ID = %d) \n"),
               Array_Domain_Type::Array_Reference_Count_Array[(this -> internalArrayID)],
               this -> internalArrayID); 
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
     if (Array_Domain_Type::smartReleaseOfInternalMemory)
        { 
        // Compute the number of arrays that are stored internally
          int numberOfOutstandingArrayObjects=0; 
          numberOfOutstandingArrayObjects = Where_Statement_Support::numberOfInternalArrays(); 
          numberOfOutstandingArrayObjects += intArray::numberOfInternalArrays(); 
          numberOfOutstandingArrayObjects += floatArray::numberOfInternalArrays(); 
          numberOfOutstandingArrayObjects += doubleArray::numberOfInternalArrays(); 

#if 0


#endif

          if (Array_Domain_Type::getNumberOfArraysInUse() <= numberOfOutstandingArrayObjects)
             { 
               if (Array_Domain_Type::Array_Reference_Count_Array[(this -> Array_ID)()]
                < Array_Domain_Type::getReferenceCountBase() || (Array_Domain_Type::Array_Reference_Count_Array[(this
                -> Array_ID)()] == Array_Domain_Type::getReferenceCountBase()) && (this
                -> builtUsingExistingData == 1)){ 
               // printf ("CALLING GlobalMemoryRelease() \n");
                    GlobalMemoryRelease(); 
               } 
               else 
                  { } 
          } 
     }                   // This could really happen legitimately so we don't want a warning message about it.
                  // printf ("A++/P++ WARNING: reference count too large to call GlobalMemoryRelease() \n");



#endif

                  // printf ("In ~Array_Domain_Type Array_ID on the stack is %d \n",SerialArray_Domain_Type::queryNextArrayID());

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Leaving ~Array_Domain_Type() -- referenceCount = %d \n"),this -> referenceCount); } 
} 
#endif


          // *********************************************************
          // *********************************************************
          // *******  INITIALIZATION SUPPORT FOR CONSTRUCTORS  *******
          // *********************************************************
          // *********************************************************

#if defined(PPP)



          // I'm not clear on how we want to initialize these so initialize them
          // to some default values for now.

#if COMPILE_DEBUG_STATEMENTS




#endif

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
          // I have had some conserns that this function was not being called at all times so
          // this is a simple print command to verify it is being called for the construction 
          // of every array.

#endif

          // Pointer to a P++ partitioning object
          // test assertion!  if not true then we need to delete the 
          // Partitioning_Object_Pointer object properly!

          // bugfix (8/15/2000) if the current object has an existing partitioning object then reuse it
          // this should fix bug2000_25.C in P++/TESTS










#if defined(USE_PADRE)
          // With the modification to have the parallelPADRE_DescriptorPointer
          // initialized in the Allocation function this pointer can be NULL


          // We want to force a new descriptor to be built -- later we might want
          // to have this use an existing PADRE_Representation object or we might
          // want a new PADRE_Descriptor to be built using the existing 
          // PADRE_Distribution or PADRE_Representation object.  But the correct
          // PADRE_Distribution is already used since the Partitioning object use in X
          // and this domain object are the same.  so the only enhansement would be
          // to have the PADRE_Representation objects shared internally.

#if 1


          // printf ("(before deleting the existing object) parallelPADRE_DescriptorPointer->getReferenceCount() = %d \n",parallelPADRE_DescriptorPointer->getReferenceCount());






#else

#endif




          // printf ("(before new objects is built) this->getReferenceCount() = %d \n",getReferenceCount());

          // We want to make this step as cheap as possible.  We reuse everything except
          // the LocalDescriptor (we pass in the "this" pointer) and the Domain for the 
          // serial array object (which we don't know yet so we have to set that later).








          // printf ("(after new objects is built) parallelPADRE_DescriptorPointer(%p)->getReferenceCount() = %d \n",
          //      parallelPADRE_DescriptorPointer,parallelPADRE_DescriptorPointer->getReferenceCount());
          // printf ("(after new objects is built) this->getReferenceCount() = %d \n",getReferenceCount());
#else
          // Pointer to Block-Parti parallel decomposition (object)





          // printf ("In Array_Domain_Type::Initialize_Parallel_Parts_Of_Domain -- BlockPartiArrayDecomposition->referenceCount = %d \n",
          //      BlockPartiArrayDecomposition->referenceCount);


          // Pointer to Block-Parti parallel distributed array descriptor





          // printf ("In Array_Domain_Type::Initialize_Parallel_Parts_Of_Domain -- BlockPartiArrayDomain->referenceCount = %d \n",
          //      BlockPartiArrayDomain->referenceCount);

          // End of if not USE_PADRE
#endif

          // Set this to NULL since we assume we will not use Index objects on a temporary (for now)


#if 0
          // These have not been initialized yet (so it is a purify UMR error to access them)





#endif

          // end of PPP
#endif

          // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
          // void Array_Domain_Type::Initialize_Domain(
          //           int Input_Array_Size_I , int Input_Array_Size_J , 
          //           int Input_Array_Size_K , int Input_Array_Size_L ) 
          // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void Array_Domain_Type::Initialize_Domain(int Number_Of_Valid_Dimensions,const int * Integer_List)


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
          printf(("Inside of Array_Domain_Type::Initialize_Domain(Number_Of_Valid_Dimensions=%d,int*) "),
          Number_Of_Valid_Dimensions); 
          for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
               { printf(("int(%d) "),Integer_List[temp]); } 
          printf(("\n")); 
     } 
#endif

          // Conditional compilation for new internalIndex based initializeDomain function
     class Internal_Index Index_List[6]; 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
        { 
          int tempBase=0; 
          int tempBound=Integer_List[temp] - 1; 
          (Index_List[temp])=Internal_Index(tempBase,(tempBound - tempBase) + 1); 
          Internal_Index_List[temp] = Index_List + temp; 
     } 

          // Initialize the rest to something
     for (temp = Number_Of_Valid_Dimensions; temp < 6; temp++)
        { 
          (Index_List[temp])=Internal_Index(0,1); 
          Internal_Index_List[temp] = Index_List + temp; 
     } 

          // Now call the new version of the function
     this -> Initialize_Domain(Number_Of_Valid_Dimensions,Internal_Index_List); 
} 


void Array_Domain_Type::Initialize_Domain(const int * Integer_List)
   { 
     this -> Initialize_Domain(6,Integer_List); 

#if COMPILE_DEBUG_STATEMENTS
     this -> Test_Preliminary_Consistency(("Called from constructor Array_Domain_Type(int,int,int,int)")); 
} 
#endif



int Array_Domain_Type::computeDimension(Internal_Index * const * internalIndexList)
   { 
   // Compute the dimension of the array from the number of non unit entries in the Index list

     if (!(internalIndexList != (Internal_Index * const * )0)){ APP_Assertion_Support("domain.C",
     2046); 
     }      // Determine the highest dimension to start with (depending on the number of non-NULL 
     // pointers in the internalIndexList (list of pointers to Index objects))
     int tempDimension=0; 
     if (internalIndexList[tempDimension] != (Internal_Index * )0)
        { 
          while(tempDimension < 5 && internalIndexList[(tempDimension + 1)] != (Internal_Index * )0)
             { 
               tempDimension++; 
          } 
     } 
     else      // printf ("In computeDimension (finding the length of the non-Null pointer list): tempDimension = %d \n",tempDimension);

        { 
          tempDimension = 0; 
     } 

          // printf ("Starting at tempDimension = %d and counting down to zero! \n",tempDimension);

          // Now count down the number of dimensions to exclude trailing dimensions with unit length (length == 1)
     int domainDimension=tempDimension + 1; 
     while(tempDimension >= 0 && ((*internalIndexList[tempDimension]).getLength)() <= 1)
        { 
        // printf ("In computeDimension: tempDimension = %d \n",tempDimension);

          domainDimension--; 
          tempDimension--; 
     } 

          // printf ("In computeDimension (value before test): domainDimension = %d \n",domainDimension);

          // fixup Domain_Dimension for special case of 1D array of size 1
     if ((internalIndexList[0] != (Internal_Index * )0 && ((*internalIndexList[0]).getLength)()
      >= 1) && (domainDimension == 0)){ domainDimension = 1; } 

     // printf ("In computeDimension (final value): domainDimension = %d \n",domainDimension);

     if (!(domainDimension >= 0)){ APP_Assertion_Support("domain.C",2084); } 
     if (!(domainDimension <= 6)){ APP_Assertion_Support("domain.C",2085); } 

     return domainDimension;
} 


int Array_Domain_Type::computeDimension(const int * integerList)
   { 
   // Compute the dimension of the array from the number of non unit entries in the integer list

     int domainDimension=6; 
     int tempDimension=5; 
     if (!(integerList != (const int * )0)){ APP_Assertion_Support("domain.C",2097); } 
     while(tempDimension >= 0 && integerList[tempDimension] <= 1)
        { 
          domainDimension--; 
          tempDimension--; 
     } 

          // fixup Domain_Dimension for special case of 1D array of size 1
     if (integerList[0] >= 1 && (domainDimension == 0))
          { domainDimension = 1; } 

     if (!(domainDimension >= 0)){ APP_Assertion_Support("domain.C",2108); } 
     if (!(domainDimension < 6)){ APP_Assertion_Support("domain.C",2109); } 

     return domainDimension;
} 



void Array_Domain_Type::Initialize_Domain(int Number_Of_Valid_Dimensions,Internal_Index * const * Internal_Index_List)


   { 
   // Start of new internalIndex based initializeDomain function
   // This is the ONLY initialization function which does the low level 
   // initialization (all others call this function)
     int temp=0; 

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        { 
          printf(("Inside of Array_Domain_Type::Initialize_Domain(Number_Of_Valid_Dimensions=%d,int*) (Array ID=%d) "),
          Number_Of_Valid_Dimensions,this -> Array_ID()); 
          for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
               { printf(("internalIndex(%d - %d by %d) "),(*Internal_Index_List[temp]).getBase(),
               (*Internal_Index_List[temp]).getBound(),(*Internal_Index_List[temp]).getStride()); } 

          printf(("\n")); 
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
    if (false){ APP_Assertion_Support("domain.C",2150); } 
#endif

#if defined(PPP)
#if defined(USE_PADRE)

#else
    // Pointer to Block-Parti parallel decomposition (object)
    // APP_ASSERT (BlockPartiArrayDomain        == NULL);
    // APP_ASSERT (BlockPartiArrayDecomposition == NULL);

#if COMPILE_DEBUG_STATEMENTS








#endif



    // Pointer to Block-Parti parallel distributed array descriptor

#endif
#endif

     if (!(Number_Of_Valid_Dimensions >= 0)){ APP_Assertion_Support("domain.C",2179); } 

     // If the stride is greater than 1 then there is some freedom in the
     // selection of the size since it could match an existing distribution.
     for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
        { } 
        // Need to find out the sizes and search for an existing partition!


        // We have to look at ALL the dimensions to figure out if we have a strided view
     bool makeStridedView=false; 

     this -> Is_A_Null_Array = ((Number_Of_Valid_Dimensions == 0)?1:0); 
     for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
        { 
#if defined(PPP)
        // P++ specific stuff




#endif

        // By definition the internal access to the arrays data starts at ZERO
          (this -> Base)[temp] = 0; 

          // create a local variable since this value is used a couple of places in this loop
          int tempLength=(((*Internal_Index_List[temp]).getBound)() - ((*Internal_Index_List[temp]).getBase)())
           + 1; 

          // This is the offset of the left edge of the array in the user's i n d e x space
          (this -> Data_Base)[temp] = ((*Internal_Index_List[temp]).getBase)(); 

          // This is the left edge of the view (initially these are the same 
          // but a view of an array could make it different from the Data_Base)
          (this -> User_Base)[temp] = ((*Internal_Index_List[temp]).getBase)(); 
          // printf ("Setup i Array_Domain_Type::Initialize_Domain (): User_Base [%d] = %d \n",temp,User_Base[temp]);

          // By definition the internal access bound is at the right edge of the data
          (this -> Bound)[temp] = tempLength - 1; 

          // NOTE: The stride of a new array domain does not have to be 1!
          (this -> Stride)[temp] = ((*Internal_Index_List[temp]).getStride)(); 

          // if this is a strided array then it must be a view
          if ((this -> Stride)[temp] > 1)
             { 
               makeStridedView = true; 
          } 

               // A new array object built from a collection of Internal_Index
               // objects can not be using indirect addressing
          (this -> Index_Array)[temp] = (intArray * )0; 

#if defined(PPP)
          // Used to support P++ indirect addressing

#endif

          if (temp == 0)
               { (this -> Size)[0] = tempLength; } 
          else 
               { (this -> Size)[temp] = (this -> Size)[(temp - 1)] * tempLength; } 

          if (!(tempLength >= 0)){ APP_Assertion_Support("domain.C",2243); } 
          if (tempLength == 0)
               { this -> Is_A_Null_Array = 1; } 

               // if defined(USE_EXPRESSION_TEMPLATES)
          (this -> IndexBase)[temp] = 0; 
          (this -> IndexStride)[temp] = 0; 
          (this -> IndexDataPointer)[temp] = (int * )0; 
     }           // endif


          // Initialize the other dimensions to avoid errors in purify (and errors in general!)
     for (temp = Number_Of_Valid_Dimensions; temp < 6; temp++)
        { 
#if defined(PPP)


#endif

        // ... (bug fix,kdb, 8/5/96) if APP_Global_Array_Base is
        //  not 0 Base will be non zero and this will be a view ...
        // Base [temp] = Data_Base [temp] = Bound [temp] = APP_Global_Array_Base;
          (this -> Data_Base)[temp] = APP_Global_Array_Base; 
          (this -> User_Base)[temp] = APP_Global_Array_Base; 
          (this -> Base)[temp] = ((this -> Bound)[temp] = 0); 
          (this -> Stride)[temp] = 1; 
          (this -> Index_Array)[temp] = (intArray * )0; 
          if (temp == 0)
               { (this -> Size)[0] = 0; } 
          else 
               { (this -> Size)[temp] = (this -> Size)[(temp - 1)]; } 

#if defined(PPP)
               // Used to support P++ indirect addressing

#endif

               // if defined(USE_EXPRESSION_TEMPLATES)
          (this -> IndexBase)[temp] = 0; 
          (this -> IndexStride)[temp] = 0; 
          (this -> IndexDataPointer)[temp] = (int * )0; 
     }           // endif


          // ... Bugfix (8/16/96,kdb) move this section of code here so Bound can be set
          // correctly in case of a null array.  Since Base is already set correctly
          // remove the code that sets it in this loop. ...
          // Bugfix (2/9/96) we have to force null arrays to have Is_Contiguous_Data = FALSE
          // plus the base and bounds are set to specific values
     if (this -> Is_A_Null_Array == 1)
        { 
          this -> Is_Contiguous_Data = 0; 
          this -> Is_A_View = 0; 
          this -> Constant_Data_Base = 1; 

          // Bugfix (11/6/2000) We want a consistant setting for this when we have a NullArray object
          // This is enforced within the Test_Consistency tests for a domain object.
          // Constant_Unit_Stride = FALSE;
          this -> Constant_Unit_Stride = 1; 
          for (temp = 0; temp < 6; temp++)
             { 
               (this -> Bound)[temp] = -1; 
          } 
     } 
     else 
#if defined(PPP)
     // Bugfix (11/6/2000)
     // Force all dimensions to be Null_Index mode when the array is a Null array.
     // This is enforced within the Test_Consistency tests for a domain object.


#endif



        { 
        // Most common case is placed first
          if (makeStridedView == 0)
             { 
               this -> Is_A_View = 0; 
               this -> Is_Contiguous_Data = 1; 
               this -> Constant_Unit_Stride = 1; 
          } 
          else 
             { 
             // If we are building a strided view then the data can't be contiguous
               this -> Is_Contiguous_Data = 0; 
               this -> Is_A_View = 1; 
               this -> Constant_Unit_Stride = 0; 
          } 

               // initialize to true then loop through axes to verify correctness (set to FALSE if incorrect)
          this -> Constant_Data_Base = 1; 
          for (temp = 1; temp < 6; temp++)
             { 
               if ((this -> Data_Base)[temp] != (this -> Data_Base)[(temp - 1)])
                    { this -> Constant_Data_Base = 0; } 
          } 
     } 

#if 0









                    // fixup Domain_Dimension for special case of 1D array of size 1





#else
     this -> Domain_Dimension = Array_Domain_Type::computeDimension(Internal_Index_List); 
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("In Array_Domain_Type constructor -- Domain_Dimension = %d \n"),this
           -> Domain_Dimension); } 
#endif

#if defined(PPP)
          // (11/19/2000) assertion added since I think we need to delete
          // any valid pointer (to avoid reference counting problems)
          // APP_ASSERT (Partitioning_Object_Pointer == NULL);

          // Pointer to a P++ partitioning object


#if defined(USE_PADRE)
          // What PADRE function do we call here?
          // it is in a later phase of initialization of the local descriptor that
          // we also initialize the local domain object (the domain object in the serial array).
          // So we do nothing here for this case.
#else
          // Pointer to Block-Parti parallel decomposition (object)


          // Pointer to Block-Parti parallel distributed array descriptor

          // end of if not USE_PADRE
#endif


#endif

     this -> Is_A_Temporary = 0; 

     // if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset=(this -> Base)[0]; 
     for (temp = 1; temp < 6; temp++)
        { 
          ExpectedOffset += (this -> Base)[temp] * (this -> Size)[(temp - 1)]; 
     } 
     this -> ExpressionTemplateOffset = ExpectedOffset; 
     if (!(this -> ExpressionTemplateOffset == ExpectedOffset)){ APP_Assertion_Support("domain.C",
     2400); 
     }      // endif
     this -> View_Offset = 0; 
     // ... Scalar_Offset won't be 0 in rare case where APP_Global_Array_Base is nonzero ...
     (this -> Scalar_Offset)[0] = -(this -> User_Base)[0] * (this -> Stride)[0]; 
     for (temp = 1; temp < 6; temp++)
        { 
        // Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Size[temp-1];
          (this -> Scalar_Offset)[temp] = (this -> Scalar_Offset)[(temp - 1)] - ((this
           -> User_Base)[temp] * (this -> Stride)[temp]) * (this -> Size)[(temp - 1)]; 
     }           // Scalar_Offset -= Data_Base[temp] * Size[temp-1];

     this -> referenceCount = Array_Domain_Type::getReferenceCountBase(); 

     // Was this descriptor used to build an array object using the adopt function?
     // This is usually FALSE.
     this -> builtUsingExistingData = 0; 

     // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
     this -> internalArrayID = (this -> Pop_Array_ID)(); 

     this -> Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION; 

     this -> Uses_Indirect_Addressing = 0; 

     // This is used for internal diagnostics
     this -> typeCode = 7000; 

#if !defined(PPP)
     if (this -> internalArrayID >= Array_Domain_Type::Array_Reference_Count_Array_Length)
          { Array_Domain_Type::Allocate_Larger_Reference_Count_Array(this -> internalArrayID); } 
#endif

          // Now reset the bases (just as was done in the old version of this function)
     for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
          { this -> setBase((*Internal_Index_List[temp]).getBase(),temp); } 

#if COMPILE_DEBUG_STATEMENTS
     this -> Test_Preliminary_Consistency(("Called from constructor Array_Domain_Type(int,int,int,int)")); 
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          { this -> display(("Called from bottom of Array_Domain_Type constructor")); } 
}           // APP_ABORT();
#endif


#if defined(APP) || defined(PPP) 

void Array_Domain_Type::Initialize_Domain(int Number_Of_Valid_Dimensions,Internal_Index * const * Internal_Index_List,
const Internal_Partitioning_Type & Internal_Partition)


   { 
   // This is all new code as of 11/17/97 (check for bugs)
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List; 

     int temp=0; 
     for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
        { 
        // Internal_Index_List[temp]->display("Index in Array_Domain_Type::Initialize_Domain");
        // Integer_List[temp] = Internal_Index_List[temp]->getLength() - APP_Global_Array_Base;
          Integer_List[temp] = ((*Internal_Index_List[temp]).getLength)(); 
     } 

     this -> Initialize_Domain(Number_Of_Valid_Dimensions,Integer_List,Internal_Partition); 

     for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
          { this -> setBase((*Internal_Index_List[temp]).getBase(),temp); } 

#if COMPILE_DEBUG_STATEMENTS
     this -> Test_Preliminary_Consistency(("Called from constructor Array_Domain_Type(int,int,int,int)")); 
} 
#endif

#endif

     //=========================================================
     // ... this is APP or PPP in i n c l u d e file ...
#if defined(APP) || defined(PPP) 

void Array_Domain_Type::Initialize_Domain(const int * Integer_List,const Internal_Partitioning_Type & Internal_Partition)


   { 
     this -> Initialize_Domain(6,Integer_List,Internal_Partition); 
} 

#if 0







#endif
     // ... end of PPP ...
#endif

     //=========================================================
     // ... this is PPP in i n c l u d e file ...
#if defined(APP) || defined(PPP)

void Array_Domain_Type::Initialize_Domain(int Number_Of_Valid_Dimensions,const int * Integer_List,
const Internal_Partitioning_Type & Internal_Partition)


   { 
   // Maybe this function should call the partition function instead of 
   // reproducing some of its functionality here!
#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS

#endif

     this -> Initialize_Domain(Number_Of_Valid_Dimensions,Integer_List); 

#if defined(PPP)
     // Callers of this function must handle reference counting for the Internal_Partitioning_Type object



     // Bugfix (9/9/2000) all trailing dimensions must have zero ghost boundary widths
     // so we can only reset valid dimensions with nonzero ghost boundary widths
     // The LocalGhostCellWidth may be set to non zero values before being used
     // to build a P++ array object (this is reset from the default values set by 
     // the preevious call to Initialize_Domain (above))










     // Now set the Partitioning_Object_Pointer to point to the input partitioning object
     // Partitioning_Object_Pointer = &((Internal_Partitioning_Type&) Internal_Partition);
#endif

#if COMPILE_DEBUG_STATEMENTS
     this -> Test_Preliminary_Consistency(("Called from Array_Domain_Type::Initialize_Domain(int,int,int,int,int)")); 
} 
#endif

     // ... end of PPP ...
#endif

     //=========================================================
     // ... this is PPP in i n c l u d e file ...
#if defined(PPP)



     // The mechanism to take an exiting distributed array and build a P++ array object is not fully implemented as yet!

     // These macros help us use the input variables so that the 
     // compiler will not warn about unused variables






#if COMPILE_DEBUG_STATEMENTS

#endif

     // ... end of PPP ...
#endif


     // ************************************************************************************
     // ************************************************************************************
     //        General Initialization of Domain objects (for Indexing operations) 
     // ************************************************************************************
     // ************************************************************************************


void Array_Domain_Type::Initialize_Domain(const Array_Domain_Type & X,Internal_Index * const * Internal_Index_List)


{ 
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
   { 
      printf(("Inside of Array_Domain_Type::Initialize_Domain ")); 
      printf(("( const Array_Domain_Type & X , Internal_Index_List )\n")); 
   } 
#endif

   if (!(Internal_Index_List != (Internal_Index * const * )0)){ APP_Assertion_Support("domain.C",
   2592); 
   } 
int i=0; 
   for (i = 0; i < 6; i++)
      { 
        (this -> Index_Array)[i] = (intArray * )0; 

#if defined(PPP)
        // Used to support P++ indirect addressing

#endif

        // if defined(USE_EXPRESSION_TEMPLATES)
        (this -> IndexBase)[i] = 0; 
        (this -> IndexStride)[i] = 0; 
        (this -> IndexDataPointer)[i] = (int * )0; 
   }         // endif


        // Domain reference counting is currently not used but will be used later!
   this -> referenceCount = Array_Domain_Type::getReferenceCountBase(); 

   this -> Uses_Indirect_Addressing = 0; 
   this -> Domain_Dimension = X.Domain_Dimension; 

   // I think this is the proper constraint (May 11, 1997)
   // ... (remove this assert, SerialArray might not live on this processor) ...
   //   APP_ASSERT (Domain_Dimension > 0);

#if 0
   // It makes sense that these should be set in a function that is supposed to Initialize the Domain
   // I'm not sure why they had not been set previously (Dan, 5/31/97)

   // if defined(USE_EXPRESSION_TEMPLATES)

   // endif
#endif

#if defined(PPP)


#endif

   // If we have a Null Array then set all the bases and bounds to a consistant value.
   // This was suggested by Kristi.  It is tested for in the Test_Consistancy
   // function.  Doing the initialization here means that we only add the overhead
   // of a conditional test to the cost of the normal indexing. (This is really not
   // a big deal).
   if (this -> Is_A_Null_Array == 1)
   { 
   // We force this to be FALSE in the case of a Null Array (generated by an 
   // Null_Index).
      this -> Is_Contiguous_Data = 0; 

      for (i = 0; i < 6; i++)
      { 
         (this -> Base)[i] = 0; 
         (this -> Bound)[i] = -1; 
         // Not certain the stride should be reset (but this is done along 
         // the axis with the Null_Index so we duplicate it here).
         (this -> Stride)[i] = 1; 
      } 
   } 
#if defined(PPP)
         // I think we have to set the Local_Mask_Index as well.
         // ... (5/22/98,kdb) fix Global_Index too ...


#endif



         // This is set so that indexed views will be handled correctly
   this -> Is_A_View = 1; 

   // A view is not a temporary 
   // Unless the original array was a temporary then the view is temporary
   // AND if the data is contiguous then the data may be stold within the
   // binary operations.

   // Make sure that X.Is_A_Temporary uses either TRUE or FALSE (1 or 0) for values
   if (!((X.Is_A_Temporary == 1) || (X.Is_A_Temporary == 0))){ APP_Assertion_Support("domain.C",
   2672); 
   }    // Bugfix (1/26/96) a view should not be a temporary since it could then be
   // deleted within the delete if temporary function if the destructors
   // for the view were called too early as is the case on the SGI.
   // The problem appeared in the P6 = -12 + (P5+P4)(all,all,3) test in
   // the testcode.C program that Kristi wrote.
   // What this means is that a view of a temporary is not a temporary in the A++
   // sense because the view is on the local scope and its destructor is called
   // by the compiler where as an A++ temporary is on the heap and its destructor is
   // called by A++.  If we didn't have it this way then the compiler could delete
   // something allocated within local scope as a local variable.

   this -> Is_A_Temporary = 0; 

   // This allows the operator()() to be a contiguous data view
   // so that the data can be swaped in this special case which is
   // important for the support of conditional operations on the 
   // multidimensional arrays (which we will move to a little later).
   bool Is_Data_Base_Constant=true; 
   int temp; 
   for (temp = 1; temp < 6; temp++)
      { Is_Data_Base_Constant = (Is_Data_Base_Constant?((this -> Data_Base)[0] == (this
       -> Data_Base)[temp]):0); } 
   this -> Constant_Data_Base = Is_Data_Base_Constant; 

   // Bug fix (8/18/94) only equality between the Strides was checked
   // must test for Unit stride (Stride[0] == 1)
   bool Is_Unit_Stride=true; 
   for (temp = 0; temp < 6; temp++)
        { Is_Unit_Stride = (Is_Unit_Stride?((this -> Stride)[temp] == 1):0); } 
   this -> Constant_Unit_Stride = Is_Unit_Stride; 

   // Bug fix (8/18/94) case of non unit stride view flaged a Contiguous of bases and 
   // bound match -- AN ERROR 
   // Is_Contiguous_Data is automatically FALSE if view is non unit stride ( BUG 35 
   // -- Jeff Saltzman )
   if (this -> Constant_Unit_Stride && !this -> Uses_Indirect_Addressing)
   { 
      bool Is_Contiguous_Data_Test=X.Is_Contiguous_Data; 
      for (temp = 0; temp < 6; temp++)
         { Is_Contiguous_Data_Test = (Is_Contiguous_Data_Test?(((this -> Base)[temp] == (X.Base)[temp])
          && ((this -> Bound)[temp] == (X.Bound)[temp])):0); } 
        this -> Is_Contiguous_Data = Is_Contiguous_Data_Test; 
   } 
   else 
   { 
      this -> Is_Contiguous_Data = 0; 
   } 

      // A view gets the same Array_ID as that which it is a view of!
      // this is how the dependence is expressed between the view and the actual array 
      // variable.
   this -> internalArrayID = X.internalArrayID; 

   // Bug Fix (9/9/93) by dquinlan
   // Views are not anything special under defered evaluation! So set them false! 
   // (WRONG) Actually views are special since the view must have its lifetime 
   // extended too!
   this -> Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION; 

   // This is a view so these should be the same.
   this -> builtUsingExistingData = X.builtUsingExistingData; 

   // if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset=(this -> Base)[0]; 
     for (temp = 1; temp < 6; temp++)
        { 
          ExpectedOffset += (this -> Base)[temp] * (this -> Size)[(temp - 1)]; 
     } 
     this -> ExpressionTemplateOffset = ExpectedOffset; 
     if (!(this -> ExpressionTemplateOffset == ExpectedOffset)){ APP_Assertion_Support("domain.C",
     2743); 
     }      // endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
       { 
         printf(("Leaving Initialize_Domain -- X.Domain_Dimension = ")); 
         printf(("%d Domain_Dimension = %d \n"),X.Domain_Dimension,this -> Domain_Dimension); 
     } 
} 
#endif


#if defined(PPP)
         // ******************************************************************************
         //     Initialization for when the SerialArray in a P++ array is a NullArray
         // ******************************************************************************



         // This function is called by the array object constructors where in the indexing of an array object
         // the case of a serialArray being a NullArray (because of indexing of the global array).

#if COMPILE_DEBUG_STATEMENTS




#endif

         // This is the only information cant needs to be reset in this case








#endif

         // ******************************************************************************
         //                     Initialization for indirect addressing 
         // ******************************************************************************

void Array_Domain_Type::Initialize_Domain(const Array_Domain_Type & X,Internal_Indirect_Addressing_Index * const * Indirect_Index_List)

{ 
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      { 
        printf(("Inside of Array_Domain_Type::Initialize_Domain ")); 
        printf(("( const Array_Domain_Type & X , Indirect_Index_List )\n")); 
   } 
#endif

        // Initialize this for purify
   this -> Uses_Indirect_Addressing = 0; 

   int temp=0; 
   for (temp = 0; temp < 6; temp++)
      { 
#if defined(PPP)
      // Used to support P++ indirect addressing

#endif

      // Initialize this to avoid purify warnings
        (this -> Scalar_Offset)[temp] = 0; 
   } 

   if (!(Indirect_Index_List != (Internal_Indirect_Addressing_Index * const * )0)){ APP_Assertion_Support("domain.C",
   2814); 
   } 
   if (X.Uses_Indirect_Addressing)
   { 
   // Case of indexing a descriptor previous indexed by indirectaddressing
      this -> Initialize_IndirectAddressing_Domain_Using_IndirectAddressing(X,Indirect_Index_List); 
   } 
   else 
   { 
      if (!(X.Uses_Indirect_Addressing == 0)){ APP_Assertion_Support("domain.C",2823); } 
      // Case of nonindirectaddressing descriptor accessed buy indirectadressing
      this -> Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing(X,Indirect_Index_List); 
   } 

      // Domain reference counting is currently not used but will be used later!
   this -> referenceCount = Array_Domain_Type::getReferenceCountBase(); 

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      { printf(("Marking descriptor as Uses_Indirect_Addressing = TRUE! \n")); } 
#endif

      // Now set this to be true because we should have been handed at least one valid intArray in the indexing list
   this -> Uses_Indirect_Addressing = 1; 

   // Dimension of view built using indirect addressing is always 1D!
   this -> Domain_Dimension = 1; 

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      { printf(("Domain_Dimension = %d  Uses_Indirect_Addressing = %s \n"),this -> Domain_Dimension,
      (this -> Uses_Indirect_Addressing)?"TRUE":"FALSE"); } 
#endif

#if 0
      // It makes sense that these should be set in a function that is supposed to Initialize the Domain
      // I'm not sure why they had not been set previously (Dan, 5/31/97)

      // if defined(USE_EXPRESSION_TEMPLATES)

      // endif
#endif

#if defined(PPP)

#if defined(USE_PADRE)

#endif

#endif

      // This is set so that indexed views will be handled correctly
   this -> Is_A_View = 1; 

   // A view is not a temporary in that it's data is not temporary and thus
   // can't be stolen for use in binary operators (actually Array_Data == NULL
   // and all references are made through the Original_Array field).
   // Unless the original array was a temporary then the view is temporary
   // AND if the data is contiguous then the data may be stold within the
   // binary operations.

   // Bugfix (3/17/96) This makes the setting ot this value consistant with the 
   // function for Index object
   this -> Is_A_Temporary = 0; 

   // This allows the operator()() to be a contiguous data view
   // so that the data can be swaped in this special case which is
   // important for the support of conditional operations on the 
   // multidimensional arrays (which we will move to a little later).

   bool Is_Data_Base_Constant=true; 
   for (temp = 1; temp < 6; temp++)
      { Is_Data_Base_Constant = (Is_Data_Base_Constant?((this -> Data_Base)[0] == (this
       -> Data_Base)[temp]):0); } 
   this -> Constant_Data_Base = Is_Data_Base_Constant; 

   // Bug fix (8/18/94) only equality between the Strides was checked
   // must test for Unit stride (Stride[0] == 1)
   bool Is_Unit_Stride=true; 
   for (temp = 0; temp < 6; temp++)
      { Is_Unit_Stride = (Is_Unit_Stride?((this -> Stride)[temp] == 1):0); } 
   this -> Constant_Unit_Stride = Is_Unit_Stride; 

   // Bug fix (8/18/94) case of non unit stride view flaged a Contiguous of bases 
   // and bound match -- AN ERROR
   // Is_Contiguous_Data is automatically FALSE if view is non unit stride 
   // ( BUG 35 -- Jeff Saltzman )
   if (this -> Constant_Unit_Stride && !this -> Uses_Indirect_Addressing)
   { 
      bool Is_Contiguous_Data_Test=X.Is_Contiguous_Data; 
      for (temp = 0; temp < 6; temp++)
         { Is_Contiguous_Data_Test = (Is_Contiguous_Data_Test?(((this -> Base)[temp] == (X.Base)[temp])
          && ((this -> Bound)[temp] == (X.Bound)[temp])):0); } 
      this -> Is_Contiguous_Data = Is_Contiguous_Data_Test; 
   } 
   else 
   { 
      this -> Is_Contiguous_Data = 0; 
   } 

      // A view gets the same Array_ID as that which it is a view of!
      // this is how the dependence is expressed between the view and the actual array 
      // variable.
   this -> internalArrayID = X.internalArrayID; 

   // Bug Fix (9/9/93) by dquinlan
   // Views are not anything special under defered evaluation! So set them false! 
   // (WRONG) Actually views are special since the view must have its lifetime 
   // extended too!
   this -> Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION; 

   // This is a view so these should be the same.
   this -> builtUsingExistingData = X.builtUsingExistingData; 

   // if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset=(this -> Base)[0]; 
     for (temp = 1; temp < 6; temp++)
        { 
          ExpectedOffset += (this -> Base)[temp] * (this -> Size)[(temp - 1)]; 
     } 
     this -> ExpressionTemplateOffset = ExpectedOffset; 
     if (!(this -> ExpressionTemplateOffset == ExpectedOffset)){ APP_Assertion_Support("domain.C",
     2935); 
     }      // endif

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 3)
      { 
      // At this point the domain is not completly initialized so we get UMR: Uninitialized memory read errors from PURIFY
      // display ("Called from Array_Domain_Type::Initialize_Domain (Array_Domain_Type,Indirect_Index_List)");
        printf(("Leaving Array_Domain_Type::Initialize_Domain ")); 
        printf(("( const Array_Domain_Type & X , Indirect_Index_List )\n")); 
   } 
} 
#endif



void Array_Domain_Type::reshape(int Number_Of_Valid_Dimensions,const int * View_Sizes,const int * View_Bases)
   { 
   // This function is only called by the reshape member function.
   // We could just call it the reshape function for a domain (that would make more sense!)

     int temp=0; 

     (this -> Size)[0] = View_Sizes[0]; 
     for (temp = 1; temp < Number_Of_Valid_Dimensions; temp++)
        { 
          (this -> Size)[temp] = (this -> Size)[(temp - 1)] * View_Sizes[temp]; 

          if (!(View_Sizes[temp] >= 0)){ APP_Assertion_Support("domain.C",2962); } 
          if (View_Sizes[temp] == 0)
               { this -> Is_A_Null_Array = 1; } 
     } 

               // Initialize the other dimensions to avoid errors in purify (and errors in general!)
     for (temp = Number_Of_Valid_Dimensions; temp < 6; temp++)
          { (this -> Size)[temp] = (this -> Size)[(temp - 1)]; } 

          // ... Since this is a view this is probably false and even if the data
          // is contiguous this will only make the code slightly less efficient ...
     this -> Is_Contiguous_Data = 0; 

     this -> Is_A_View = 1; 

     // ... Scalar_Offset won't be 0 in rare case where APP_Global_Array_Base
     // is nonzero and Size has changed so reset this ...
     (this -> Scalar_Offset)[0] = -(this -> User_Base)[0] * (this -> Stride)[0]; 
     for (temp = 1; temp < 6; temp++)
        { 
          (this -> Scalar_Offset)[temp] = (this -> Scalar_Offset)[(temp - 1)] - ((this
           -> User_Base)[temp] * (this -> Stride)[temp]) * (this -> Size)[(temp - 1)]; 
     } 
          // ... fix bases so that view will be in correct place ...
     for (temp = 0; temp < Number_Of_Valid_Dimensions; temp++)
        { 
          (this -> Base)[temp] = View_Bases[temp]; 
          (this -> Bound)[temp] += (this -> Base)[temp]; 
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
Array_Domain_Type::Array_Domain_Type(const Array_Domain_Type & X,Internal_Index * const * Index_List)


   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        { 
          printf(("Inside of Array_Domain_Type::Array_Domain_Type ")); 
          printf(("( const Array_Domain_Type & X , Internal_Index** Index_List )")); 
          printf(("(this = %p)\n"),this); 
     } 
#endif

     if (X.Uses_Indirect_Addressing == 1)
        { 
          printf(("Sorry, not implemented: Case of Index objects use upon view built ")); 
          printf(("from indirect addressing not implemented \n")); 
          APP_ABORT(); 
     } 

          // bug fix (8/19/96,kdb) previous code didn't work with views of views
          // View_Offset = 0;
          // if defined(USE_EXPRESSION_TEMPLATES)
     this -> ExpressionTemplateOffset = X.ExpressionTemplateOffset; 
     // endif
     this -> View_Offset = X.View_Offset; 
     this -> Preinitialize_Domain(X); 
     int i=0; 
     for (i = 0; i < 6; i++)
        { 
          if (Index_List[i] != (Internal_Index * )0)
               { this -> Initialize_View_Of_Target(i,*Index_List[i],X); } 
          else 
               { this -> Initialize_Non_Indexed_Dimension(i,X); } 

          if (i == 0)
               { this -> View_Offset += ((this -> User_Base)[i] - (X.User_Base)[i]) * (X.Stride)[i]; } 
          else 
               { this -> View_Offset += (((this -> User_Base)[i] - (X.User_Base)[i]) * (X.Stride)[i])
                * (this -> Size)[(i - 1)]; } 
               // if defined(USE_EXPRESSION_TEMPLATES)
          if (i == 0)
               { this -> ExpressionTemplateOffset += (this -> Base)[i] - (X.Base)[i]; } 
          else 
               { this -> ExpressionTemplateOffset += ((this -> Base)[i] - (X.Base)[i])
                * (this -> Size)[(i - 1)]; } 
     }                // endif


#if defined(PPP)

#if defined(USE_PADRE)

#endif
#endif
     this -> Initialize_Domain(X,Index_List); 

#if 0


#endif

     // printf ("BEFORE RESET: View_Offset = %d Stride[0] = %d \n",View_Offset,Stride[0]);
     // printf ("BEFORE RESET: View_Offset = %d Stride[0] = %d User_Base[0] = %d \n",View_Offset,Stride[0],User_Base[0]);

     (this -> Scalar_Offset)[0] = -(this -> User_Base)[0] * (this -> Stride)[0]; 
     (this -> Scalar_Offset)[0] += this -> View_Offset; 
     // Scalar_Offset[0] += View_Offset * Stride[0];
     for (i = 1; i < 6; i++)
          { (this -> Scalar_Offset)[i] = (this -> Scalar_Offset)[(i - 1)] - ((this -> User_Base)[i]
           * (this -> Stride)[i]) * (this -> Size)[(i - 1)]; } 
#if 0


#endif

     if (!(this -> internalArrayID == X.internalArrayID)){ APP_Assertion_Support("domain.C",
     3076); 
     } 
#if COMPILE_DEBUG_STATEMENTS
     this -> Test_Consistency(("Called from Array_Domain_Type (Array_Domain_Type,Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type*) \n")); 
} 
#endif




Array_Domain_Type & Array_Domain_Type::displayReferenceCounts(const char * label) const
   { 
     printf(("     Inside of Array_Domain_Type::displayReferenceCounts(%s) \n"),label); 
     printf(("          Array_Domain_Type Reference Count (Array id = %d) = %d \n"),this
      -> Array_ID(),this -> getReferenceCount()); 
#if defined(PPP)









#endif

     // optionally print out the reference count information about the indirection vectors
     if ((this -> usesIndirectAddressing)() == 1)
        { 
          int i=0; 
          for (i = 0; i < 6; i++)
             { 
               if ((this -> Index_Array)[i] != (intArray * )0)
                  { 
                    printf(("          indirect addressing intArray [%d] -- Array Reference Count (Array id = %d) = %d \n"),
                    i,(*(this -> Index_Array)[i]).Array_ID(),(*(this -> Index_Array)[i]).getReferenceCount()); 
                    (*(this -> Index_Array)[i]).displayReferenceCounts(); 
               } 
               else 
                  { 
                    printf(("          indirect addressing intArray [%d] == NULL \n"),
                    i); 
               } 
          } 
     } 
                    // Cast away const here
     return ((Array_Domain_Type & )*this);
} 

     // ************************************************************************
     // Array_Domain_Type constructors for indirect addressing
     // ************************************************************************

Array_Domain_Type::Array_Domain_Type(const Array_Domain_Type & X,Internal_Indirect_Addressing_Index * const * Indirect_Index_List)

   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Array_Domain_Type ( const Array_Domain_Type & X , Internal_Indirect_Addressing_Index* Indirect_Index_List ) (this = %p)\n"),
          this); } 
#endif

          // *******************************************************************
          // This code is derived from the similar Array_Domain_Type for Internal_Index objects
          // *******************************************************************
          // if defined(USE_EXPRESSION_TEMPLATES)
     this -> ExpressionTemplateOffset = X.ExpressionTemplateOffset; 
     // endif
     this -> View_Offset = X.View_Offset; 
     // *******************************************************************

     this -> Preinitialize_Domain(X); 

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     // NOTE: This loop over the number of dimensions is at least somewhat 
     //       redundent with the one in the Initialize_Domain function!


#endif

     int i=0; 
     for (i = 0; i < 6; i++)
        { 
          if (Indirect_Index_List[i] != (Internal_Indirect_Addressing_Index * )0)
             { 
               if (!(((*Indirect_Index_List[i]).intArrayInStorage == 0) || ((*Indirect_Index_List[i]).IndexInStorage
                == 0))){ APP_Assertion_Support("domain.C",3160); } 

               if ((*Indirect_Index_List[i]).intArrayInStorage == 1)
                  { 
                  // printf ("In indirect addressing constructor: Axis = %d Indirect_Index_List[%d]->intArrayInStorage == TRUE \n",i,i);
                    this -> Initialize_Indirect_Addressing_View_Of_Target(i,*(*Indirect_Index_List[i]).IndirectionArrayPointer,
                    X); 
               } 

               else if ((*Indirect_Index_List[i]).IndexInStorage == 1)
                       { 
                       // printf ("In indirect addressing constructor: Axis = %d Indirect_Index_List[%d]->IndexInStorage == TRUE \n",i,i);
                         this -> Initialize_Indirect_Addressing_View_Of_Target(i,(*Indirect_Index_List[i]).Index_Data,
                         X); 
               } 
               else 
                       { 
               // printf ("In indirect addressing constructor: Axis = %d Indirect_Index_List[%d]->IndexInStorage == FALSE \n",i,i);
                         this -> Initialize_Indirect_Addressing_View_Of_Target(i,(*Indirect_Index_List[i]).Scalar,
                         X); 
               } 
          } 
          else 
             { 
             // printf ("In indirect addressing constructor: Axis = %d Non Indexed dimension \n",i,i);
               this -> Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension(i,
               X); 
          } 
               // *******************************************************************
               // This code is derived from the similar Array_Domain_Type for Internal_Index objects
               // *******************************************************************
          if (i == 0)
               { this -> View_Offset += ((this -> User_Base)[i] - (X.User_Base)[i]) * (X.Stride)[i]; } 
          else 
               { this -> View_Offset += (((this -> User_Base)[i] - (X.User_Base)[i]) * (X.Stride)[i])
                * (this -> Size)[(i - 1)]; }                // if defined(USE_EXPRESSION_TEMPLATES)
          if (i == 0)
               { this -> ExpressionTemplateOffset = (this -> Base)[i] - (X.Base)[i]; } 
          else 
               { this -> ExpressionTemplateOffset = ((this -> Base)[i] - (X.Base)[i])
                * (this -> Size)[(i - 1)]; } 
     }                // endif
               // *******************************************************************

               // display ("Axis by axis -- intermediate Domain in constructor Array_Domain_Type (Array_Domain_Type,Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type) \n");


               // This function overwrites some of the date just setup (see note above)
     this -> Initialize_Domain(X,Indirect_Index_List); 

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 2)
        { 
          this -> displayReferenceCounts(("AFTER Initialize_Domain ( X , Indirect_Index_List ) (for indirect addressing)")); 
     } 
#endif

#if defined(PPP)

#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Before call to Test_Consistency! (this = %p)\n"),this); } 
#endif

#if COMPILE_DEBUG_STATEMENTS
     this -> Test_Consistency(("Called from Array_Domain_Type (Array_Domain_Type,Internal_Indirect_Addressing_Index*)")); 
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Leaving Array_Domain_Type::Array_Domain_Type ( const Array_Domain_Type & X , Internal_Indirect_Addressing_Index* Indirect_Index_List ) (this = %p)\n"),
          this); } 
#endif

          // *******************************************************************
          // This code is derived from the similar Array_Domain_Type for Internal_Index objects
          // *******************************************************************
   (this -> Scalar_Offset)[0] = -(this -> User_Base)[0] * (this -> Stride)[0]; 
   (this -> Scalar_Offset)[0] += this -> View_Offset; 
   for (i = 1; i < 6; i++)
        { (this -> Scalar_Offset)[i] = (this -> Scalar_Offset)[(i - 1)] - ((this -> User_Base)[i]
         * (this -> Stride)[i]) * (this -> Size)[(i - 1)]; }         // *******************************************************************
        // Old code (which was inconsistant with the non-indirect addressing code)
        // View_Offset = 0;
        // for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        //   Scalar_Offset[i] = 0;
        // *******************************************************************

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 2)
        { 
          this -> displayReferenceCounts(("At END of constructor (for indirect addressing)")); 
     } 
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





          // Make sure we don't strand an existing (different PADRE descriptor)

#endif

          // Use the PADRE_Descriptor from the source of the view!



#endif
#endif
#endif


Array_Domain_Type::Array_Domain_Type()
   { 
   // This builds the NULL Array_Domain (used in NULL arrays)!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        { 
          printf(("Inside of Array_Domain_Type::Array_Domain_Type() this = %p \n"),this); 
     }           // printf ("WARNING: It is a ERROR to use this descriptor constructor except for NULL arrays! \n");
          // APP_ABORT();

#endif

     int Integer_List[6]; 
     // We have to initialize this to avoid UMR in purify
     Integer_List[0] = 0; 
     this -> Initialize_Domain(0,Integer_List); 
} 


#ifdef USE_STRING_SPECIFIC_CODE

     // this is non-portable code (that we are currently testing)
     // include <string.h>

#if 0








#endif


void Array_Domain_Type::parseString(const char * dataString,int & pos,int * dimentionArray)
   { 
   // This function parses a string to d e f i n e the data that will be stored 
   // in an array object.  Current problem with this function is that it 
   // does not correctly interprete the dimensions of the array in the case
   // of a multidimensional array declaration (as a string).

     static int dimension=(0); 
     static int maxDimension=(0); 

     // skip leading white space
     pos += strspn((dataString + pos),(" ")); 

     while(dataString[pos] == 123)
        { 
          dimension++; 
          maxDimension = ((dimension > maxDimension)?dimension:maxDimension); 

          pos++; 

          // Figure out the array dimensions only increment dimensions
          // that have not previously been incremented
          // Note that the lowest dimension (the highest in the permuted
          // dimensions such as what we compute here) MUST be computed separately
          // (since it is the inner most string).
          if (dimension >= 2)
             { 
               dimentionArray[(dimension - 2)]++; 
               // reset lower level entries in the dimentionArray
               for (int i=dimension - 1; i < 6; i++)
                    { dimentionArray[i] = 0; } 
          } 

                    // printf ("dimension = %d \n",dimension);
                    // for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
                    //      printf ("dimentionArray[%d] = %d \n",i,dimentionArray[i]);

          this -> parseString(dataString,pos,dimentionArray); 
          dimension--; 
     } 

          // Now read the number string!
          // printf ("&(dataString[pos]) = %s pos = %d  dimension = %d  maxDimension = %d \n",
          //      &(dataString[pos]),pos,dimension,maxDimension);
     char * delimiterListForInnerString=" 0123456789,"; 
     int length=strspn(dataString + pos,delimiterListForInnerString); 
     char * copyString=new char [length + 1]; 
     copyString[length] = 0; 
     strncpy(copyString,dataString + pos,length); 
     copyString[length] = 0; 

     if (strlen(copyString) > 0)
        { 
        // printf ("copyString = %s \n",copyString);

        // Now parse the inner string to get the values
          char * delimiterListForDataValues=" ,"; 
          char * nextSubString=((char * )0); 
          int counter=0; 
          if ((nextSubString = strtok(copyString,delimiterListForDataValues)) != (char * )0)
             { 
               counter = 1; 
               // printf ("nextSubString = %s \n",nextSubString);
               while((nextSubString = strtok(((char * )0),delimiterListForDataValues))
                != (char * )0){ 
               // printf ("nextSubString = %s \n",nextSubString);
                    counter++; 
               } 
               dimentionArray[(dimension - 1)] = counter; 
          } 
          else           // printf ("counter = %d dimension = %d dimentionArray[%d] = %d \n",
          //      counter,dimension,dimension,dimentionArray[dimension-1]);


             { } 
     }              // printf ("nextSubString = NULL \n");



     pos += strspn((dataString + pos),(" 0123456789,")); 

     if (dataString[pos] == 125)
        { 
        // printf ("Found a '}' at the end! (pos = %d) \n",pos);
          pos++; 
     } 
     else 
        { } 
        // printf ("Can NOT find a '}' at the end! (pos = %d) \n",pos);


     if (dataString[pos] == 44)
        { 
        // printf ("Found a comma! (pos = %d) \n",pos);
          pos++; 
     } 
     else 
        { } 
}         // printf ("Can NOT find a comma! (pos = %d) \n",pos);


#endif


        // Array_Domain_Type::Array_Domain_Type ( const char* dataString )
Array_Domain_Type::Array_Domain_Type(const AppString & dataString)
   { 
   // This builds an array from a string

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        { 
          printf(("Inside of Array_Domain_Type::Array_Domain_Type(const char*) this = %p \n"),
          this); 
     } 
#endif

#ifdef USE_STRING_SPECIFIC_CODE
     char * numberString=strdup(dataString.getInternalString()); 
#else







#endif

     int position=0; 

     int permutedDimentionArray[6]; 
     int i; 

     // All arrays are by default 1x1x1x...
     for (i = 0; i < 6; i++)
          { permutedDimentionArray[i] = 0; } 

#ifdef USE_STRING_SPECIFIC_CODE
     this -> parseString(numberString,position,permutedDimentionArray); 
#endif

     // for (i=0; i < MAX_ARRAY_DIMENSION; i++)
     //      printf ("permutedDimentionArray[%d] = %d \n",i,permutedDimentionArray[i]);

     i = 0; 
     while(i < 6 && permutedDimentionArray[i] != 0)
        { 
          i++; 
     } 

     int numberOfDimensions=i; 
     // printf ("numberOfDimensions = %d \n",numberOfDimensions);

     int dimentionArray[6]; 
     // Integer_Array_MAX_ARRAY_DIMENSION_Type dimentionArray;
     for (i = 0; i < numberOfDimensions; i++)
        { 
          dimentionArray[i] = permutedDimentionArray[((numberOfDimensions - i) - 1)]; 
     }           // printf ("dimentionArray[%d] = %d \n",i,dimentionArray[i]);


     int Integer_List[6]; 
     // We have to initialize this to avoid UMR in purify
     // Integer_List[0] = 0;
     // Initialize_Domain(0,Integer_List);

     for (i = 0; i < numberOfDimensions; i++)
        { 
          Integer_List[i] = dimentionArray[i]; 
     }           // printf ("Integer_List[%d] = %d \n",i,Integer_List[i]);


     this -> Initialize_Domain(numberOfDimensions,Integer_List); 
} 

Array_Domain_Type::Array_Domain_Type(const Internal_Index & I)
   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 

     // This has to be part of the non-internal debug code path
     for (int temp=1; temp < 6; temp++)
          { Internal_Index_List[temp] = (Internal_Index * )0; } 

     this -> Initialize_Domain(1,Internal_Index_List); 
} 
#if (MAX_ARRAY_DIMENSION >= 2)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J)
   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 

     // This has to be part of the non-internal debug code path
     for (int temp=2; temp < 6; temp++)
          { Internal_Index_List[temp] = (Internal_Index * )0; } 

     this -> Initialize_Domain(2,Internal_Index_List); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Index & K)
   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     Internal_Index_List[2] = (Internal_Index * )(&K); 

     // This has to be part of the non-internal debug code path
     for (int temp=3; temp < 6; temp++)
          { Internal_Index_List[temp] = (Internal_Index * )0; } 

     this -> Initialize_Domain(3,Internal_Index_List); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Index & K,const Internal_Index & L)
   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     Internal_Index_List[2] = (Internal_Index * )(&K); 
     Internal_Index_List[3] = (Internal_Index * )(&L); 

     // This has to be part of the non-internal debug code path
     for (int temp=4; temp < 6; temp++)
          { Internal_Index_List[temp] = (Internal_Index * )0; } 

     this -> Initialize_Domain(4,Internal_Index_List); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Index & K,const Internal_Index & L,const Internal_Index & M)

   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     Internal_Index_List[2] = (Internal_Index * )(&K); 
     Internal_Index_List[3] = (Internal_Index * )(&L); 
     Internal_Index_List[4] = (Internal_Index * )(&M); 

     // This has to be part of the non-internal debug code path
     for (int temp=5; temp < 6; temp++)
          { Internal_Index_List[temp] = (Internal_Index * )0; } 

     this -> Initialize_Domain(5,Internal_Index_List); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Index & K,const Internal_Index & L,const Internal_Index & M,const Internal_Index & N)

   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     Internal_Index_List[2] = (Internal_Index * )(&K); 
     Internal_Index_List[3] = (Internal_Index * )(&L); 
     Internal_Index_List[4] = (Internal_Index * )(&M); 
     Internal_Index_List[5] = (Internal_Index * )(&N); 

     // This has to be part of the non-internal debug code path
     for (int temp=6; temp < 6; temp++)
          { Internal_Index_List[temp] = (Internal_Index * )0; } 

     this -> Initialize_Domain(6,Internal_Index_List); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 7)














     // This has to be part of the non-internal debug code path





#endif
#if (MAX_ARRAY_DIMENSION >= 8)















     // This has to be part of the non-internal debug code path





#endif


Array_Domain_Type::Array_Domain_Type(int Number_Of_Valid_Dimensions,const int * Integer_List)


   { 
     this -> Initialize_Domain(Number_Of_Valid_Dimensions,Integer_List); 
} 
#if defined(USE_PADRE) && defined(PPP)
     // The present implementation of PADRE requires the use of the object we just
     // constructed so we have to initialize the PADRE object AFTER we have initialized the
     // this object (the parallel Array_Domain_Type).
     // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
     // parallelPADRE_DescriptorPointer = 
     //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
     // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);

#endif
#endif


Array_Domain_Type::Array_Domain_Type(int Number_Of_Valid_Dimensions,Internal_Index * const * Internal_Index_List)


   { 
     this -> Initialize_Domain(Number_Of_Valid_Dimensions,Internal_Index_List); 
} 
#if defined(USE_PADRE) && defined(PPP)
     // The present implementation of PADRE requires the use of the object we just
     // constructed so we have to initialize the PADRE object AFTER we have initialized the
     // this object (the parallel Array_Domain_Type).
     // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
     // parallelPADRE_DescriptorPointer = 
     //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
     // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);

#endif
#endif


Array_Domain_Type::Array_Domain_Type(const int * Integer_List)
   { 
     this -> Initialize_Domain(6,Integer_List); 
} 
#if defined(USE_PADRE) && defined(PPP)
     // The present implementation of PADRE requires the use of the object we just
     // constructed so we have to initialize the PADRE object AFTER we have initialized the
     // this object (the parallel Array_Domain_Type).
     // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
     // parallelPADRE_DescriptorPointer = 
     //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
     // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);

#endif
#endif


Array_Domain_Type::Array_Domain_Type(int i,int j,int k,int l,int m,int n)
   { 
Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List={i, j, k, l, m, n}; 
     this -> Initialize_Domain(6,Integer_List); 
} 
#if defined(USE_PADRE) && defined(PPP)
     // The present implementation of PADRE requires the use of the object we just
     // constructed so we have to initialize the PADRE object AFTER we have initialized the
     // this object (the parallel Array_Domain_Type).
     // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
     // parallelPADRE_DescriptorPointer = 
     //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
     // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);

#endif
#endif


#if defined(PPP)




#if defined(USE_PADRE) && defined(PPP)
     // The present implementation of PADRE requires the use of the object we just
     // constructed so we have to initialize the PADRE object AFTER we have initialized the
     // this object (the parallel Array_Domain_Type).
     // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
     // parallelPADRE_DescriptorPointer =
     //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
     // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);

#endif
#endif

#endif

#if defined(PPP) || defined(APP)
     // Array_Domain_Type::Array_Domain_Type( 
     //           int Input_Array_Size_I , int Input_Array_Size_J , 
     //           int Input_Array_Size_K , int Input_Array_Size_L ,
     //           const Partitioning_Type & Partition ) 
Array_Domain_Type::Array_Domain_Type(int Number_Of_Valid_Dimensions,const int * Integer_List,
const Internal_Partitioning_Type & Internal_Partition)


   { 
#if 1
     this -> Initialize_Domain(Number_Of_Valid_Dimensions,Integer_List,Internal_Partition); 
#else




     // The LocalGhostCellWidth may be set to non zero values before being used
     // to build a P++ array object



     // InternalGhostCellWidth [temp] = Internal_Partition.LocalGhostCellWidth [temp];


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

#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     this -> Test_Preliminary_Consistency(("Called from constructor Array_Domain_Type(int,int,int,int)")); 
} 
#endif

#endif // ... end of PPP ...

#if defined(APP) || defined(PPP)
Array_Domain_Type::Array_Domain_Type(int Number_Of_Valid_Dimensions,Internal_Index * const * Internal_Index_List,
const Internal_Partitioning_Type & Internal_Partition)


   { 
     this -> Initialize_Domain(Number_Of_Valid_Dimensions,Internal_Index_List,Internal_Partition); 
} 
#if defined(USE_PADRE) && defined(PPP)
     // The present implementation of PADRE requires the use of the object we just
     // constructed so we have to initialize the PADRE object AFTER we have initialized the
     // this object (the parallel Array_Domain_Type).
     // parallelDescriptor.intializeConstructor(this);
#if !defined(TEST_USE_OF_PADRE)
     // parallelPADRE_DescriptorPointer = 
     //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> (this);
     // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);

#endif
#endif


Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Partitioning_Type & Internal_Partition)


   { 
   // I.display("Internal_Index in Array_Domain_Type::Array_Domain_Type(I,Internal_Partition)");
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     this -> Initialize_Domain(1,Internal_Index_List,Internal_Partition); 
} 
#if (MAX_ARRAY_DIMENSION >= 2)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Partitioning_Type & Internal_Partition)

   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     this -> Initialize_Domain(2,Internal_Index_List,Internal_Partition); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Index & K,const Internal_Partitioning_Type & Internal_Partition)


   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     Internal_Index_List[2] = (Internal_Index * )(&K); 
     this -> Initialize_Domain(3,Internal_Index_List,Internal_Partition); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Index & K,const Internal_Index & L,const Internal_Partitioning_Type & Internal_Partition)


   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     Internal_Index_List[2] = (Internal_Index * )(&K); 
     Internal_Index_List[3] = (Internal_Index * )(&L); 
     this -> Initialize_Domain(4,Internal_Index_List,Internal_Partition); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Index & K,const Internal_Index & L,const Internal_Index & M,const Internal_Partitioning_Type & Internal_Partition)



   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     Internal_Index_List[2] = (Internal_Index * )(&K); 
     Internal_Index_List[3] = (Internal_Index * )(&L); 
     Internal_Index_List[4] = (Internal_Index * )(&M); 
     this -> Initialize_Domain(5,Internal_Index_List,Internal_Partition); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
Array_Domain_Type::Array_Domain_Type(const Internal_Index & I,const Internal_Index & J,
const Internal_Index & K,const Internal_Index & L,const Internal_Index & M,const Internal_Index & N,
const Internal_Partitioning_Type & Internal_Partition)


   { 
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; 
     Internal_Index_List[0] = (Internal_Index * )(&I); 
     Internal_Index_List[1] = (Internal_Index * )(&J); 
     Internal_Index_List[2] = (Internal_Index * )(&K); 
     Internal_Index_List[3] = (Internal_Index * )(&L); 
     Internal_Index_List[4] = (Internal_Index * )(&M); 
     Internal_Index_List[5] = (Internal_Index * )(&N); 
     this -> Initialize_Domain(6,Internal_Index_List,Internal_Partition); 
} 
#endif
#if (MAX_ARRAY_DIMENSION >= 7)

















#endif
#if (MAX_ARRAY_DIMENSION >= 8)


















#endif
#endif

     // *****************************************************************************
     // *****************************************************************************
     // ***********************  MEMBER FUNCTION DEFINITIONS  ***********************
     // *****************************************************************************
     // *****************************************************************************


int Array_Domain_Type::getGhostBoundaryWidth(int axis) const
   { 
   // Error checking on input value
     if (!(axis < 6)){ APP_Assertion_Support("domain.C",3936); } 
     if (!(axis >= 0)){ APP_Assertion_Support("domain.C",3937); } 

#if defined(PPP)
     // We want to report the array objects ghost boundary width which might be 
     // different from that of the partitioning since the partitioning is a guide 
     // and the individual array objects can have their ghost boundaries setup separately

#if 0










#endif

#else
     // Avoid compiler warning about unused input variable
     if (&axis){ } 
     return 0;
} 
#endif




void Array_Domain_Type::setInternalGhostCellWidth(int * Integer_List)

   { 
#if defined(PPP) 
   // This function modifies both the parallel and the serial Array_Domain


#if COMPILE_DEBUG_STATEMENTS







#endif

   // Modify the parallel descriptor
   // get the old BlockPartiArrayDomain and build a new one






   // Initialize arrays used to construction of BlockPartiArrayDomain


   // Initialize descriptor data with new values



   // We want the array sizes with no ghost boundaries (I think this is what PARTI requires)
   // Array_Sizes            [temp] += Integer_List [temp] - InternalGhostCellWidth [temp];
   // 2 * InternalGhostCellWidth [temp];
   // printf ("Inside of Array_Domain_Type::setInternalGhostCellWidth --- Array_Sizes[%d] = %d \n",
   //      temp,Array_Sizes[temp]);


#if 0


   // Internal_Partitioning_Type::Build_DefaultBlockPartiArrayDomain ( 
   //      BlockPartiArrayDecomposition , Array_Sizes ,
   //      Integer_List , Default_ExternalGhostCellWidth );

















   // Swap BlockPartiArrayDomains
   // delete BlockPartiArrayDomain;

#if 0




#endif




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





#else

   // delete BlockPartiArrayDomain;

#if 0




#endif



#endif // End of if not USE_PADRE
#endif

#if 0
   // Modify the serial descriptor





#endif

   // display ("look at the descriptor from within Array_Domain_Type::setInternalGhostCellWidth");
   // APP_ABORT();
#else
   // If this is not a P++ distributed issue then there is nothing to do!

   // Avoid compiler warning about unused variable
     if (&Integer_List){ } 
} 
#endif


     // ... PPP in i n c l u d e ...
#if defined(PPP) 



#if COMPILE_DEBUG_STATEMENTS


#endif

     // First delete the existing partition (if it exists)


     // Bugfix (11/14/2000) Make reference counting consistent with code elsewhere
     // if (Partitioning_Object_Pointer->referenceCount-- == Internal_Partitioning_Type::getReferenceCountBase())







#if defined(USE_PADRE)
     // What PADRE function do we call here?
     // printf ("NEED TO CALL PADRE \n"); APP_ABORT();
     // printf ("####################################################### \n");
     // printf ("Don't we need to build a new PADRE distribution object? \n");
     // printf ("####################################################### \n");
     // printf ("Set parallelPADRE_DescriptorPointer = NULL in Array_Domain::partition() \n");


#if 0







#endif

     // printf ("In Array_Domain::partition() -- delete parallelPADRE_DescriptorPointer -- commented out! \n");




     // printf ("calling delete parallelPADRE_DescriptorPointer \n");





     // Now reinitialize the PADRE object pointer with a new PADRE_Descriptor
     // This will use the new PADRE_Distribution object which we provide explicitly

     // parallelPADRE_DescriptorPointer =
     //      new PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> 
     //         ( this , Internal_Partition.distributionPointer );


     // printf ("Default distribution reference count = %d \n",
     //      PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>::
     //           getDefaultDistributionReferenceCount());
#else
     // Now delete the Parti specific data structures related to the old partitioning


     // printf ("In Array_Domain_Type::partition (Internal_Partitioning_Type) --- Calling delete_DECOMP \n");







     // printf ("In Array_Domain_Type::partition (Internal_Partitioning_Type) --- Calling delete_DARRAY \n");




     // End of not USE_PADRE
#endif

     // This function works with the doubleArray::partition member function



#if COMPILE_DEBUG_STATEMENTS


#endif

     // Array objects keep there own ghost boundaries (though this might not
     // work when using PADRE -- we might need another PADRE_Descriptor object)
     // printf ("Are the new ghost boundaries passed to PADRE? \n");

     // bugfix (12/19/2000) If the ghost boundaries are nonzero then they 
     // are limited to only non unit size axis.
     // for (temp=0; temp < MAX_ARRAY_DIMENSION; temp++)











     // Internal_Partition.display("Call display in Array_Domain::partition()");

#if COMPILE_DEBUG_STATEMENTS


#endif

#if COMPILE_DEBUG_STATEMENTS
     // We can't test this here since the Internal_Partitioning_Type's list of array objects 
     // has not been updated yet.  So the test is done in the doubleArray::partition member function.
     // Test_Consistency ("Called from Array_Domain_Type::partition(Internal_Partitioning_Type)");

     // A test that we can do is:


#endif

#if COMPILE_DEBUG_STATEMENTS


#endif

     // ... end of PPP ...
#endif

     // ... only PPP  in i n c l u d e  ...
#if defined(PPP) 
#if defined(USE_PADRE)
     // What PADRE function do we call here?
#else



#if COMPILE_DEBUG_STATEMENTS


#endif


     // APP_ASSERT (Partitioning_Object_Pointer != NULL);



































#endif // End of if not USE_PADRE
#endif // end of PPP

     // **********************************************************************************
     //                           COPY CONSTRUCTOR
     // **********************************************************************************

Array_Domain_Type::Array_Domain_Type(const Array_Domain_Type & X,bool AvoidBuildingIndirectAddressingView,
int Type_Of_Copy)


   { 
   // I don't know if it is important to know what type of copy we want this far into the hierarchy.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Array_Domain_Type (const Array_Domain_Type & X, bool,int) \n")); } 
#endif

     int i=0; 
     switch(Type_Of_Copy)
     { 

          case 200:
          // Shallow copies are the same as deep copies since there are no pointers
          // break;


          case 201:case 202:


          // The use of referenceCount in PADRE objects (built in 
          // Initialize_Parallel_Parts_Of_Domain()) forces the referenceCount variable to
          // be initialized here (before calling Initialize_Parallel_Parts_Of_Domain()).
               { this -> referenceCount = Array_Domain_Type::getReferenceCountBase(); 

               // P++ specific code is place here!
#if defined(PPP)
               // Initialize PADRE or Block PARTI pointers to avoid UMR in purify
#if defined(USE_PADRE)

#else


#endif

               // Initialize Partitioning Object pointer to avoid UMR in purify


               // Copy the parallel specific parts of X to this object!


               // Should the copy constructor set this to NULL.  As a result of this
               // the Consistancy test at the end of this function had to be disabled.
               // I think this should be NULL since a Copy constructor in P++ should end
               // a statement.  Imagine passing an expresion to a copy constructor.
               // However one could argue that this should be handled in the array objects
               // copy constructor not the descriptor copy constructor.  For now we will leave 
               // it this way and add this question to the internal design list.

               // Array_Conformability_Info = X.Array_Conformability_Info;
#endif

               for (i = 0; i < 6; i++)
                  { 
#if defined(PPP)






#endif
                    (this -> Size)[i] = (X.Size)[i]; 
                    (this -> Base)[i] = (X.Base)[i]; 
                    (this -> Bound)[i] = (X.Bound)[i]; 
                    (this -> Stride)[i] = (X.Stride)[i]; 
                    (this -> Data_Base)[i] = (X.Data_Base)[i]; 
                    (this -> User_Base)[i] = (X.User_Base)[i]; 

                    // printf ("Setup in Array_Domain_Type COPY CONSTRUCTOR (DEEPCOPY): User_Base [%d] = %d \n",i,User_Base[i]);

                    (this -> Scalar_Offset)[i] = (X.Scalar_Offset)[i]; 

                    // printf ("Setup in Array_Domain_Type COPY CONSTRUCTOR (DEEPCOPY): Scalar_Offset[%d] = %d \n",i,Scalar_Offset[i]);
                    (this -> Index_Array)[i] = (X.Index_Array)[i]; 
                    if ((this -> Index_Array)[i] != (intArray * )0)
                         { (*(this -> Index_Array)[i]).incrementReferenceCount(); } 

#if defined(PPP)
                         // Local_Index_Array   [i] = X.Local_Index_Array  [i];



#endif

                    (this -> IndexBase)[i] = (X.IndexBase)[i]; 
                    (this -> IndexStride)[i] = (X.IndexStride)[i]; 
                    (this -> IndexDataPointer)[i] = (X.IndexDataPointer)[i]; 
               } 

               this -> Domain_Dimension = X.Domain_Dimension; 
               this -> Constant_Data_Base = X.Constant_Data_Base; 
               this -> Constant_Unit_Stride = X.Constant_Unit_Stride; 

               // Controls use of non FORTRAN 90/HPF Indexing
               // Use_Fortran_Indexing = TRUE;

               this -> Is_A_View = X.Is_A_View; 
               this -> Is_A_Temporary = X.Is_A_Temporary; 
               this -> Is_A_Null_Array = X.Is_A_Null_Array; 
               this -> Is_Contiguous_Data = X.Is_Contiguous_Data; 

               // Was this descriptor used to build an array object using the adopt function?
               // This is usually FALSE.
               this -> builtUsingExistingData = 0; 

               // Array_ID           = Global_Array_ID++;
               // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
               // Array_ID           = Stack_Of_Array_IDs.Pop();
               // printf ("Pop_Array_ID called from COPY CONSTRUCTOR! \n");
               this -> internalArrayID = (this -> Pop_Array_ID)(); 

               // printf ("In Array_Domain COPY constructor (X input): X.internalArrayID = %d \n",X.internalArrayID);
               // printf ("In Array_Domain COPY constructor: internalArrayID = %d \n",internalArrayID);

#if !defined(PPP)
               // Since this is a new array object is should have an initialize reference count on its
               // raw data.  This is required here because the reference counting mechanism reused the
               // value of zero for one existing reference and no references (this will be fixed soon).
               this -> resetRawDataReferenceCount(); 
#endif

               this -> ExpressionTemplateOffset = X.ExpressionTemplateOffset; 

               this -> View_Offset = X.View_Offset; 

               this -> Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION; 

               // used in internal diagnostics
        this -> typeCode = X.typeCode; 

               this -> Uses_Indirect_Addressing = X.Uses_Indirect_Addressing; 
               break; 
          } 
          case 203:
          // Call the operator= since we want to avoid any colapse of the data an rerepresentation through
          // a modified descriptor

          // Need to initialize a few things for the operator= to work properly!
               { this -> Is_A_Null_Array = X.Is_A_Null_Array; 
#if defined(PPP)

#if defined(USE_PADRE)

#else


#endif
#endif

               // Now just call the operator= to do the rest (this will NOT cause the copy of the array to be colapsed)
               (*this)=X; 

               // X.display("X in COPY CONSTRUCTOR");

               // Test using the bool operator==
               if (!((*this)==X)){ APP_Assertion_Support("domain.C",4443); } 
               break; 
          } 
          case 204:
               { printf(("DEEPCOLAPSEDALIGNEDCOPY not supported yet! \n")); 
               APP_ABORT(); 
               break; 
          } 
          default:
               { printf(("ERROR: default reached in COPY Constructor (Type_Of_Copy = %d) \n"),
               Type_Of_Copy); 
               APP_ABORT(); 
               break; 
          } 
     } 

#if 0
             // The use of referenceCount in PADRE objects (built in 
             // Initialize_Parallel_Parts_Of_Domain()) forces the referenceCount variable to
             // be initialized here (before calling Initialize_Parallel_Parts_Of_Domain()).


             // P++ specific code is place here!
#if defined(PPP)
             // Initialize PADRE or Block PARTI pointers to avoid UMR in purify
#if defined(USE_PADRE)

#else


#endif

             // Initialize Partitioning Object pointer to avoid UMR in purify


             // Copy the parallel specific parts of X to this object!


             // Should the copy constructor set this to NULL.  As a result of this
             // the Consistancy test at the end of this function had to be disabled.
             // I think this should be NULL since a Copy constructor in P++ should end
             // a statement.  Imagin passing an expresion to a copy constructor.
             // However one could argue that this should be handled in the array objects
             // copy constructor not the descriptor copy constructor.  For now we will leave 
             // it this way and add this question to the internal design list.

             // Array_Conformability_Info = X.Array_Conformability_Info;
#endif



#if defined(PPP)






#endif




















             // Controls use of non FORTRAN 90/HPF Indexing
             // Use_Fortran_Indexing = TRUE;






             // Was this descriptor used to build an array object using the adopt function?
             // This is usually FALSE.


             // Array_ID           = Global_Array_ID++;
             // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
             // Array_ID           = Stack_Of_Array_IDs.Pop();
             // printf ("Pop_Array_ID called from COPY CONSTRUCTOR! \n");


             // printf ("In Array_Domain COPY constructor (X input): X.internalArrayID = %d \n",X.internalArrayID);
             // printf ("In Array_Domain COPY constructor: internalArrayID = %d \n",internalArrayID);

#if !defined(PPP)
             // Since this is a new array object is should have an initialize reference count on its
             // raw data.  This is required here because the reference counting mechanism reused the
             // value of zero for one existing reference and no references (this will be fixed soon).

#endif








#endif

             // Often we want the copy to be reduced in the since that a copy of a 
             // view using indirect addressing is contiguous. This is the case for temporaries.
     if (AvoidBuildingIndirectAddressingView)
        { 
        // Set the base values of a temporary to the value APP_Global_Array_Base
          int Difference=0; 
          int temp=0; 
          // ... (8/7/98,kdb) use Data_Base of X instead of
          //  APP_Global_Array_Base ...
          //int temp_base = APP_Global_Array_Base;
   int temp_base=(X.Data_Base)[0]; 

   if (this -> Uses_Indirect_Addressing)
             { for (temp = 0; temp < 6; temp++)
  { if ((this -> Index_Array)[temp])
     { temp_base = ((*(this -> Index_Array)[temp]).getBase)(0); } 
             } 
   } Difference = temp_base - (this -> Data_Base)[0]; 
          (this -> Data_Base)[0] += Difference; 
          (this -> User_Base)[0] += Difference; 

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
          this -> Is_A_Temporary = 1; 

          // Fix Scalar_Offset since Data_Base may be changing
          // ... (11/20/98; kdb) View_Offset should be added; usually 0 here
          //  so hard to detect ...
          // Scalar_Offset[0] = -User_Base[0] * Stride[0];
          (this -> Scalar_Offset)[0] = -(this -> User_Base)[0] * (this -> Stride)[0] + this
           -> View_Offset; for (temp = 1; temp < 6; temp++)
             { 
               (this -> Scalar_Offset)[temp] = (this -> Scalar_Offset)[(temp - 1)] - ((this
                -> User_Base)[temp] * (this -> Stride)[temp]) * (this -> Size)[(temp - 1)]; 
          } 
               // BUG FIX (10/26/95, kdb): If intArray isn't in dimension 0
               // first dimension of X could be a scalar and have length 1
               // temporary should be changed so first dimension is the same
               // size as the intArray
          if (this -> Uses_Indirect_Addressing)
             { 
             // Base [0] = APP_Global_Array_Base;
               (this -> Base)[0] = 0; 
        (this -> Stride)[0] = 1; 

               for (temp = 0; temp < 6; temp++)
                  { 
                    if ((this -> Index_Array)[temp])
      { 
                         (this -> Bound)[0] = ((*(this -> Index_Array)[temp]).elementCount)()
                          - 1; (this -> Size)[0] = (this -> Bound)[0] + 1; 
                    } 
               } 
               for (temp = 1; temp < 6; temp++)
                  { (this -> Size)[temp] = (this -> Size)[0]; } 
          } 

                  // Since this temporary will be built to be the size of the intArray used to
                  // do the indirect addressing it will not be indirectly addressed.
          if (this -> Uses_Indirect_Addressing)
             { 
               this -> Uses_Indirect_Addressing = 0; 
               for (temp = 0; temp < 6; temp++)
                  { 
                    if ((this -> Index_Array)[temp])
                       { 
                       // Added conventional mechanism for reference counting control
                       // operator delete no longer decriments the referenceCount.
                         (*(this -> Index_Array)[temp]).decrementReferenceCount(); 
                         if (((*(this -> Index_Array)[temp]).getReferenceCount)() < intArray::getReferenceCountBase())
                              { delete (this -> Index_Array)[temp]; } 
                    } 
                    (this -> Index_Array)[temp] = (intArray * )0; 
               } 

                    // If descriptor was a view using mixed intArray and scalar indexing then
                    // we have to set the scalar indexed dimensions to be consistant with a 1D array
                    // so we set all but the first dimension to be 0 for base and bound and set the 
                    // stride to 1.  Thus the resulting temporary is a 1D array (with no indirect addressing).
               if ((this -> Size)[0] == 0)
        { 
    this -> Is_A_Null_Array = 1; 
                  for (temp = 1; temp < 6; temp++)
    { 
       (this -> Base)[temp] = 0; 
       (this -> Bound)[temp] = -1; 
       (this -> Stride)[temp] = 1; 
                  } 
               } 
               else 
        { 
                  for (temp = 1; temp < 6; temp++)
                  { 
                  // Base   [temp] = APP_Global_Array_Base;
                  // Bound  [temp] = APP_Global_Array_Base;

                  // It seems that this is not required since the non expression 
                  // template version does not see these if the indirect addressing 
                  // is in use.
                  // printf ("Inside of Array_Domain_Type COPY constructor need to fixup the Bases (or Data_Base) \n");
                  // printf ("     to be consistant with the expression template implementation \n");
                    (this -> Base)[temp] = 0; 
                    (this -> Bound)[temp] = 0; 
                    (this -> Stride)[temp] = 1; 
                  } 
               } 

                    // APP_ASSERT (Temporary_Domain->Is_Contiguous_Data == TRUE);
                    // Bugfix (1/17/96) In the case of a temporary built from a view obtained
                    // via indirect addressing we can't say that the data is contiguous.
                    // But as I recall there was a better fix than this (Kristi suggested this fix).
                    // I need to understand this problem better.
                    // Is_Contiguous_Data = TRUE;
               this -> Is_Contiguous_Data = 0; 
          } 
     } 
               // Base of block for "if (AvoidBuildingIndirectAddressingView)"


#if defined(PPP)
               // A temporary must carry along the information accumulated in the 
               // evaluation of the P++ array statement so it can be modified by each
               // operation (if required) and used to interpret the message passing
               // when the operator= is executed.





#endif

#if !defined(PPP)
               // printf ("Inside of copy constructor descriptor() Array_ID = %d \n",Array_ID);
               // The reference coutning for the raw data is stored in the SerialArrayDomain object.
     if (this -> internalArrayID >= Array_Domain_Type::Array_Reference_Count_Array_Length)
          { Array_Domain_Type::Allocate_Larger_Reference_Count_Array(this -> internalArrayID); } 
#endif

#if COMPILE_DEBUG_STATEMENTS
          // We can't test the consistancy here because if an a temporary is input we will
          // have a temporary with a Array_Conformability_Info == NULL which would be detected as an error
          // Question: should the copy constructor always set Is_A_Temporary to FALSE?
          // OR: should the copy constructor always set Array_Conformability_Info = X.Array_Conformability_Info?
#if !defined(PPP)
     this -> Test_Consistency(("Called from Array_Domain_Type COPY CONSTRUCTOR #1")); 
} 
#endif
#endif




     // **********************************************************************************
     //                                bool OPERATOR == 
     // **********************************************************************************


void Array_Domain_Type::equalityTest(bool & returnValue,bool test,const char * description) const
   { 
   // This is a support function for the operator==
     returnValue = returnValue && test; 
     if (returnValue == 0)
        { 
          printf(("Test %s is %s, input returnValue = FALSE \n"),description,(test == 1)?"TRUE":"FALSE"); 
          // display("FAILED EQUALITY TEST");
          APP_ABORT(); 
     } 
} 


bool Array_Domain_Type::operator==(const Array_Domain_Type & X) const
   { 
   // This is the bool equals test operator.  Note that the semantics of this operator
   // is very specific (array_ID's and referenceCounts need not be equal for example).  The output of the copy
   // constructor will only pass the equals test if the DEEPALIGNEDCOPY option is used.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::operator== (const Array_Domain_Type & X) \n")); } 
#endif

     bool returnValue=true; 

     int i=0; 

     // We permit future tests to have different semantics which will be specific as input
     int typeOfTest=203; 
     switch(typeOfTest)
     { 

          case 203:

          // P++ specific code is place here!
#if defined(PPP)
          // Initialize PADRE or Block PARTI pointers to avoid UMR in purify
#if defined(USE_PADRE)
#if 0



#endif
#else






#endif





          // equalityTest (returnValue, Array_Conformability_Info == X.Array_Conformability_Info, "Array_Conformability_Info");
#endif

          // printf ("View_Offset = %d X.View_Offset = %d \n",View_Offset,X.View_Offset);

               { this -> equalityTest(returnValue,this -> View_Offset == X.View_Offset,
               ("View_Offset")); 
               for (i = 0; i < 6; i++)
                  { 
                    this -> equalityTest(returnValue,(this -> Size)[i] == (X.Size)[i],
                    ("Size")); 
                    // printf ("Base [i] = %d X.Base [i] = %d \n",Base [i],X.Base [i]);

                    this -> equalityTest(returnValue,(this -> Base)[i] == (X.Base)[i],
                    ("Base")); 
                    // printf ("Bound [i] = %d X.Bound [i] = %d \n",Bound [i],X.Bound [i]);

                    this -> equalityTest(returnValue,(this -> Bound)[i] == (X.Bound)[i],
                    ("Bound")); 
                    this -> equalityTest(returnValue,(this -> Stride)[i] == (X.Stride)[i],
                    ("Stride")); 
                    this -> equalityTest(returnValue,(this -> Data_Base)[i] == (X.Data_Base)[i],
                    ("Data_Base")); 
                    // printf ("User_Base [i] = %d X.User_Base [i] = %d \n",User_Base[i],X.User_Base[i]);

                    this -> equalityTest(returnValue,(this -> User_Base)[i] == (X.User_Base)[i],
                    ("User_Base")); 
                    // printf ("Scalar_Offset [i] = %d X.Scalar_Offset [i] = %d \n",Scalar_Offset[i],X.Scalar_Offset[i]);

                    // equalityTest (returnValue, Scalar_Offset [i] == X.Scalar_Offset[i], "Scalar_Offset");

                    this -> equalityTest(returnValue,(this -> Index_Array)[i] == (X.Index_Array)[i],
                    ("Index_Array")); 
                    this -> equalityTest(returnValue,(this -> IndexBase)[i] == (X.IndexBase)[i],
                    ("IndexBase")); 
                    this -> equalityTest(returnValue,(this -> IndexStride)[i] == (X.IndexStride)[i],
                    ("IndexStride")); 
                    this -> equalityTest(returnValue,(this -> IndexDataPointer)[i] == (X.IndexDataPointer)[i],
                    ("IndexDataPointer")); 
               } 
#if defined(PPP)
#if 0



#endif






#if 0


#endif



#if 0


#endif










#endif


               this -> equalityTest(returnValue,this -> Domain_Dimension == X.Domain_Dimension,
               ("Domain_Dimension")); 
               // printf ("Constant_Data_Base   = %s \n",Constant_Data_Base ? "TRUE" : "FALSE");
               // printf ("X.Constant_Data_Base = %s \n",X.Constant_Data_Base ? "TRUE" : "FALSE");

               this -> equalityTest(returnValue,this -> Constant_Data_Base == X.Constant_Data_Base,
               ("Constant_Data_Base")); 
               // printf ("Constant_Unit_Stride   = %s \n",Constant_Unit_Stride ? "TRUE" : "FALSE");
               // printf ("X.Constant_Unit_Stride = %s \n",X.Constant_Unit_Stride ? "TRUE" : "FALSE");

               this -> equalityTest(returnValue,this -> Constant_Unit_Stride == X.Constant_Unit_Stride,
               ("Constant_Unit_Stride")); 
               this -> equalityTest(returnValue,this -> Is_A_View == X.Is_A_View,("Is_A_View")); 
               this -> equalityTest(returnValue,this -> Is_A_Temporary == X.Is_A_Temporary,
               ("Is_A_Temporary")); 
               this -> equalityTest(returnValue,this -> Is_A_Null_Array == X.Is_A_Null_Array,
               ("Is_A_Null_Array")); 
               this -> equalityTest(returnValue,this -> Is_Contiguous_Data == X.Is_Contiguous_Data,
               ("Is_Contiguous_Data")); 
               // Was this descriptor used to build an array object using the adopt function?
               // This is usually FALSE.
               this -> equalityTest(returnValue,this -> builtUsingExistingData == X.builtUsingExistingData,
               ("builtUsingExistingData")); 

#if 0
               // This is not used but perhaps should be tested since it is set (and so we want it to be checked)




#endif

               this -> equalityTest(returnValue,this -> View_Offset == X.View_Offset,
               ("View_Offset")); 

               this -> equalityTest(returnValue,this -> Is_Built_By_Defered_Evaluation
                == X.Is_Built_By_Defered_Evaluation,("Is_Built_By_Defered_Evaluation")); 


               // used in internal diagnostics
        this -> equalityTest(returnValue,this -> typeCode == X.typeCode,("typeCode")); 

               this -> equalityTest(returnValue,this -> Uses_Indirect_Addressing == X.Uses_Indirect_Addressing,
               ("Uses_Indirect_Addressing")); 
               break; 
          } 
          case 200:
          // Shallow copies are the same as deep copies since there are no pointers


          case 201:case 202:

          case 204:
               { printf(("(DEEPCOLAPSEDALIGNEDCOPY && DEEPCOPY && DEEPCOLAPSEDCOPY && SHALLOWCOPY) not supported yet! in operator== (typeOfTest = %d) \n"),
               typeOfTest); 
               APP_ABORT(); 
               break; 
          } 
          default:
               { printf(("ERROR: default reached in operator== (typeOfTest = %d) \n"),
               typeOfTest); 
               APP_ABORT(); 
               break; 
          } 
     } 
     return returnValue;
} 


     // ***********************************************************************************
     //                        Support for Indirect Addressing
     // ***********************************************************************************

#if (defined(SERIAL_APP) || defined(USE_PADRE)) && !defined(PPP)
     // This constuctor is used to build a SerialArray_Domain_Type from an Array_Domain_Type
     // It is here to support the use of PADRE.  This function is a copy of the COPY CONSTRUCTOR





#if COMPILE_DEBUG_STATEMENTS



     // Since the X's data is not setup yet it is a problem to call this function!
     // We need to consider a less complex interface relationship (however this is more a P++ issue than a PADRE issue).
     // X.display("Display Parallel Descriptor");

#endif

     // Avoid compiler warning about unused input variable




     // The use of referenceCount in PADRE objects (built in 
     // Initialize_Parallel_Parts_Of_Domain()) forces the referenceCount variable to
     // be initialized here (before calling Initialize_Parallel_Parts_Of_Domain()).


     // Remove this soon!
     // The typeCode records the element type associated with the array for which
     // we use this domain object.  This is used only for internal diagnostics.
     // The value 'u' makes the element type as undefined.  This mechanism might be
     // replaced by an alternative mechanism in the future.
     // setTypeCode(APP_UNDEFINED_ELEMENT_TYPE);


#if defined(PPP)


     // APP_ASSERT(BlockPartiArrayDecomposition != NULL);
     // APP_ASSERT( BlockPartiArrayDomain   != NULL );

     // Should the copy constructor set this to NULL.  As a result of this
     // the Consistancy test at the end of this function had to be disabled.
     // I think this should be NULL since a Copy constructor in P++ should end
     // a statement.  Imagin passing an expresion to a copy constructor.
     // However one could argue that this should be handled in the array objects
     // copy constructor not the descriptor copy constructor.  For now we will leave 
     // it this way and add this question to the internal design list.

     // Array_Conformability_Info = X.Array_Conformability_Info;
#endif



#if defined(PPP)








#endif









     // printf ("Setup in (special) SerialArray_Domain_Type COPY CONSTRUCTOR: Scalar_Offset[%d] = %d \n",i,Scalar_Offset[i]);

#if 1
     // The types of intArrays in this object and the X object (input) are different types
     // so we can't assign pointers to them.

#else



#endif

     // if defined(USE_EXPRESSION_TEMPLATES)



     // endif




     // printf ("Domain_Dimension = %d  X.Domain_Dimension = %d \n",Domain_Dimension,X.Domain_Dimension);




     // Controls use of non FORTRAN 90/HPF Indexing
     // Use_Fortran_Indexing = TRUE;






     // used in internal diagnostics


     // Was this descriptor used to build an array object using the adopt function?
     // This is usually FALSE


     // Array_ID           = Global_Array_ID++;
     // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
     // Array_ID           = Stack_Of_Array_IDs.Pop();
     // printf ("Pop_Array_ID called from COPY CONSTRUCTOR! \n");


     // if defined(USE_EXPRESSION_TEMPLATES)

     // endif







     // Often we want the copy to be reduced in the sense that a copy of a 
     // view using indirect addressing is contiguous. This is the case for temporaries.


     // Set the base values of a temporary to the value APP_Global_Array_Base












     // printf ("Setup in SerialArray_Domain_Type::SerialArray_Domain_Type (const Array_Domain_Type & X): User_Base [%d] = %d \n",temp,User_Base[temp]);

     // ... (10/31/96,kdb) also need to reset Global and Local_Mask_Index
     // to be consistent with this ...


     // Mark the array as a temporary (very important step)


     // Fix Scalar_Offset since Data_Base may be changing








     // BUG FIX (10/26/95, kdb): If intArray isn't in dimension 0
     // first dimension of X could be a scalar and have length 1
     // temporary should be changed so first dimension is the same
     // size as the intArray


     // Base [0] = APP_Global_Array_Base;














     // Since this temporary will be built to be the size of the intArray used to
     // do the indirect addressing it will not be indirectly addressed.







     // Added conventional mechanism for reference counting control
     // operator delete no longer decriments the referenceCount.







     // If descriptor was a view using mixed intArray and scalar indexing then
     // we have to set the scalar indexed dimensions to be consistant with a 1D array
     // so we set all but the first dimension to be 0 for base and bound and set the 
     // stride to 1.  Thus the resulting temporary is a 1D array (with no indirect addressing).


     // Base   [temp] = APP_Global_Array_Base;
     // Bound  [temp] = APP_Global_Array_Base;

     // It seems that this is not required since the non expression template version
     // does not see these if the indirect addressing is in use.
     // printf ("Inside of Array_Domain_Type COPY constructor need to fixup the Bases (or Data_Base) \n");
     // printf ("     to be consistant with the expression template implementation \n");





     // APP_ASSERT (Temporary_Domain->Is_Contiguous_Data == TRUE);
     // Bugfix (1/17/96) In the case of a temporary built from a view obtained
     // via indirect addressing we can't say that the data is contiguous.
     // But as I recall there was a better fix than this (Kristi suggested this fix).
     // I need to understand this problem better.
     // Is_Contiguous_Data = TRUE;




#if defined(PPP)
     // A temporary must carry along the information accumulated in the 
     // evaluation of the P++ array statement so it can be modified by each
     // operation (if required) and used to interpret the message passing
     // when the operator= is executed.





#endif

#if !defined(PPP)
     // printf ("Inside of copy constructor descriptor() Array_ID = %d \n",Array_ID);


#endif

#if COMPILE_DEBUG_STATEMENTS
     // We can't test the consistancy here because if an a temporary is input we will
     // have a temporary with a Array_Conformability_Info == NULL which would be detected as an error
     // Question: should the copy constructor always set Is_A_Temporary to FALSE?
     // OR: should the copy constructor always set Array_Conformability_Info = X.Array_Conformability_Info?
#if !defined(PPP)

#endif
#endif




     // This function translates the base to zero.  It is used as part of the 
     // PADRE where the globalDomain object is stored in the PADRE_Representation
     // object in a translation invariant form (meaning base zero and assumed independent of base).


     // if defined(SERIAL_APP)
#endif


     // **********************************************************************************
     //                           EQUALS OPERATOR
     // **********************************************************************************

Array_Domain_Type & Array_Domain_Type::operator=(const Array_Domain_Type & X)
   { 
   // This function is called by the Descriptor operator= and breaks the association of
   // the partitioning to the one in X.

   // This function is also called by the copy constructor when the typeOfCopy == DEEPALIGNEDCOPY

   // At present this function is only called in the optimization of assignment and temporary use
   // in lazy_statement.C!  
   // NOT TRUE! The Expression template option force frequent calls to this function!
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::operator= (const Array_Domain_Type & X) \n")); } 
#endif

     int i=0; 

     // The use of referenceCount in PADRE objects (built in 
     // Initialize_Parallel_Parts_Of_Domain()) forces the referenceCount variable to
     // be initialized here (before calling Initialize_Parallel_Parts_Of_Domain()).
     this -> referenceCount = Array_Domain_Type::getReferenceCountBase(); 

     // printf ("In Array_Domain_Type::operator=(): Is_A_Null_Array = %s \n",(Is_A_Null_Array) ? "TRUE" : "FALSE");

#if defined(PPP)
     // ... (9/1/97,kdb) this is a memory leak if these aren't null
     // but only check when not a null array because these might be
     // set to non null uninitialized garbage so test will be incorrect ...


#if defined(USE_PADRE)
     // What function of PADRE should we use?


     // PADRE requires that the reference counts be 
     // checked before calling the delete operator






#else


     // printf ("In Array_Domain_Type::operator= (Array_Domain_Type) --- Calling delete_DECOMP \n");





     // printf ("BlockPartiArrayDecomposition == NULL \n");



     // printf ("In Array_Domain_Type::operator= (Array_Domain_Type) --- Calling delete_DARRAY \n");





     // printf ("BlockPartiArrayDomain == NULL \n");

#endif


     // We don't want to set this to NULL since it has already been initialized!
     // Partitioning_Object_Pointer = NULL;


#if defined(USE_PADRE)
     // What function of PADRE should we use?
     // printf ("NEED TO CALL PADRE \n"); APP_ABORT();
#else


#endif










     // Local_Index_Array      [i] = X.Local_Index_Array      [i];




#endif

     this -> Domain_Dimension = X.Domain_Dimension; 
     this -> Constant_Data_Base = X.Constant_Data_Base; 
     this -> Constant_Unit_Stride = X.Constant_Unit_Stride; 

     // Controls use of non FORTRAN 90/HPF Indexing
     // Use_Fortran_Indexing = TRUE;

     for (i = 0; i < 6; i++)
        { 
          (this -> Size)[i] = (X.Size)[i]; 
          (this -> Base)[i] = (X.Base)[i]; 
          (this -> Bound)[i] = (X.Bound)[i]; 
          (this -> Stride)[i] = (X.Stride)[i]; 
          (this -> Data_Base)[i] = (X.Data_Base)[i]; 
          (this -> User_Base)[i] = (X.User_Base)[i]; 

          // printf ("Setup in Array_Domain_Type::operator=: User_Base [%d] = %d \n",i,User_Base[i]);

          (this -> Scalar_Offset)[i] = (X.Scalar_Offset)[i]; 

          // printf ("Setup in Array_Domain_Type::operator=: Scalar_Offset[%d] = %d \n",i,Scalar_Offset[i]);

          (this -> Index_Array)[i] = (X.Index_Array)[i]; 
          if ((this -> Index_Array)[i] != (intArray * )0)
               { (*(this -> Index_Array)[i]).incrementReferenceCount(); } 

               // if defined(USE_EXPRESSION_TEMPLATES)
          (this -> IndexBase)[i] = (X.IndexBase)[i]; 
          (this -> IndexStride)[i] = (X.IndexStride)[i]; 
          (this -> IndexDataPointer)[i] = (X.IndexDataPointer)[i]; 
     }           // endif


     this -> Is_A_View = X.Is_A_View; 
     this -> Is_A_Temporary = X.Is_A_Temporary; 
     this -> Is_A_Null_Array = X.Is_A_Null_Array; 
     this -> Is_Contiguous_Data = X.Is_Contiguous_Data; 

     // used in internal diagnostics
     this -> typeCode = X.typeCode; 

     // if defined(USE_EXPRESSION_TEMPLATES)
     this -> ExpressionTemplateOffset = X.ExpressionTemplateOffset; 
     // endif
     this -> View_Offset = X.View_Offset; 

     // printf ("In Array_Domain_Type::operator=: Scalar_Offset not reinitialized! \n");

     // Array_ID           = Global_Array_ID++;
     // Bug fix (5/2/94) to prevent LARGE Reference_Count_Array
     // Array_ID           = Stack_Of_Array_IDs.Pop();

     // Because of the copying of the descriptors in the expression template 
     // implementation we copy the Array_ID as well!
     // Array_ID           = Pop_Array_ID();
     // ... note: this causes problems with the deep copy in the copy
     //  constructor but is fixed for now by saving old Array_ID ...
     this -> internalArrayID = X.internalArrayID; 

     // Was this descriptor used to build an array object using the adopt function?
     // This is usually FALSE.
     this -> builtUsingExistingData = 0; 

     // Is_Built_By_Defered_Evaluation = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;
     this -> Is_Built_By_Defered_Evaluation = 0; 

     this -> Uses_Indirect_Addressing = X.Uses_Indirect_Addressing; 

#if !defined(PPP)
     // printf ("Inside of Array_Domain_Type::operator=() internalArrayID = %d \n",internalArrayID);
     if (this -> internalArrayID >= Array_Domain_Type::Array_Reference_Count_Array_Length)
          { Array_Domain_Type::Allocate_Larger_Reference_Count_Array(this -> internalArrayID); } 
#endif

#if COMPILE_DEBUG_STATEMENTS
     this -> Test_Consistency(("Called from Array_Domain_Type::operator=")); 
#endif

     return *this;
} 

     // Should be inlined since it is used in a lot of places within A++!

int Array_Domain_Type::Array_Size() const
   { 
   // This function computes the number of elements in the array object!
     int i=0;      // indexing variable
     int Number_Of_Elements=1; 

     // The case of Indirect_Addressing does not seem to be handled properly
     // so for the moment we will make sure that it is FALSE.
     // APP_ASSERT (Uses_Indirect_Addressing == FALSE);

     if (this -> Uses_Indirect_Addressing)
        { 
          do { 
               if ((this -> Index_Array)[i])
                    { Number_Of_Elements = ((*(this -> Index_Array)[i]).elementCount)(); } 
               i++; 
          } while (Number_Of_Elements <= 1 && i < 6);
     } 



     else if (this -> Is_A_Null_Array)
             { 
             // A Null Array has ZERO elements!
               Number_Of_Elements = 0; 
     } 
     else 
             { 
             // Otherwise the Number of elements is a product of the size of each dimension
             // Number_Of_Elements = (((Bound[0]-Base[0])+1)/Stride[0]) * (((Bound[1]-Base[1])+1)/Stride[1]) * 
             //                      (((Bound[2]-Base[2])+1)/Stride[2]) * (((Bound[3]-Base[3])+1)/Stride[3]);
               for (i = 0; i < 6; i++)
                    { Number_Of_Elements *= (((this -> Bound)[i] - (this -> Base)[i])
                     + 1) / (this -> Stride)[i]; } 
     } 

     return Number_Of_Elements;
} 

#if defined(PPP)



     // Left_Number_Of_Points and Right_Number_Of_Points start counting at the left and right most
     // ghost boundaries which on small arrays (with the adjacent processor having a number of
     // elements in the partition equal to the ghost boundary width) can mean that a partition might
     // mistakenly be considered to be the leftmost processor.  We use the PARTI descriptor to 
     // resolve this since it assumes the world starts at zero (P++ makes no such assumption).



#if defined(USE_PADRE)
     // What function of PADRE should we use?
     // printf ("NEED TO CALL PADRE \n"); APP_ABORT();


#else
     // printf ("Axis = %d \n",Axis);









     // Along higher axes we want to force isLeftPartition == TRUE




     // USE_PADRE not defined
#endif







     // Left_Number_Of_Points and Right_Number_Of_Points start counting at the left and right most
     // ghost boundaries which on small arrays (with the adjacent processor having a number of
     // elements in the partition equal to the ghost boundary width) can mean that a partition might
     // mistakenly be considered to be the leftmost processor.  We use the PARTI descriptor to 
     // resolve this since it assumes the world starts at zero (P++ makes no such assumption).



#if defined(USE_PADRE)
     // What function of PADRE should we use?
     // printf ("NEED TO CALL PADRE \n"); APP_ABORT();


#else
     // printf ("Axis = %d \n",Axis);













     // Along higher axes we want to force isMiddlePartition == TRUE




     // USE_PADRE not defined
#endif







     // Left_Number_Of_Points and Right_Number_Of_Points start counting at the left and right most
     // ghost boundaries which on small arrays (with the adjacent processor having a number of
     // elements in the partition equal to the ghost boundary width) can mean that a partition might
     // mistakenly be considered to be the leftmost processor.  We use the PARTI descriptor to
     // resolve this since it assumes the world starts at zero (P++ makes no such assumption).


#if defined(USE_PADRE)
     // What function of PADRE should we use?
     // printf ("NEED TO CALL PADRE \n"); APP_ABORT();


#else
     // printf ("Axis = %d \n",Axis);











     // Along higher axes we want to force isRightPartition == TRUE




     // USE_PADRE not defined
#endif







     // Check to see if local processor has a part of the P++ array object
     // since not all processors have to own a part of every array object.

     // (bug fix 3/5/96 kdb) BlockPartiArrayDomain doesn't
     // reflect views so use Local_Mask_Index instead if any are
     // a Null_Index all should be so check axis 0

#if COMPILE_DEBUG_STATEMENTS





#endif






     //---------------------------------------------------------------------



     // Find number of processor where indexVals lives






     // Bugfix (Brian Miller - 10/5/2000)
     // indexValsNoOffset[i] = (indexVals[i]-Data_Base[i]) * Stride[i];




#if defined(USE_PADRE)

#else

#endif




     //---------------------------------------------------------------------







     // ... Find last dimension that is indirectly addressed and use that for
     //  control since all will be the same length. ...





     // ... Index_Arrays must be 1d so only use length of first dimension. ...























     //for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = 0;








     // ... loop through Index_Arrays in each dimension finding off processor 
     //  points and then storing the associated processor number ...  











     // ... supplementLocations and localLocations contain the position
     //  of the Index_Array value globally (i.e. the position it has
     //  in the parallel Index_Array attached to the parallel domain) ...


     // need to subtract base for location






















     //---------------------------------------------------------------------































     //for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = 0;

     // ... loop through I_A finding off processor points and then
     //  storing the associated processor number ...



























     //---------------------------------------------------------------------







     //  ... Use last indirectly addressed dimension for control since all
     //    will be the same length and distributed the same. ...





     // ... Index_Arrays are 1d so use first dimension. ...























     //for (nd=0;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = 0;

     // ... loop through Index_Arrays finding off processor points and then
     //  storing the associated processor number ...














































     //---------------------------------------------------------------------







     // ... this routine only can be called by a 1d array but a view
     //  will have Domain_Dimension 2 and so assert won't work ...
     //APP_ASSERT (Domain_Dimension == 1);
























     //for (nd=1;nd<MAX_ARRAY_DIMENSION;nd++) indexVals[nd] = 0;

     // ... loop through I_A_list finding off processor points and then
     //  storing the associated processor number ...














     /*supplementData[nd][cntr] = indexVals[nd];*/









     /*localData[nd][cntrl] = indexVals[nd];*/








     //---------------------------------------------------------------------

#endif

     //---------------------------------------------------------------------

int Array_Domain_Type::getRawDataSize(int Axis) const
   { 
   // This function is useful in accessing the size along each axis of the actual 
   // raw data.  It is mostly used in conjunction with explicit indexing
   // of the raw data obtained from the getDataPointer member function.

     int Raw_Data_Size=0; 

#if COMPILE_DEBUG_STATEMENTS
     if (!(Axis >= 0 && Axis < 6)){ APP_Assertion_Support("domain.C",5915); } 
     if ((this -> Size)[Axis] == 0)
          { for (int temp=0; temp < 6; temp++)
               { if (!((this -> Size)[temp] == 0)){ APP_Assertion_Support("domain.C",
               5918); 
               } 
          } 
     } 
#endif

#if 0







               // this data is stored in a premultiplied form to simplify the MDI layer


#else
               // (7/28/2000) We want the original size independent of the view - but the null array could be a view
               // in which case we don't want to treat it differently just because it is a null array view.
     if (Axis == 0)
        { 
          Raw_Data_Size = (this -> Size)[0]; 
     } 


          // this data is stored in a premultiplied form to simplify the MDI layer
          // Since the Size is multiplied with each dimension it is only zero if all are zero!

     else if ((this -> Size)[Axis] == 0)
               { Raw_Data_Size = 0; } 
     else 
               { Raw_Data_Size = (this -> Size)[Axis] / (this -> Size)[(Axis - 1)]; } 

#endif          
     return Raw_Data_Size;
} 


void Array_Domain_Type::getRawDataSize(int * Data_Size) const
   { 
   // It is useful to have a way of getting the size of the array data
   // (especially in the case of a view) since computing it directly from 
   // the base and bound of the descriptor would be incorrect in the case of
   // a view.  This sort of operation comes up over and over again and so we
   // provide a member function for it.  The input is assumed to be a size
   // consistant with the number of dimensions of the A++/P++ arrays.

     int temp; 
#if COMPILE_DEBUG_STATEMENTS
     if ((this -> Size)[0] == 0)
          { for (temp = 0; temp < 6; temp++)
               { if (!((this -> Size)[temp] == 0)){ APP_Assertion_Support("domain.C",
               5966); 
               } 
          } 
     } 
#endif

#if 0


               // In case of Size[0] == 0 we have to avoid dividing by ZERO









#else
               // (7/28/2000) We want the original size independent of the view - but the null array could be a view
               // in which case we don't want to treat it differently just because it is a null array view.
               // Since the Size is multiplied with each dimension it is only zero if all are zero!
     if ((this -> Size)[0] == 0)
        { 
        // In case of Size[MAX_ARRAY_DIMENSION-1] == 0 we have to avoid dividing by ZERO
          for (temp = 0; temp < 6; temp++)
               { Data_Size[temp] = 0; } 
     } 
     else 
        { 
          Data_Size[0] = (this -> Size)[0]; 
          for (temp = 1; temp < 6; temp++)
               { Data_Size[temp] = (this -> Size)[temp] / (this -> Size)[(temp - 1)]; } 
     } 
} 
#endif



bool Array_Domain_Type::isSameBase(const Array_Domain_Type & X) const
   { 
   // This function is a dimension independent test of equality
   // between the bases of each axis of two array descriptors.

     bool Return_Value=true; 
     int temp=0; 
     do { 
          Return_Value = ((this -> getRawBase)(temp) == (X.getRawBase)(temp)); 
          temp++; 
     } while (temp < 6 && (Return_Value == 1));


     return Return_Value;
} 


bool Array_Domain_Type::isSameBound(const Array_Domain_Type & X) const
   { 
   // This function is a dimension independent test of equality
   // between the bounds of each axis of two array descriptors.

     bool Return_Value=true; 
     int temp=0; 
     do { 
          Return_Value = ((this -> getRawBound)(temp) == (X.getRawBound)(temp)); 
          temp++; 
     } while (temp < 6 && (Return_Value == 1));


     return Return_Value;
} 


bool Array_Domain_Type::isSameStride(const Array_Domain_Type & X) const
   { 
   // This function is a dimension independent test of equality
   // between the stride of each axis of two array descriptors.

     bool Return_Value=true; 
     int temp=0; 
     do { 
          Return_Value = ((this -> getRawStride)(temp) == (X.getRawStride)(temp)); 
          temp++; 
     } while (temp < 6 && (Return_Value == 1));


     return Return_Value;
} 


bool Array_Domain_Type::isSameLength(const Array_Domain_Type & X) const
   { 
   // This function is a dimension independent test of equality
   // between the length of each axis of two array descriptors.

     bool Return_Value=true; 
     int temp=0; 
     do { 
          Return_Value = ((this -> getLength)(temp) == (X.getLength)(temp)); 
          temp++; 
     } while (temp < 6 && (Return_Value == 1));


     return Return_Value;
} 

#if defined(APP) || defined(PPP)

bool Array_Domain_Type::isSameGhostBoundaryWidth(const Array_Domain_Type & X) const
   { 
   // This function is a dimension independent test of equality
   // between the ghost boundary widths of each axis of two array descriptors.

     bool Return_Value=true; 
#if defined(PPP)






#endif

     return Return_Value;
} 


bool Array_Domain_Type::isSameDistribution(const Array_Domain_Type & X) const
   { 
     bool Return_Value=true; 

#if defined(PPP)



     // If they use the same partition then they have the same 
     // distribution only if they are the same size








#endif

     return Return_Value;
} 
#endif


bool Array_Domain_Type::isSimilar(const Array_Domain_Type & X) const
   { 
   // This function is a dimension independent test of equality
   // between the bases of each axis of two array descriptors.

     bool Return_Value=((this -> isSameBase)(X) && (this -> isSameBound)(X)) && (this
      -> isSameStride)(X); return Return_Value;
} 


int Array_Domain_Type::computeArrayDimension(int * Array_Sizes)
   { 
#if 1
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
        { 
          printf(("Inside of Array_Domain_Type::computeArrayDimension --- \n")); 
          for (int i=0; i < 6; i++)
               { printf((" %d"),Array_Sizes[i]); } 
          printf(("\n")); 
     } 

     int temp; 
     for (temp = 0; temp < 6; temp++)
        { 
          if (!(Array_Sizes[temp] >= 0)){ APP_Assertion_Support("domain.C",6140); } 
     } 
#endif

     int Array_Dimension=6; 
     bool Done=false; 
     int j=5; 
     while(!Done && j >= 0)
        { 
          if (Array_Sizes[j] <= ((j > 0)?1:0))
               { Array_Dimension--; } 
          else 
               { Done = true; } 
          j--; 
     } 
     if (!(Array_Dimension >= 0)){ APP_Assertion_Support("domain.C",6155); } 
     if (!(Array_Dimension <= 6)){ APP_Assertion_Support("domain.C",6156); } 

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          { printf(("Inside of Array_Domain_Type::computeArrayDimension (int*) -- Array_Dimension = %d \n"),
          Array_Dimension); } 
#endif

     return Array_Dimension;
} 
#else
     // return computeArrayDimension (Array_Sizes[0],Array_Sizes[1],Array_Sizes[2],Array_Sizes[3]);

#endif



int Array_Domain_Type::computeArrayDimension(const Array_Domain_Type & X)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          { printf(("Inside of Array_Domain_Type::computeArrayDimension () \n")); } 
#endif

     int Array_Sizes[6]; 

     if (X.Is_A_Null_Array == 1)
          { return 0;} 

          // Remember that Sizes in the Array_descriptor_Type are stored already multiplied by 
          // the lower dimension sizes to simplify the internal address arithmitic.
     Array_Sizes[0] = (X.Size)[0]; 
     int temp; 
     for (temp = 1; temp < 6; temp++)
          { Array_Sizes[temp] = (X.Size)[temp] / (X.Size)[(temp - 1)]; } 

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          { printf(("Calling Array_Domain_Type::computeArrayDimension () \n")); } 
#endif
     return Array_Domain_Type::computeArrayDimension(Array_Sizes);
} 

#if !defined(PPP)

void Array_Domain_Type::Allocate_Larger_Reference_Count_Array(int New_Size)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Inside of Array_Domain_Type::Allocate_Larger_Reference_Count_Array (%d) current size = %d \n"),
          New_Size,Array_Domain_Type::Array_Reference_Count_Array_Length); } 
#endif

     const int referenceCountArrayChunkSize=100; 

     // Make the array size a little larger to account for growth!
     // New_Size += 300;
     New_Size += 100; 

     // Note that on 4.1.3 systems realloc does not work when the pointer is NULL (a bug) so we
     // have to allocate some memory using malloc to initialize the pointer!
     if (Array_Domain_Type::Array_Reference_Count_Array == (int * )0)
        { 
          if (Array_Domain_Type::Array_Reference_Count_Array_Length != 0)
             { 
               printf(("ERROR: Array_Reference_Count_Array_Length != 0 at first use of malloc! \n")); 
               APP_ABORT(); 
          } 

          Array_Domain_Type::Array_Reference_Count_Array = (int * )valloc((New_Size * 4)); 
          if (!(Array_Domain_Type::Array_Reference_Count_Array != (int * )0)){ APP_Assertion_Support("domain.C",
          6225); 
          } 
   if (Diagnostic_Manager::getTrackArrayData() == 1)
             { 
             // printf ("tracking array data (initial allocation)! \n");
               Diagnostic_Manager::diagnosticInfoArray = (DiagnosticInfo * * )valloc((New_Size
                * 4)); 
               if (!(Diagnostic_Manager::diagnosticInfoArray != (DiagnosticInfo * * )0)){ APP_Assertion_Support("domain.C",
               6231); 
               } 
   } 
     } 
     else 
        { 
          Array_Domain_Type::Array_Reference_Count_Array = (int * )realloc(((char * )Array_Domain_Type::Array_Reference_Count_Array),
          (New_Size * 4)); 
          if (!(Array_Domain_Type::Array_Reference_Count_Array != (int * )0)){ APP_Assertion_Support("domain.C",
          6238); 
          } 
   if (Diagnostic_Manager::getTrackArrayData() == 1)
             { 
             // printf ("tracking array data (realloc)! \n");
               Diagnostic_Manager::diagnosticInfoArray = (DiagnosticInfo * * )realloc(((char * )Diagnostic_Manager::diagnosticInfoArray),
               (New_Size * 4)); 

               if (!(Diagnostic_Manager::diagnosticInfoArray != (DiagnosticInfo * * )0)){ APP_Assertion_Support("domain.C",
               6245); 
               } 
   } 
     } 
     if (!(Array_Domain_Type::Array_Reference_Count_Array != (int * )0)){ APP_Assertion_Support("domain.C",
     6250); 
     } 
     if (Diagnostic_Manager::getTrackArrayData() == 1)
        { 
          if (!(Diagnostic_Manager::diagnosticInfoArray != (DiagnosticInfo * * )0)){ APP_Assertion_Support("domain.C",
          6253); 
          } 
     }           // Zero the new elements of the array!
          // This is where the reference count array is initialized
     int i; 
     int initialReferenceCountValue=Array_Domain_Type::getReferenceCountBase() - 1; 
     for (i = Array_Domain_Type::Array_Reference_Count_Array_Length; i < New_Size; i++)
        { 
        // initialize to starting values
          Array_Domain_Type::Array_Reference_Count_Array[i] = initialReferenceCountValue; 
     } 

     if (Diagnostic_Manager::getTrackArrayData() == 1)
        { 
          if (!(Diagnostic_Manager::diagnosticInfoArray != (DiagnosticInfo * * )0)){ APP_Assertion_Support("domain.C",
          6268); 
          } 
          for (i = Array_Domain_Type::Array_Reference_Count_Array_Length; i < New_Size; i++)
             { 
             // initialize new pointers to NULL
        Diagnostic_Manager::diagnosticInfoArray[i] = (DiagnosticInfo * )0; 
          } 
     } 

     Array_Domain_Type::Array_Reference_Count_Array_Length = New_Size; 
} 
#endif

#if defined(PPP)
     //-------------------------------------------------------------------------
     // This function is specific for use with P++ indirect addressing!



     // By initializing the Local_Array_Index initially to the serial part of the Index_Array
     // we can test the P++ indirect addressing on a single process and THEN move onto the
     // proper initialization of the Local_Index_Array using the valid serial portion of the parallel
     // indirect addressing vector (parallel array).  This pemits an incremental approach to the
     // support of P++ indirect addressing.

#if COMPILE_DEBUG_STATEMENTS





#endif

     // Avoid compiler warning about unused input variable


     // Initialize the Local_Index_Array pointers for each intArray used in the indirect addressing
















     //-------------------------------------------------------------------------
#if 0
     // I think this function is no longer used!
     // This function is specific for use with P++ indirect addressing!



     // Index value used below

#if COMPILE_DEBUG_STATEMENTS


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


#endif

     // The whole point is to cache the Local_Index_Arrays for reuse -- though we have to figure out
     // if they have been changed so maybe this is expensive anyway.  This later work has not
     // been implemented yet.




#if COMPILE_DEBUG_STATEMENTS





#endif




     // printf ("Initialize the Replicated_Index_intArray array! \n");



     // Put P++ intArray into the cache
     // Indirect_Addressing_Cache_Domain *Cache_Domain = Indirect_Addressing_Cache.put (X);
     // Distrbuted_Array_To_Replicated_Array_Conversion (X);

     // printf ("Initialize the Replicated_Index_intArray[%d] \n",i);






     // For now we handle the case of replicating the P++ intArray onto only a single processor
     // so we can debug indirect addressing on a single processor first.
     // printf ("Conversion of P++ intArray to intSerialArray only handled for single processor case! \n");




     // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");




     // printf ("Replicated_Index_intArray[%d] = %p \n",i,Replicated_Index_intArray[i]);



     // printf ("Initialize the Valid_Local_Entries_Mask! \n");
     // Build a mask which marks where the entries in the Replicated_intArray are valid for the local
     // serial Array (i.e. those entries that are in bounds).  I think we need all the dimension
     // information to do this!!!!


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



     // printf ("Initialize the Valid_Local_Entries_Mask i = %d \n",i);





     // printf ("Local_Base = %d  Local_Bound = %d \n",Local_Base,Local_Bound);



     // printf ("FIRST assignment of Valid_Local_Entries_Mask \n");





     // printf ("LATER assignment of Valid_Local_Entries_Mask \n");











     // display("THIS Domain");
     // Original_Domain.display("ORIGINAL Domain");
     // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");






     // Valid_Local_Entries_Mask.display("Inside of Array_Domain_Type::Compute_Local_Index_Arrays -- Valid_Entries_Mask");

     // printf ("Initialize the Valid_Local_Entries! \n");
     // Build a set of  i n d e x  values that are associated with where the Valid_Entries_Mask is TRUE



     // Valid_Local_Entries.display("Valid_Local_Entries");

     // printf ("Initialize the Local_Index_Array array! \n");
     // Now initialize the Local_Index_Array pointers for each intArray used in the indirect addressing


     // printf ("Initialize the Local_Index_Array array[%d] \n",i);







     // printf ("BEFORE -- build new intSerialArray ((*Replicated_Index_intArray[i]) (Valid_Local_Entries)) \n");

     // printf ("AFTER -- build new intSerialArray ((*Replicated_Index_intArray[i]) (Valid_Local_Entries)) \n");

     // error checking









     // Reenable the where statement if it was in use at the invocation of this function


#if COMPILE_DEBUG_STATEMENTS


#endif



#endif
     //----------------------------------------------------------------------
#endif

#if 0
#if defined(PPP)



#if COMPILE_DEBUG_STATEMENTS


#endif
     // Copy relevant data from old descriptor (this should be a function)

     // printf ("In Array_Domain_Type::Update_Parallel_Information_Using_Old_Domain - INCREMENT REFERENCE COUNTS OF THESE OBJECTS!!! \n");




#if defined(USE_PADRE)
     // What PADRE function do we call here?

#else
#if 1







#else


#endif
#endif // End of USE_PADRE not defined








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

void Array_Domain_Type::Test_Preliminary_Consistency(const char * Label) const
   { 
#if (EXTRA_ERROR_CHECKING == FALSE)


#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          { printf(("Inside of Array_Domain_Type::Test_Preliminary_Consistency! (Label = %s) (Domain_Dimension = %d) \n"),
          Label,this -> Domain_Dimension); } 
#endif

     int temp=0; 
#if !defined(PPP)
     // APP_ASSERT(internalArrayID < Array_Reference_Count_Array_Length);
     if (this -> internalArrayID >= Array_Domain_Type::Array_Reference_Count_Array_Length)
        { 
          printf(("ERROR: internalArrayID >= Array_Reference_Count_Array_Length in Array_Domain_Type::Test_Preliminary_Consistency \n")); 
          APP_ABORT(); 
     } 
#endif

          // The base is always the base into the raw memory and so can not be negative
          // so we might as well test it since it has would have caught a P++ bug I just fixed
     for (temp = 0; temp < 6; temp++)
        { 
        // Bugfix (3/17/96)
        // I can't see why the case of Uses_Indirect_Addressing == TRUE should change
        // the general rule that the Base should be >= 0.  So I have modified the test.
        // I think that having made the mixed intArray scalar indexing consistant with 
        // that of Index object scalar indexing will simplify this problem.
        // APP_ASSERT(Uses_Indirect_Addressing || (Base[temp] >= 0));
          if ((this -> Base)[temp] < 0)
             { 
               this -> display(("ERROR in Array_Domain_Type::Test_Preliminary_Consistency")); 
          } 
          if (!((this -> Base)[temp] >= 0)){ APP_Assertion_Support("domain.C",6602); } 
     } 

     if (this -> Uses_Indirect_Addressing == 1)
        { 
          bool Valid_Pointer_To_intArray_Found=false; 
          for (temp = 0; temp < 6; temp++)
             { 
               if ((this -> Index_Array)[temp] != (intArray * )0)
                  { 
                    Valid_Pointer_To_intArray_Found = true; 
                    (*(this -> Index_Array)[temp]).Test_Consistency(Label); 
               } 
          } 
          if (!(Valid_Pointer_To_intArray_Found == 1)){ APP_Assertion_Support("domain.C",
          6616); 
          } 
     } 
#if 0
          // If we test this variable (used for default initialization) and the user
          // application does not reference it (indirectly through a member function using
          // it as a default parameter) then the use gets an undefined symbol at link time.

#endif

          // Since I just fixed a bug related to this we add it as a test
     if (this -> Is_A_Null_Array == 1)
        { 
          bool Confirmed_Null_Array=false; 
          for (temp = 0; temp < 6; temp++)
               { if ((this -> Base)[temp] == (this -> Bound)[temp] + 1)
                    { Confirmed_Null_Array = true; } 
          } if (Confirmed_Null_Array == 0)
               { this -> display(("ERROR in Array_Domain_Type::Test_Consistancy()")); } 
          if (!(Confirmed_Null_Array == 1)){ APP_Assertion_Support("domain.C",6635); } 
     } 

#if 1
#if !defined(PPP)
          // check for illegal values in the reference counts
          // if (Array_Reference_Count_Array [internalArrayID] < 0)
          // *********************************************************************************
          // Later we have to remove the -1 adjustment to the getReferenceCountBase() function
          // *********************************************************************************
     if (Array_Domain_Type::Array_Reference_Count_Array[(this -> internalArrayID)] < Array_Domain_Type::getReferenceCountBase()
      - 1){ 
          printf(("ERROR: Array_Reference_Count_Array [internalArrayID] < getReferenceCountBase()-1 --- Array_Reference_Count_Array [internalArrayID] = %d \n"),
          Array_Domain_Type::Array_Reference_Count_Array[(this -> internalArrayID)]); 
          this -> display(("ERROR: Array_Reference_Count_Array [internalArrayID] < getReferenceCountBase()-1")); 
     } 
     if (!(Array_Domain_Type::Array_Reference_Count_Array[(this -> internalArrayID)] >= Array_Domain_Type::getReferenceCountBase()
      - 1)){ APP_Assertion_Support("domain.C",6651); } 
     if (!(Array_Domain_Type::Array_Reference_Count_Array[(this -> internalArrayID)] < 100000)){ APP_Assertion_Support("domain.C",
     6652); 
     }      // Was this descriptor used to build an array object using the adopt function?
     // This is usually FALSE.
     if (this -> builtUsingExistingData == 1)
        { 
        // If we have used the adopt function with some existing data
        // then the reference count for the data reflects this by being 
        // greater than or equal to one instead of zero.
        // APP_ASSERT(Array_Reference_Count_Array [internalArrayID] >= 1);
          if (!(Array_Domain_Type::Array_Reference_Count_Array[(this -> internalArrayID)]
           >= Array_Domain_Type::getReferenceCountBase() + 1)){ APP_Assertion_Support("domain.C",
          6662); } 
     } 
#endif
#else

#endif

          // This is the reference count for the domain object (not the array data)
     if (this -> referenceCount < Array_Domain_Type::getReferenceCountBase())
          { printf(("referenceCount = %d \n"),this -> referenceCount); } 
     if (!(this -> referenceCount >= Array_Domain_Type::getReferenceCountBase())){ APP_Assertion_Support("domain.C",
     6672); 
     } 
#if defined(PPP)
     // Temp range checking on these variables (for debugging only)
#if 0
     // This is commented out so we can run larger problems






#endif






















     // This is the case of P++ on a single processor!


#if COMPILE_DEBUG_STATEMENTS



#endif




     // ... these won't be set correctly if indirect addressing
     //  is used but it doesn't seem to matter ...




#if 0




#endif





     // Mode could be Null_Index or Index_Triplet so just make sure it is not an All_Index
     // There is no test for an Internal_Index except a consistancy test










#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          { printf(("Leaving Array_Domain_Type::Test_Preliminary_Consistency! (Label = %s) \n"),
          Label); } 
} 
#endif


void Array_Domain_Type::Test_Distribution_Consistency(const char * Label) const
   { 
#if (EXTRA_ERROR_CHECKING == FALSE)


#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
        { 
          printf(("Inside of Array_Domain_Type::Test_Distribution_Consistency! (Label = %s) \n"),
          Label); 
     } 
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





          // Make sure that the PADRE_Distribution that the Partition object uses
          // is the same as the one that is used by the PADRE_Descriptor









          // Need to find the PADRE_Distribution used by the default distribution





#if PADRE_DISTRIBUTION_CONSISTENCY_TEST
          // It is not clear if this is a valid test -- it seems that it should be
          // but there are many special cases where it fails!
















          // APP_ASSERT (PartitionObjectPADRE_DistributionPointer == 
          //             ArrayDomainObjectPADRE_DistributionPointer);
#endif

#else
          // We don't have a specific test for the non-PADRE case but I think we should
          // implement one that is similar.














          // printf ("In Array_Domain_Type::Test_Distribution_Consistency(): BlockPartiArrayDomain == NULL \n");

#endif
#endif



void Array_Domain_Type::Test_Consistency(const char * Label) const
   { 
#if (EXTRA_ERROR_CHECKING == FALSE)


#endif

#if 0
#if defined(PPP)

#else

#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
        { 
        // printf ("Inside of Array_Domain_Type::Test_Consistency! (Label = %s) (Domain_Dimension = %d) \n", Label,Domain_Dimension);
          printf(("Inside of Array_Domain_Type::Test_Consistency! (Label = %s) \n"),Label); 
          printf(("Domain_Dimension = %d \n"),this -> Domain_Dimension); 
     } 
#endif

     this -> Test_Preliminary_Consistency(Label); 

#if 0
     // Make sure that the Domain_Dimension value matches what we can compute from the stored 
     // values of the array sizes in the descriptor.  This just checks consistency which is what
     // this function is all about.








#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          { printf(("Inside of Array_Domain_Type::Test_Consistency!  PASSED Description Check \n")); } 
#endif

     if (!(this -> referenceCount >= Array_Domain_Type::getReferenceCountBase())){ APP_Assertion_Support("domain.C",
     6893); 
     } 
     if (!(this -> internalArrayID > 0)){ APP_Assertion_Support("domain.C",6894); } 

     if (!(this -> typeCode > 0)){ APP_Assertion_Support("domain.C",6896); } 
     if (!((((this -> typeCode == 7001) || (this -> typeCode == 7002)) || (this -> typeCode
      == 7003)) || (this -> typeCode == 7000))){ APP_Assertion_Support("domain.C",6897); } 


#if USE_ORDER_N_SQUARED_TESTS
     // This is an expensive O(n^2) operation so use it saringly as a test
     // Search the stack for the existing Array_ID in use (it should not be there!)
     // We might also search for redundent entries -- this is not implemented yet
     // since this would be expensive.





     // Note Pointer arithmetic


     // Top_Of_Stack = Bottom_Of_Stack + Stack_Depth;
     // printf ("In while loop: *StackEntry = %d == internalArrayID = %d \n",*StackEntry,internalArrayID);
     // printf ("*StackEntry     = %d \n",*StackEntry);




#endif

     // Reference this variable so that purify can check that it is initialized!
     if (!((this -> ExpressionTemplateOffset == 0) || this -> ExpressionTemplateOffset
      != 0)){ APP_Assertion_Support("domain.C",6923); } 
     if (!((this -> Is_A_View == 1) || (this -> Is_A_View == 0))){ APP_Assertion_Support("domain.C",
     6924); 
     } 
     if (!((this -> Is_A_Temporary == 1) || (this -> Is_A_Temporary == 0))){ APP_Assertion_Support("domain.C",
     6925); 
     } 
     if (!((this -> Is_Contiguous_Data == 1) || (this -> Is_Contiguous_Data == 0))){ APP_Assertion_Support("domain.C",
     6926); 
     } 
     if (!((this -> Is_Built_By_Defered_Evaluation == 1) || (this -> Is_Built_By_Defered_Evaluation
      == 0))){ APP_Assertion_Support("domain.C",6927); } 
     if (!((this -> Uses_Indirect_Addressing == 1) || (this -> Uses_Indirect_Addressing
      == 0))){ APP_Assertion_Support("domain.C",6928); } 
     if (!((this -> Constant_Data_Base == 1) || (this -> Constant_Data_Base == 0))){ APP_Assertion_Support("domain.C",
     6929); 
     } 
     if (!((this -> Constant_Unit_Stride == 1) || (this -> Constant_Unit_Stride == 0))){ APP_Assertion_Support("domain.C",
     6930); 
     } 
     if (!((this -> Is_A_Null_Array == 1) || (this -> Is_A_Null_Array == 0))){ APP_Assertion_Support("domain.C",
     6931); 
     } 
     if (!(this -> Domain_Dimension >= 0 && this -> Domain_Dimension <= 6)){ APP_Assertion_Support("domain.C",
     6933); 
     } 
int i; 
     for (i = 0; i < 6; i++)
        { 
          if (!(this -> Is_A_View || ((this -> Base)[i] == 0))){ APP_Assertion_Support("domain.C",
          6938); 
          } 
     } 
     if (this -> Is_A_Null_Array == 1)
        { 
           if (!(this -> Is_Contiguous_Data == 0)){ APP_Assertion_Support("domain.C",
           6943); 
           }            // (8/10/2000) We want a consistant setting for this when we have a NullArray object
           // temporarily disabled to pass P++ test_Ppp_execution
           // printf ("In Array_Domain_Type::Test_Consistency(): Constant_Unit_Stride == %s \n",(Constant_Unit_Stride == TRUE) ? "TRUE" : "FALSE");
           if (!(this -> Constant_Unit_Stride == 1)){ APP_Assertion_Support("domain.C",
           6948); 
           } 
           for (i = 0; i < 6; i++)
              { 
                if (!((this -> Base)[i] == 0)){ APP_Assertion_Support("domain.C",6952); } 
                if (!((this -> Bound)[i] == -1)){ APP_Assertion_Support("domain.C",6953); } 
           } 
     } 

                // If we are using indirect addressing then there must be a valid intArray in use
                // however if not then all the pointers to the intArrays should be NULL
     if (this -> Uses_Indirect_Addressing == 1)
        { 
           if (!(this -> Is_Contiguous_Data == 0)){ APP_Assertion_Support("domain.C",
           6961); 
           } 
           bool IndexArrayInUse=false; 
           for (i = 0; i < 6; i++)
              { 
                if ((this -> Index_Array)[i] != (intArray * )0)
                   { 
                     IndexArrayInUse = true; 
                } 
           } 
           if (!(IndexArrayInUse == 1)){ APP_Assertion_Support("domain.C",6970); } 

           // Call the Test_Consistency function on the indirect addressing vectors!
           for (i = 0; i < 6; i++)
              { 
                if ((this -> Index_Array)[i] != (intArray * )0)
                   { 
                     (*(this -> Index_Array)[i]).Test_Consistency(("Indirect Addressing Array")); 
                } 
           } 
     } 
     else 
        { 
           for (i = 0; i < 6; i++)
              { 
                if (!((this -> Index_Array)[i] == (intArray * )0)){ APP_Assertion_Support("domain.C",
                6985); 
                } 
           } 
     }                 // See if this is true in general!
                // APP_ASSERT(User_Base[0] == Data_Base[0] + Base[0]);

#if defined(PPP)
                // printf ("In Array_Domain_Type::Test_Consistency(): Checking SerialArray_Domain_Type::Array_Reference_Count_Array[%d] = %d \n",
                //      internalArrayID,SerialArray_Domain_Type::Array_Reference_Count_Array[internalArrayID]);

                // Bugfix (11/4/2000)
                // P++ arrays should not use the SerialArray_Domain_Type::Array_Reference_Count_Array
                // for tracking reference counts (we need to detect this since it is an error).


                // Bugfix (9/23/2000) This was not previously looping over all i and was using the value of i that
                // previous loops had set i to be (on the DEC this was MAX_ARRAY_DIMENSION)
                // NOTE: The out of bounds array access is NOT caught by purify!!!
                // printf ("Value for i in Test_Consistency() = %d \n",i);
                // printf ("Index_Array[i]       = %p \n",Index_Array[i]);






                // Test added (8/8/2000)











                // We want a consistant setting for this when we have a NullArray object
                // APP_ASSERT ( Constant_Unit_Stride == FALSE );







                // In the global descriptor the local serial array might be a 
                // NullArray (i.e. data is partitioned onto other processors)















                // Case of serialArray being a NullArray (so check the Global_Index)




















#if defined(USE_PADRE)
                // What PADRE function do we call here?
                // printf ("NEED TO CALL PADRE \n"); 
                // APP_ABORT();
                // printf ("WARNING: Ingnoring PADRE specific code in Array_Domain_Type::Test_Consistency! \n");
                // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
                // parallelPADRE_DescriptorPointer->testConsistency(Label);






                // Test commented out on (12/4/2000)
                // APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
#else





                // Make sure that the dimension of the PARTI descriptor matches that of the P++ descriptor!








                //    APP_ASSERT ( BlockPartiArrayDomain->nDims == Array_Domain_Type::computeArrayDimension(*this) );


                // For now we make it an error if the distribution is not done on all processors!
                // BLOCK-PARTI returns a -1 if the array has no distribution on a processor

                // These Block Parti values are checked since as a test of the BlockParti descriptor.
                // The base and bound for block parti assume the default base of an array is ZERO
                // P++ makes up for this internally to provide greater flexibility of the P++ array objects.




                // Data not partitioned on this processor (so verify same result for gUBnd)




                // Data is present on this processor





                // End of USE_PADRE not defined
#endif

                // If an array is a temporary then it contians the information about how the 
                // parallel operations was done so that the message passing can be interpreted
                // at either the operator= or the replace operator (operations that specify the end of
                // an array statement).


                // ... WARNING: turn this off for now ...
#if 0






#endif



                // A non temporary can have a valid Array_Conformability_Info pointer as in:
                //   A(I) = (B+C)(I);
                // Here the temporary B+C is a view and not an A++ temporary (in the A++ sense) but it
                // would have a valid Array_Conformability_Info pointer.
#if 0






#endif


                // Bugfix (9/9/2000) all trailing dimensions must have zero ghost boundary widths





                // These should always be set to ZERO since they are not used





#if 0
                // In the transpose function in testppp this code generates an error by calling the Array_ID()
                // doubleArray member function when the array objects Array_Domain is NULL!
                // So for now we just turn this off.





















































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
          { printf(("Leaving Array_Domain_Type::Test_Consistency! (Label = %s) \n"),Label); } 
} 
#endif


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


void Array_Domain_Type::Error_Checking_For_Index_Operators(Internal_Index * const * Internal_Index_List) const

   { 
   // This function provides error checking for the indexing operators. It is only called
   // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Error_Checking_For_Index_Operators \n")); } 
#endif

          // This array is used when the values of the input Index objects or intArray objects
          // can be simplified to be integers (used in mixed indexing)
     int Integer_List[6]; 

     int i=0; 

     // ... code change (8/20/96, kdb) new scalar indexing changes
     //   meaning of correct range for indexing. The range now must
     //   be between User_Base and User_Base + Count ...
     // Not sure if this is the correct defaults
     for (i = 0; i < 6; i++)
          { Integer_List[i] = (this -> User_Base)[i]; } 
          //Integer_List[i] = Base[i] + Data_Base[i];;

     if (!(Internal_Index_List != (Internal_Index * const * )0)){ APP_Assertion_Support("domain.C",
     7293); 
     }      // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
     // objects can be either an intArray or a scalar (integer) so we have to process the list

     bool Indirect_Addressing_In_Use=false; 
     bool Index_Object_In_Use=false; 
     for (i = 0; i < 6; i++)
        { 
          if (Internal_Index_List[i] != (Internal_Index * )0)
               { Index_Object_In_Use = true; } 
     } 

     if (Index_Object_In_Use && Indirect_Addressing_In_Use)
        { 
          printf(("ERROR: Can\047t mix Index objects with intArray object (indirect addressing) indexing! \n")); 
          APP_ABORT(); 
     } 

          // ... code change (8/20/96, kdb) new scalar indexing changes ...
          // Compute Local_Base and Local_Bound
     int Local_Base[6]; 
     int Local_Bound[6]; 
     for (i = 0; i < 6; i++)
        { 
          Local_Base[i] = (this -> User_Base)[i]; 
          Local_Bound[i] = (this -> User_Base)[i] + ((this -> Bound)[i] - (this -> Base)[i])
           / (this -> Stride)[i];           //Local_Base [i] = Data_Base[i]+Base[i];
          //Local_Bound[i] = Data_Base[i]+Bound[i];
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             { printf(("Local_Base[%d] = %d Local_Bound[%d] = %d \n"),i,Local_Base[i],
             i,Local_Bound[i]); } 
     } 
#endif


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

     int Min[6]; 
     for (i = 0; i < 6; i++)
        { Min[i] = ((Internal_Index_List[i])?(((*Internal_Index_List[i]).Index_Mode == 100)?((*Internal_Index_List[i]).Base):(Local_Base[i])):(Integer_List[i])); } 
        // Min[i] = (intArrayList[i]) ? min(*intArrayList[i]) : (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Base : Local_Base[i] : Integer_List[i];

     int Max[6]; 
     for (i = 0; i < 6; i++)
          { Max[i] = ((Internal_Index_List[i])?(((*Internal_Index_List[i]).Index_Mode
           == 100)?((*Internal_Index_List[i]).Bound):(Local_Bound[i])):(Integer_List[i])); }           // Max[i] = (intArrayList[i]) ? max(*intArrayList[i]) : (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Bound : Local_Bound[i] : Integer_List[i];

          // Bugfix (3/30/95) added test for Null_Array since P++ requires that we allow indexing
          // of the global indexspace outside of a given partition
     if (this -> Is_A_Null_Array == 0)
        { 
        // Check bounds!
          for (i = 0; i < 6; i++)
             { 
               if (Min[i] < Local_Base[i] || Max[i] > Local_Bound[i])
                  { 
                    printf(("\n")); 
                    printf(("**************************************************************** \n")); 
                    printf(("ERROR: (Array:Error_Checking_For_Index_Operators) INDEXING OUT OF BOUNDS                      \n")); 
                    printf(("**************************************************************** \n")); 
                    printf(("ERROR: in Array Indexing using: \n")); 
                    int j; 
                    for (j = 0; j < 6; j++)
                       { 
                         printf(("     (Min[%d] = %d,Max[%d] = %d) \n"),j,Min[j],j,Max[j]); 
                    } 
                    printf(("**************************************************************** \n")); 
                    printf(("Original Array object base and Bounds are: \n")); 
                    for (j = 0; j < 6; j++)
                       { 
                         printf(("     (Local_Base[%d] = %d,Local_Bound[%d] = %d) \n"),
                         j,Local_Base[j],j,Local_Bound[j]); 
                    } 
                    printf(("**************************************************************** \n")); 
                    printf(("\n")); 
                    APP_ABORT(); 
               } 
          } 
     } 
#else

#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          { printf(("Leaving Array_Domain_Type::Error_Checking_For_Index_Operators! \n")); } 
} 
#endif



void Array_Domain_Type::Error_Checking_For_Index_Operators(Internal_Indirect_Addressing_Index * const * Indirect_Index_List) const

   { 
   // This function provides error checking for the indexing operators. It is only called
   // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Error_Checking_For_Index_Operators(Indirect_Index_List) \n")); } 
#endif

          // This array is used when the values of the input Index objects or intArray objects
          // can be simplified to be integers (used in mixed indexing)
     int Integer_List[6]; 
     class Internal_Index * Index_List[6]; 

     int i=0; 

     // ... code change (8/20/96, kdb) new scalar indexing changes ...
     // Not sure if this is the correct defaults
     for (i = 0; i < 6; i++)
          { Integer_List[i] = (this -> User_Base)[i]; } 
          //Integer_List[i] = Base[i] + Data_Base[i];

     if (!(Indirect_Index_List != (Internal_Indirect_Addressing_Index * const * )0)){ APP_Assertion_Support("domain.C",
     7417); 
     } 
class intArray * intArrayList[6]; 
     for (i = 0; i < 6; i++)
        { 
          Index_List[i] = (Internal_Index * )0; 
          intArrayList[i] = (intArray * )0; 
     } 

          // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
          // objects can be either an intArray or a scalar (integer) so we have to process the list
     for (i = 0; i < 6; i++)
        { 
          if (Indirect_Index_List[i] != (Internal_Indirect_Addressing_Index * )0)
             { 
               if ((*Indirect_Index_List[i]).intArrayInStorage)
                    { intArrayList[i] = (*Indirect_Index_List[i]).IndirectionArrayPointer; } 


               else if ((*Indirect_Index_List[i]).IndexInStorage)
                         { Index_List[i] = &(*Indirect_Index_List[i]).Index_Data; } 
               else 
                         { Integer_List[i] = (*Indirect_Index_List[i]).Scalar; } 
          } 
     } 


     int Size_Of_Parameter[6]; 
     for (i = 0; i < 6; i++)
        { 
        // Error checking for conformable indirect addressing
        // Size_Of_Parameter[i] = (intArrayList[i] != NULL) ? intArrayList[i]->elementCount() : 1;
          if (intArrayList[i] != (intArray * )0)
             { 
               Size_Of_Parameter[i] = ((*intArrayList[i]).elementCount)(); 
          }                // printf ("Case of intArrayList[%d] != NULL Size_Of_Parameter[%d] = %d \n",i,i,Size_Of_Parameter[i]);




          else if (Index_List[i] != (Internal_Index * )0)
                  { 
                    Size_Of_Parameter[i] = ((*Index_List[i]).length)(); 
          } 
          else           // printf ("Case of Index_List[%d] != NULL Size_Of_Parameter[%d] = %d \n",i,i,Size_Of_Parameter[i]);

                  { 
                    Size_Of_Parameter[i] = 1; 
          }                     // printf ("Case of SCALAR != NULL Size_Of_Parameter[%d] = %d \n",i,Size_Of_Parameter[i]);



#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               { printf(("Size_Of_Parameter[%d] = %d \n"),i,Size_Of_Parameter[i]); } 
     } 
#endif


     int Max_Size=0; 
     for (i = 0; i < 6; i++)
        { 
          Max_Size = ((Size_Of_Parameter[i] > Max_Size)?(Size_Of_Parameter[i]):Max_Size); 
     } 

     for (i = 0; i < 6; i++)
        { 
        // Zero should be an acceptable value since in the case of a NULL array used as an indirection array
        // We allow the mixing of scalars and intArray indexing (indirect addressing)
          if (Size_Of_Parameter[i] != Max_Size && Size_Of_Parameter[i] != 1)
             { 
               printf(("\n")); 
               printf(("******************************************************************** \n")); 
               printf(("ERROR - intArrays used for indirect addressing must be the same size \n")); 
               printf(("******************************************************************** \n")); 
               printf(("MORE INFO: Inside of indexing operator error checking -- non conformable indirect addressing Max_Size = %d  Size_Of_Parameter list - \n"),
               Max_Size); 
int j; 
               for (j = 0; j < 6; j++)
                    { printf(("%d "),Size_Of_Parameter[i]); } 
               printf(("\n")); 
               printf(("******************************************************************** \n")); 
               printf(("\n")); 
               APP_ABORT(); 
          } 
     } 

               // ... code change (8/20/96, kdb) new scalar indexing changes ...
               // Compute Local_Base and Local_Bound
     int Local_Base[6]; 
     int Local_Bound[6]; 
     for (i = 0; i < 6; i++)
        { 
          Local_Base[i] = (this -> User_Base)[i]; 
          Local_Bound[i] = (this -> User_Base)[i] + ((this -> Bound)[i] - (this -> Base)[i])
           / (this -> Stride)[i];           //Local_Base [i] = Data_Base[i]+Base[i];
          //Local_Bound[i] = Data_Base[i]+Bound[i];
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             { printf(("Local_Base[%d] = %d Local_Bound[%d] = %d \n"),i,Local_Base[i],
             i,Local_Bound[i]); } 
     } 
#endif


             // Optionally turn off the bounds checking since it adds to the output when debugging!
#if TURN_ON_BOUNDS_CHECKING
     bool intArrayTemporaryList[6]; 
     for (i = 0; i < 6; i++)
        { 
          intArrayTemporaryList[i] = ((intArrayList[i])?((((*intArrayList[i]).Array_Descriptor).Array_Domain).Is_A_Temporary):0); 
     } 

          // Set Array destriptor as a non-temporary!
          // Otherwise the temporaries will be deleted by the min max functions!
          // Bug fix (12/14/94) We have to set the Is_A_Temporary flag consistently in A++ and P++
          // so in P++ we have to change the flag status for both P++ and the internal A++ array.
          // and then reset them both in the case of P++.
     for (i = 0; i < 6; i++)
        { 
          if (intArrayTemporaryList[i] == 1)
             { 
               (((*intArrayList[i]).Array_Descriptor).Array_Domain).Is_A_Temporary = 0; 
          } 
     } 
#if defined(PPP)


#endif



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
     for (i = 0; i < 6; i++)
          { if (intArrayList[i] != (intArray * )0)
               { (*intArrayList[i]).Test_Consistency(("Array_Domain_Type::Error_Checking_For_Index_Operators (intArrayList[i])")); } 
     } 
#endif

     int Min[6]; 
     for (i = 0; i < 6; i++)
        { 
        // Min[i] = (intArrayList[i]) ? min(*intArrayList[i]) : Integer_List[i];
          if (intArrayList[i] != (intArray * )0)
               { Min[i] = min((*intArrayList[i])); } 


          else if (Index_List[i] != (Internal_Index * )0)
                    { Min[i] = (*Index_List[i]).Base; } 
          else 
                    { Min[i] = Integer_List[i]; } 
     } 



     int Max[6]; 
     for (i = 0; i < 6; i++)
        { 
        // Max[i] = (intArrayList[i]) ? max(*intArrayList[i]) : Integer_List[i];
          if (intArrayList[i] != (intArray * )0)
               { Max[i] = max((*intArrayList[i])); } 


          else if (Index_List[i] != (Internal_Index * )0)
                    { Max[i] = (*Index_List[i]).Bound; } 
          else 
                    { Max[i] = Integer_List[i]; } 
     } 


                    // Reset Array destriptor as a temporary!
     for (i = 0; i < 6; i++)
        { 
          if (intArrayTemporaryList[i] == 1)
             { 
               (((*intArrayList[i]).Array_Descriptor).Array_Domain).Is_A_Temporary = 1; 
          } 
     } 
#if defined(PPP)


#endif



               // Bugfix (3/30/95) added test for Null_Array since P++ requires that we allow indexing
               // of the global indexspace outside of a given partition
     if (this -> Is_A_Null_Array == 0)
        { 
        // Check bounds!
          for (i = 0; i < 6; i++)
             { 
               if (Min[i] < Local_Base[i] || Max[i] > Local_Bound[i])
                  { 
                    printf(("\n")); 
                    printf(("**************************************************************** \n")); 
                    printf(("     ERROR: (indirect addressing\?) INDEXING OUT OF BOUNDS                      \n")); 
                    printf(("**************************************************************** \n")); 
                    printf(("ERROR: in Array Indexing using: \n")); 
                    int j; 
                    for (j = 0; j < 6; j++)
                       { 
                         printf(("     (Min[%d] = %d,Max[%d] = %d) \n"),j,Min[j],j,Max[j]); 
                    } 
                    printf(("**************************************************************** \n")); 
                    printf(("Original Array object base and Bounds are: \n")); 
                    for (j = 0; j < 6; j++)
                       { 
                         printf(("     (Local_Base[%d] = %d,Local_Bound[%d] = %d) \n"),
                         j,Local_Base[j],j,Local_Bound[j]); 
                    } 
                    printf(("**************************************************************** \n")); 
                    printf(("\n")); 
                    APP_ABORT(); 
               } 
          } 
     } 
#else

#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          { printf(("Leaving Array_Domain_Type::Error_Checking_For_Index_Operators! \n")); } 
} 
#endif


          // **************************************************************************
          // This function does the bounds checking for the scalar indexing of A++
          // array objects.  Its purpose is to localize all the error checking for
          // scalar indexing.
          // **************************************************************************
          // Old prototype
          // void Array::Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const

void Array_Domain_Type::Error_Checking_For_Scalar_Index_Operators(int * const * Integer_List) const

   { 
   // Set maximum dimension of indexing
     int Problem_Dimension=6; 

     // Lower the dimension as indicated by the input

     int i; 
     for (i = 5; i >= 0; i--)
          { if (Integer_List[i] == (int * )0)
               { Problem_Dimension--; } 
     } 
               // No support for the scalar indexing of views obtained through indirect addressing
     if (this -> Uses_Indirect_Addressing)
        { 
        // printf ("Error Checking for scalar Indexing operations on views built through indirect addressing not implemented yet! \n");
        // APP_ABORT();
          if (!(Problem_Dimension == 1)){ APP_Assertion_Support("domain.C",7670); } 
          for (i = 0; i < 6; i++)
             { 
               if ((this -> Index_Array)[i] != (intArray * )0)
                    { (((*(this -> Index_Array)[i]).Array_Descriptor).Array_Domain).Error_Checking_For_Scalar_Index_Operators(Integer_List); } 
          } 
     } 
     else 
        { 
        // ... first check dimensionality ...
   if (this -> Domain_Dimension > Problem_Dimension)
   { 
     for (i = Problem_Dimension; i < this -> Domain_Dimension; i++)
     { 
       if ((this -> Base)[i] != (this -> Bound)[i])
       { 
         printf(("ERROR: Not enough subscripts, \n")); 
         printf(("  Domain_Dimension = %d, \n"),this -> Domain_Dimension); 

                printf(("  Number of subscripts is %d.\n"),Problem_Dimension); 

                APP_ABORT(); 
       } 
     } 
   } 
   else if (this -> Domain_Dimension < Problem_Dimension)
   { 
     for (i = this -> Domain_Dimension; i < Problem_Dimension; i++)
     { 
     // ... code change (8/20/96, kdb) new scalar indexing changes ...
     //if ((Base[i] != Bound[i]) && 
     //  ((Base[i]+Data_Base[i]) != *Integer_List[i]))
       if ((this -> Base)[i] != (this -> Bound)[i] && (this -> User_Base)[i] != *Integer_List[i])

              { 
  printf(("ERROR: too many subscripts, \n")); 
                printf(("  Number of subscripts is %d.\n"),Problem_Dimension); 

         printf(("  Domain_Dimension = %d, \n"),this -> Domain_Dimension); 

                APP_ABORT(); 
       } 
     } 
   } 
          for (i = 0; i < Problem_Dimension; i++)
             { 
             // ... code change (8/20/96, kdb) new scalar indexing changes ...
             //if ( (*Integer_List[i] < Base[i]  + Data_Base[i]) ||
             //   (*Integer_List[i] > Bound[i] + Data_Base[i]) )
               if (*Integer_List[i] < (this -> User_Base)[i] || *Integer_List[i] > (this
                -> User_Base)[i] + ((this -> Bound)[i] - (this -> Base)[i]) / (this -> Stride)[i])

                  { 
                    printf(("ERROR: Scalar Index for dimension(%d) = %d out of bounds! \n"),
                    i,*Integer_List[i]); 
                    printf(("********************************************************* \n")); 
                    printf(("Scalar values used in indexing: \n")); 
                    int j; 
                    for (j = 0; j < Problem_Dimension; j++)
                       { 
                         printf(("     Scalar Index (along dimension %d) = %d \n"),j,
                         *Integer_List[j]); 
                    } 
                    printf(("**************************************************************** \n")); 
                    printf(("Original Array object base and Bounds are: \n")); 
                    for (j = 0; j < 6; j++)
                       { 
                       //printf ("    (Base[%d] = %d,Bound[%d] = %d)\n",
                       //j,Base[j]+Data_Base[j],j,Bound[j]+Data_Base[j]);
                         printf(("     (Base[%d] = %d,Bound[%d] = %d)\n"),j,(this -> User_Base)[j],
                         j,((this -> Bound)[j] - (this -> Base)[j]) / (this -> Stride)[j]
                          + (this -> Data_Base)[j]); 
                    } 
                    printf(("**************************************************************** \n")); 
                    printf(("\n")); 
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

#endif

                    // defined in array.C



void Array_Domain_Type::Initialize_View_Of_Target(int Axis,const Internal_Index & X,const Array_Domain_Type & Target_Domain)

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
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Initialize_View_Of_Target ( int, Internal_Index, Array_Domain_Type ) \n")); } 

          // We don't want to print this out all the time
     if (APP_DEBUG > 5)
        { 
          X.display(("Inside of Array_Domain_Type::Initialize_View_Of_Target: X Internal_Index object")); 
          Target_Domain.display(("Inside of Array_Domain_Type::Initialize_View_Of_Target: Target_Domain")); 
     } 
#endif

     if (Target_Domain.Uses_Indirect_Addressing)
        { 
          printf(("Bug in Internal_Index operations on views built through indirect addressing (not fixed yet!) \n")); 
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


          // ... this doesn't work because Local_Index_Mask needs to know 
          // global info for correct numbering ...
          // Local_Mask_Index [Axis] = Target_Domain.Local_Mask_Index [Axis](x);





          // ... NOTE: there must be an easier way to compute the first value
          // secified by Global_Index that is larger than or equal to the
          // Target_Domain.Local_Mask_Index[Axis].Base! ...











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


          // ... bug fix (9/5/96,kdb) make sure that Temp_Base is contained locally
          // and is in the range specified by X before going to the trouble of finding
          // Temp_Bound.  Otherwise tthe Local_Mask_Index will be NULL ...









          // ... divide and multiple to find the Bound less than or equal to
          //  the Target_Domain Bound ...


          /*
            // Target_Domain.Local_Mask_Index[Axis].Stride) *
            // Target_Domain.Local_Mask_Index[Axis].Stride;
	    */













#if COMPILE_DEBUG_STATEMENTS


#endif



          // Mode could be Null_Index so just make sure it is not and All_Index



#if COMPILE_DEBUG_STATEMENTS





#endif















#if COMPILE_DEBUG_STATEMENTS







#endif




          // There may be some redundent initialization here


#endif

          // Used in Initialize_Non_Indexed_Dimension but set in Initialize_Domain!
     this -> Uses_Indirect_Addressing = 0; 

     (this -> Size)[Axis] = (Target_Domain.Size)[Axis]; 

     // Bug fix (11/11/94) Data base is same between view and original descriptor so that
     // the orginal geometry can be computed (i.e. it is preserved in the view)
     (this -> Data_Base)[Axis] = (Target_Domain.Data_Base)[Axis]; 

     if (X.Index_Mode == 100)
        { 
        // Most common case!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               { printf(("X.Index_Mode == Index_Triplet \n")); } 
#endif
               // Bug fix (8/11/94)
               // Base   [Axis] = X.Base  - Target_Domain.Data_Base[Axis];
               // Bound  [Axis] = X.Bound - Target_Domain.Data_Base[Axis];
          (this -> User_Base)[Axis] = X.Base; 

          // printf ("Setup in Array_Domain_Type::Initialize_View_Of_Target ( int, Internal_Index, Array_Domain_Type ): User_Base[%d] = %d \n",Axis,User_Base[Axis]);

          // Note that the strides multiply!
          (this -> Stride)[Axis] = X.Stride * (Target_Domain.Stride)[Axis]; 

          // This is the reason we require the User_Base for computing 
          // views of views (the strides > 1) make this more complex
          (this -> Base)[Axis] = (X.Base - (Target_Domain.User_Base)[Axis]) * (Target_Domain.Stride)[Axis]
           + (Target_Domain.Base)[Axis]; (this -> Bound)[Axis] = (this -> Base)[Axis]
           + (X.Count - 1) * (this -> Stride)[Axis]; 
          // For example -- if we  i n d e x  a stride 2 view with a
          // stride 2  i n d e x  then we require stride 4 access!
          // Most of the time Target_Domain.Stride [Axis] == 1!

          if (!((this -> Stride)[Axis] >= 1)){ APP_Assertion_Support("domain.C",7972); } 
     } 


     else if (X.Index_Mode == 102)
             { 
             // All_Index is the default mode for an Index and it means access all of the 
             // dimension of any array it is applied to in the construction of a view 
             // through indexing.
             // 2nd most common case!
               (this -> User_Base)[Axis] = (Target_Domain.User_Base)[Axis]; 
               (this -> Base)[Axis] = (Target_Domain.Base)[Axis]; 
               (this -> Bound)[Axis] = (Target_Domain.Bound)[Axis]; 
               (this -> Stride)[Axis] = (Target_Domain.Stride)[Axis]; 
     } 
               // Modify the Geometry base to reflect the indexed view
               // Geometry_Base [Axis] = Target_Domain.Geometry_Base[Axis];




          else if (X.Index_Mode == 101)
                  { 
                    (this -> User_Base)[Axis] = (this -> Data_Base)[Axis]; 
                    (this -> Base)[Axis] = 0; 
                    (this -> Bound)[Axis] = -1; 
                    (this -> Stride)[Axis] = 1; 
                    this -> Is_A_Null_Array = 1; 

                    // We force this to be FALSE in the case of a Null Array (generated by an Null_Index).
                    this -> Is_Contiguous_Data = 0; 
          } 
          else           // Modify the Geometry base to reflect the indexed view
          // Geometry_Base [Axis] = Target_Domain.Geometry_Base[Axis];

#if COMPILE_DEBUG_STATEMENTS

                  { 
                    printf(("ERROR: Inside of Array_Domain_Type::Initialize_View_Of_Target ")); 
                    printf(("-- Index_Mode unknown! \n")); 
                    APP_ABORT(); 
          } 
} 
#endif




                    // ************************************************************************************
                    // ************************************************************************************
                    //             Initialization of descriptors for Non specified dimensions
                    // ************************************************************************************
                    // ************************************************************************************

void Array_Domain_Type::Initialize_Non_Indexed_Dimension(int Axis,const Array_Domain_Type & Target_Domain)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        { 
          printf(("Inside of Array_Domain_Type::Initialize_Non_Indexed_Dimension ")); 
          printf(("( Array_Domain_Type & Target_Domain , ")); 
          printf(("int (Axis = %d) ) \n"),Axis); 
     } 
#endif

     if (!(this -> Uses_Indirect_Addressing == 0)){ APP_Assertion_Support("domain.C",
     8035); 
     } 
     (this -> Data_Base)[Axis] = (Target_Domain.Data_Base)[Axis]; 
     (this -> User_Base)[Axis] = (Target_Domain.User_Base)[Axis]; 

     // This forces non indexed dimension (last dimensions) 
     // to be equivalent to indexing with an ALL Index object.
     (this -> Base)[Axis] = (Target_Domain.Base)[Axis]; 
     (this -> Bound)[Axis] = (Target_Domain.Bound)[Axis]; 

     (this -> Size)[Axis] = (Target_Domain.Size)[Axis]; 
     (this -> Stride)[Axis] = (Target_Domain.Stride)[Axis]; 
} 
#if defined(PPP)









     // There may be some redundent initialization here


#endif

#if 0











#endif


     // ************************************************************************************
     // ************************************************************************************
     //                Initialization of descriptors for Indirect Addressing
     // ************************************************************************************
     // The scalar values are setup differently for Indirect addressing so we require a 
     // special function for this
     // ************************************************************************************
     // ************************************************************************************

void Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target(int Axis,int x,const Array_Domain_Type & Target_Domain)

   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target ( int, int=%d, Array_Domain_Type ) \n"),
          x); } 
#endif

     (this -> Size)[Axis] = (Target_Domain.Size)[Axis]; 
     // Bugfix (3/16/96) must match behavior of Internal_Index indexing (non indirect addressing)
     // Base      [Axis] = x;
     (this -> Base)[Axis] = x - (Target_Domain.Data_Base)[Axis]; 
     (this -> Bound)[Axis] = (this -> Base)[Axis]; 
     (this -> Stride)[Axis] = (Target_Domain.Stride)[Axis]; 
     (this -> Data_Base)[Axis] = (Target_Domain.Data_Base)[Axis]; 
     (this -> User_Base)[Axis] = (Target_Domain.User_Base)[Axis]; 
} 
#if defined(PPP)
     // In P++ we save the global Index that was used since it helps drive the message passing
     // The Local_Index is not set here (it is set in the Parallel_Conformability_Enforcement function)







     // I don't know if this is a good test to have here!





     //printf ("Since indirect addressing is not debugged the following may be incorrect! \n");
     //APP_ABORT();


     // Global_Index[Axis]     = Target_Domain.Global_Index[Axis];








     // There may be some redundent initialization here


#endif


     // ************************************************************************************
     // The scalar values are setup differently for Indirect addressing so we require a
     // special function for this
     // ************************************************************************************
     // ************************************************************************************

void Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target(int Axis,const Internal_Index & X,
const Array_Domain_Type & Target_Domain)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target ( int, Internal_Index, Array_Domain_Type ) \n")); } 
#endif

     if (X.Index_Mode != 101)
     { 
        (this -> Size)[Axis] = (Target_Domain.Size)[Axis]; 
        //Base      [Axis] = X.Base - Target_Domain.Data_Base[Axis];
        (this -> Base)[Axis] = (X.Base - (Target_Domain.User_Base)[Axis]) * (Target_Domain.Stride)[Axis]
         + (Target_Domain.Base)[Axis]; 
        //Stride    [Axis] = X.Stride;
        (this -> Stride)[Axis] = X.Stride * (Target_Domain.Stride)[Axis]; 
        //Bound     [Axis] = X.Bound;
        (this -> Bound)[Axis] = (this -> Base)[Axis] + (X.Count - 1) * (this -> Stride)[Axis]; 
        (this -> Data_Base)[Axis] = (Target_Domain.Data_Base)[Axis]; 
        //User_Base [Axis] = Target_Domain.User_Base[Axis];
        (this -> User_Base)[Axis] = X.Base; 
     } 
     else 
     { 
        (this -> Size)[Axis] = (Target_Domain.Size)[Axis]; 
        (this -> Base)[Axis] = 0; 
        (this -> Stride)[Axis] = 1; 
        (this -> Bound)[Axis] = -1; 
        (this -> Data_Base)[Axis] = (Target_Domain.Data_Base)[Axis]; 
        (this -> User_Base)[Axis] = 0; 
     } 
} 
#if defined(PPP)
        // In P++ we save the global Index that was used since it helps drive the message passing
        // The Local_Index is not set here (it is set in the Parallel_Conformability_Enforcement function)







        // I don't know if this is a good test to have here!





        //printf ("Since indirect addressing is not debugged the following may be incorrect (Initialize_Indirect_Addressing_View_Of_Target(int,Internal_Index,Array_Domain_Type))! \n");
        //APP_ABORT();


        // Global_Index[Axis]     = Target_Domain.Global_Index[Axis];







        // There may be some redundent initialization here


#endif


        // ************************************************************************************
        // ************************************************************************************
        //                Initialization of descriptors for Indirect Addressing
        // ************************************************************************************
        // ************************************************************************************

void Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target(int Axis,const intArray & X,
const Array_Domain_Type & Target_Domain)
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
     if (APP_DEBUG > 5)
        { 
          printf(("Inside of Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target ( int, intArray, Array_Domain_Type ) \n")); 
          X.display(("X intArray Index object")); 
     } 
#endif
          // Used in Initialize_Non_Indexed_Dimension but set in Initialize_Domain!
     this -> Uses_Indirect_Addressing = 1; 

     (this -> Size)[Axis] = (Target_Domain.Size)[Axis]; 

     // The Data_Base is set to APP_Global_Array_Base not the Base.
     // Base      [Axis] = APP_Global_Array_Base;
     (this -> Base)[Axis] = 0; 
     (this -> Bound)[Axis] = (this -> Base)[Axis]; 
     (this -> Stride)[Axis] = (Target_Domain.Stride)[Axis]; 

     if (Target_Domain.Uses_Indirect_Addressing)
        { 
        // If indexed with a intArray we assume Geometry_Base [Axis] = 0
        // We have to assume something and we can not afford to search for the
        // minimum of the intArray
          (this -> Data_Base)[Axis] = APP_Global_Array_Base; 
          (this -> User_Base)[Axis] = APP_Global_Array_Base; 
     } 
     else 
        { 
        // Bug fix (9/27/94) make consistant with scalar and Index object indexing!
          (this -> Data_Base)[Axis] = (Target_Domain.Data_Base)[Axis]; 
          (this -> User_Base)[Axis] = (Target_Domain.User_Base)[Axis]; 
     } 

          // We can pick any axis since the intArray objects are conformable
     if (Axis == 0)
          { (this -> Bound)[Axis] += (X.elementCount)() - 1; } 
} 
#if defined(PPP)
          // printf ("Since indirect addressing is not debugged the following may be incorrect! \n");
          // WARNING: this is not correct but it might not be used anyways in indirect addressing


          // special case for Axis 0 for indirect addressing to make consistant with base and bound









          // There may be some redundent initialization here


#endif


          // ************************************************************************************
          // ************************************************************************************
          //             Initialization of descriptors for Non specified dimensions
          // ************************************************************************************
          // ************************************************************************************

void Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension(int Axis,
const Array_Domain_Type & X)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Inside of Array_Domain_Type::Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension ( Array_Domain_Type & X , int (Axis = %d) ) \n"),
          Axis); } 
#endif

     if (!(this -> Uses_Indirect_Addressing == 1)){ APP_Assertion_Support("domain.C",
     8302); 
     } 
     (this -> Data_Base)[Axis] = (X.Data_Base)[Axis]; 
     (this -> User_Base)[Axis] = (X.User_Base)[Axis]; 

     // Bugfix (10/3/2000) Fix for base of non accessed dimension in indirect addressing
     // Base          [Axis] = X.Data_Base[Axis];
     (this -> Base)[Axis] = (X.User_Base)[Axis]; 

     (this -> Bound)[Axis] = (this -> Base)[Axis]; 

     (this -> Size)[Axis] = (X.Size)[Axis]; 
     (this -> Stride)[Axis] = (X.Stride)[Axis]; 
} 
#if defined(PPP)









     // There may be some redundent initialization here


#endif


     // ************************************************************************************
     // ************************************************************************************
     //             General Initialization of descriptors 
     // ************************************************************************************
     // ************************************************************************************

void Array_Domain_Type::Preinitialize_Domain(const Array_Domain_Type & X)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Preinitialize_Domain ( const Array_Domain_Type & X )\n")); } 
#endif

          // Take this out soon!
          // The typeCode records the element type associated with the array for which
          // we use this domain object.  This is used only for internal diagnostics.
          // The value 'u' makes the element type as undefined.  This mechanism might be
          // replaced by an alternative mechanism in the future.
          // setTypeCode(APP_UNDEFINED_ELEMENT_TYPE);
     this -> typeCode = 7000; 

     // Make sure that only the values of TRUE and FALSE (1 and 0) are used
     if (!((X.Is_A_Null_Array == 1) || (X.Is_A_Null_Array == 0))){ APP_Assertion_Support("domain.C",
     8354); 
     } 
     this -> Is_A_Null_Array = X.Is_A_Null_Array; 
} 

     // ************************************************************************************
     // ************************************************************************************
     //             General Initialization of descriptors 
     // ************************************************************************************
     // ************************************************************************************

void Array_Domain_Type::Initialize_IndirectAddressing_Domain_Using_IndirectAddressing(const Array_Domain_Type & X,
Internal_Indirect_Addressing_Index * const * Indirect_Index_List)

   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Inside of Array_Domain_Type::Initialize_IndirectAddressing_Domain_Using_IndirectAddressing ( const Array_Domain_Type & X , ALL PARMETERS )\n")); } 
#endif

     printf(("Inside of Array_Domain_Type::Initialize_IndirectAddressing_Domain_Using_IndirectAddressing \n")); 
     printf(("Sorry, not implemented: Indexing of views built from indirect addressing temporarily disabled \n")); 
     APP_ABORT(); 

     int i=0; 
     class intArray * intArrayList[6]; 
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List; 
     class Internal_Index * Index_List[6]; 
     for (i = 0; i < 6; i++)
        { 
          Index_List[i] = (Internal_Index * )0; 
          intArrayList[i] = (intArray * )0; 

          // if defined(USE_EXPRESSION_TEMPLATES)
          (this -> IndexBase)[i] = 0; 
          (this -> IndexStride)[i] = 0; 
          (this -> IndexDataPointer)[i] = (int * )0; 
     }           // endif


          // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
          // objects can be either an intArray or a scalar (integer) so we have to process the list.
          // Or it can be an Internal_Index (added (3/15/96)).
     for (i = 0; i < 6; i++)
        { 
          if (Indirect_Index_List[i] != (Internal_Indirect_Addressing_Index * )0)
             { 
               if ((*Indirect_Index_List[i]).intArrayInStorage)
                  { 
                    intArrayList[i] = (*Indirect_Index_List[i]).IndirectionArrayPointer; 
               } 


               else if ((*Indirect_Index_List[i]).IndexInStorage)
                       { 
                       // printf ("Case of Index and intArray indexing not handled yet! \n");
                       // APP_ABORT();
                         Index_List[i] = &(*Indirect_Index_List[i]).Index_Data; 
               } 
               else 
                       { 
                         Integer_List[i] = (*Indirect_Index_List[i]).Scalar; 
               } 
          } 
     } 


     if (!(X.Uses_Indirect_Addressing == 1)){ APP_Assertion_Support("domain.C",8420); } 

     // **********************************************************************
     // Case of intArray indexing of an A++ object (a view) already indexed 
     // by indirect addressing.
     // **********************************************************************
     for (i = 0; i < 6; i++)
        { 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               { printf(("X.Uses_Indirect_Addressing == TRUE (i=%d) \n"),i); } 
#endif
               // Case of Target being an array previously indexed using indirect addressing
          if ((X.Index_Array)[i] != (intArray * )0)
             { 
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    { printf(("   X.Index_Array [i] is a valid pointer = %p \n"),(X.Index_Array)[i]); } 
#endif
                    // Index_Array [i] = new intArray;
               if (this -> Domain_Dimension == 1)
                  { 
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 0)
                         { printf(("Domain_Dimension == 1! \n")); } 
#endif

                         // Only the first intArray is meaningful here since it is reused 
                         // to INDEX the X.Index_Array in each dimension.
                    if (intArrayList[0] != (intArray * )0)
                       { 
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 0)
                              { printf(("intArrayList[0] is a valid pointer! \n")); } 
#endif
                              // Bug fix (4/17/94) Must force a copy since recursive views 
                              // of indirect addressing can not be readily supported!
                              // actually we only want the size of the Parm0_intArray!
                              // the copying is redundent and inefficient
                         (this -> Index_Array)[i] = new intArray((*intArrayList[0])); 
                         (*(this -> Index_Array)[i])=(*(X.Index_Array)[i])(Internal_Indirect_Addressing_Index(*intArrayList[0])); 
                    } 


                    else if (Index_List[0] != (Internal_Index * )0)
                            { 
                              (this -> Index_Array)[i] = new intArray(); 
                              (*(this -> Index_Array)[i]).reference((*(X.Index_Array)[i])(*Index_List[0])); 
                    } 
                    else 
                            { 
                            // printf ("ERROR: Not a valid dimension accessed in Initialize Domain! \n");
                            // APP_ABORT();
                              (this -> Index_Array)[i] = new intArray(); 
                              (*(this -> Index_Array)[i]).reference(intArray((*(X.Index_Array)[i])(Integer_List[0]))); 
                    } 
               } 

#if COMPILE_DEBUG_STATEMENTS
               if (this -> Domain_Dimension > 1)
                  { 
                    printf(("SORRY not implemented (view of indirect addressing view (Domain_Dimension > 1))! \n")); 
                    APP_ABORT(); 
               } 
          } 
          else 
#endif

             { 
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    { printf(("   X.Index_Array [i] is NULL = %p \n"),(X.Index_Array)[i]); } 
#endif
               (this -> Index_Array)[i] = (intArray * )0; 
          } 
     } 
} 

               // ************************************************************************************
               // ************************************************************************************
               //             General Initialization of descriptors 
               // ************************************************************************************
               // ************************************************************************************

void Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing(const Array_Domain_Type & X,
Internal_Indirect_Addressing_Index * const * Indirect_Index_List)

   { 
   // This function only needs to handle the case of the intArrays not the Index or Scalar indexing
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Inside of Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing ( const Array_Domain_Type & X , Indirect_Index_List )\n")); } 
#endif

     int i=0; 
     class intArray * intArrayList[6]; 
     class Internal_Index * Index_List[6]; 
     for (i = 0; i < 6; i++)
        { 
          intArrayList[i] = (intArray * )0; 
          Index_List[i] = (Internal_Index * )0; 

          // if defined(USE_EXPRESSION_TEMPLATES)
          (this -> IndexBase)[i] = 0; 
          (this -> IndexStride)[i] = 0; 
          (this -> IndexDataPointer)[i] = (int * )0; 
     }           // endif


          // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
          // objects can be either an intArray or a scalar (integer) so we have to process the list
     for (i = 0; i < 6; i++)
        { 
          if (Indirect_Index_List[i] != (Internal_Indirect_Addressing_Index * )0)
             { 
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    { printf(("Indirect_Index_List[%d] != NULL \n"),i); } 
#endif
               if ((*Indirect_Index_List[i]).intArrayInStorage)
                  { 
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         { printf(("Indirect_Index_List[i]->intArrayInStorage == TRUE \n")); } 
                    if (!((*Indirect_Index_List[i]).IndirectionArrayPointer != (intArray * )0)){ APP_Assertion_Support("domain.C",
                    8543); 
                    }                     // Test the integer array object being used as an indirection vector!
                    (*(*Indirect_Index_List[i]).IndirectionArrayPointer).Test_Consistency(("Inside of Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing(Array_Domain_Type,Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type)")); 

#endif

                    intArrayList[i] = (*Indirect_Index_List[i]).IndirectionArrayPointer; 
               } 
               else 
                  { 
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         { printf(("Indirect_Index_List[i]->intArrayInStorage == FALSE \n")); } 
#endif
                    if ((*Indirect_Index_List[i]).IndexInStorage)
                       { 
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                              { printf(("Indirect_Index_List[i]->IndexInStorage == FALSE \n")); } 
#endif
                         Index_List[i] = &(*Indirect_Index_List[i]).Index_Data; 
                    } 


#if COMPILE_DEBUG_STATEMENTS

                    else if (APP_DEBUG > 1)
                              { printf(("Indirect_Index_List[i]->Scalar = %d \n"),(*Indirect_Index_List[i]).Scalar); } 
               } 
          } 
          else 
#endif
          // We don't have to worry about this case since it has 
          // nothing to do with indirect addressing (I think)
          // This is a REACHABLE case!
          // Integer_List[i] = Indirect_Index_List[i]->Scalar;
          // printf ("This case should be unreachable in Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing() \n");
          // APP_ABORT();




             { } 
     }              // printf ("Indirect_Index_List[%d] == NULL \n",i);



     if (!(X.Uses_Indirect_Addressing == 0)){ APP_Assertion_Support("domain.C",8586); } 

     for (i = 0; i < 6; i++)
          { (this -> Index_Array)[i] = (intArray * )0; } 

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("X.Uses_Indirect_Addressing == FALSE \n")); } 
#endif

     for (i = 0; i < 6; i++)
        { 
          if (!((intArrayList[i] == (intArray * )0) || (Index_List[i] == (Internal_Index * )0))){ APP_Assertion_Support("domain.C",
          8598); 
          } 
          if (intArrayList[i] != (intArray * )0)
             { 
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    { printf(("Build intArrayList[%d] using intArray \n"),i); } 
#endif
               if ((((*intArrayList[i]).Array_Descriptor).Array_Domain).Uses_Indirect_Addressing)
                  { 
                  // If the intArray being used for indirect addressing is an array which uses
                  // indirect addressing thenwe are forced to make a copy of the resulting array because
                  // the MDI layer can not handle such nested indirect addressing complexity.
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 2)
                         { printf(("intArrayList[i]->Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE! \n")); } 
#endif
                    if (!((this -> Index_Array)[i] == (intArray * )0)){ APP_Assertion_Support("domain.C",
                    8614); 
                    } 
                    (this -> Index_Array)[i] = new intArray((*intArrayList[i])); 
                    if (!((this -> Index_Array)[i] != (intArray * )0)){ APP_Assertion_Support("domain.C",
                    8616); 
                    } 
               } 
               else 
                  { 
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 2)
                         { printf(("intArrayList[i]->Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == FALSE! \n")); } 
#endif
                    (*intArrayList[i]).incrementReferenceCount(); 
                    if (!((this -> Index_Array)[i] == (intArray * )0)){ APP_Assertion_Support("domain.C",
                    8625); 
                    } 
                    (this -> Index_Array)[i] = intArrayList[i]; 
                    if (!((this -> Index_Array)[i] != (intArray * )0)){ APP_Assertion_Support("domain.C",
                    8627); 
                    } 
               } 
#if COMPILE_DEBUG_STATEMENTS
               if (Diagnostic_Manager::getReferenceCountingReport() > 0)
                  { 
                  // This mechanism outputs reports which allow us to trace the reference counts
                    (*(this -> Index_Array)[i]).displayReferenceCounts(("In Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing(): indirection vector (intArray)")); 
               } 
               (*(this -> Index_Array)[i]).Test_Consistency(("Testing intArray indexing in Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing")); 
#endif

               // How take care of cases where the input indirection arrays where
               // temporaries (the results if expressions)
               if (!(intArrayList[i] != (intArray * )0)){ APP_Assertion_Support("domain.C",
               8641); 
               }                // if (intArrayList[i]->Array_Descriptor.Array_Domain.Is_A_Temporary)
               if ((*intArrayList[i]).isTemporary())
                  { 
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 0)
                         { printf(("intArrayList[i]->isTemporary() == TRUE! \n")); } 
#endif
                         // Added conventional mechanism for reference counting control
                         // operator delete no longer decriments the referenceCount.
                    (*intArrayList[i]).decrementReferenceCount(); 
                    if (((*intArrayList[i]).getReferenceCount)() < intArray::getReferenceCountBase())
                         { delete intArrayList[i]; } 
                         // Bugfix (12/15/94) to allow processing of intArray_index variables
                         // APP_ASSERT(intArrayList[i]->Array_Domain != NULL);
                    (((*intArrayList[i]).Array_Descriptor).Array_Domain).Is_A_Temporary
                     = 0; 
               } 
#if defined(PPP)

                    // APP_ASSERT(intArrayList[i]->Array_Descriptor.SerialArray->
                    //            Array_Descriptor.Array_Domain != NULL);


#endif


#if COMPILE_DEBUG_STATEMENTS
               if (Diagnostic_Manager::getReferenceCountingReport() > 0)
                  { 
                  // This mechanism outputs reports which allow us to trace the reference counts
                    (*(this -> Index_Array)[i]).displayReferenceCounts(("Index_Array [i] in Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing")); 
               } 
          } 
     } 
#endif



#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Leaving Array_Domain_Type::Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing ( const Array_Domain_Type & X , Indirect_Index_List )\n")); } 
} 
#endif




Array_Domain_Type * Array_Domain_Type::Vectorizing_Domain(const Array_Domain_Type & X)
   { 
   // Build a descriptor to fill in!
     class Array_Domain_Type * Return_Domain=new Array_Domain_Type(); 

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Inside of Array_Domain_Type::Vectorizing_Domain ( const Array_Domain_Type & X ) (this = %p)\n"),
          Return_Domain); } 
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
          if (APP_DEBUG > 0)
               { printf(("X.Is_Contiguous_Data == TRUE \n")); } 
#endif

          int i=0; 
          for (i = 0; i < 6; i++)
             { 
               (Return_Domain -> Size)[i] = (X.Size)[3]; 
               (Return_Domain -> Data_Base)[i] = APP_Global_Array_Base; 
               // Return_Domain->Geometry_Base[i] = APP_Global_Array_Base;
               (Return_Domain -> Base)[i] = 0; 
               (Return_Domain -> Bound)[i] = 0; 
               (Return_Domain -> Stride)[i] = 1; 
               (Return_Domain -> Index_Array)[i] = (intArray * )0; 
          } 
#if defined(PPP)

#endif


               // Specific for 0th dimension
          (Return_Domain -> Bound)[0] = (X.Size)[5] - 1; 

          Return_Domain -> Is_A_View = 1; 
          Return_Domain -> Is_A_Temporary = 0; 
          Return_Domain -> Is_Contiguous_Data = 1; 
          Return_Domain -> Is_A_Null_Array = 0; 
          Return_Domain -> Is_Built_By_Defered_Evaluation = 0; 

          // Used in Initialize_Non_Indexed_Dimension but set in Initialize_Domain!
          Return_Domain -> Uses_Indirect_Addressing = 0; 
          Return_Domain -> internalArrayID = X.internalArrayID; 
          Return_Domain -> referenceCount = Array_Domain_Type::getReferenceCountBase(); 
          Return_Domain -> Domain_Dimension = 1; 
          Return_Domain -> Constant_Data_Base = 1; 
          Return_Domain -> Constant_Unit_Stride = 1; 
     } 
     else 
        { 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               { printf(("X.Is_Contiguous_Data == FALSE \n")); } 
#endif

          if (X.Uses_Indirect_Addressing)
             { 
               printf(("\n")); 
               printf(("\n")); 
               printf(("*************************************************************** \n")); 
               printf(("ERROR: Sorry not implemented -- linearized view (vectorization) of indirect addressed view! \n")); 
               printf(("Views formed by indirect addressing are already 1 dimensional (so use of operator()() should not be required!) \n")); 
               printf(("*************************************************************** \n")); 
               APP_ABORT(); 
          } 

          Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List_1; 
          Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List_2; 

          int Dimension[6]; 
          class Internal_Index Index_List[6]; 
                   class Range Range_List[6]; 

          int Total_Size=0; 
          int i=0; 
          for (i = 0; i < 6; i++)
             { 
               Dimension[i] = (X.getLength)(i); 
               Total_Size *= Dimension[i]; 

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    { printf(("Total_Size = %d  Dimension[i] = %d \n"),Total_Size,Dimension[i]); } 
#endif

               (Return_Domain -> Size)[i] = (X.Size)[i]; 
               if (X.Is_A_View)
                  { 
                  // If indexed with a intArray we assume Geometry_Base [Axis] = 0
                  // We have to assume something and we can not afford to search for the
                  // minimum of the intArray
                    (Return_Domain -> Data_Base)[i] = APP_Global_Array_Base; 
               } 
               else                // Return_Domain->Geometry_Base[i] = APP_Global_Array_Base;

                  { 
                    (Return_Domain -> Data_Base)[i] = (X.Data_Base)[i]; 
               }                     // Return_Domain->Geometry_Base[i] = X.Geometry_Base[i];


               (Return_Domain -> Base)[i] = 0; 
               (Return_Domain -> Bound)[i] = 0; 
               (Return_Domain -> Stride)[i] = (X.Stride)[i]; 
               (Return_Domain -> Index_Array)[i] = (intArray * )0; 

#if defined(PPP)

#endif

               // Build Mask to be size of A++ object before the view was taken
               (Range_List[i])=Range((X.Data_Base)[i],(X.Data_Base)[i] + ((X.Size)[i]
                - 1));                // Internal_Index_List_1[i] = &((Internal_Index) Range_List[i]);
               Internal_Index_List_1[i] = (Internal_Index * )(Range_List + i); 

               (Index_List[i])=Internal_Index((X.Base)[0],Dimension[i],(X.Stride)[0]); 
               // Internal_Index_List_2[i] = &((Internal_Index) Index_List[i]);
               Internal_Index_List_2[i] = Index_List + i; 
          } 

               // intArray Mask ( ((Range*)Internal_Index_List_1) );
          class intArray MaskintArray(Internal_Index_List_1); 

          // Build Map into interior
          class intArray * Map=new intArray(Total_Size); 

          (Return_Domain -> Index_Array)[0] = Map; 
          (Return_Domain -> Bound)[0] = Total_Size - 1; 

          Return_Domain -> Is_A_View = 1; 
          Return_Domain -> Is_A_Temporary = 0; 
          Return_Domain -> Is_Contiguous_Data = 0; 
          Return_Domain -> Is_A_Null_Array = 0; 
          Return_Domain -> Is_Built_By_Defered_Evaluation = 0; 

          // Used in Initialize_Non_Indexed_Dimension but set in Initialize_Domain!
          Return_Domain -> Uses_Indirect_Addressing = 1; 

          Return_Domain -> internalArrayID = X.internalArrayID; 

          Return_Domain -> referenceCount = Array_Domain_Type::getReferenceCountBase(); 

          Return_Domain -> Domain_Dimension = 1; 
          Return_Domain -> Constant_Data_Base = 0; 
          Return_Domain -> Constant_Unit_Stride = 0; 

          // This is a view so these should be the same.
          Return_Domain -> builtUsingExistingData = X.builtUsingExistingData; 

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 5)
             { 
               for (int j=0; j < 6; j++)
                  { 
                    (Range_List[j]).display(("Range")); 
                    (Index_List[j]).display(("Index")); 
               } 
          } 
#endif

                    // Initialize the Mask
          Mask=0; 

          // Set view of Mask to non-zero value (like 1)
          // Mask(I,J,K,L) = 1;
          Mask(Internal_Index_List_2)=1; 

#if !defined(USE_EXPRESSION_TEMPLATES)
          // Build intArray to access view where Mask is non-zero
          (*Map)=Mask.indexMap(); 
#else



#endif

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 5)
             { 
               Mask.display(("Mask")); 
               Map -> display(("Map")); 
          } 
     } 
#endif


#if COMPILE_DEBUG_STATEMENTS
     Return_Domain -> Test_Consistency(("Called from Array_Domain_Type::Vectorizing_Domain (Array_Domain_Type)")); 
#endif
     return Return_Domain;
} 



void Array_Domain_Type::setBase(int New_Base_For_All_Axes)
   { 
#if COMPILE_DEBUG_STATEMENTS
   // Enforce bound on Alt_Base depending on INT_MAX and INT_MIN
     static int Max_Base=(2147483647); 
     static int Min_Base=(-1); 

     // error checking!
     if (!(New_Base_For_All_Axes > Min_Base && New_Base_For_All_Axes < Max_Base)){ APP_Assertion_Support("domain.C",
     8896); 
     } 
#endif

     // NEW VERSION (copied from the orignal version in typeArray::setBase(int))
     int temp=0;      // iteration variable
     for (temp = 0; temp < 6; temp++)
        { 
          if (!((this -> isView)() || ((this -> Base)[temp] == 0))){ APP_Assertion_Support("domain.C",
          8903); 
          } 
          int Difference=New_Base_For_All_Axes - ((this -> Data_Base)[temp] + (this -> Base)[temp]); 

          (this -> Data_Base)[temp] += Difference; 
          (this -> User_Base)[temp] += Difference; 

          if (temp == 0)
               { (this -> Scalar_Offset)[0] = this -> View_Offset - (this -> User_Base)[0]
                * (this -> Stride)[0]; } 
          else { (this -> Scalar_Offset)[temp] = (this -> Scalar_Offset)[(temp - 1)] - ((this
           -> User_Base)[temp] * (this -> Stride)[temp]) * (this -> Size)[(temp - 1)]; 
          } 
     } 
} 
#if defined(PPP)
          // Now  the Global_Index and Local_Mask_Index Index objects



          // printf ("getRawBase(temp) = %d \n",getRawBase(temp));
          // Global_Index [temp].display("Global_Index [temp]");
          // Local_Mask_Index [temp].display("Local_Mask_Index [temp]");

          // Error checking that works for only the 1 processor case





#endif




void Array_Domain_Type::setBase(int New_Base,int Axis)
   { 
#if COMPILE_DEBUG_STATEMENTS
   // Enforce bound on Alt_Base depending on INT_MAX and INT_MIN
     static int Max_Base=(2147483647); 
     static int Min_Base=(-1); 

     // error checking!
     if (!(New_Base > Min_Base && New_Base < Max_Base)){ APP_Assertion_Support("domain.C",
     8942); 
     }      // APP_ASSERT(Array_Descriptor != NULL);
#endif

     // This should help make sure the paramerters were input in the correct order
     if (!(Axis >= 0)){ APP_Assertion_Support("domain.C",8947); } 
     if (!(Axis < 6)){ APP_Assertion_Support("domain.C",8948); } 

     int i=0; 
     int Temp_Scalar_Offset=0; 
     int Difference=New_Base - ((this -> Data_Base)[Axis] + (this -> Base)[Axis]); 

     Temp_Scalar_Offset = Difference * (this -> Stride)[Axis]; 
     if (Axis > 0)
          { Temp_Scalar_Offset *= (this -> Size)[(Axis - 1)]; } 

     if (!((this -> isView)() || ((this -> Base)[Axis] == 0))){ APP_Assertion_Support("domain.C",
     8958); 
     } 
     (this -> Data_Base)[Axis] = New_Base - (this -> Base)[Axis]; 
     (this -> User_Base)[Axis] += Difference; 

     for (i = Axis; i < 6; i++)
          { (this -> Scalar_Offset)[i] -= Temp_Scalar_Offset; } 
} 
#if defined(PPP)
          // Now  the Global_Index and Local_Mask_Index Index objects



          // Error checking specific to one processor only!





#endif

#if 0
          // I don't know why this is commented out (6/14/2000)
          // though it seems that the reason is that data set by these macros are
          // translation independent.
#if defined(PPP)

#else

#endif
#endif



#if defined(USE_EXPRESSION_TEMPLATES)



          // This function is not implemented 






#endif

#if defined(SERIAL_APP)



          // This function is called by the Array_Descriptor_Type::Allocate_Parallel_Array() function


          // Use Range::operator()(Range) to help simplify the computation of the 
          // base and bound for the view formed when the stride is greater then 1.



#if 0


#endif









          // This is the case for the middle processor (when partitioned over 3 processors) when:
          //     <type>Array X(Range(6,8,2));
#if 0

#endif
          // Compute new values for Serial array domain (before assignment)


          // Does not change (I think)


#if 0


#endif
          // Set the stride to that of the global domain











#if 0

#endif
          // Compute new values for Serial array domain (before assignment)

          // int newUserBase = intersectionRange.getBase();

          // Does not change (I think)


#if 0


#endif
          // Set the stride to that of the global domain

          // User_Base [axis] = newUserBase * newStride;









#if 0









#endif


#if 0


#endif

          // if this is a strided array then this must be false

          // globalDomain.display("globalDomain in Array_Domain_Type::adjustSerialDomainForStridedAllocation");












#if 0

#endif






#endif

#if 0











#endif


#if defined(PPP)

#if defined(PPP)



          // A Null array can be part of the left edge or the right edge of a distributed axis
          // This function is helpful in determining which it is.



#if defined(USE_PADRE)


#else
          // printf ("In isLeftNullArray(): Axis = %d \n",Axis);



          // printf ("In Array_Domain::isLeftNullArray(axis): Domain_Dimension = %d \n",Domain_Dimension);



          // printf ("     gUBnd(BlockPartiArrayDomain,Axis) = %d \n",gUBnd(BlockPartiArrayDomain,Axis));
          // printf ("     lalbnd(BlockPartiArrayDomain,Axis,Base[Axis],1) = %d \n",lalbnd(BlockPartiArrayDomain,Axis,Base[Axis],1));





          // Along higher axes we want to force isLeftPartition == TRUE




          // USE_PADRE not defined
#endif







          // A Null array can be part of the left edge or the right edge of a distributed axis
          // This function is helpful in determining which it is.



#if defined(USE_PADRE)


#else
          // printf ("In isRightNullArray(): Axis = %d \n",Axis);








          // Along higher axes we want to force isRightPartition == TRUE




          // USE_PADRE not defined
#endif



#endif





          // Support function for allocation of P++ array objects
          // This function modifies the serialArray descriptor to 
          // make it a proper view of the serial array excluding 
          // the ghost boundaries

#if COMPILE_DEBUG_STATEMENTS


#endif




          // Need to set the upper bound on the iteration to MAX_ARRAY_DIMENSION
          // since all the variables for each dimension must be initialized.



#if 0




#endif

          // If this is not true then things get screwed up
          // APP_ASSERT (serialArrayDomain.getBound(j) >= serialArrayDomain.getBase(j));

          // This function is used tohide the use of A++/P++ specific range objects and 
          // provide an interface that both the PADRE and nonPADRE versions can call.
          // void adjustSerialDomainForStridedAllocation ( int axis,  const Array_Domain_Type & globalDomain );


          // printf ("##### Calling Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");

          // printf ("##### DONE: Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");


          // This could be implemented without the function call overhead
          // represented here but this simplifies the initial implementation for now

          // The left side uses the bases which are relative to a unit stride (a relatively simple case)!


#if 0












#endif



          // int tempLocalUnitStrideBase = getBase(j) + (serialArrayDomain.getBase(j) - getBase(j)) * Stride[j];

          // This variable is set in code just below


#if 0

#endif

          // The bound is the funky case since the bases are relative to stride 1 and 
          // the bounds are relative to stride.

          // int tempLocalBound = tempLocalUnitStrideBase + (serialArrayDomain.getBound(j) - serialArrayDomain.getBase(j)) * Stride[j];



#if 0

#endif

          // This variable is set in code just below






#if 0

#endif
          // tempLocalBound  = getLocalBase(j);

          // gUBnd(Array_Domain.BlockPartiArrayDomain,j);




#if 0

#endif
          // tempLocalBound  = getBound(j);
          // tempLocalBound  = getBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];




#if 0


          // printf ("gUBnd(Array_Domain.BlockPartiArrayDomain,j) = %d \n",gUBnd(Array_Domain.BlockPartiArrayDomain,j));
#endif





#if 0

#endif
          // If this is not true then things get screwed up






#if 0


#endif


          // printf ("In postAllocationSupport: first instance: j = %d tempLocalBound = %d tempGlobalBound_A = %d \n",j,tempLocalBound,tempGlobalBound_A);

          // bugfix (8/15/2000) account for stride
          // int Right_Size_Size = (tempGlobalBound_A - tempLocalBound) / Array_Domain.Stride[j];
          // int Right_Size_Size = (tempGlobalBound_A - tempLocalBound);

          // Do these depend upon the stride in anyway?



#if 0

          // printf ("In postAllocationSupport: tempLocalBound = %d tempGlobalBound_A = %d \n",tempLocalBound,tempGlobalBound_A);



#endif

          // Comment out the Global Index since it should already be set properly
          // and this will just screw it up! We have to set it (I think)
#if 0





#endif
          // Array_Domain.Global_Index [j] = Index (getBase(j),(getBound(j)-getBase(j))+1,1);
          // Use the range object instead

#if 0





#endif






          // If the data is not contiguous on the global level then it could not 
          // be locally contiguous (unless the partitioning was just right --but 
          // we don't worry about that case) if this processor is on the left or 
          // right and the ghost bundaries are of non-zero width
          // then the left and right processors can not have:


          // Additionally if we have ghost boundaries then these are always hidden 
          // in a view and so the data can't be contiguous



          // In this later case since we have to modify the parallel domain object to be consistant


          // Note: we are changing the parallel array descriptor (not done much in this function)








          // Bugfix (17/10/96)
          // The Local_Mask_Index is set to NULL in the case of repartitioning
          // an array distributed first across one processor and then across 2 processors.
          // The second processor is a NullArray and then it has a valid local array (non nullarray).
          // But the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
          // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
          // setup.  The problem is that it is either not setup or sometimes setup incorrectly.
          // So we have to set it to a non-nullArray as a default.


          // This resets the Local_Mask_Index to be consistant with what it usually is going into 
          // this function. This provides a definition of Local_Mask_Index consistant with
          // the global range of the array. We cannot let the Local_Mask_Index be a Null_Index
          // since it would not allow the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
          // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
          // setup.
          // Array_Domain.Local_Mask_Index[j].display("INSIDE OF ALLOCATE PARALLEL ARRAY -- CASE OF NULL_INDEX");
          // Array_Domain.Local_Mask_Index[j] = Range (Array_Domain.Base[j],Array_Domain.Bound[j]);










#if COMPILE_DEBUG_STATEMENTS







#endif

          // If the parallel array descriptor is a view then we have to
          // adjust the view we build on the local partition!
          // Bases are relative to unit stride 


          // Bounds are relative to strides (non-unit strides)
          // int bound_offset = getLocalBound(j) - getBound(j);
          // Convert the bound to a unit stride bound so it can be compared to the value of getBound(j)
#if 0





#else
          //          int tempLocalBound  = getBase(j) + (serialArrayDomain.getBound(j) - getBase(j)) * Stride[j];
          //          int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Stride[j];

          // I think these are already computed above!



          // printf ("In allocate.C: 2nd instance: j = %d tempLocalBound = %d tempGlobalBound_B = %d \n",j,tempLocalBound,tempGlobalBound_B);
#endif

          // Now that we have a unit stride bound we can do the subtraction





          // Need to account for the stride


          // Only could the left end once in adjusting for stride!
          // Count = 1 + (Count-1) * Array_Domain.Stride[j];

#if 0


#endif

#if 0














#endif

          /*
            // ... bug fix, 4/8/96, kdb, at the end the local base and
            // bound won't include ghost cells on the left and right
            // processors respectively so adjust Count ...
            */





          /*
                 // if this is a view, the ghost boundary cells are
                 // already removed by Original_Base_Offset_Of_View and
                 // Original_Bound_Offset_Of_View
                 */










#if 0

#endif



          // This should have already been set properly (so why reset it?)
          // Actually it is not set properly all the time at least so we have to reset it.
#if 0




#endif
          // This should be relative to stride 1 base/bound data since the local mask is relative to the local data directly (unit stride)
          // Array_Domain.Local_Mask_Index[j] = Index(getLocalBase(j) + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);



#if 0




#endif

          // Make the SerialArray a view of the valid portion of the local partition


          /*
                 // ... (bug fix, 5/21/96,kdb) bases and bounds need to
                 // be adjusted by Original_Base_Offset_Of_View and
                 // Original_Bound_Offset_Of_View no matter what processor ...
                 */



          // ... bug fix (8/26/96, kdb) User_Base must reflect the view ...







          // ... bug fix (8/26/96, kdb) User_Base must reflect the ghost cell ...







          // Bugfix (10/19/95) if we modify the base and bound in the 
          // descriptor then the data is no longer contiguous
          // meaning that it is no longer binary conformable








          // j >= Domain_Dimension




#if 0




#endif
          // (6/28/2000) part of fix to permit allocation of strided array data
          // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,1);
          // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,Local_Stride);



#if 0




#endif



          // build a null internalIndex for this case




          // ... add correct view offset and Scalar Offset ...


          // bugfix (7/16/2000) The View_Offset is not computed with a stride!

          // The Scalar_Offset is computed with a stride!




          // The View_Offset is not computed with a stride!





          // Error checking for same strides in parallel and serial array


#if 0





#endif

#if 0
          // New test (7/2/2000)
          // In general this test is not always valid (I think it was a mistake to add it - but
          // we might modify it in the future so I will leave it here for now (commented out))
          // This test is not working where the array on one processor is a NullArray (need to fix this test)


          // recompute the partition information for this axis




          // Error checking on left and right edges of the partition




          // display("ERROR: getRawBase(j) != SerialArray->getRawBase(j)");










          // display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
          // SerialArray->Array_Descriptor.display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");






#endif
          // end of j loop






          // Start out true

#if 0


#endif



          // Start out true


#if 0


#endif



#if 0





#endif

          // ... add View_Offset to Scalar_Offset (we can't do this inside the
          // loop above because View_Offset is a sum over all dimensions). Also
          // set View_Pointers now. ...




          // printf ("SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
          //      j,SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j]);


          /* SERIAL_POINTER_LIST_INITIALIZATION_MACRO; */
          /* DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO; */

          // if we have generated a null view of a valid serial array then we have 
          // to make the descriptor conform to some specific rules
          // 1. A Null_Array has to have the Is_Contiguous_Data flag FALSE
          // 2. A Null_Array has to have the Base and Bound 0 and -1 (repectively)
          //    for ALL dimensions
          // 3. The Local_Mask_Index in the Parallel Descriptor must be a Null Index



          // We could call a function here to set the domain to represent a Null Array






          // Call this specialized function for this purpose (it is used elsewhere as well)





#if 0
          // (12/14/2000) set in resetDomainForNullArray()
          // Modify the parallel domain


          // Added (12/14/2000)


#endif
          // Modify the serial domain




          // (12/13/2000) Added test: set in resetDomainForNullArray()






#endif










int Array_Domain_Type::CLASS_ALLOCATION_POOL_SIZE=(100); 
                   /* Static variable */

class Array_Domain_Type * Array_Domain_Type::Current_Link=(((Array_Domain_Type * )0)); 

int Array_Domain_Type::Memory_Block_Index=(0); 

const int Array_Domain_Type::Max_Number_Of_Memory_Blocks=(10000); 

unsigned char * Array_Domain_Type::Memory_Block_List[10000]; 

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void * Array_Domain_Type::operator new(size_t Size)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Call Array_Domain_Type::operator new! Memory_Block_Index = %d \n"),
          Array_Domain_Type::Memory_Block_Index); } 
#endif

#if USE_CPP_NEW_DELETE_OPERATORS

#else
          // Because of the way the size of the memory blocks doubles in size
          // for each proceeding memory block 100 is a good limit for the size of
          // the memory block list!

          // These were taken out to allow the new operator to be inlined!
          // const int Max_Number_Of_Memory_Blocks = 1000;
          // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
          // static int Memory_Block_Index = 0;

     if (Size != 328)
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


          // printf ("In Array_Domain_Type::operator new: Using the pool mechanism Size(%d) == sizeof(Array_Domain_Type)(%d) \n",Size,sizeof(Array_Domain_Type));


     else if (Array_Domain_Type::Current_Link == (Array_Domain_Type * )0)
             { 
             // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    { printf(("Call malloc for Array Memory_Block_Index = %d \n"),Array_Domain_Type::Memory_Block_Index); } 
#endif

                    // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Array_Domain_Type::Current_Link = (Array_Domain_Type * )valloc((Array_Domain_Type::CLASS_ALLOCATION_POOL_SIZE
                * 328)); 
#else

#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    { printf(("Called malloc for Array Memory_Block_Index = %d \n"),Array_Domain_Type::Memory_Block_Index); } 
#endif

#if EXTRA_ERROR_CHECKING
               if (Array_Domain_Type::Current_Link == (Array_Domain_Type * )0)
                  { 
                    printf(("ERROR: malloc == NULL in Array::operator new! \n")); 
                    APP_ABORT(); 
               } 

                    // Initialize the Memory_Block_List to NULL
                    // This is used to delete the Memory pool blocks to free memory in use
                    // and thus prevent memory-in-use errors from Purify
               if (Array_Domain_Type::Memory_Block_Index == 0)
                  { 
                    for (int i=0; i < 9999; i++)
                       { Array_Domain_Type::Memory_Block_List[i] = (unsigned char * )0; } 
               } 
#endif

               Array_Domain_Type::Memory_Block_List[(Array_Domain_Type::Memory_Block_Index++)]
                = (unsigned char * )Array_Domain_Type::Current_Link; 
#if EXTRA_ERROR_CHECKING
               // Bounds checking!
               if (Array_Domain_Type::Memory_Block_Index >= 10000)
                  { 
                    printf(("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n"),
                    Array_Domain_Type::Memory_Block_Index,10000); 
                    APP_ABORT(); 
               } 
#endif

                    // Initialize the free list of pointers!
               for (int i=0; i < Array_Domain_Type::CLASS_ALLOCATION_POOL_SIZE - 1; i++)
                  { 
                    (Array_Domain_Type::Current_Link[i]).freepointer = Array_Domain_Type::Current_Link
                     + (i + 1); 
               } 
                    // Set the pointer of the last one to NULL!
               (Array_Domain_Type::Current_Link[(Array_Domain_Type::CLASS_ALLOCATION_POOL_SIZE
                - 1)]).freepointer = (Array_Domain_Type * )0; 
     } 

               // Save the start of the list and remove the first link and return that
               // first link as the new object!

     class Array_Domain_Type * Forward_Link=Array_Domain_Type::Current_Link; 
     Array_Domain_Type::Current_Link = Array_Domain_Type::Current_Link -> freepointer; 

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          { printf(("Returning from Array_Domain_Type::operator new! (with address of %p) \n"),
          Forward_Link); } 
#endif

     return Forward_Link;
} 
     // case of USE_CPP_NEW_DELETE_OPERATORS
#endif

#endif

void Array_Domain_Type::operator delete(void * Pointer,size_t sizeOfObject)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        { 
          printf(("In Array_Domain_Type::operator delete: Size(%d)  sizeof(Array_Domain_Type)(%d) \n"),
          sizeOfObject,328); 
     } 
#endif

#if USE_CPP_NEW_DELETE_OPERATORS

#else
     if (sizeOfObject != 328)
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
               printf(("In Array_Domain_Type::operator delete: Calling global delete (free) because Size(%d) != sizeof(Array_Domain_Type)(%d) \n"),
               sizeOfObject,328); 
          } 
#endif

          free(Pointer); 
     } 
     else 
        { 
          class Array_Domain_Type * New_Link=((Array_Domain_Type * )Pointer); 

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             { 
               printf(("In Array_Domain_Type::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(Array_Domain_Type)(%d) \n"),
               Pointer,sizeOfObject,328); 
          } 
#endif
          if (New_Link != (Array_Domain_Type * )0)
             { 
             // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link -> freepointer = Array_Domain_Type::Current_Link; 
               Array_Domain_Type::Current_Link = New_Link; 
          } 
          else 
#if EXTRA_ERROR_CHECKING

             { 
               printf(("ERROR: In Array_Domain_Type::operator delete - attempt made to delete a NULL pointer! \n")); 
               APP_ABORT(); 
          } 
     } 
#endif


#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Leaving Array_Domain_Type::operator delete! \n")); } 
} 
#endif

// case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
