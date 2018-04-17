// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma implementation "descriptor.h"
#endif











#include "A++.h"
extern "C"
   {
/* machine.h is found in MDI/machine.h through a link in A++/inc lude and P++/inc lude */
#include "machine.h"
   }

// defined in array.C
extern int APP_Global_Array_Base;

#define INLINE_DESCRIPTOR_SUPPORT FALSE
#define FILE_LEVEL_DEBUG 0

// This turns on the calls to the bounds checking function
#define BOUNDS_ERROR_CHECKING TRUE

// It helps to set this to FALSE sometimes for debugging code
// this enambles the A++/P++ operations in the bounds checking function
#define TURN_ON_BOUNDS_CHECKING   TRUE

 

/*include(../src/descriptor_macro.m4)*/

/*define(CLASS_TEMPLATE, )*/
/*define(CLASS_TEMPLATE, template<class T``,'' int Templated_Dimension>)*/

/*define(ARRAY_DESCRIPTOR_TYPE, $2Array_Descriptor_Type<T``,''Template_Dimension>)*/
/*define(ARRAY_DESCRIPTOR_TYPE, $1$2Array_Descriptor_Type)*/















#define INTARRAY
/*``#define'' CLASS_TEMPLATE*/  
/*``#define'' CLASS_TEMPLATE  template<class T``,''int Templated_Dimension>*/

/*``#define'' ARRAY_DESCRIPTOR_TYPE intArray_Descriptor_Type*/
/*``#define'' ARRAY_DESCRIPTOR_TYPE Array_Descriptor_Type<T`,'Template_Dimension>*/

#if !defined(PPP)
// *********************************************************
// Hash tables can be used to to avoid redundent allocation
// and deallocation associated with the use of temporaries.
// Temporaries are created and saved for reuse when the same
// size temporary is required again. No smart system for aging
// of hashed memory is used to avoid the acumulation of reserved
// memory for reuse.  Also no measurable advantage of this
// feature has been observed.
// *********************************************************
// intArray_Data_Hash_Table intArray_Descriptor_Type::Hash_Table;
 intArray_Data_Hash_Table intArray_Descriptor_Type::Hash_Table;
#endif


// *****************************************************************************
// *****************************************************************************
// *********  NEW OPERATOR INITIALIZATION MEMBER FUNCTION DEFINITION  **********
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

void intArray_Descriptor_Type::New_Function_Loop ()
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

//template<class T, int Template_Dimension>

void intArray_Descriptor_Type::freeMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of intArray_Descriptor_Type::freeMemoryInUse() \n");
#endif 

  // This function is useful in conjuction with the Purify (from Pure Software Inc.)
  // it frees memory allocated for use internally in A++ <type>Array objects.
  // This memory is used internally and is reported as "in use" by Purify
  // if it is not freed up using this function.  This function works with
  // similar functions for each A++ object to free up all of the A++ memory in
  // use internally.

#if !defined(PPP)
  // Bugfix (12/24/97)
  // We don't store the Array_Domain_Type::Array_Reference_Count_Array
  // in the intArray_Descriptor_Type so we can't free it here
  // this is taken care of in the Array_Domain_Type::freeMemoryInUse()
  // member function!
  // free memory allocated for reference counting!
  // if (Array_Domain_Type::Array_Reference_Count_Array != NULL)
  //      free ((char*) Array_Domain_Type::Array_Reference_Count_Array);
#endif

  // free memory allocated for memory pools!
     for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++) 
          if (Memory_Block_List [i] != NULL)  
               free ((char*) (Memory_Block_List[i]));
   }

// *****************************************************************************
// *****************************************************************************
// ****************  INITIALIZATION SPECIFIC TO PARALLEL DATA  *****************
// *****************************************************************************
// *****************************************************************************

#if 0
   // I don't think this is called!
#if defined(PPP)
//template<class T, int Template_Dimension>

void intArray_Descriptor_Type::
Initialize_Parallel_Parts_Of_Descriptor ( const intArray_Descriptor_Type & X )
   {
     Array_Domain.Initialize_Parallel_Parts_Of_Domain (X.Array_Domain);
   }
#endif
#endif

// *****************************************************************************
// *****************************************************************************
// **************  BASE SPECIFICATION MEMBER FUNCTION DEFINITION  **************
// *****************************************************************************
// *****************************************************************************


void
intArray_Descriptor_Type::setBase( int New_Base_For_All_Axes )
   {
  // NEW VERSION OF SETBASE
#if COMPILE_DEBUG_STATEMENTS
  // Enforce bound on Alt_Base depending on INT_MAX and INT_MIN
     static int Max_Base = INT_MAX;
     static int Min_Base = INT_MIN;

  // error checking!
     APP_ASSERT((New_Base_For_All_Axes > Min_Base) && (New_Base_For_All_Axes < Max_Base));
#endif

     int temp       = 0;  // iteration variable
     int Difference = 0;

#if defined(PPP)
  // APP_ASSERT(SerialArray != NULL);

  // Adjust the Data_Base on the LOCAL SerialArray relative to the change in the
  // GLOBAL base
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          Difference = New_Base_For_All_Axes - (Array_Domain.Data_Base[temp] +
                                                Array_Domain.Base[temp]);

          if (SerialArray != NULL)
             {
            // printf ("SHOULDN'T WE USE SerialArray->getBase(temp) instead of getBase(temp) \n");
	    /*
	    // ... (5/18/98, kdb) bug fix, getRawBase now corresponds to setBase
	    //   (getBase gets User_Base instead of Data_Base + Base) ...
	    */
               int New_Serial_Base = SerialArray->getRawBase(temp) + Difference;
            // int New_Serial_Base = SerialArray->getBase(temp) + Difference;
            // int New_Serial_Base = getBase(temp) + Difference;
            // printf ("New_Serial_Base = %d \n",New_Serial_Base);
               SerialArray->setBase (New_Serial_Base,temp);
             }
        }
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
#endif

     Array_Domain.setBase(New_Base_For_All_Axes);

  // Now initialize the View pointers (located in the 
  // descriptor -- since they are typed pointers)
#if defined(PPP)
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
     if (SerialArray != NULL)
        {
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
        }
#else
  // POINTER_LIST_INITIALIZATION_MACRO;
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
#endif
   }


void
intArray_Descriptor_Type::setBase( int New_Base, int Axis )
   {
#if defined(PPP)
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
     if (SerialArray != NULL)
        {
       // Call setBase on the SerialArray also
          int Difference = New_Base - (Array_Domain.Data_Base[Axis] + Array_Domain.Base[Axis]);

          int New_Serial_Base = SerialArray->getBase(Axis) + Difference;
          SerialArray->setBase (New_Serial_Base,Axis);

          Array_Domain.setBase (New_Base,Axis);

       // Now initialize the View pointers (located in the 
       // descriptor -- since they are typed pointers)
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
        }
#else
     Array_Domain.setBase (New_Base,Axis);

  // Now initialize the View pointers (located in the 
  // descriptor -- since they are typed pointers)
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
#endif
   }
//====================================================================

// *****************************************************************************
// *****************************************************************************
// ********************  DISPLAY MEMBER FUNCTION DEFINITION  *******************
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

void
intArray_Descriptor_Type::display(const char *Label ) const
   {
     int i=0;
     printf ("Array_Descriptor_Type::display() -- %s \n",Label);

     Array_Domain.display(Label);

#if defined(PPP)
     printf ("getLocalBase (output of getLocalBase member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalBase(i));
     printf ("\n");

     printf ("getLocalBound (output of getLocalBound member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalBound(i));
     printf ("\n");

     printf ("getLocalStride (output of getLocalStride member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalStride(i));
     printf ("\n");
#endif

#if !defined(PPP)
     printf ("Array_Data                    = %p \n",Array_Data);
#endif
     printf ("ExpressionTemplateDataPointer = %p \n",ExpressionTemplateDataPointer);

#if !defined(PPP)
        printf ("Array_View_Pointer0 = %p (Array_Data-Array_View_Pointer0) = %d \n",
             Array_View_Pointer0,(Array_Data-Array_View_Pointer0));
#if MAX_ARRAY_DIMENSION>1
        printf ("Array_View_Pointer1 = %p (Array_View_Pointer0-Array_View_Pointer1) = %d \n",
             Array_View_Pointer1,(Array_View_Pointer0-Array_View_Pointer1));
#endif
#if MAX_ARRAY_DIMENSION>2
        printf ("Array_View_Pointer2 = %p (Array_View_Pointer1-Array_View_Pointer2) = %d \n",
             Array_View_Pointer2,(Array_View_Pointer1-Array_View_Pointer2));
#endif
#if MAX_ARRAY_DIMENSION>3
        printf ("Array_View_Pointer3 = %p (Array_View_Pointer2-Array_View_Pointer3) = %d \n",
             Array_View_Pointer3,(Array_View_Pointer2-Array_View_Pointer3));
#endif
#if MAX_ARRAY_DIMENSION>4
        printf ("Array_View_Pointer4 = %p (Array_View_Pointer3-Array_View_Pointer4) = %d \n",
             Array_View_Pointer4,(Array_View_Pointer3-Array_View_Pointer4));
#endif
#if MAX_ARRAY_DIMENSION>5
        printf ("Array_View_Pointer5 = %p (Array_View_Pointer4-Array_View_Pointer5) = %d \n",
             Array_View_Pointer5,(Array_View_Pointer4-Array_View_Pointer5));
#endif
#if MAX_ARRAY_DIMENSION>6
        printf ("Array_View_Pointer6 = %p (Array_View_Pointer5-Array_View_Pointer6) = %d \n",
             Array_View_Pointer6,(Array_View_Pointer5-Array_View_Pointer6));
#endif
#if MAX_ARRAY_DIMENSION>7
        printf ("Array_View_Pointer7 = %p (Array_View_Pointer6-Array_View_Pointer7) = %d \n",
             Array_View_Pointer7,(Array_View_Pointer6-Array_View_Pointer7));
#endif
#endif

  // The only additional thing to do is print out the values of the pointers 
  // in this class since that is the only difference between a descriptor and a domain.
   }

// *****************************************************************************
// *****************************************************************************
// **************************  DESTRUCTOR DEFINITIONS  *************************
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

intArray_Descriptor_Type::~intArray_Descriptor_Type ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Descriptor_Type::destructor! %p Array_ID = %d referenceCount = %d \n",
               this,Array_Domain.Array_ID(),referenceCount);
#endif

#if 0
#if defined(PPP)
     printf ("Why can't we assert that SerialArray == NULL (SerialArray = %s) in the ~intArray_Descriptor_Type()? \n",
          (SerialArray == NULL) ? "NULL" : "VALID POINTER");
#else
     printf ("Why can't we assert that Array_Data == NULL (Array_Data = %s) in the ~intArray_Descriptor_Type()? \n",
          (Array_Data == NULL) ? "NULL" : "VALID POINTER");
#endif
#endif

#if defined(USE_PADRE) && defined(PPP)
     if (Array_Domain.parallelPADRE_DescriptorPointer != NULL)
        {
       // printf ("In intArray_Descriptor_Type::destructor(): delete the PADRE_Descriptor \n");
          Array_Domain.parallelPADRE_DescriptorPointer->decrementReferenceCount();
       // printf ("Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount() = %d \n",
       //      Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount());
          if (Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount() < 
              Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCountBase())
               delete Array_Domain.parallelPADRE_DescriptorPointer;
          Array_Domain.parallelPADRE_DescriptorPointer = NULL;
        }
#endif

  // Reset reference count!  This should always be the value of getReferenceCountBase() anyway
     referenceCount = getReferenceCountBase();

  // printf ("In ~intArray_Descriptor_Type Array_ID on the stack is %d \n",SerialArray_Domain_Type::queryNextArrayID());

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Leaving ~Array_Descriptor_Type() -- referenceCount = %d \n",referenceCount);
#endif
   }

// *****************************************************************************
// *****************************************************************************
// *******************  CONSTRUCTOR INITIALIZATION SUPPORT  ********************
// *****************************************************************************
// *****************************************************************************


void
intArray_Descriptor_Type::Preinitialize_Descriptor()
   {
  // At this point (when this function is called) the array data HAS NOT been allocated!

  // Set reference count!
     referenceCount = getReferenceCountBase();

  // We set these to NULL to avoid UMR (in purify) -- but these are RESET
  // after the pointer to the array data when it is known.
#if defined(PPP)
     intSerialArray *Data_Pointer = SerialArray;
     if (Data_Pointer == NULL)
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
        }
       else
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO
        }
#else
     int *Data_Pointer = Array_Data;
     if (Data_Pointer == NULL)
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
        }
       else
        {
          DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
        }
#endif

#if COMPILE_DEBUG_STATEMENTS
  // Test_Consistency ("Called from intArray_Descriptor_Type::Preinitialize_Descriptor()");
  // printf ("In Preinitialize_Descriptor(): Commented out Test_Consistency since not enough data is available at this stage! \n");
#endif
   }

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void intArray_Descriptor_Type::
Initialize_Descriptor(
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List,
     const Internal_Partitioning_Type* Internal_Partition )
   {
  // This function is required because sometimes the descriptor must be reinitialized
  // (this happens in the dynamic re-dimensioning of the array object for example).
  // This function is also called from within the array class adopt function.
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of intArray_Descriptor_Type::Initialize_Descriptor(Number_Of_Valid_Dimensions=%d,int*) ",
               Number_Of_Valid_Dimensions);
        }
#endif

  // printf ("BEFORE Preinitialize_Descriptor() getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // APP_ASSERT (Array_Data == NULL);
  // setDataPointer(NULL);
  // resetRawDataReferenceCount();

  // Bugfix (11/6/2000) Move preinitialixation of the descriptor to after the initialization of the domain.
  // Preinitialize_Descriptor();

#if 0
  // We cannot assert this here since we want the Array_Domain.Initialize_Domain
  // to delete the old partitioning objects if they exist

#if defined(PPP) && !defined(USE_PADRE)
#if COMPILE_DEBUG_STATEMENTS
     if (Array_Domain.BlockPartiArrayDomain        != NULL)
          printf ("WARNING: In Initialize_Descriptor --- Array_Domain.BlockPartiArrayDomain        != NULL \n");

     if (Array_Domain.BlockPartiArrayDecomposition        != NULL)
          printf ("WARNING: In Initialize_Descriptor --- Array_Domain.BlockPartiArrayDecomposition != NULL \n");
#endif

     APP_ASSERT (Array_Domain.BlockPartiArrayDomain        == NULL);
     APP_ASSERT (Array_Domain.BlockPartiArrayDecomposition == NULL);
#endif
#endif

  // This appears to be the second initialization of the Array_Domain object!!!
  // We need to find out where this is called and see that the correct Array_Domain
  // constructor is called instead.  Upon investigation this "Initialization_Descriptor()"
  // function is not called by any of the Array_Descriptor constructors.
#if defined(PPP)
     if (Internal_Partition != NULL)
        {
          Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List,*Internal_Partition);
        }
       else
        {
          Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
        }
#else
  // Avoid compiler warning about unused input variable
     if (&Internal_Partition);

     Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
#endif

  // Bugfix (11/6/2000) Move preinitialixation of the descriptor to after the initialization of the domain.
     Preinitialize_Descriptor();

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
  // For P++ we only track the serial array objects not the serial array objects
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[Array_ID()] != NULL);
#ifdef DOUBLEARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_DOUBLE_ELEMENT_TYPE);
#endif
#ifdef FLOATARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_FLOAT_ELEMENT_TYPE);
#endif
#ifdef INTARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_INT_ELEMENT_TYPE);
#endif
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[Array_ID()] != NULL);
        }
#endif
   }

// Fixup_Descriptor_With_View( 
//     int Number_Of_Valid_Dimensions ,
//     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )

void
intArray_Descriptor_Type::reshape( 
   int Number_Of_Valid_Dimensions ,
   const int* View_Sizes, const int* View_Bases)
   {
  // This function is only called by the array object's reshape member function
     Array_Domain.reshape (Number_Of_Valid_Dimensions, View_Sizes, View_Bases);
   }


// *****************************************************************************
// *****************************************************************************
// *************************  CONSTRUCTORS DEFINITIONS  ************************
// *****************************************************************************
// *****************************************************************************

//template<class T , int Template_Dimension>

intArray_Descriptor_Type::
intArray_Descriptor_Type() 
   : Array_Domain()
   {
  // This builds the NULL Array_Descriptor (used in NULL arrays)!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of intArray_Descriptor_Type::Array_Descriptor_Type() this = %p \n",this);
       // printf ("WARNING: It is a ERROR to use this descriptor constructor except for NULL arrays! \n");
       // APP_ABORT();
        }
#endif

     setDataPointer(NULL);
#if !defined(PPP)
     resetRawDataReferenceCount();
#endif
  // printf ("In intArray_Descriptor_Type::intArray_Descriptor_Type() -- getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
     Preinitialize_Descriptor();

#if 0
  // Set reference count!
     referenceCount = getReferenceCountBase();

#if !defined(PPP)
     Array_Data = NULL;
  // DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
#else
     SerialArray = NULL;
#endif
  // We want to use this macro with both A++ and P++
  // ExpressionTemplateDataPointer = NULL;
     DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
#endif

#if COMPILE_DEBUG_STATEMENTS
  // Can't run these tests in a NULL array
  // Test_Consistency ("Called from default constructor!");
#endif
   }

intArray_Descriptor_Type::
intArray_Descriptor_Type ( const Internal_Index & I )
   : Array_Domain(I)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if (MAX_ARRAY_DIMENSION >= 2)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J )
   : Array_Domain(I,J)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K )
   : Array_Domain(I,J,K)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 4)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L )
   : Array_Domain(I,J,K,L)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M )
   : Array_Domain(I,J,K,L,M)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N )
   : Array_Domain(I,J,K,L,M,N)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 7)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O )
   : Array_Domain(I,J,K,L,M,N,O)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 8)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O, const Internal_Index & P )
   : Array_Domain(I,J,K,L,M,N,O,P)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

//template<class T , int Template_Dimension>

intArray_Descriptor_Type::
intArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   : Array_Domain (Number_Of_Valid_Dimensions,Integer_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
#if 0
  // Set reference count!
     referenceCount = getReferenceCountBase();
  // Initialize_Descriptor (Number_Of_Valid_Dimensions,Integer_List);
#endif
   }

#if defined(APP) || defined(PPP)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if (MAX_ARRAY_DIMENSION >= 2)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 4)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 7)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,O,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 8)
intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O, const Internal_Index & P,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,O,P,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif
#endif
 
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif


intArray_Descriptor_Type::
intArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List )
   : Array_Domain (Number_Of_Valid_Dimensions,Internal_Index_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }


intArray_Descriptor_Type::
intArray_Descriptor_Type( ARGUMENT_LIST_MACRO_INTEGER )
   : Array_Domain (VARIABLE_LIST_MACRO_INTEGER)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if 0
//template<class T , int Template_Dimension>

intArray_Descriptor_Type::
intArray_Descriptor_Type(
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   : Array_Domain(MAX_ARRAY_DIMENSION,Integer_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if defined(PPP) || defined(APP)

intArray_Descriptor_Type::
intArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
     const Internal_Partitioning_Type & Internal_Partition )
   : Array_Domain (Number_Of_Valid_Dimensions,Internal_Index_List,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if defined(PPP)
// intArray_Descriptor_Type::intArray_Descriptor_Type(
//           int Input_Array_Size_I , int Input_Array_Size_J ,
//           int Input_Array_Size_K , int Input_Array_Size_L ,
//           const Partitioning_Type & Partition )

intArray_Descriptor_Type::
intArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List , 
     const Internal_Partitioning_Type & Internal_Partition )
   : Array_Domain(Number_Of_Valid_Dimensions,Integer_List,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }


intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const int* Data_Pointer,
     ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE )
   : Array_Domain(VARIABLE_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE)
   {
  // Avoid compiler warning about unused input variable
     if (&Data_Pointer);

     printf ("ERROR: Parallel intArray_Descriptor_Type::intArray_Descriptor_Type (const int* Data_Pointer,ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE) --- not implemented! \n");
     APP_ABORT();

  // We can't set the int pointer here  This must be done at a later stage
  // setDataPointer (Data_Pointer);
  // incrementRawDataReferenceCount();
     setDataPointer (NULL);
     Preinitialize_Descriptor();
   }
#else // end of PPP


intArray_Descriptor_Type::
intArray_Descriptor_Type ( 
     const int* Data_Pointer, 
     ARGUMENT_LIST_MACRO_CONST_REF_RANGE )
   : Array_Domain(VARIABLE_LIST_MACRO_CONST_REF_RANGE)
   {
  // Mark this object as getting it's data from an external source
     Array_Domain.builtUsingExistingData = TRUE;

#if !defined(PPP)
  // Array_Data = Data_Pointer;
  // Cast away const here to preserve use of const data pointer
  // when building array object using a pointer to existing data
     setDataPointer ((int*)Data_Pointer);
     incrementRawDataReferenceCount();

     APP_ASSERT (Array_Domain.builtUsingExistingData == TRUE);
#else
     SerialArray = NULL;
     printf ("ERROR: intArray_Descriptor_Type::intArray_Descriptor_Type (const int* Data_Pointer,ARGUMENT_LIST_MACRO_CONST_REF_RANGE) should not be defined for P++ \n");
     APP_ABORT();
#endif
     Preinitialize_Descriptor();
   }
#endif // end of else PPP


intArray_Descriptor_Type::
intArray_Descriptor_Type(
     const Array_Domain_Type & X,
     bool AvoidBuildingIndirectAddressingView )
  // We call the Array_Domain copy constructor to do the preinitialization
   : Array_Domain(X,AvoidBuildingIndirectAddressingView)
   {
  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     
#if !defined(PPP)
  // Since this is a new array object is should have an initialize reference count on its
  // raw data.  This is required here because the reference counting mechanism reused the
  // value of zero for one existing reference and no references (this will be fixed soon).
     resetRawDataReferenceCount();
#endif

     Preinitialize_Descriptor();

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from intArray_Descriptor_Type (Array_Domain_Type) \n");
#endif
   }

// ***************************************************************************************
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

// intArray_Descriptor_Type::intArray_Descriptor_Type 
//    ( const intArray_Descriptor_Type & X , 
//      const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
// template<class T , int Template_Dimension>

intArray_Descriptor_Type::
intArray_Descriptor_Type (
     const Array_Domain_Type & X ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
   : Array_Domain (X,Index_List)
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
   {
      printf ("Inside of intArray_Descriptor_Type::intArray_Descriptor_Type ");
      printf ("( const Array_Domain_Type & X , Internal_Index** Index_List )");
      printf ("(this = %p)\n",this);
   }
#endif

  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     Preinitialize_Descriptor();

     APP_ASSERT(Array_ID() == X.Array_ID());

#if COMPILE_DEBUG_STATEMENTS
   Test_Consistency ("Called from intArray_Descriptor_Type (Array_Domain_Type,Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type*) \n");
#endif
}

// ************************************************************************
// intArray_Descriptor_Type constructors for indirect addressing
// ************************************************************************
// intArray_Descriptor_Type ( const intArray_Descriptor_Type & X , 
//      const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
//template<class T , int Template_Dimension>

intArray_Descriptor_Type::
intArray_Descriptor_Type ( const Array_Domain_Type & X , 
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
   : Array_Domain(X,Indirect_Index_List)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of intArray_Descriptor_Type::intArray_Descriptor_Type ( const Array_Domain_Type & X , Internal_Indirect_Addressing_Index* Indirect_Index_List ) (this = %p)\n",this);
#endif

  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     Preinitialize_Descriptor();

     APP_ASSERT(Array_ID() == X.Array_ID());
   }


#ifdef USE_STRING_SPECIFIC_CODE

// ************************************************************************
// intArray_Descriptor_Type constructors for initialization using a string
// ************************************************************************
// intArray_Descriptor_Type::intArray_Descriptor_Type ( const char* dataString )
intArray_Descriptor_Type::intArray_Descriptor_Type ( const AppString & dataString )
   : Array_Domain(dataString)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

// *****************************************************************************
// *****************************************************************************
// ***********************  MEMBER FUNCTION DEFINITIONS  ***********************
// *****************************************************************************
// *****************************************************************************

// if defined(APP) || defined(PPP)

int
intArray_Descriptor_Type::getGhostBoundaryWidth ( int Axis ) const
   {
     int width = Array_Domain.getGhostBoundaryWidth(Axis);
  // APP_ASSERT (width == Array_Domain.InternalGhostCellWidth[Axis]);
     return width;
   }
// endif


void intArray_Descriptor_Type::setInternalGhostCellWidth ( 
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   {
     Array_Domain.setInternalGhostCellWidth(Integer_List);
   }

#if defined(PPP) || defined(APP)

void intArray_Descriptor_Type::
partition ( const Internal_Partitioning_Type & Internal_Partition )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::partition ( const Internal_Partitioning_Type & Internal_Partition ) \n");
#endif

#if defined(PPP) 
  // ... only actually partion for P++ ...
     Array_Domain.partition (Internal_Partition);
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Leaving intArray_Descriptor_Type::partition \n");
#endif
   }
#endif // end of PPP or APP

#if defined(PPP) 
#if defined(USE_PADRE)
  // What PADRE function should we call here?
#else

DARRAY* intArray_Descriptor_Type::
Build_BlockPartiArrayDomain ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::Build_BlockPartiArrayDomain \n");
#endif

     return Array_Domain.Build_BlockPartiArrayDomain();
   }
#endif // End of USE_PADRE not defined
#endif

// **********************************************************************************
//                           COPY CONSTRUCTOR
// **********************************************************************************

intArray_Descriptor_Type::intArray_Descriptor_Type ( const intArray_Descriptor_Type & X, int Type_Of_Copy )
   : Array_Domain(X.Array_Domain,FALSE,Type_Of_Copy)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::intArray_Descriptor_Type (const intArray_Descriptor_Type & X,bool, int) \n");
#endif

  // This may cause data to be set to null initial values and later reset
  // Works with P++ (maybe not A++)
     setDataPointer(NULL);
     Preinitialize_Descriptor();

#if !defined(PPP)
#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from intArray_Descriptor_Type COPY CONSTRUCTOR");
#endif
#endif
   }

// **********************************************************************************
//                           EQUALS OPERATOR
// **********************************************************************************
#if 0
// This is a more general template function but it does not work with many compilers!
//template<class T , int Template_Dimension>

//template<class S , int SecondDimension>

// intArray_Descriptor_Type & intArray_Descriptor_Type::
// operator=( const intArray_Descriptor_Type & X )
intArray_Descriptor_Type & intArray_Descriptor_Type::
operator=( const intArray_Descriptor_Type<S,SecondDimension> & X )
   {
  // The funtion body is the same as that of the operator= below!
     return *this;
   }
#endif

// operator=( const intArray_Descriptor_Type & X )
//template<class T , int Template_Dimension>

intArray_Descriptor_Type & intArray_Descriptor_Type::
operator=( const intArray_Descriptor_Type & X )
   {
  // At present this function is only called in the optimization of assignment and temporary use
  // in lazy_statement.C!  NOT TRUE! The Expression template option force frequent calls to this function!
#if COMPILE_DEBUG_STATEMENTS
#if !defined(PPP)
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type<T,int>::operator= (const intArray_Descriptor_Type<T,int> & X) Array_Data = %p \n",Array_Data);
#endif
#endif

  // Set reference count!
     referenceCount = getReferenceCountBase();

  // For now we don't set any of the pointers in the Array_Descriptor functions
  // these are set in the array class member functions (we will fix this later)
     Array_Domain = X.Array_Domain;

  // Copy Pointers
  // We must be careful about mixing pointer types
  // Array_Data = X.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from intArray_Descriptor_Type::operator=");
#endif

#if COMPILE_DEBUG_STATEMENTS
#if !defined(PPP)
     if (APP_DEBUG > 0)
          printf ("Leaving intArray_Descriptor_Type::operator= (const intArray_Descriptor_Type & X) Array_Data = %p \n",Array_Data);
#endif
#endif
     return *this;
   }

#if 0
// Should be inlined since it is used in a lot of places within A++!
//template<class T , int Template_Dimension>

int intArray_Descriptor_Type::Array_Size () const
   {
     return Array_Domain.Array_Size();
   }
#endif

#if defined(PPP)
//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::isLeftPartition( int Axis ) const
   {
     return Array_Domain.isLeftPartition(Axis);
   }

//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::isMiddlePartition( int Axis ) const
   {
     return Array_Domain.isMiddlePartition(Axis);
   }

//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::isRightPartition( int Axis ) const
   {
     return Array_Domain.isRightPartition(Axis);
   }

//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::isNonPartition( int Axis ) const
   {
     return Array_Domain.isNonPartition(Axis);
   }

#if defined(PPP)

bool
intArray_Descriptor_Type::isLeftNullArray( int Axis ) const
   {
  // A Null array can be part of the left edge or the right edge of a distributed axis
  // This function is helpful in determining which it is.

     int result = FALSE;

#if 1
     APP_ASSERT (SerialArray != NULL);
     result = Array_Domain.isLeftNullArray(SerialArray->Array_Descriptor.Array_Domain,Axis);
#else
#if defined(USE_PADRE)
     APP_ASSERT (Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     result = Array_Domain.parallelPADRE_DescriptorPointer->isLeftPartition(Axis);
#else
  // printf ("In isLeftNullArray(): Axis = %d \n",Axis);

     APP_ASSERT (SerialArray != NULL);

     if ( SerialArray->isNullArray() == TRUE )
        {
       // printf ("In intArray_Descriptor_Type::isLeftNullArray(axis): Domain_Dimension = %d \n",Array_Domain.Domain_Dimension);
          if ( Axis < Array_Domain.Domain_Dimension)
             {
               APP_ASSERT(Array_Domain.BlockPartiArrayDomain != NULL);
            // printf ("     gUBnd(BlockPartiArrayDomain,Axis) = %d \n",gUBnd(Array_Domain.BlockPartiArrayDomain,Axis));
            // printf ("     lalbnd(BlockPartiArrayDomain,Axis,Base[Axis],1) = %d \n",lalbnd(Array_Domain.BlockPartiArrayDomain,Axis,Array_Domain.Base[Axis],1));

               result = (gUBnd(Array_Domain.BlockPartiArrayDomain,Axis) < lalbnd(Array_Domain.BlockPartiArrayDomain,Axis,Array_Domain.Base[Axis],1));
             }
            else
             {
            // Along higher axes we want to force isLeftPartition == TRUE
               result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif
#endif

     return result;
   }


bool
intArray_Descriptor_Type::isRightNullArray( int Axis ) const
   {
  // A Null array can be part of the left edge or the right edge of a distributed axis
  // This function is helpful in determining which it is.

     int result = FALSE;

#if 1
     APP_ASSERT (SerialArray != NULL);
     result = Array_Domain.isLeftNullArray(SerialArray->Array_Descriptor.Array_Domain,Axis);
#else
#if defined(USE_PADRE)
     APP_ASSERT (Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     result = Array_Domain.parallelPADRE_DescriptorPointer->isLeftPartition(Axis);
#else
  // printf ("In isRightNullArray(): Axis = %d \n",Axis);
     if ( isNullArray() == TRUE )
        {
          if ( Axis < Array_Domain.Domain_Dimension)
             {
               result = !isLeftNullArray(Axis);
             }
            else
             {
            // Along higher axes we want to force isLeftPartition == TRUE
               result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif
#endif

     return result;
   }
#endif

//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int intArray_Descriptor_Type::findProcNum ( int* indexVals ) const
   {
  // Find number of processor where indexVals lives
     return Array_Domain.findProcNum(indexVals);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>
#if 0

int* intArray_Descriptor_Type::setupProcessorList
   (const intSerialArray& I, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorList
                 (I,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,
                  localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
intArray_Descriptor_Type::
setupProcessorList(
    int* I_A, int list_size, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorList
                 (I_A,list_size,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
intArray_Descriptor_Type::
setupProcessorListOnPosition(
     const intSerialArray& I, int& numberOfSupplements, 
     int** supplementLocationsPtr, int** supplementDataPtr, 
     int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorListOnPosition
                  (I,numberOfSupplements,supplementLocationsPtr,
                   supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
intArray_Descriptor_Type::
setupProcessorListOnPosition(
    int* I_A, int list_size, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorListOnPosition
                 (I_A,list_size,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//end of setupProcessorList section that is turned off 
#endif
//---------------------------------------------------------------------
#endif

//---------------------------------------------------------------------
#if 0
//template<class T , int Template_Dimension>

int intArray_Descriptor_Type::getRawDataSize( int Axis ) const
   {
     return Array_Domain.getRawDataSize(Axis);
   }

//template<class T , int Template_Dimension>

void intArray_Descriptor_Type::getRawDataSize( int* Data_Size ) const
   {
     Array_Domain.getRawDataSize(Data_Size);
   }

//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::
isSameBase ( const intArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameBase(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::
isSameBound ( const intArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameBound(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::
isSameStride ( const intArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameStride(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::
isSameLength ( const intArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameLength(X.Array_Domain);
   }
#endif

#if defined(PPP)
//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::
isSameGhostBoundaryWidth ( const intArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameGhostBoundaryWidth(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::
isSameDistribution ( const intArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameDistribution(X.Array_Domain);
   }
#endif

#if 0
//template<class T , int Template_Dimension>

bool intArray_Descriptor_Type::
isSimilar ( const intArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSimilar(X.Array_Domain);
   }
#endif

#if 0
// I think this is commented out because it is not required!
//template<class T , int Template_Dimension>

int intArray_Descriptor_Type::
computeArrayDimension ( ARGUMENT_LIST_MACRO_INTEGER )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of intArray_Descriptor_Type::computeArrayDimension (%d,%d,%d,%d) \n",
               VARIABLE_LIST_MACRO_INTEGER );
#endif

     APP_ASSERT(Array_Size_L >= 0);
     APP_ASSERT(Array_Size_K >= 0);
     APP_ASSERT(Array_Size_J >= 0);
     APP_ASSERT(Array_Size_I >= 0);

     APP_ASSERT(i >= 0);
     APP_ASSERT(j >= 0);
     APP_ASSERT(k >= 0);
     APP_ASSERT(l >= 0);
#if MAX_ARRAY_DIMENSION > 4
     APP_ASSERT(m >= 0);
#if MAX_ARRAY_DIMENSION > 5
     APP_ASSERT(n >= 0);
#if MAX_ARRAY_DIMENSION > 6
     APP_ASSERT(o >= 0);
#if MAX_ARRAY_DIMENSION > 7
     APP_ASSERT(p >= 0);
#endif
#endif
#endif
#endif

     int Array_Dimension = 0;
     if ( i > 0 ) Array_Dimension = 1;
     if ( j > 1 ) Array_Dimension = 2;
     if ( k > 1 ) Array_Dimension = 3;
     if ( l > 1 ) Array_Dimension = 4;
#if MAX_ARRAY_DIMENSION > 4
     if ( m > 1 ) Array_Dimension = 5;
#if MAX_ARRAY_DIMENSION > 5
     if ( n > 1 ) Array_Dimension = 6;
#if MAX_ARRAY_DIMENSION > 6
     if ( o > 1 ) Array_Dimension = 7;
#if MAX_ARRAY_DIMENSION > 7
     if ( o > 1 ) Array_Dimension = 8;
#if MAX_ARRAY_DIMENSION > 7
     APP_ABORT();
#endif
#endif
#endif
#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of intArray_Descriptor_Type::computeArrayDimension (%d,%d,%d,%d) -- Array_Dimension = %d \n",
               Array_Size_I,Array_Size_J,Array_Size_K,Array_Size_L,Array_Dimension);
#endif

     return Array_Dimension;
   }
#endif

//template<class T , int Template_Dimension>

int intArray_Descriptor_Type::
computeArrayDimension ( int* Array_Sizes )
   {
     return Array_Domain_Type::computeArrayDimension(Array_Sizes);
   }

//template<class T , int Template_Dimension>

int intArray_Descriptor_Type::
computeArrayDimension ( const intArray_Descriptor_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of intArray_Descriptor_Type::computeArrayDimension () \n");
#endif

     return Array_Domain_Type::computeArrayDimension(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

void intArray_Descriptor_Type::
Build_Temporary_By_Example ( const intArray_Descriptor_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::Build_Temporary_By_Example(intArray_Descriptor_Type) \n");
     X.Test_Consistency ("Test INPUT: Called from intArray_Descriptor_Type::Build_Temporary_By_Example");
#endif

     int Original_Array_ID = Array_ID();
     int Example_Array_ID  = X.Array_ID();
     (*this) = X;

  // Restore the original array id
     Array_Domain.setArray_ID (Original_Array_ID);

  // Make sure we return a temporary (so set it so)
     setTemporary(TRUE);

  // ... (4/23/98, kdb) can't reset base here if using indirect addressing
  //  because i n d e x ing intArrays will be incorrect relative to Data_Base ...
     if (!Array_Domain.Uses_Indirect_Addressing)
          setBase(APP_Global_Array_Base);

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency
          ("Test OUTPUT: Called from intArray_Descriptor_Type::Build_Temporary_By_Example(intArray_Descriptor_Type)");
#endif
   }

#if defined(PPP)
//-------------------------------------------------------------------------
// This function is specific for use with P++ indirect addressing!
//template<class T , int Template_Dimension>

void intArray_Descriptor_Type::
Compute_Local_Index_Arrays ( const intArray_Descriptor_Type & Original_Descriptor )
   {

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
     {   
        printf ("Inside of intArray_Descriptor_Type::Compute_Local_Index_Arrays()");
        printf (" (Array_ID = %d)\n",Array_ID());
     }   
#endif

     Array_Domain.Compute_Local_Index_Arrays(Original_Descriptor.Array_Domain);
   }   
//-------------------------------------------------------------------------
#if 0
// This function is specific for use with P++ indirect addressing!
//template<class T , int Template_Dimension>

void intArray_Descriptor_Type::
Compute_Local_Index_Arrays ( const intArray_Descriptor_Type & Original_Descriptor )
   {
     int i; // Index value used below

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::Compute_Local_Index_Arrays() (Array_ID = %d)\n",Array_ID());
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
          printf ("Inside of intArray_Descriptor_Type::Compute_Local_Index_Arrays() -- Caching of local intArray usage not implemented yet! \n");
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
       // Indirect_Addressing_Cache_Descriptor *Cache_Descriptor = Indirect_Addressing_Cache.put (X);
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
               Replicated_Index_intArray[i] = Index_Array[i]->SerialArray;
               Index_Array[i]->SerialArray->incrementReferenceCount();
            // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");
               APP_ASSERT(Replicated_Index_intArray[i]->Array_Descriptor->Is_A_Temporary == FALSE);
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
               int Local_Base  = Original_Descriptor.Data_Base[i] + Original_Descriptor.Base[i];
               int Local_Bound = Original_Descriptor.Data_Base[i] + Original_Descriptor.Bound[i];

            // printf ("Local_Base = %d  Local_Bound = %d \n",Local_Base,Local_Bound);
               if (Valid_Local_Entries_Mask.Array_Descriptor->Is_A_Null_Array)
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
                 // display("THIS Descriptor");
                 // Original_Descriptor.display("ORIGINAL Descriptor");
                 // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");
                    Valid_Local_Entries_Mask.view("Valid_Local_Entries_Mask");
                    APP_ABORT();
                  }
             }
        }

  // Valid_Local_Entries_Mask.display("Inside of intArray_Descriptor_Type::Compute_Local_Index_Arrays -- Valid_Entries_Mask");

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
               if ( sum ( (*Index_Array[i]->SerialArray) != (*Local_Index_Array[i]) ) != 0 )
                  {
                    printf ("ERROR: On a single processor the Local_Index_Array should match the Index_Array[i]->SerialArray \n");
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
//template<class T , int Template_Dimension>

void intArray_Descriptor_Type::
Update_Parallel_Information_Using_Old_Descriptor ( const intArray_Descriptor_Type & Old_Array_Descriptor )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of intArray_Descriptor_Type::Update_Parallel_Information_Using_Old_Descriptor! \n");
#endif
  // Copy relevant data from old descriptor (this should be a function)

     Array_Domain.Update_Parallel_Information_Using_Old_Descriptor(Old_Array_Descriptor.Array_Domain);
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
//template<class T , int Template_Dimension>

void
intArray_Descriptor_Type::Test_Preliminary_Consistency( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since intArray_Descriptor_Type::Test_Preliminary_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of intArray_Descriptor_Type::Test_Preliminary_Consistency! (Label = %s) (Descriptor_Dimension = %d) \n",
               Label,numberOfDimensions());
#endif

     Array_Domain.Test_Preliminary_Consistency(Label);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Leaving intArray_Descriptor_Type::Test_Preliminary_Consistency! (Label = %s) \n",Label);
#endif
   }

//template<class T , int Template_Dimension>

void
intArray_Descriptor_Type::Test_Consistency( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since intArray_Descriptor_Type::Test_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of intArray_Descriptor_Type::Test_Consistency! (Label = %s) (Descriptor_Dimension = %d) \n",
               Label,numberOfDimensions());
#endif

#if defined(PPP)
     if (SerialArray != NULL)
        {
          if (getRawDataReferenceCount() < getRawDataReferenceCountBase())
               printf ("In intArray_Descriptor_Type::Test_Consistency(): Array_ID() = %d getRawDataReferenceCount() = %d \n",
                    Array_ID(),getRawDataReferenceCount());
          APP_ASSERT (getRawDataReferenceCount() >= getRawDataReferenceCountBase());
        }
#else
     if (getRawDataReferenceCount() < 0)
          printf ("In intArray_Descriptor_Type::Test_Consistency(): getRawDataReferenceCount() = %d \n",
               getRawDataReferenceCount());
     APP_ASSERT (getRawDataReferenceCount() >= 0);
#endif

#if 0
     printf ("sizeof(array_descriptor)             = %d \n",sizeof(array_descriptor) );
     printf ("sizeof(intArray_Descriptor_Type) = %d \n",sizeof(intArray_Descriptor_Type) );
#endif
#if defined(APP) || defined(SERIAL_APP)
     APP_ASSERT ( sizeof(array_descriptor) == sizeof(intArray_Descriptor_Type) );
#endif

#if defined(PPP)
  // By definition of getRawDataReferenceCount() the following should be TRUE!
     if (SerialArray != NULL)
        {
          APP_ASSERT ( SerialArray->getReferenceCount() == getRawDataReferenceCount() );
        }
#endif

#if !defined(PPP)
  // Reference this variable so that purify can check that it is initialized!
     APP_ASSERT (Array_Data == NULL || Array_Data != NULL);
#endif

  // Reference this variable so that purify can check that it is initialized!
     APP_ASSERT (ExpressionTemplateDataPointer == NULL || ExpressionTemplateDataPointer != NULL);

  // I don't know what this should be tested against to verify correctness!
  // APP_ASSERT (ExpressionTemplateDataPointer == Array_Data + Array_Domain.ExpressionTemplateOffset);

  // I don't know what these should be tested against either!
     APP_ASSERT (Array_View_Pointer0 == NULL || Array_View_Pointer0 != NULL);
#if MAX_ARRAY_DIMENSION>1
     APP_ASSERT (Array_View_Pointer1 == NULL || Array_View_Pointer1 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>2
     APP_ASSERT (Array_View_Pointer2 == NULL || Array_View_Pointer2 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>3
     APP_ASSERT (Array_View_Pointer3 == NULL || Array_View_Pointer3 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>4
     APP_ASSERT (Array_View_Pointer4 == NULL || Array_View_Pointer4 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>5
     APP_ASSERT (Array_View_Pointer5 == NULL || Array_View_Pointer5 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>6
     APP_ASSERT (Array_View_Pointer6 == NULL || Array_View_Pointer6 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>7
     APP_ASSERT (Array_View_Pointer7 == NULL || Array_View_Pointer7 != NULL);
#endif

  // Uncomment this line so that the array domain is tested!
     Array_Domain.Test_Consistency(Label);

#if 0
// We had to put this test into the array object's Test_Consistency member function! 
#if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset = Array_Domain.Base[0];
     for (int i=1; i < MAX_ARRAY_DIMENSION; i++)
        {
          ExpectedOffset += Array_Domain.Base[i] * Array_Domain.Size[i-1];
        }
     APP_ASSERT (Array_Domain.ExpressionTemplateOffset == ExpectedOffset);
     if (Array_Data != NULL)
        {
          if (ExpressionTemplateDataPointer != Array_Data+ExpectedOffset)
             {
               printf ("Array_Data = %p \n",Array_Data);
               printf ("ExpressionTemplateDataPointer = %p \n",ExpressionTemplateDataPointer);
               printf ("ExpectedOffset = %d \n",ExpectedOffset);
               display("ERROR in intArray_Descriptor_Type<T,Template_Dimension>::Test_Consistency()");
               APP_ABORT();
             }
          APP_ASSERT (ExpressionTemplateDataPointer == Array_Data+ExpectedOffset);
        }
#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Leaving intArray_Descriptor_Type::Test_Consistency! (Label = %s) \n",Label);
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
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

// ****************************************************************************
// ****************************************************************************
// **************  ERROR CHECKING FUNCTION FOR INDEXING OPERATORS  ************
// ****************************************************************************
// ****************************************************************************

#if 0
//template<class T , int Template_Dimension>

void intArray_Descriptor_Type::
Error_Checking_For_Index_Operators (
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::Error_Checking_For_Index_Operators \n");
#endif

     Array_Domain.Error_Checking_For_Index_Operators(Internal_Index_List);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving intArray_Descriptor_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }
#endif

#if 0
//template<class T , int Template_Dimension>

void intArray_Descriptor_Type::
Error_Checking_For_Index_Operators (
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::Error_Checking_For_Index_Operators(Indirect_Index_List) \n");
#endif

     Array_Domain.Error_Checking_For_Index_Operators(Indirect_Index_List);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving intArray_Descriptor_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }
#endif

#if 0
// **************************************************************************
// This function does the bounds checking for the scalar indexing of A++
// array objects.  Its purpose is to localize all the error checking for
// scalar indexing.
// **************************************************************************
// Old prototype
// void intArray::Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const
//template<class T , int Template_Dimension>

void intArray_Descriptor_Type::
Error_Checking_For_Scalar_Index_Operators ( 
     const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const
   {
     Array_Domain.Error_Checking_For_Scalar_Index_Operators(Integer_List);
   }
#endif

// *************************************************************************************************
// ********************  APPEND index_operator.C  **************************************************
// *************************************************************************************************

/* Actually this is a little too long for the AT&T compiler to inline! */
//template<class T , int Template_Dimension>

intArray_Descriptor_Type* intArray_Descriptor_Type::
Vectorizing_Descriptor ( const intArray_Descriptor_Type & X )
   {
  // Build a descriptor to fill in!
     intArray_Descriptor_Type* Return_Descriptor = new intArray_Descriptor_Type;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of intArray_Descriptor_Type::Vectorizing_Descriptor ( const intArray_Descriptor_Type & X ) (this = %p)\n",Return_Descriptor);
#endif

  // Avoid compiler warning about unused input variable
     if (&X);

     printf ("Inside of intArray_Descriptor_Type::Vectorizing_Descriptor: not sure how to set it up! \n");
     APP_ABORT();

  // Return_Descriptor.Array_Domain = Vectorizing_Domain (X.Array_Domain);

     return Return_Descriptor;
   }


void
intArray_Descriptor_Type::
Allocate_Array_Data ( bool Force_Memory_Allocation )
   {
  // This function allocates the internal data for the intArray_Descriptor_Tyep object.  In A++
  // this allocates the the raw int data array.  In P++ it allocates the internal
  // A++ array for the current processor (using the sizes defined in the DARRAY
  // parallel descriptor).
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("*** Allocating the Array_Data (or getting it from the hash table)! \n");
#endif
 
#if defined(PPP)
  // The allocation of data in the parallel environment is especially complicated
  // so this is broken out as a separate function and is implemented only for P++.
     Allocate_Parallel_Array (Force_Memory_Allocation);
#else
     APP_ASSERT(Array_Data == NULL);
     int* Array_Data_Pointer = NULL;

// KCC gives an error about Expression_Tree_Node_Type not being a complete type
// this is because the general use of this function outside of A++ directly
// uses a subset of the header files used to compile A++.
#if !defined(USE_EXPRESSION_TEMPLATES)
  // if (!Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION || Force_Memory_Allocation)
     if (Force_Memory_Allocation)
#else
     if (Force_Memory_Allocation)
#endif
        {
          if (intArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE)
             {
               Array_Data_Pointer = Hash_Table.Get_Primative_Array ( Array_Size() );
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call MDI_int_Allocate! \n");
#endif
#if !defined(USE_EXPRESSION_TEMPLATES)
               Array_Data_Pointer = MDI_int_Allocate ( (array_domain*)(&Array_Domain) );
#else
            // Need to allocate space without using the MDI layer
            // printf ("Need to allocate space without using the MDI layer! \n");
            // APP_ABORT();

               int Size = Array_Domain.Size[MAX_ARRAY_DIMENSION-1];
               if (Size > 0)
                  {
                    Array_Data_Pointer = (int*) APP_MALLOC ( Size * sizeof(double) );
  
                    if (Array_Data_Pointer == NULL)
                       {
                         printf ("HEAP ERROR: Array_Data_Pointer == NULL in void intArray_Descriptor_Type::Allocate_Array_Data(bool) (Size = %d)! \n", Size);
                         APP_ABORT();
                       }
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (Size < 0)
                       {
                         printf ("ERROR: Size < 0 Size = %d in void intArray_Descriptor_Type::Allocate_Array_Data(bool)! \n",Size);
                         APP_ABORT();
                       }
#endif
                    Array_Data_Pointer = NULL;
                  }
#endif
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Allocated Array_Data_Pointer = %p \n",Array_Data_Pointer);
#endif

  // Make the assignment of the new data pointer!
     Array_Data = Array_Data_Pointer;

  // printf ("In A++: setting up the Array_View_Pointer's \n");

  // Now that we have added tests into the Test_Consistancy function
  // (equivalent to that in the scalar indexing) we have to set these 
  // pointers as early as possible
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO

  // This test is repeated in the upper level array object's Test_Consistancy member function
     APP_ASSERT (Array_View_Pointer0 == Array_Data + Array_Domain.Scalar_Offset[0]);
     APP_ASSERT (Array_View_Pointer1 == Array_Data + Array_Domain.Scalar_Offset[1]);
     APP_ASSERT (Array_View_Pointer2 == Array_Data + Array_Domain.Scalar_Offset[2]);
     APP_ASSERT (Array_View_Pointer3 == Array_Data + Array_Domain.Scalar_Offset[3]);
     APP_ASSERT (Array_View_Pointer4 == Array_Data + Array_Domain.Scalar_Offset[4]);
     APP_ASSERT (Array_View_Pointer5 == Array_Data + Array_Domain.Scalar_Offset[5]);
#endif
   }
 
#if defined(PPP) && defined(USE_PADRE)

void intArray_Descriptor_Type::
setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::setLocalDomainInPADRE_Descriptor(%p) \n",inputDomain);
#endif

     if (Array_Domain.parallelPADRE_DescriptorPointer != NULL)
          Array_Domain.parallelPADRE_DescriptorPointer->setLocalDomain(inputDomain);
   }
#endif

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" void MDI_int_Deallocate ( int* Data_Pointer , array_domain* Descriptor );
#endif

void
intArray_Descriptor_Type::Delete_Array_Data ()
   {
#if defined(PPP)
     intSerialArray* Data_Pointer = SerialArray;
#else
     int* Data_Pointer = Array_Data;
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Descriptor_Type::Delete_Array_Data Array_ID = %d getRawDataReferenceCount() = %d  Data_Pointer = %p \n",
               Array_ID(),getRawDataReferenceCount(),Data_Pointer);

     if (getRawDataReferenceCount() < 0)
          printf ("getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
#endif

     APP_ASSERT (getRawDataReferenceCount() >= 0);
 
  // P++ objects must always try to delete their SerialArray objects because then the
  // SerialArray reference count will be decremented (at least) or the SerialArray
  // deleted (at most) within the delete operator.  However we find this feature of the delete
  // operator to be a little too clever and so we should consider removing  from the 
  // decrementation of the reference count from the delete operator and have it done by the user
  // instead.  this would mean that the user would have to take care of incrementing and decrementing
  // the reference count instead of just the incrementing (this is more symetric -- i.e. superior).
  // A++ and SerialArray objects should always decrement their
  // reference count (and then delete the raw data if the reference coutn is less
  // than ZERO) here since there destructor has called this function (for that purpose).
  // Within this system the delete operator takes care of decrementing the reference count!
#if defined(PPP)
  // printf ("In intArray_Descriptor_Type::Delete_Array_Data shouldn't we call decrementRawDataReferenceCount()??? \n");
  // APP_ABORT();
  // APP_ASSERT (getRawDataReferenceCount() >= 0);
     APP_ASSERT (getRawDataReferenceCount() >= getReferenceCountBase());
  // decrementRawDataReferenceCount();
     bool Delete_Raw_Data = TRUE;
#else
  // APP_ASSERT (getRawDataReferenceCount() >= 0);
     APP_ASSERT (getRawDataReferenceCount() >= getReferenceCountBase());
     decrementRawDataReferenceCount();
  // int Reference_Count_Lower_Bound = 0;
  // bool Delete_Raw_Data = ( getRawDataReferenceCount() < Reference_Count_Lower_Bound );
     bool Delete_Raw_Data = ( getRawDataReferenceCount() < getReferenceCountBase() );
#endif
 
     if ( Delete_Raw_Data )
        {
#if COMPILE_DEBUG_STATEMENTS
          if ( (APP_DEBUG > 1) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Deleting the data since reference count was == getReferenceCountBase()! \n");
#endif
          if (Data_Pointer != NULL)
             {
            // In P++ we delete the reference counted SerialArray and in A++ and Serial_A++ (the lower level)
            // we can optionally save the data for reuse (through a hash table mechanism).
#if defined(PPP)
#if defined(USE_PADRE)
            // We have to remove the references in PADRE to the P++ SerialArray_Domain objects
            // before we remove the SerialArray objects which contain the SerialArray_Domain objects.
               setLocalDomainInPADRE_Descriptor(NULL);
#endif
            // The destructor we decrement the referenceCount in the SerialArray
               Data_Pointer->decrementReferenceCount();
               if (Data_Pointer->getReferenceCount() < getReferenceCountBase())
                    delete Data_Pointer;
               Data_Pointer = NULL;
#else
            // This is done above before the if test -- it is a bug to do it here!
            // decrementRawDataReferenceCount();
               if (intArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE)
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 2)
                         printf ("Calling Hash_Table.Put_Primative_Array_Into_Storage \n");
#endif
                    Hash_Table.Put_Primative_Array_Into_Storage ( Data_Pointer , Array_Size() );
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("Calling MDI_int_Deallocate! Data_Pointer = %p \n",Data_Pointer);
#endif
                 // MDI_int_Deallocate ( Data_Pointer , (array_domain*)Array_Descriptor.Array_Domain );
                 // MDI_int_Deallocate ( Data_Pointer , &((array_domain)Array_Descriptor.Array_Domain) );
#if !defined(USE_EXPRESSION_TEMPLATES)
                    MDI_int_Deallocate ( Data_Pointer , (array_domain*)(&Array_Domain) );
#else
                 // printf ("Need to free space without calling the MDI layer \n");
                 // APP_ABORT();
                    APP_ASSERT (Data_Pointer != NULL);
                    free (Data_Pointer);
#endif
                  }
#endif
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Data_Pointer == NULL in intArray::Delete_Array_Data! \n");
#endif
             }
        }
#if COMPILE_DEBUG_STATEMENTS
       else
        {
          if ( (APP_DEBUG > 1) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Skip deallocation of array data since Array_ID = %d getRawDataReferenceCount() = %d Data_Pointer = %p \n",
                    Array_ID(),getRawDataReferenceCount(),Data_Pointer);
        }
#endif
 
#if defined(PPP)
     SerialArray = NULL;
#else
     Array_Data = NULL;
#endif
   }


#if 0


void
intArray_Descriptor_Type::postAllocationSupport()
   {
  // Support function for allocation of P++ array objects
  // This function modifies the serialArray descriptor to 
  // make it a proper view of the serial array excluding 
  // the ghost boundaries

     bool Generated_A_Null_Array              = FALSE;
     int SerialArray_Cannot_Have_Contiguous_Data = FALSE;

  // Need to set the upper bound on the iteration to MAX_ARRAY_DIMENSION
  // since all the variables for each dimension must be initialized.
     int j;
     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
#if 0
          printf ("(after setBase): getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
               j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));
	  printf ("(after setBase): SerialArray->getRawBase(%d) = %d  getRawBase(%d) = %d  SerialArray->getRawBound(%d) = %d getRawBound(%d) = %d \n",
		  j,SerialArray->getRawBase(j),j,getRawBase(j),j,SerialArray->getRawBound(j),j,getRawBound(j));
#endif

       // This function is used tohide the use of A++/P++ specific range objects and 
       // provide an interface that both the PADRE and nonPADRE versions can call.
       // void adjustSerialDomainForStridedAllocation ( int axis,  const Array_Domain_Type & globalDomain );
          if ( (Array_Domain.Stride [j] > 1) && (SerialArray->isNullArray() == FALSE) )
             {
            // printf ("##### Calling Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");
               SerialArray->Array_Descriptor.Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain);
            // printf ("##### DONE: Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");
             }

#if 0
#if 0
          printf ("Bases[%d] = %d \n",j,Bases[j]);
          printf ("Array_Domain.getDataBaseVariable(%d) = %d \n",
                   j,Array_Domain.getDataBaseVariable(j));
          printf ("AFTER SerialArray->setBase (%d) --- SerialArray->getRawBase(%d) = %d \n",
                   Bases[j]+Array_Domain.getDataBaseVariable(j),j,SerialArray->getRawBase(j));
          printf ("SerialArray->getDataBaseVariable(%d) = %d \n",
                   j,SerialArray->Array_Descriptor.Array_Domain.getDataBaseVariable(j));
#endif

       // This could be implemented without the function call overhead
       // represented here but this simplifies the initial implementation for now

	  printf ("In Allocate.C: getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
		  j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));
	  printf ("In Allocate.C: getLocalStride(%d) = %d  getStride(%d) = %d \n",
		  j,getLocalStride(j),j,getStride(j));

	  printf ("In Allocate.C: SerialArray->getRawBase(%d) = %d  getRawBase(%d) = %d  SerialArray->getRawBound(%d) = %d getRawBound(%d) = %d \n",
		  j,SerialArray->getRawBase(j),j,getRawBase(j),j,SerialArray->getRawBound(j),j,getRawBound(j));
	  printf ("In Allocate.C: SerialArray->getRawStride(%d) = %d  getRawStride(%d) = %d \n",
		  j,SerialArray->getRawStride(j),j,getRawStride(j));
#endif

       // The left side uses the bases which are relative to a unit stride (a relatively simple case)!
          int Left_Size_Size  = (getLocalBase(j) - getBase(j)) * Array_Domain.Stride[j];

       // The bound is the funky case since the bases are relative to stride 1 and 
       // the bounds are relative to stride.
#if 0
       // int Right_Size_Size = (getBound(j) - getLocalBound(j)) * Array_Domain.Stride[j];
          int Right_Size_Size = 0;
               if (Is_A_Left_Partition == TRUE)
                    Right_Size_Size = (getBound(j) - getLocalBound(j)) * Array_Domain.Stride[j];
	       else
                {
                  int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
                  Right_Size_Size = getBound(j) - tempLocalBound;
                }
#else
       // int tempLocalBound  = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];

       // int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
       // int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

       // printf ("getBase(%d) = %d getLocalBase(%d) = %d getLocalBound(%d) = %d Array_Domain.Stride[%d] = %d \n",
       //      j,getBase(j),j,getLocalBase(j),j,getLocalBound(j),j,Array_Domain.Stride[j]);

       // int tempLocalBound = getBase(j) + (getLocalBound(j) - getBase(j)) * Array_Domain.Stride[j];
       // int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
          int tempLocalUnitStrideBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Array_Domain.Stride[j];
          int tempLocalBound = tempLocalUnitStrideBase + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];

          int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

       // printf ("tempLocalUnitStrideBase = %d \n",tempLocalUnitStrideBase);

          int Right_Size_Size = 0;

          if(SerialArray->isNullArray() == TRUE)
             {
            // printf ("found a null array (locally) so reset tempLocalBound to getLocalBase(%d) = %d \n",j,getLocalBase(j));
	       int nullArrayOnLeftEdge = isLeftNullArray(j);
               if (nullArrayOnLeftEdge == TRUE)
                  {
                 // printf ("Set tempLocalBound to base! \n");
                 // tempLocalBound  = getLocalBase(j);
                    tempLocalBound  = getBase(j);
                 // gUBnd(Array_Domain.BlockPartiArrayDomain,j);
	       
                  }
                 else
                  {
                 // printf ("Set tempLocalBound to bound! \n");
                 // tempLocalBound  = getBound(j);
                 // tempLocalBound  = getBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
                    tempLocalBound  = getBound(j);
                  }

               int rightEdgeSize = tempGlobalBound - tempLocalBound;
               Right_Size_Size = (rightEdgeSize == 0) ? 0 : rightEdgeSize + 1;
             }
            else
             {
            // printf ("NOT A NULL ARRAY (locally) \n");
               Right_Size_Size = (tempGlobalBound - tempLocalBound);
             }
	  
       // printf ("In allocate.C: first instance: j = %d tempLocalBound = %d tempGlobalBound = %d \n",j,tempLocalBound,tempGlobalBound);

       // bugfix (8/15/2000) account for stride
       // int Right_Size_Size = (tempGlobalBound - tempLocalBound) / Array_Domain.Stride[j];
       // int Right_Size_Size = (tempGlobalBound - tempLocalBound);
#endif

       // Do these depend upon the stride in anyway?
          Array_Domain.Left_Number_Of_Points [j] = (Left_Size_Size  >= 0) ? Left_Size_Size  : 0;
          Array_Domain.Right_Number_Of_Points[j] = (Right_Size_Size >= 0) ? Right_Size_Size : 0;

#if 0
          printf ("Left_Size_Size = %d Right_Size_Size = %d \n",Left_Size_Size,Right_Size_Size);
          printf ("tempLocalBound = %d tempGlobalBound = %d \n",tempLocalBound,tempGlobalBound);
	  printf ("Array_Domain.Left_Number_Of_Points [%d] = %d Array_Domain.Right_Number_Of_Points[%d] = %d \n",
               j,Array_Domain.Left_Number_Of_Points [j],
               j,Array_Domain.Right_Number_Of_Points[j]);
#endif

       // Comment out the Global Index since it should already be set properly
       // and this will just screw it up! We have to set it (I think)
#if 0
	  printf ("BEFORE RESET: Array_Domain.Global_Index [%d] = (%d,%d,%d,%s) \n",j,
               Array_Domain.Global_Index [j].getBase(),
               Array_Domain.Global_Index [j].getBound(),
               Array_Domain.Global_Index [j].getStride(),
               Array_Domain.Global_Index[j].getModeString());
#endif
       // Array_Domain.Global_Index [j] = Index (getBase(j),(getBound(j)-getBase(j))+1,1);
       // Use the range object instead
          Array_Domain.Global_Index [j] = Range (getRawBase(j),getRawBound(j),getRawStride(j));
#if 0
	  printf ("AFTER RESET: Array_Domain.Global_Index [%d] = (%d,%d,%d,%s) \n",j,
               Array_Domain.Global_Index [j].getBase(),
               Array_Domain.Global_Index [j].getBound(),
               Array_Domain.Global_Index [j].getStride(),
               Array_Domain.Global_Index[j].getModeString());
#endif

          APP_ASSERT (Array_Domain.Global_Index [j].getMode() != Null_Index);

          int ghostBoundaryWidth = Array_Domain.InternalGhostCellWidth[j];
          APP_ASSERT (ghostBoundaryWidth >= 0);

       // If the data is not contiguous on the global level then it could not 
       // be locally contiguous (unless the partitioning was just right --but 
       // we don't worry about that case) if this processor is on the left or 
       // right and the ghost bundaries are of non-zero width
       // then the left and right processors can not have:
          SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = Array_Domain.Is_Contiguous_Data;

       // Additionally if we have ghost boundaries then these are always hidden 
       // in a view and so the data can't be contiguous
          if (Array_Domain.InternalGhostCellWidth [j] > 0)
               SerialArray_Cannot_Have_Contiguous_Data = TRUE;

       // In this later case since we have to modify the parallel domain object to be consistant
          if (SerialArray_Cannot_Have_Contiguous_Data == TRUE)
             {
            // Note: we are changing the parallel array descriptor (not done much in this function)
               Array_Domain.Is_Contiguous_Data                               = FALSE;
               SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = FALSE;
             }

          int Local_Stride = SerialArray->Array_Descriptor.Array_Domain.Stride [j];

          if (j < Array_Domain.Domain_Dimension)
             {
            // Bugfix (17/10/96)
            // The Local_Mask_Index is set to NULL in the case of repartitioning
            // an array distributed first across one processor and then across 2 processors.
            // The second processor is a NullArray and then it has a valid local array (non nullarray).
            // But the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
            // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
            // setup.  The problem is that it is either not setup or sometimes setup incorrectly.
            // So we have to set it to a non-nullArray as a default.
               if (Array_Domain.Local_Mask_Index[j].getMode() == Null_Index)
                  {
                 // This resets the Local_Mask_Index to be consistant with what it usually is going into 
                 // this function. This provides a definition of Local_Mask_Index consistant with
                 // the global range of the array. We cannot let the Local_Mask_Index be a Null_Index
                 // since it would not allow the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
                 // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
                 // setup.
                 // Array_Domain.Local_Mask_Index[j].display("INSIDE OF ALLOCATE PARALLEL ARRAY -- CASE OF NULL_INDEX");
                 // Array_Domain.Local_Mask_Index[j] = Range (Array_Domain.Base[j],Array_Domain.Bound[j]);
                    Array_Domain.Local_Mask_Index[j] = Array_Domain.Global_Index[j];
                  }

               APP_ASSERT (Array_Domain.Local_Mask_Index[j].getMode() != Null_Index);
               APP_ASSERT (Array_Domain.Local_Mask_Index[j].getMode() != All_Index);

               bool Is_A_Left_Partition   = Array_Domain.isLeftPartition  (j);
               bool Is_A_Right_Partition  = Array_Domain.isRightPartition (j);
               bool Is_A_Middle_Partition = Array_Domain.isMiddlePartition(j);

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
               int base_offset  = getBase(j) - getLocalBase(j);

            // Bounds are relative to strides (non-unit strides)
            // int bound_offset = getLocalBound(j) - getBound(j);
            // Convert the bound to a unit stride bound so it can be compared to the value of getBound(j)
#if 0
               int tempLocalBound = 0;
               if (Is_A_Left_Partition == TRUE)
                    tempLocalBound = getLocalBound(j);
                 else
                    tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
#else
   //          int tempLocalBound  = getBase(j) + (getLocalBound(j) - getBase(j)) * Array_Domain.Stride[j];
   //          int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

            // I think these are already computed above!
               int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
               int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

            // printf ("In allocate.C: 2nd instance: j = %d tempLocalBound = %d tempGlobalBound = %d \n",j,tempLocalBound,tempGlobalBound);
#endif

            // Now that we have a unit stride bound we can do the subtraction
               int bound_offset = tempLocalBound - tempGlobalBound;

               int Original_Base_Offset_Of_View  = (Array_Domain.Is_A_View) ? (base_offset>0 ? base_offset : 0) : 0;
               int Original_Bound_Offset_Of_View = (Array_Domain.Is_A_View) ? (bound_offset>0 ? bound_offset : 0) : 0;

            // Need to account for the stride
            // int Count = ((getLocalBound(j) - getLocalBase(j))+1) -
            //             (Original_Base_Offset_Of_View + Original_Bound_Offset_Of_View);
               int Count = ((getLocalBound(j)-getLocalBase(j))+1) -
                           (Original_Base_Offset_Of_View + Original_Bound_Offset_Of_View);
            // Only could the left end once in adjusting for stride!
            // Count = 1 + (Count-1) * Array_Domain.Stride[j];

#if 0
               printf ("Array_Domain.Is_A_View = %s \n",(Array_Domain.Is_A_View == TRUE) ? "TRUE" : "FALSE");

               printf ("(compute Count): getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
               j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));

	       printf ("base_offset = %d  tempGlobalBound = %d tempLocalBound = %d bound_offset = %d \n",
                    base_offset,tempGlobalBound,tempLocalBound,bound_offset);
               printf ("SerialArray->Array_Descriptor.Array_Domain.getBase(%d) = %d \n",
                  j,SerialArray->Array_Descriptor.Array_Domain.getBase(j));
               printf ("SerialArray->Array_Descriptor.Array_Domain.getBound(%d) = %d \n",
                  j,SerialArray->Array_Descriptor.Array_Domain.getBound(j));
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

	       if (!Array_Domain.Is_A_View)
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
               printf ("In Allocate: Final Count = %d \n",Count);
#endif

               if (Count > 0)
                  {
                 // This should have already been set properly (so why reset it?)
                 // Actually it is not set properly all the time at least so we have to reset it.
#if 0
                    printf ("BEFORE 1st RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                 // This should be relative to stride 1 base/bound data since the local mask is relative to the local data directly (unit stride)
                 // Array_Domain.Local_Mask_Index[j] = Index(getLocalBase(j) + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);
		    int tempLocalBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Local_Stride;
                    Array_Domain.Local_Mask_Index[j] = Index(tempLocalBase + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);

#if 0
                    printf ("AFTER 1st RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif

                 // Make the SerialArray a view of the valid portion of the local partition
                    SerialArray->Array_Descriptor.Array_Domain.Is_A_View = TRUE;

                 /*
                 // ... (bug fix, 5/21/96,kdb) bases and bounds need to
                 // be adjusted by Original_Base_Offset_Of_View and
                 // Original_Bound_Offset_Of_View no matter what processor ...
                 */

                    SerialArray->Array_Descriptor.Array_Domain.Base [j] += Original_Base_Offset_Of_View;
                    SerialArray->Array_Descriptor.Array_Domain.Bound[j] -= Original_Bound_Offset_Of_View;
                 // ... bug fix (8/26/96, kdb) User_Base must reflect the view ...
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] += Original_Base_Offset_Of_View;

                    if (!Array_Domain.Is_A_View)
                       {
                         if (Is_A_Left_Partition)
                            {
                              SerialArray->Array_Descriptor.Array_Domain.Base[j] += ghostBoundaryWidth;
                           // ... bug fix (8/26/96, kdb) User_Base must reflect the ghost cell ...
                              SerialArray->Array_Descriptor.Array_Domain.User_Base[j] += ghostBoundaryWidth;
                            }

                         if (Is_A_Right_Partition)
                              SerialArray->Array_Descriptor.Array_Domain.Bound[j] -= ghostBoundaryWidth;
                       }

                 // Bugfix (10/19/95) if we modify the base and bound in the 
                 // descriptor then the data is no longer contiguous
                 // meaning that it is no longer binary conformable
                    if (ghostBoundaryWidth > 0)
                         SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = FALSE;
                  }
                 else
                  {
                    Generated_A_Null_Array = TRUE;
                  }
             }
            else // j >= Array_Domain.Domain_Dimension
             {
               int Count = (getLocalBound(j)-getLocalBase(j)) + 1;
               if (Count > 0)
                  {
#if 0
                    printf ("BEFORE 2nd RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                 // (6/28/2000) part of fix to permit allocation of strided array data
                 // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,1);
                 // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,Local_Stride);
		    int tempLocalBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Local_Stride;
                    Array_Domain.Local_Mask_Index[j] = Index(tempLocalBase, Count, Local_Stride);

#if 0
                    printf ("AFTER 2nd RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                  }
                 else
                  {
                 // build a null internalIndex for this case
                    Array_Domain.Local_Mask_Index[j] = Index (0,0,1);
                  }
             }

#if 1
       // ... add correct view offset and Scalar Offset ...
          if (j==0)
             {
#if 0
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
                    SerialArray->Array_Descriptor.Array_Domain.Base[0] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[0];
#else
            // bugfix (7/16/2000) The View_Offset is not computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
	            SerialArray->Array_Descriptor.Array_Domain.Base[0];
#endif
            // The Scalar_Offset is computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[0] =
                  - SerialArray->Array_Descriptor.Array_Domain.User_Base[0] *
	            SerialArray->Array_Descriptor.Array_Domain.Stride[0];
             }
            else
             {
#if 0
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
#else
            // The View_Offset is not computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
#endif
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] =
                    SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j-1] -
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
             }
#else
       // ... add correct view offset and Scalar Offset ...
          if (j==0)
             {
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
	            SerialArray->Array_Descriptor.Array_Domain.Base[0];
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[0] =
                  - SerialArray->Array_Descriptor.Array_Domain.User_Base[0];
             }
            else
             {
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
            // Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Size[temp-1];
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] =
                    SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j-1] -
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
             }
#endif
       // Error checking for same strides in parallel and serial array
          APP_ASSERT ( SerialArray->isNullArray() ||
                       (Array_Domain.Stride [j] == SerialArray->Array_Descriptor.Array_Domain.Stride [j]) );

#if 0
          for (int temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               printf ("Check values: SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
                    temp,SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[temp]);
             }
#endif

#if 0
       // New test (7/2/2000)
       // In general this test is not always valid (I think it was a mistake to add it - but
       // we might modify it in the future so I will leave it here for now (commented out))
       // This test is not working where the array on one processor is a NullArray (need to fix this test)
          if (SerialArray->isNullArray() == FALSE)
             {
            // recompute the partition information for this axis
               bool Is_A_Left_Partition   = Array_Domain.isLeftPartition  (j);
               bool Is_A_Right_Partition  = Array_Domain.isRightPartition (j);
               bool Is_A_Middle_Partition = Array_Domain.isMiddlePartition(j);

            // Error checking on left and right edges of the partition
               if (Is_A_Left_Partition)
                  {
                    if ( getRawBase(j) != SerialArray->getRawBase(j) )
                       {
                      // display("ERROR: getRawBase(j) != SerialArray->getRawBase(j)");
		         printf ("getRawBase(%d) = %d  SerialArray->getRawBase(%d) = %d \n",
                              j,getRawBase(j),j,SerialArray->getRawBase(j));
                       }
                    APP_ASSERT( getRawBase(j) == SerialArray->getRawBase(j) );
                  }

               if (Is_A_Right_Partition)
                  {
                    if ( getRawBound(j) != SerialArray->getRawBound(j) )
                       {
                      // display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
                      // SerialArray->Array_Descriptor.display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
		         printf ("getRawBound(%d) = %d  SerialArray->getRawBound(%d) = %d \n",
                              j,getRawBound(j),j,SerialArray->getRawBound(j));
                       }
                    APP_ASSERT( getRawBound(j) == SerialArray->getRawBound(j) );
                  }
             }
#endif
        } // end of j loop


     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
          if (j == 0)
             {
            // Start out true
               SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = TRUE;
#if 0
               printf ("Initial setting: SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = %s \n",
                    SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");
#endif
             }
            else
             {
            // Start out true
               SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = 
                    SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base && 
                    (SerialArray->Array_Descriptor.Array_Domain.Data_Base[j] == 
                     SerialArray->Array_Descriptor.Array_Domain.Data_Base[j-1]);
#if 0
               printf ("axis = %d Constant_Data_Base = %s \n",
                    j,SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");
#endif
             }
        }

#if 0
     printf ("Final value: SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = %s \n",
          SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");

     printf ("SerialArray->Array_Descriptor.Array_Domain.View_Offset = %d \n",
          SerialArray->Array_Descriptor.Array_Domain.View_Offset);
#endif

  // ... add View_Offset to Scalar_Offset (we can't do this inside the
  // loop above because View_Offset is a sum over all dimensions). Also
  // set View_Pointers now. ...
     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
          SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] +=
               SerialArray->Array_Descriptor.Array_Domain.View_Offset;
       // printf ("SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
       //      j,SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j]);
        }

  /* SERIAL_POINTER_LIST_INITIALIZATION_MACRO; */
     DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;

  // if we have generated a null view of a valid serial array then we have 
  // to make the descriptor conform to some specific rules
  // 1. A Null_Array has to have the Is_Contiguous_Data flag FALSE
  // 2. A Null_Array has to have the Base and Bound 0 and -1 (repectively)
  //    for ALL dimensions
  // 3. The Local_Mask_Index in the Parallel Descriptor must be a Null Index

     if (Generated_A_Null_Array == TRUE)
        {
       // We could call a function here to set the domain to represent a Null Array
          SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data   = FALSE;
          SerialArray->Array_Descriptor.Array_Domain.Is_A_View            = TRUE;
          SerialArray->Array_Descriptor.Array_Domain.Is_A_Null_Array      = TRUE;

          SerialArray->Array_Descriptor.Array_Domain.Constant_Unit_Stride = TRUE;

          int j;
          for (j=0; j < MAX_ARRAY_DIMENSION; j++)
             {
               Array_Domain.Local_Mask_Index   [j] = Index (0,0,1,Null_Index);
               SerialArray->Array_Descriptor.Array_Domain.Base     [j] =  0;
               SerialArray->Array_Descriptor.Array_Domain.Bound    [j] = -1;
               SerialArray->Array_Descriptor.Array_Domain.User_Base[j] =  0;
             }
        }
   }


#endif
 

#undef INTARRAY

#define DOUBLEARRAY 
/*``#define'' CLASS_TEMPLATE*/  
/*``#define'' CLASS_TEMPLATE  template<class T``,''int Templated_Dimension>*/

/*``#define'' ARRAY_DESCRIPTOR_TYPE doubleArray_Descriptor_Type*/
/*``#define'' ARRAY_DESCRIPTOR_TYPE Array_Descriptor_Type<T`,'Template_Dimension>*/

#if !defined(PPP)
// *********************************************************
// Hash tables can be used to to avoid redundent allocation
// and deallocation associated with the use of temporaries.
// Temporaries are created and saved for reuse when the same
// size temporary is required again. No smart system for aging
// of hashed memory is used to avoid the acumulation of reserved
// memory for reuse.  Also no measurable advantage of this
// feature has been observed.
// *********************************************************
// doubleArray_Data_Hash_Table doubleArray_Descriptor_Type::Hash_Table;
 doubleArray_Data_Hash_Table doubleArray_Descriptor_Type::Hash_Table;
#endif


// *****************************************************************************
// *****************************************************************************
// *********  NEW OPERATOR INITIALIZATION MEMBER FUNCTION DEFINITION  **********
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

void doubleArray_Descriptor_Type::New_Function_Loop ()
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

//template<class T, int Template_Dimension>

void doubleArray_Descriptor_Type::freeMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of doubleArray_Descriptor_Type::freeMemoryInUse() \n");
#endif 

  // This function is useful in conjuction with the Purify (from Pure Software Inc.)
  // it frees memory allocated for use internally in A++ <type>Array objects.
  // This memory is used internally and is reported as "in use" by Purify
  // if it is not freed up using this function.  This function works with
  // similar functions for each A++ object to free up all of the A++ memory in
  // use internally.

#if !defined(PPP)
  // Bugfix (12/24/97)
  // We don't store the Array_Domain_Type::Array_Reference_Count_Array
  // in the doubleArray_Descriptor_Type so we can't free it here
  // this is taken care of in the Array_Domain_Type::freeMemoryInUse()
  // member function!
  // free memory allocated for reference counting!
  // if (Array_Domain_Type::Array_Reference_Count_Array != NULL)
  //      free ((char*) Array_Domain_Type::Array_Reference_Count_Array);
#endif

  // free memory allocated for memory pools!
     for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++) 
          if (Memory_Block_List [i] != NULL)  
               free ((char*) (Memory_Block_List[i]));
   }

// *****************************************************************************
// *****************************************************************************
// ****************  INITIALIZATION SPECIFIC TO PARALLEL DATA  *****************
// *****************************************************************************
// *****************************************************************************

#if 0
   // I don't think this is called!
#if defined(PPP)
//template<class T, int Template_Dimension>

void doubleArray_Descriptor_Type::
Initialize_Parallel_Parts_Of_Descriptor ( const doubleArray_Descriptor_Type & X )
   {
     Array_Domain.Initialize_Parallel_Parts_Of_Domain (X.Array_Domain);
   }
#endif
#endif

// *****************************************************************************
// *****************************************************************************
// **************  BASE SPECIFICATION MEMBER FUNCTION DEFINITION  **************
// *****************************************************************************
// *****************************************************************************


void
doubleArray_Descriptor_Type::setBase( int New_Base_For_All_Axes )
   {
  // NEW VERSION OF SETBASE
#if COMPILE_DEBUG_STATEMENTS
  // Enforce bound on Alt_Base depending on INT_MAX and INT_MIN
     static int Max_Base = INT_MAX;
     static int Min_Base = INT_MIN;

  // error checking!
     APP_ASSERT((New_Base_For_All_Axes > Min_Base) && (New_Base_For_All_Axes < Max_Base));
#endif

     int temp       = 0;  // iteration variable
     int Difference = 0;

#if defined(PPP)
  // APP_ASSERT(SerialArray != NULL);

  // Adjust the Data_Base on the LOCAL SerialArray relative to the change in the
  // GLOBAL base
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          Difference = New_Base_For_All_Axes - (Array_Domain.Data_Base[temp] +
                                                Array_Domain.Base[temp]);

          if (SerialArray != NULL)
             {
            // printf ("SHOULDN'T WE USE SerialArray->getBase(temp) instead of getBase(temp) \n");
	    /*
	    // ... (5/18/98, kdb) bug fix, getRawBase now corresponds to setBase
	    //   (getBase gets User_Base instead of Data_Base + Base) ...
	    */
               int New_Serial_Base = SerialArray->getRawBase(temp) + Difference;
            // int New_Serial_Base = SerialArray->getBase(temp) + Difference;
            // int New_Serial_Base = getBase(temp) + Difference;
            // printf ("New_Serial_Base = %d \n",New_Serial_Base);
               SerialArray->setBase (New_Serial_Base,temp);
             }
        }
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
#endif

     Array_Domain.setBase(New_Base_For_All_Axes);

  // Now initialize the View pointers (located in the 
  // descriptor -- since they are typed pointers)
#if defined(PPP)
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
     if (SerialArray != NULL)
        {
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
        }
#else
  // POINTER_LIST_INITIALIZATION_MACRO;
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
#endif
   }


void
doubleArray_Descriptor_Type::setBase( int New_Base, int Axis )
   {
#if defined(PPP)
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
     if (SerialArray != NULL)
        {
       // Call setBase on the SerialArray also
          int Difference = New_Base - (Array_Domain.Data_Base[Axis] + Array_Domain.Base[Axis]);

          int New_Serial_Base = SerialArray->getBase(Axis) + Difference;
          SerialArray->setBase (New_Serial_Base,Axis);

          Array_Domain.setBase (New_Base,Axis);

       // Now initialize the View pointers (located in the 
       // descriptor -- since they are typed pointers)
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
        }
#else
     Array_Domain.setBase (New_Base,Axis);

  // Now initialize the View pointers (located in the 
  // descriptor -- since they are typed pointers)
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
#endif
   }
//====================================================================

// *****************************************************************************
// *****************************************************************************
// ********************  DISPLAY MEMBER FUNCTION DEFINITION  *******************
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

void
doubleArray_Descriptor_Type::display(const char *Label ) const
   {
     int i=0;
     printf ("Array_Descriptor_Type::display() -- %s \n",Label);

     Array_Domain.display(Label);

#if defined(PPP)
     printf ("getLocalBase (output of getLocalBase member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalBase(i));
     printf ("\n");

     printf ("getLocalBound (output of getLocalBound member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalBound(i));
     printf ("\n");

     printf ("getLocalStride (output of getLocalStride member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalStride(i));
     printf ("\n");
#endif

#if !defined(PPP)
     printf ("Array_Data                    = %p \n",Array_Data);
#endif
     printf ("ExpressionTemplateDataPointer = %p \n",ExpressionTemplateDataPointer);

#if !defined(PPP)
        printf ("Array_View_Pointer0 = %p (Array_Data-Array_View_Pointer0) = %d \n",
             Array_View_Pointer0,(Array_Data-Array_View_Pointer0));
#if MAX_ARRAY_DIMENSION>1
        printf ("Array_View_Pointer1 = %p (Array_View_Pointer0-Array_View_Pointer1) = %d \n",
             Array_View_Pointer1,(Array_View_Pointer0-Array_View_Pointer1));
#endif
#if MAX_ARRAY_DIMENSION>2
        printf ("Array_View_Pointer2 = %p (Array_View_Pointer1-Array_View_Pointer2) = %d \n",
             Array_View_Pointer2,(Array_View_Pointer1-Array_View_Pointer2));
#endif
#if MAX_ARRAY_DIMENSION>3
        printf ("Array_View_Pointer3 = %p (Array_View_Pointer2-Array_View_Pointer3) = %d \n",
             Array_View_Pointer3,(Array_View_Pointer2-Array_View_Pointer3));
#endif
#if MAX_ARRAY_DIMENSION>4
        printf ("Array_View_Pointer4 = %p (Array_View_Pointer3-Array_View_Pointer4) = %d \n",
             Array_View_Pointer4,(Array_View_Pointer3-Array_View_Pointer4));
#endif
#if MAX_ARRAY_DIMENSION>5
        printf ("Array_View_Pointer5 = %p (Array_View_Pointer4-Array_View_Pointer5) = %d \n",
             Array_View_Pointer5,(Array_View_Pointer4-Array_View_Pointer5));
#endif
#if MAX_ARRAY_DIMENSION>6
        printf ("Array_View_Pointer6 = %p (Array_View_Pointer5-Array_View_Pointer6) = %d \n",
             Array_View_Pointer6,(Array_View_Pointer5-Array_View_Pointer6));
#endif
#if MAX_ARRAY_DIMENSION>7
        printf ("Array_View_Pointer7 = %p (Array_View_Pointer6-Array_View_Pointer7) = %d \n",
             Array_View_Pointer7,(Array_View_Pointer6-Array_View_Pointer7));
#endif
#endif

  // The only additional thing to do is print out the values of the pointers 
  // in this class since that is the only difference between a descriptor and a domain.
   }

// *****************************************************************************
// *****************************************************************************
// **************************  DESTRUCTOR DEFINITIONS  *************************
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

doubleArray_Descriptor_Type::~doubleArray_Descriptor_Type ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Descriptor_Type::destructor! %p Array_ID = %d referenceCount = %d \n",
               this,Array_Domain.Array_ID(),referenceCount);
#endif

#if 0
#if defined(PPP)
     printf ("Why can't we assert that SerialArray == NULL (SerialArray = %s) in the ~doubleArray_Descriptor_Type()? \n",
          (SerialArray == NULL) ? "NULL" : "VALID POINTER");
#else
     printf ("Why can't we assert that Array_Data == NULL (Array_Data = %s) in the ~doubleArray_Descriptor_Type()? \n",
          (Array_Data == NULL) ? "NULL" : "VALID POINTER");
#endif
#endif

#if defined(USE_PADRE) && defined(PPP)
     if (Array_Domain.parallelPADRE_DescriptorPointer != NULL)
        {
       // printf ("In doubleArray_Descriptor_Type::destructor(): delete the PADRE_Descriptor \n");
          Array_Domain.parallelPADRE_DescriptorPointer->decrementReferenceCount();
       // printf ("Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount() = %d \n",
       //      Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount());
          if (Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount() < 
              Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCountBase())
               delete Array_Domain.parallelPADRE_DescriptorPointer;
          Array_Domain.parallelPADRE_DescriptorPointer = NULL;
        }
#endif

  // Reset reference count!  This should always be the value of getReferenceCountBase() anyway
     referenceCount = getReferenceCountBase();

  // printf ("In ~doubleArray_Descriptor_Type Array_ID on the stack is %d \n",SerialArray_Domain_Type::queryNextArrayID());

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Leaving ~Array_Descriptor_Type() -- referenceCount = %d \n",referenceCount);
#endif
   }

// *****************************************************************************
// *****************************************************************************
// *******************  CONSTRUCTOR INITIALIZATION SUPPORT  ********************
// *****************************************************************************
// *****************************************************************************


void
doubleArray_Descriptor_Type::Preinitialize_Descriptor()
   {
  // At this point (when this function is called) the array data HAS NOT been allocated!

  // Set reference count!
     referenceCount = getReferenceCountBase();

  // We set these to NULL to avoid UMR (in purify) -- but these are RESET
  // after the pointer to the array data when it is known.
#if defined(PPP)
     doubleSerialArray *Data_Pointer = SerialArray;
     if (Data_Pointer == NULL)
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
        }
       else
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO
        }
#else
     double *Data_Pointer = Array_Data;
     if (Data_Pointer == NULL)
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
        }
       else
        {
          DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
        }
#endif

#if COMPILE_DEBUG_STATEMENTS
  // Test_Consistency ("Called from doubleArray_Descriptor_Type::Preinitialize_Descriptor()");
  // printf ("In Preinitialize_Descriptor(): Commented out Test_Consistency since not enough data is available at this stage! \n");
#endif
   }

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void doubleArray_Descriptor_Type::
Initialize_Descriptor(
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List,
     const Internal_Partitioning_Type* Internal_Partition )
   {
  // This function is required because sometimes the descriptor must be reinitialized
  // (this happens in the dynamic re-dimensioning of the array object for example).
  // This function is also called from within the array class adopt function.
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of doubleArray_Descriptor_Type::Initialize_Descriptor(Number_Of_Valid_Dimensions=%d,int*) ",
               Number_Of_Valid_Dimensions);
        }
#endif

  // printf ("BEFORE Preinitialize_Descriptor() getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // APP_ASSERT (Array_Data == NULL);
  // setDataPointer(NULL);
  // resetRawDataReferenceCount();

  // Bugfix (11/6/2000) Move preinitialixation of the descriptor to after the initialization of the domain.
  // Preinitialize_Descriptor();

#if 0
  // We cannot assert this here since we want the Array_Domain.Initialize_Domain
  // to delete the old partitioning objects if they exist

#if defined(PPP) && !defined(USE_PADRE)
#if COMPILE_DEBUG_STATEMENTS
     if (Array_Domain.BlockPartiArrayDomain        != NULL)
          printf ("WARNING: In Initialize_Descriptor --- Array_Domain.BlockPartiArrayDomain        != NULL \n");

     if (Array_Domain.BlockPartiArrayDecomposition        != NULL)
          printf ("WARNING: In Initialize_Descriptor --- Array_Domain.BlockPartiArrayDecomposition != NULL \n");
#endif

     APP_ASSERT (Array_Domain.BlockPartiArrayDomain        == NULL);
     APP_ASSERT (Array_Domain.BlockPartiArrayDecomposition == NULL);
#endif
#endif

  // This appears to be the second initialization of the Array_Domain object!!!
  // We need to find out where this is called and see that the correct Array_Domain
  // constructor is called instead.  Upon investigation this "Initialization_Descriptor()"
  // function is not called by any of the Array_Descriptor constructors.
#if defined(PPP)
     if (Internal_Partition != NULL)
        {
          Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List,*Internal_Partition);
        }
       else
        {
          Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
        }
#else
  // Avoid compiler warning about unused input variable
     if (&Internal_Partition);

     Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
#endif

  // Bugfix (11/6/2000) Move preinitialixation of the descriptor to after the initialization of the domain.
     Preinitialize_Descriptor();

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
  // For P++ we only track the serial array objects not the serial array objects
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[Array_ID()] != NULL);
#ifdef DOUBLEARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_DOUBLE_ELEMENT_TYPE);
#endif
#ifdef FLOATARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_FLOAT_ELEMENT_TYPE);
#endif
#ifdef INTARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_INT_ELEMENT_TYPE);
#endif
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[Array_ID()] != NULL);
        }
#endif
   }

// Fixup_Descriptor_With_View( 
//     int Number_Of_Valid_Dimensions ,
//     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )

void
doubleArray_Descriptor_Type::reshape( 
   int Number_Of_Valid_Dimensions ,
   const int* View_Sizes, const int* View_Bases)
   {
  // This function is only called by the array object's reshape member function
     Array_Domain.reshape (Number_Of_Valid_Dimensions, View_Sizes, View_Bases);
   }


// *****************************************************************************
// *****************************************************************************
// *************************  CONSTRUCTORS DEFINITIONS  ************************
// *****************************************************************************
// *****************************************************************************

//template<class T , int Template_Dimension>

doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type() 
   : Array_Domain()
   {
  // This builds the NULL Array_Descriptor (used in NULL arrays)!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of doubleArray_Descriptor_Type::Array_Descriptor_Type() this = %p \n",this);
       // printf ("WARNING: It is a ERROR to use this descriptor constructor except for NULL arrays! \n");
       // APP_ABORT();
        }
#endif

     setDataPointer(NULL);
#if !defined(PPP)
     resetRawDataReferenceCount();
#endif
  // printf ("In doubleArray_Descriptor_Type::doubleArray_Descriptor_Type() -- getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
     Preinitialize_Descriptor();

#if 0
  // Set reference count!
     referenceCount = getReferenceCountBase();

#if !defined(PPP)
     Array_Data = NULL;
  // DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
#else
     SerialArray = NULL;
#endif
  // We want to use this macro with both A++ and P++
  // ExpressionTemplateDataPointer = NULL;
     DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
#endif

#if COMPILE_DEBUG_STATEMENTS
  // Can't run these tests in a NULL array
  // Test_Consistency ("Called from default constructor!");
#endif
   }

doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( const Internal_Index & I )
   : Array_Domain(I)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if (MAX_ARRAY_DIMENSION >= 2)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J )
   : Array_Domain(I,J)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K )
   : Array_Domain(I,J,K)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 4)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L )
   : Array_Domain(I,J,K,L)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M )
   : Array_Domain(I,J,K,L,M)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N )
   : Array_Domain(I,J,K,L,M,N)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 7)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O )
   : Array_Domain(I,J,K,L,M,N,O)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 8)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O, const Internal_Index & P )
   : Array_Domain(I,J,K,L,M,N,O,P)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

//template<class T , int Template_Dimension>

doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   : Array_Domain (Number_Of_Valid_Dimensions,Integer_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
#if 0
  // Set reference count!
     referenceCount = getReferenceCountBase();
  // Initialize_Descriptor (Number_Of_Valid_Dimensions,Integer_List);
#endif
   }

#if defined(APP) || defined(PPP)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if (MAX_ARRAY_DIMENSION >= 2)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 4)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 7)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,O,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 8)
doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O, const Internal_Index & P,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,O,P,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif
#endif
 
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif


doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List )
   : Array_Domain (Number_Of_Valid_Dimensions,Internal_Index_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }


doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type( ARGUMENT_LIST_MACRO_INTEGER )
   : Array_Domain (VARIABLE_LIST_MACRO_INTEGER)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if 0
//template<class T , int Template_Dimension>

doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type(
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   : Array_Domain(MAX_ARRAY_DIMENSION,Integer_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if defined(PPP) || defined(APP)

doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
     const Internal_Partitioning_Type & Internal_Partition )
   : Array_Domain (Number_Of_Valid_Dimensions,Internal_Index_List,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if defined(PPP)
// doubleArray_Descriptor_Type::doubleArray_Descriptor_Type(
//           int Input_Array_Size_I , int Input_Array_Size_J ,
//           int Input_Array_Size_K , int Input_Array_Size_L ,
//           const Partitioning_Type & Partition )

doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List , 
     const Internal_Partitioning_Type & Internal_Partition )
   : Array_Domain(Number_Of_Valid_Dimensions,Integer_List,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }


doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const double* Data_Pointer,
     ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE )
   : Array_Domain(VARIABLE_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE)
   {
  // Avoid compiler warning about unused input variable
     if (&Data_Pointer);

     printf ("ERROR: Parallel doubleArray_Descriptor_Type::doubleArray_Descriptor_Type (const double* Data_Pointer,ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE) --- not implemented! \n");
     APP_ABORT();

  // We can't set the double pointer here  This must be done at a later stage
  // setDataPointer (Data_Pointer);
  // incrementRawDataReferenceCount();
     setDataPointer (NULL);
     Preinitialize_Descriptor();
   }
#else // end of PPP


doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( 
     const double* Data_Pointer, 
     ARGUMENT_LIST_MACRO_CONST_REF_RANGE )
   : Array_Domain(VARIABLE_LIST_MACRO_CONST_REF_RANGE)
   {
  // Mark this object as getting it's data from an external source
     Array_Domain.builtUsingExistingData = TRUE;

#if !defined(PPP)
  // Array_Data = Data_Pointer;
  // Cast away const here to preserve use of const data pointer
  // when building array object using a pointer to existing data
     setDataPointer ((double*)Data_Pointer);
     incrementRawDataReferenceCount();

     APP_ASSERT (Array_Domain.builtUsingExistingData == TRUE);
#else
     SerialArray = NULL;
     printf ("ERROR: doubleArray_Descriptor_Type::doubleArray_Descriptor_Type (const double* Data_Pointer,ARGUMENT_LIST_MACRO_CONST_REF_RANGE) should not be defined for P++ \n");
     APP_ABORT();
#endif
     Preinitialize_Descriptor();
   }
#endif // end of else PPP


doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type(
     const Array_Domain_Type & X,
     bool AvoidBuildingIndirectAddressingView )
  // We call the Array_Domain copy constructor to do the preinitialization
   : Array_Domain(X,AvoidBuildingIndirectAddressingView)
   {
  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     
#if !defined(PPP)
  // Since this is a new array object is should have an initialize reference count on its
  // raw data.  This is required here because the reference counting mechanism reused the
  // value of zero for one existing reference and no references (this will be fixed soon).
     resetRawDataReferenceCount();
#endif

     Preinitialize_Descriptor();

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from doubleArray_Descriptor_Type (Array_Domain_Type) \n");
#endif
   }

// ***************************************************************************************
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

// doubleArray_Descriptor_Type::doubleArray_Descriptor_Type 
//    ( const doubleArray_Descriptor_Type & X , 
//      const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
// template<class T , int Template_Dimension>

doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type (
     const Array_Domain_Type & X ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
   : Array_Domain (X,Index_List)
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
   {
      printf ("Inside of doubleArray_Descriptor_Type::doubleArray_Descriptor_Type ");
      printf ("( const Array_Domain_Type & X , Internal_Index** Index_List )");
      printf ("(this = %p)\n",this);
   }
#endif

  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     Preinitialize_Descriptor();

     APP_ASSERT(Array_ID() == X.Array_ID());

#if COMPILE_DEBUG_STATEMENTS
   Test_Consistency ("Called from doubleArray_Descriptor_Type (Array_Domain_Type,Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type*) \n");
#endif
}

// ************************************************************************
// doubleArray_Descriptor_Type constructors for indirect addressing
// ************************************************************************
// doubleArray_Descriptor_Type ( const doubleArray_Descriptor_Type & X , 
//      const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
//template<class T , int Template_Dimension>

doubleArray_Descriptor_Type::
doubleArray_Descriptor_Type ( const Array_Domain_Type & X , 
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
   : Array_Domain(X,Indirect_Index_List)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of doubleArray_Descriptor_Type::doubleArray_Descriptor_Type ( const Array_Domain_Type & X , Internal_Indirect_Addressing_Index* Indirect_Index_List ) (this = %p)\n",this);
#endif

  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     Preinitialize_Descriptor();

     APP_ASSERT(Array_ID() == X.Array_ID());
   }


#ifdef USE_STRING_SPECIFIC_CODE

// ************************************************************************
// doubleArray_Descriptor_Type constructors for initialization using a string
// ************************************************************************
// doubleArray_Descriptor_Type::doubleArray_Descriptor_Type ( const char* dataString )
doubleArray_Descriptor_Type::doubleArray_Descriptor_Type ( const AppString & dataString )
   : Array_Domain(dataString)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

// *****************************************************************************
// *****************************************************************************
// ***********************  MEMBER FUNCTION DEFINITIONS  ***********************
// *****************************************************************************
// *****************************************************************************

// if defined(APP) || defined(PPP)

int
doubleArray_Descriptor_Type::getGhostBoundaryWidth ( int Axis ) const
   {
     int width = Array_Domain.getGhostBoundaryWidth(Axis);
  // APP_ASSERT (width == Array_Domain.InternalGhostCellWidth[Axis]);
     return width;
   }
// endif


void doubleArray_Descriptor_Type::setInternalGhostCellWidth ( 
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   {
     Array_Domain.setInternalGhostCellWidth(Integer_List);
   }

#if defined(PPP) || defined(APP)

void doubleArray_Descriptor_Type::
partition ( const Internal_Partitioning_Type & Internal_Partition )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::partition ( const Internal_Partitioning_Type & Internal_Partition ) \n");
#endif

#if defined(PPP) 
  // ... only actually partion for P++ ...
     Array_Domain.partition (Internal_Partition);
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Leaving doubleArray_Descriptor_Type::partition \n");
#endif
   }
#endif // end of PPP or APP

#if defined(PPP) 
#if defined(USE_PADRE)
  // What PADRE function should we call here?
#else

DARRAY* doubleArray_Descriptor_Type::
Build_BlockPartiArrayDomain ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::Build_BlockPartiArrayDomain \n");
#endif

     return Array_Domain.Build_BlockPartiArrayDomain();
   }
#endif // End of USE_PADRE not defined
#endif

// **********************************************************************************
//                           COPY CONSTRUCTOR
// **********************************************************************************

doubleArray_Descriptor_Type::doubleArray_Descriptor_Type ( const doubleArray_Descriptor_Type & X, int Type_Of_Copy )
   : Array_Domain(X.Array_Domain,FALSE,Type_Of_Copy)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::doubleArray_Descriptor_Type (const doubleArray_Descriptor_Type & X,bool, int) \n");
#endif

  // This may cause data to be set to null initial values and later reset
  // Works with P++ (maybe not A++)
     setDataPointer(NULL);
     Preinitialize_Descriptor();

#if !defined(PPP)
#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from doubleArray_Descriptor_Type COPY CONSTRUCTOR");
#endif
#endif
   }

// **********************************************************************************
//                           EQUALS OPERATOR
// **********************************************************************************
#if 0
// This is a more general template function but it does not work with many compilers!
//template<class T , int Template_Dimension>

//template<class S , int SecondDimension>

// doubleArray_Descriptor_Type & doubleArray_Descriptor_Type::
// operator=( const doubleArray_Descriptor_Type & X )
doubleArray_Descriptor_Type & doubleArray_Descriptor_Type::
operator=( const doubleArray_Descriptor_Type<S,SecondDimension> & X )
   {
  // The funtion body is the same as that of the operator= below!
     return *this;
   }
#endif

// operator=( const doubleArray_Descriptor_Type & X )
//template<class T , int Template_Dimension>

doubleArray_Descriptor_Type & doubleArray_Descriptor_Type::
operator=( const doubleArray_Descriptor_Type & X )
   {
  // At present this function is only called in the optimization of assignment and temporary use
  // in lazy_statement.C!  NOT TRUE! The Expression template option force frequent calls to this function!
#if COMPILE_DEBUG_STATEMENTS
#if !defined(PPP)
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type<T,int>::operator= (const doubleArray_Descriptor_Type<T,int> & X) Array_Data = %p \n",Array_Data);
#endif
#endif

  // Set reference count!
     referenceCount = getReferenceCountBase();

  // For now we don't set any of the pointers in the Array_Descriptor functions
  // these are set in the array class member functions (we will fix this later)
     Array_Domain = X.Array_Domain;

  // Copy Pointers
  // We must be careful about mixing pointer types
  // Array_Data = X.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from doubleArray_Descriptor_Type::operator=");
#endif

#if COMPILE_DEBUG_STATEMENTS
#if !defined(PPP)
     if (APP_DEBUG > 0)
          printf ("Leaving doubleArray_Descriptor_Type::operator= (const doubleArray_Descriptor_Type & X) Array_Data = %p \n",Array_Data);
#endif
#endif
     return *this;
   }

#if 0
// Should be inlined since it is used in a lot of places within A++!
//template<class T , int Template_Dimension>

int doubleArray_Descriptor_Type::Array_Size () const
   {
     return Array_Domain.Array_Size();
   }
#endif

#if defined(PPP)
//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::isLeftPartition( int Axis ) const
   {
     return Array_Domain.isLeftPartition(Axis);
   }

//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::isMiddlePartition( int Axis ) const
   {
     return Array_Domain.isMiddlePartition(Axis);
   }

//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::isRightPartition( int Axis ) const
   {
     return Array_Domain.isRightPartition(Axis);
   }

//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::isNonPartition( int Axis ) const
   {
     return Array_Domain.isNonPartition(Axis);
   }

#if defined(PPP)

bool
doubleArray_Descriptor_Type::isLeftNullArray( int Axis ) const
   {
  // A Null array can be part of the left edge or the right edge of a distributed axis
  // This function is helpful in determining which it is.

     int result = FALSE;

#if 1
     APP_ASSERT (SerialArray != NULL);
     result = Array_Domain.isLeftNullArray(SerialArray->Array_Descriptor.Array_Domain,Axis);
#else
#if defined(USE_PADRE)
     APP_ASSERT (Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     result = Array_Domain.parallelPADRE_DescriptorPointer->isLeftPartition(Axis);
#else
  // printf ("In isLeftNullArray(): Axis = %d \n",Axis);

     APP_ASSERT (SerialArray != NULL);

     if ( SerialArray->isNullArray() == TRUE )
        {
       // printf ("In doubleArray_Descriptor_Type::isLeftNullArray(axis): Domain_Dimension = %d \n",Array_Domain.Domain_Dimension);
          if ( Axis < Array_Domain.Domain_Dimension)
             {
               APP_ASSERT(Array_Domain.BlockPartiArrayDomain != NULL);
            // printf ("     gUBnd(BlockPartiArrayDomain,Axis) = %d \n",gUBnd(Array_Domain.BlockPartiArrayDomain,Axis));
            // printf ("     lalbnd(BlockPartiArrayDomain,Axis,Base[Axis],1) = %d \n",lalbnd(Array_Domain.BlockPartiArrayDomain,Axis,Array_Domain.Base[Axis],1));

               result = (gUBnd(Array_Domain.BlockPartiArrayDomain,Axis) < lalbnd(Array_Domain.BlockPartiArrayDomain,Axis,Array_Domain.Base[Axis],1));
             }
            else
             {
            // Along higher axes we want to force isLeftPartition == TRUE
               result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif
#endif

     return result;
   }


bool
doubleArray_Descriptor_Type::isRightNullArray( int Axis ) const
   {
  // A Null array can be part of the left edge or the right edge of a distributed axis
  // This function is helpful in determining which it is.

     int result = FALSE;

#if 1
     APP_ASSERT (SerialArray != NULL);
     result = Array_Domain.isLeftNullArray(SerialArray->Array_Descriptor.Array_Domain,Axis);
#else
#if defined(USE_PADRE)
     APP_ASSERT (Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     result = Array_Domain.parallelPADRE_DescriptorPointer->isLeftPartition(Axis);
#else
  // printf ("In isRightNullArray(): Axis = %d \n",Axis);
     if ( isNullArray() == TRUE )
        {
          if ( Axis < Array_Domain.Domain_Dimension)
             {
               result = !isLeftNullArray(Axis);
             }
            else
             {
            // Along higher axes we want to force isLeftPartition == TRUE
               result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif
#endif

     return result;
   }
#endif

//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int doubleArray_Descriptor_Type::findProcNum ( int* indexVals ) const
   {
  // Find number of processor where indexVals lives
     return Array_Domain.findProcNum(indexVals);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>
#if 0

int* doubleArray_Descriptor_Type::setupProcessorList
   (const intSerialArray& I, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorList
                 (I,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,
                  localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
doubleArray_Descriptor_Type::
setupProcessorList(
    int* I_A, int list_size, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorList
                 (I_A,list_size,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
doubleArray_Descriptor_Type::
setupProcessorListOnPosition(
     const intSerialArray& I, int& numberOfSupplements, 
     int** supplementLocationsPtr, int** supplementDataPtr, 
     int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorListOnPosition
                  (I,numberOfSupplements,supplementLocationsPtr,
                   supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
doubleArray_Descriptor_Type::
setupProcessorListOnPosition(
    int* I_A, int list_size, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorListOnPosition
                 (I_A,list_size,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//end of setupProcessorList section that is turned off 
#endif
//---------------------------------------------------------------------
#endif

//---------------------------------------------------------------------
#if 0
//template<class T , int Template_Dimension>

int doubleArray_Descriptor_Type::getRawDataSize( int Axis ) const
   {
     return Array_Domain.getRawDataSize(Axis);
   }

//template<class T , int Template_Dimension>

void doubleArray_Descriptor_Type::getRawDataSize( int* Data_Size ) const
   {
     Array_Domain.getRawDataSize(Data_Size);
   }

//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::
isSameBase ( const doubleArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameBase(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::
isSameBound ( const doubleArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameBound(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::
isSameStride ( const doubleArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameStride(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::
isSameLength ( const doubleArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameLength(X.Array_Domain);
   }
#endif

#if defined(PPP)
//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::
isSameGhostBoundaryWidth ( const doubleArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameGhostBoundaryWidth(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::
isSameDistribution ( const doubleArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameDistribution(X.Array_Domain);
   }
#endif

#if 0
//template<class T , int Template_Dimension>

bool doubleArray_Descriptor_Type::
isSimilar ( const doubleArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSimilar(X.Array_Domain);
   }
#endif

#if 0
// I think this is commented out because it is not required!
//template<class T , int Template_Dimension>

int doubleArray_Descriptor_Type::
computeArrayDimension ( ARGUMENT_LIST_MACRO_INTEGER )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of doubleArray_Descriptor_Type::computeArrayDimension (%d,%d,%d,%d) \n",
               VARIABLE_LIST_MACRO_INTEGER );
#endif

     APP_ASSERT(Array_Size_L >= 0);
     APP_ASSERT(Array_Size_K >= 0);
     APP_ASSERT(Array_Size_J >= 0);
     APP_ASSERT(Array_Size_I >= 0);

     APP_ASSERT(i >= 0);
     APP_ASSERT(j >= 0);
     APP_ASSERT(k >= 0);
     APP_ASSERT(l >= 0);
#if MAX_ARRAY_DIMENSION > 4
     APP_ASSERT(m >= 0);
#if MAX_ARRAY_DIMENSION > 5
     APP_ASSERT(n >= 0);
#if MAX_ARRAY_DIMENSION > 6
     APP_ASSERT(o >= 0);
#if MAX_ARRAY_DIMENSION > 7
     APP_ASSERT(p >= 0);
#endif
#endif
#endif
#endif

     int Array_Dimension = 0;
     if ( i > 0 ) Array_Dimension = 1;
     if ( j > 1 ) Array_Dimension = 2;
     if ( k > 1 ) Array_Dimension = 3;
     if ( l > 1 ) Array_Dimension = 4;
#if MAX_ARRAY_DIMENSION > 4
     if ( m > 1 ) Array_Dimension = 5;
#if MAX_ARRAY_DIMENSION > 5
     if ( n > 1 ) Array_Dimension = 6;
#if MAX_ARRAY_DIMENSION > 6
     if ( o > 1 ) Array_Dimension = 7;
#if MAX_ARRAY_DIMENSION > 7
     if ( o > 1 ) Array_Dimension = 8;
#if MAX_ARRAY_DIMENSION > 7
     APP_ABORT();
#endif
#endif
#endif
#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of doubleArray_Descriptor_Type::computeArrayDimension (%d,%d,%d,%d) -- Array_Dimension = %d \n",
               Array_Size_I,Array_Size_J,Array_Size_K,Array_Size_L,Array_Dimension);
#endif

     return Array_Dimension;
   }
#endif

//template<class T , int Template_Dimension>

int doubleArray_Descriptor_Type::
computeArrayDimension ( int* Array_Sizes )
   {
     return Array_Domain_Type::computeArrayDimension(Array_Sizes);
   }

//template<class T , int Template_Dimension>

int doubleArray_Descriptor_Type::
computeArrayDimension ( const doubleArray_Descriptor_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of doubleArray_Descriptor_Type::computeArrayDimension () \n");
#endif

     return Array_Domain_Type::computeArrayDimension(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

void doubleArray_Descriptor_Type::
Build_Temporary_By_Example ( const doubleArray_Descriptor_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::Build_Temporary_By_Example(doubleArray_Descriptor_Type) \n");
     X.Test_Consistency ("Test INPUT: Called from doubleArray_Descriptor_Type::Build_Temporary_By_Example");
#endif

     int Original_Array_ID = Array_ID();
     int Example_Array_ID  = X.Array_ID();
     (*this) = X;

  // Restore the original array id
     Array_Domain.setArray_ID (Original_Array_ID);

  // Make sure we return a temporary (so set it so)
     setTemporary(TRUE);

  // ... (4/23/98, kdb) can't reset base here if using indirect addressing
  //  because i n d e x ing intArrays will be incorrect relative to Data_Base ...
     if (!Array_Domain.Uses_Indirect_Addressing)
          setBase(APP_Global_Array_Base);

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency
          ("Test OUTPUT: Called from doubleArray_Descriptor_Type::Build_Temporary_By_Example(doubleArray_Descriptor_Type)");
#endif
   }

#if defined(PPP)
//-------------------------------------------------------------------------
// This function is specific for use with P++ indirect addressing!
//template<class T , int Template_Dimension>

void doubleArray_Descriptor_Type::
Compute_Local_Index_Arrays ( const doubleArray_Descriptor_Type & Original_Descriptor )
   {

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
     {   
        printf ("Inside of doubleArray_Descriptor_Type::Compute_Local_Index_Arrays()");
        printf (" (Array_ID = %d)\n",Array_ID());
     }   
#endif

     Array_Domain.Compute_Local_Index_Arrays(Original_Descriptor.Array_Domain);
   }   
//-------------------------------------------------------------------------
#if 0
// This function is specific for use with P++ indirect addressing!
//template<class T , int Template_Dimension>

void doubleArray_Descriptor_Type::
Compute_Local_Index_Arrays ( const doubleArray_Descriptor_Type & Original_Descriptor )
   {
     int i; // Index value used below

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::Compute_Local_Index_Arrays() (Array_ID = %d)\n",Array_ID());
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
          printf ("Inside of doubleArray_Descriptor_Type::Compute_Local_Index_Arrays() -- Caching of local intArray usage not implemented yet! \n");
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
       // Indirect_Addressing_Cache_Descriptor *Cache_Descriptor = Indirect_Addressing_Cache.put (X);
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
               Replicated_Index_intArray[i] = Index_Array[i]->SerialArray;
               Index_Array[i]->SerialArray->incrementReferenceCount();
            // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");
               APP_ASSERT(Replicated_Index_intArray[i]->Array_Descriptor->Is_A_Temporary == FALSE);
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
               int Local_Base  = Original_Descriptor.Data_Base[i] + Original_Descriptor.Base[i];
               int Local_Bound = Original_Descriptor.Data_Base[i] + Original_Descriptor.Bound[i];

            // printf ("Local_Base = %d  Local_Bound = %d \n",Local_Base,Local_Bound);
               if (Valid_Local_Entries_Mask.Array_Descriptor->Is_A_Null_Array)
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
                 // display("THIS Descriptor");
                 // Original_Descriptor.display("ORIGINAL Descriptor");
                 // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");
                    Valid_Local_Entries_Mask.view("Valid_Local_Entries_Mask");
                    APP_ABORT();
                  }
             }
        }

  // Valid_Local_Entries_Mask.display("Inside of doubleArray_Descriptor_Type::Compute_Local_Index_Arrays -- Valid_Entries_Mask");

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
               if ( sum ( (*Index_Array[i]->SerialArray) != (*Local_Index_Array[i]) ) != 0 )
                  {
                    printf ("ERROR: On a single processor the Local_Index_Array should match the Index_Array[i]->SerialArray \n");
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
//template<class T , int Template_Dimension>

void doubleArray_Descriptor_Type::
Update_Parallel_Information_Using_Old_Descriptor ( const doubleArray_Descriptor_Type & Old_Array_Descriptor )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of doubleArray_Descriptor_Type::Update_Parallel_Information_Using_Old_Descriptor! \n");
#endif
  // Copy relevant data from old descriptor (this should be a function)

     Array_Domain.Update_Parallel_Information_Using_Old_Descriptor(Old_Array_Descriptor.Array_Domain);
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
//template<class T , int Template_Dimension>

void
doubleArray_Descriptor_Type::Test_Preliminary_Consistency( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since doubleArray_Descriptor_Type::Test_Preliminary_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of doubleArray_Descriptor_Type::Test_Preliminary_Consistency! (Label = %s) (Descriptor_Dimension = %d) \n",
               Label,numberOfDimensions());
#endif

     Array_Domain.Test_Preliminary_Consistency(Label);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Leaving doubleArray_Descriptor_Type::Test_Preliminary_Consistency! (Label = %s) \n",Label);
#endif
   }

//template<class T , int Template_Dimension>

void
doubleArray_Descriptor_Type::Test_Consistency( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since doubleArray_Descriptor_Type::Test_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of doubleArray_Descriptor_Type::Test_Consistency! (Label = %s) (Descriptor_Dimension = %d) \n",
               Label,numberOfDimensions());
#endif

#if defined(PPP)
     if (SerialArray != NULL)
        {
          if (getRawDataReferenceCount() < getRawDataReferenceCountBase())
               printf ("In doubleArray_Descriptor_Type::Test_Consistency(): Array_ID() = %d getRawDataReferenceCount() = %d \n",
                    Array_ID(),getRawDataReferenceCount());
          APP_ASSERT (getRawDataReferenceCount() >= getRawDataReferenceCountBase());
        }
#else
     if (getRawDataReferenceCount() < 0)
          printf ("In doubleArray_Descriptor_Type::Test_Consistency(): getRawDataReferenceCount() = %d \n",
               getRawDataReferenceCount());
     APP_ASSERT (getRawDataReferenceCount() >= 0);
#endif

#if 0
     printf ("sizeof(array_descriptor)             = %d \n",sizeof(array_descriptor) );
     printf ("sizeof(doubleArray_Descriptor_Type) = %d \n",sizeof(doubleArray_Descriptor_Type) );
#endif
#if defined(APP) || defined(SERIAL_APP)
     APP_ASSERT ( sizeof(array_descriptor) == sizeof(doubleArray_Descriptor_Type) );
#endif

#if defined(PPP)
  // By definition of getRawDataReferenceCount() the following should be TRUE!
     if (SerialArray != NULL)
        {
          APP_ASSERT ( SerialArray->getReferenceCount() == getRawDataReferenceCount() );
        }
#endif

#if !defined(PPP)
  // Reference this variable so that purify can check that it is initialized!
     APP_ASSERT (Array_Data == NULL || Array_Data != NULL);
#endif

  // Reference this variable so that purify can check that it is initialized!
     APP_ASSERT (ExpressionTemplateDataPointer == NULL || ExpressionTemplateDataPointer != NULL);

  // I don't know what this should be tested against to verify correctness!
  // APP_ASSERT (ExpressionTemplateDataPointer == Array_Data + Array_Domain.ExpressionTemplateOffset);

  // I don't know what these should be tested against either!
     APP_ASSERT (Array_View_Pointer0 == NULL || Array_View_Pointer0 != NULL);
#if MAX_ARRAY_DIMENSION>1
     APP_ASSERT (Array_View_Pointer1 == NULL || Array_View_Pointer1 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>2
     APP_ASSERT (Array_View_Pointer2 == NULL || Array_View_Pointer2 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>3
     APP_ASSERT (Array_View_Pointer3 == NULL || Array_View_Pointer3 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>4
     APP_ASSERT (Array_View_Pointer4 == NULL || Array_View_Pointer4 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>5
     APP_ASSERT (Array_View_Pointer5 == NULL || Array_View_Pointer5 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>6
     APP_ASSERT (Array_View_Pointer6 == NULL || Array_View_Pointer6 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>7
     APP_ASSERT (Array_View_Pointer7 == NULL || Array_View_Pointer7 != NULL);
#endif

  // Uncomment this line so that the array domain is tested!
     Array_Domain.Test_Consistency(Label);

#if 0
// We had to put this test into the array object's Test_Consistency member function! 
#if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset = Array_Domain.Base[0];
     for (int i=1; i < MAX_ARRAY_DIMENSION; i++)
        {
          ExpectedOffset += Array_Domain.Base[i] * Array_Domain.Size[i-1];
        }
     APP_ASSERT (Array_Domain.ExpressionTemplateOffset == ExpectedOffset);
     if (Array_Data != NULL)
        {
          if (ExpressionTemplateDataPointer != Array_Data+ExpectedOffset)
             {
               printf ("Array_Data = %p \n",Array_Data);
               printf ("ExpressionTemplateDataPointer = %p \n",ExpressionTemplateDataPointer);
               printf ("ExpectedOffset = %d \n",ExpectedOffset);
               display("ERROR in doubleArray_Descriptor_Type<T,Template_Dimension>::Test_Consistency()");
               APP_ABORT();
             }
          APP_ASSERT (ExpressionTemplateDataPointer == Array_Data+ExpectedOffset);
        }
#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Leaving doubleArray_Descriptor_Type::Test_Consistency! (Label = %s) \n",Label);
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
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

// ****************************************************************************
// ****************************************************************************
// **************  ERROR CHECKING FUNCTION FOR INDEXING OPERATORS  ************
// ****************************************************************************
// ****************************************************************************

#if 0
//template<class T , int Template_Dimension>

void doubleArray_Descriptor_Type::
Error_Checking_For_Index_Operators (
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::Error_Checking_For_Index_Operators \n");
#endif

     Array_Domain.Error_Checking_For_Index_Operators(Internal_Index_List);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving doubleArray_Descriptor_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }
#endif

#if 0
//template<class T , int Template_Dimension>

void doubleArray_Descriptor_Type::
Error_Checking_For_Index_Operators (
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::Error_Checking_For_Index_Operators(Indirect_Index_List) \n");
#endif

     Array_Domain.Error_Checking_For_Index_Operators(Indirect_Index_List);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving doubleArray_Descriptor_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }
#endif

#if 0
// **************************************************************************
// This function does the bounds checking for the scalar indexing of A++
// array objects.  Its purpose is to localize all the error checking for
// scalar indexing.
// **************************************************************************
// Old prototype
// void doubleArray::Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const
//template<class T , int Template_Dimension>

void doubleArray_Descriptor_Type::
Error_Checking_For_Scalar_Index_Operators ( 
     const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const
   {
     Array_Domain.Error_Checking_For_Scalar_Index_Operators(Integer_List);
   }
#endif

// *************************************************************************************************
// ********************  APPEND index_operator.C  **************************************************
// *************************************************************************************************

/* Actually this is a little too long for the AT&T compiler to inline! */
//template<class T , int Template_Dimension>

doubleArray_Descriptor_Type* doubleArray_Descriptor_Type::
Vectorizing_Descriptor ( const doubleArray_Descriptor_Type & X )
   {
  // Build a descriptor to fill in!
     doubleArray_Descriptor_Type* Return_Descriptor = new doubleArray_Descriptor_Type;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of doubleArray_Descriptor_Type::Vectorizing_Descriptor ( const doubleArray_Descriptor_Type & X ) (this = %p)\n",Return_Descriptor);
#endif

  // Avoid compiler warning about unused input variable
     if (&X);

     printf ("Inside of doubleArray_Descriptor_Type::Vectorizing_Descriptor: not sure how to set it up! \n");
     APP_ABORT();

  // Return_Descriptor.Array_Domain = Vectorizing_Domain (X.Array_Domain);

     return Return_Descriptor;
   }


void
doubleArray_Descriptor_Type::
Allocate_Array_Data ( bool Force_Memory_Allocation )
   {
  // This function allocates the internal data for the doubleArray_Descriptor_Tyep object.  In A++
  // this allocates the the raw double data array.  In P++ it allocates the internal
  // A++ array for the current processor (using the sizes defined in the DARRAY
  // parallel descriptor).
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("*** Allocating the Array_Data (or getting it from the hash table)! \n");
#endif
 
#if defined(PPP)
  // The allocation of data in the parallel environment is especially complicated
  // so this is broken out as a separate function and is implemented only for P++.
     Allocate_Parallel_Array (Force_Memory_Allocation);
#else
     APP_ASSERT(Array_Data == NULL);
     double* Array_Data_Pointer = NULL;

// KCC gives an error about Expression_Tree_Node_Type not being a complete type
// this is because the general use of this function outside of A++ directly
// uses a subset of the header files used to compile A++.
#if !defined(USE_EXPRESSION_TEMPLATES)
  // if (!Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION || Force_Memory_Allocation)
     if (Force_Memory_Allocation)
#else
     if (Force_Memory_Allocation)
#endif
        {
          if (doubleArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE)
             {
               Array_Data_Pointer = Hash_Table.Get_Primative_Array ( Array_Size() );
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call MDI_double_Allocate! \n");
#endif
#if !defined(USE_EXPRESSION_TEMPLATES)
               Array_Data_Pointer = MDI_double_Allocate ( (array_domain*)(&Array_Domain) );
#else
            // Need to allocate space without using the MDI layer
            // printf ("Need to allocate space without using the MDI layer! \n");
            // APP_ABORT();

               int Size = Array_Domain.Size[MAX_ARRAY_DIMENSION-1];
               if (Size > 0)
                  {
                    Array_Data_Pointer = (double*) APP_MALLOC ( Size * sizeof(double) );
  
                    if (Array_Data_Pointer == NULL)
                       {
                         printf ("HEAP ERROR: Array_Data_Pointer == NULL in void doubleArray_Descriptor_Type::Allocate_Array_Data(bool) (Size = %d)! \n", Size);
                         APP_ABORT();
                       }
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (Size < 0)
                       {
                         printf ("ERROR: Size < 0 Size = %d in void doubleArray_Descriptor_Type::Allocate_Array_Data(bool)! \n",Size);
                         APP_ABORT();
                       }
#endif
                    Array_Data_Pointer = NULL;
                  }
#endif
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Allocated Array_Data_Pointer = %p \n",Array_Data_Pointer);
#endif

  // Make the assignment of the new data pointer!
     Array_Data = Array_Data_Pointer;

  // printf ("In A++: setting up the Array_View_Pointer's \n");

  // Now that we have added tests into the Test_Consistancy function
  // (equivalent to that in the scalar indexing) we have to set these 
  // pointers as early as possible
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO

  // This test is repeated in the upper level array object's Test_Consistancy member function
     APP_ASSERT (Array_View_Pointer0 == Array_Data + Array_Domain.Scalar_Offset[0]);
     APP_ASSERT (Array_View_Pointer1 == Array_Data + Array_Domain.Scalar_Offset[1]);
     APP_ASSERT (Array_View_Pointer2 == Array_Data + Array_Domain.Scalar_Offset[2]);
     APP_ASSERT (Array_View_Pointer3 == Array_Data + Array_Domain.Scalar_Offset[3]);
     APP_ASSERT (Array_View_Pointer4 == Array_Data + Array_Domain.Scalar_Offset[4]);
     APP_ASSERT (Array_View_Pointer5 == Array_Data + Array_Domain.Scalar_Offset[5]);
#endif
   }
 
#if defined(PPP) && defined(USE_PADRE)

void doubleArray_Descriptor_Type::
setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::setLocalDomainInPADRE_Descriptor(%p) \n",inputDomain);
#endif

     if (Array_Domain.parallelPADRE_DescriptorPointer != NULL)
          Array_Domain.parallelPADRE_DescriptorPointer->setLocalDomain(inputDomain);
   }
#endif

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" void MDI_double_Deallocate ( double* Data_Pointer , array_domain* Descriptor );
#endif

void
doubleArray_Descriptor_Type::Delete_Array_Data ()
   {
#if defined(PPP)
     doubleSerialArray* Data_Pointer = SerialArray;
#else
     double* Data_Pointer = Array_Data;
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Descriptor_Type::Delete_Array_Data Array_ID = %d getRawDataReferenceCount() = %d  Data_Pointer = %p \n",
               Array_ID(),getRawDataReferenceCount(),Data_Pointer);

     if (getRawDataReferenceCount() < 0)
          printf ("getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
#endif

     APP_ASSERT (getRawDataReferenceCount() >= 0);
 
  // P++ objects must always try to delete their SerialArray objects because then the
  // SerialArray reference count will be decremented (at least) or the SerialArray
  // deleted (at most) within the delete operator.  However we find this feature of the delete
  // operator to be a little too clever and so we should consider removing  from the 
  // decrementation of the reference count from the delete operator and have it done by the user
  // instead.  this would mean that the user would have to take care of incrementing and decrementing
  // the reference count instead of just the incrementing (this is more symetric -- i.e. superior).
  // A++ and SerialArray objects should always decrement their
  // reference count (and then delete the raw data if the reference coutn is less
  // than ZERO) here since there destructor has called this function (for that purpose).
  // Within this system the delete operator takes care of decrementing the reference count!
#if defined(PPP)
  // printf ("In doubleArray_Descriptor_Type::Delete_Array_Data shouldn't we call decrementRawDataReferenceCount()??? \n");
  // APP_ABORT();
  // APP_ASSERT (getRawDataReferenceCount() >= 0);
     APP_ASSERT (getRawDataReferenceCount() >= getReferenceCountBase());
  // decrementRawDataReferenceCount();
     bool Delete_Raw_Data = TRUE;
#else
  // APP_ASSERT (getRawDataReferenceCount() >= 0);
     APP_ASSERT (getRawDataReferenceCount() >= getReferenceCountBase());
     decrementRawDataReferenceCount();
  // int Reference_Count_Lower_Bound = 0;
  // bool Delete_Raw_Data = ( getRawDataReferenceCount() < Reference_Count_Lower_Bound );
     bool Delete_Raw_Data = ( getRawDataReferenceCount() < getReferenceCountBase() );
#endif
 
     if ( Delete_Raw_Data )
        {
#if COMPILE_DEBUG_STATEMENTS
          if ( (APP_DEBUG > 1) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Deleting the data since reference count was == getReferenceCountBase()! \n");
#endif
          if (Data_Pointer != NULL)
             {
            // In P++ we delete the reference counted SerialArray and in A++ and Serial_A++ (the lower level)
            // we can optionally save the data for reuse (through a hash table mechanism).
#if defined(PPP)
#if defined(USE_PADRE)
            // We have to remove the references in PADRE to the P++ SerialArray_Domain objects
            // before we remove the SerialArray objects which contain the SerialArray_Domain objects.
               setLocalDomainInPADRE_Descriptor(NULL);
#endif
            // The destructor we decrement the referenceCount in the SerialArray
               Data_Pointer->decrementReferenceCount();
               if (Data_Pointer->getReferenceCount() < getReferenceCountBase())
                    delete Data_Pointer;
               Data_Pointer = NULL;
#else
            // This is done above before the if test -- it is a bug to do it here!
            // decrementRawDataReferenceCount();
               if (doubleArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE)
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 2)
                         printf ("Calling Hash_Table.Put_Primative_Array_Into_Storage \n");
#endif
                    Hash_Table.Put_Primative_Array_Into_Storage ( Data_Pointer , Array_Size() );
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("Calling MDI_double_Deallocate! Data_Pointer = %p \n",Data_Pointer);
#endif
                 // MDI_double_Deallocate ( Data_Pointer , (array_domain*)Array_Descriptor.Array_Domain );
                 // MDI_double_Deallocate ( Data_Pointer , &((array_domain)Array_Descriptor.Array_Domain) );
#if !defined(USE_EXPRESSION_TEMPLATES)
                    MDI_double_Deallocate ( Data_Pointer , (array_domain*)(&Array_Domain) );
#else
                 // printf ("Need to free space without calling the MDI layer \n");
                 // APP_ABORT();
                    APP_ASSERT (Data_Pointer != NULL);
                    free (Data_Pointer);
#endif
                  }
#endif
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Data_Pointer == NULL in doubleArray::Delete_Array_Data! \n");
#endif
             }
        }
#if COMPILE_DEBUG_STATEMENTS
       else
        {
          if ( (APP_DEBUG > 1) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Skip deallocation of array data since Array_ID = %d getRawDataReferenceCount() = %d Data_Pointer = %p \n",
                    Array_ID(),getRawDataReferenceCount(),Data_Pointer);
        }
#endif
 
#if defined(PPP)
     SerialArray = NULL;
#else
     Array_Data = NULL;
#endif
   }


#if 0


void
doubleArray_Descriptor_Type::postAllocationSupport()
   {
  // Support function for allocation of P++ array objects
  // This function modifies the serialArray descriptor to 
  // make it a proper view of the serial array excluding 
  // the ghost boundaries

     bool Generated_A_Null_Array              = FALSE;
     int SerialArray_Cannot_Have_Contiguous_Data = FALSE;

  // Need to set the upper bound on the iteration to MAX_ARRAY_DIMENSION
  // since all the variables for each dimension must be initialized.
     int j;
     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
#if 0
          printf ("(after setBase): getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
               j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));
	  printf ("(after setBase): SerialArray->getRawBase(%d) = %d  getRawBase(%d) = %d  SerialArray->getRawBound(%d) = %d getRawBound(%d) = %d \n",
		  j,SerialArray->getRawBase(j),j,getRawBase(j),j,SerialArray->getRawBound(j),j,getRawBound(j));
#endif

       // This function is used tohide the use of A++/P++ specific range objects and 
       // provide an interface that both the PADRE and nonPADRE versions can call.
       // void adjustSerialDomainForStridedAllocation ( int axis,  const Array_Domain_Type & globalDomain );
          if ( (Array_Domain.Stride [j] > 1) && (SerialArray->isNullArray() == FALSE) )
             {
            // printf ("##### Calling Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");
               SerialArray->Array_Descriptor.Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain);
            // printf ("##### DONE: Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");
             }

#if 0
#if 0
          printf ("Bases[%d] = %d \n",j,Bases[j]);
          printf ("Array_Domain.getDataBaseVariable(%d) = %d \n",
                   j,Array_Domain.getDataBaseVariable(j));
          printf ("AFTER SerialArray->setBase (%d) --- SerialArray->getRawBase(%d) = %d \n",
                   Bases[j]+Array_Domain.getDataBaseVariable(j),j,SerialArray->getRawBase(j));
          printf ("SerialArray->getDataBaseVariable(%d) = %d \n",
                   j,SerialArray->Array_Descriptor.Array_Domain.getDataBaseVariable(j));
#endif

       // This could be implemented without the function call overhead
       // represented here but this simplifies the initial implementation for now

	  printf ("In Allocate.C: getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
		  j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));
	  printf ("In Allocate.C: getLocalStride(%d) = %d  getStride(%d) = %d \n",
		  j,getLocalStride(j),j,getStride(j));

	  printf ("In Allocate.C: SerialArray->getRawBase(%d) = %d  getRawBase(%d) = %d  SerialArray->getRawBound(%d) = %d getRawBound(%d) = %d \n",
		  j,SerialArray->getRawBase(j),j,getRawBase(j),j,SerialArray->getRawBound(j),j,getRawBound(j));
	  printf ("In Allocate.C: SerialArray->getRawStride(%d) = %d  getRawStride(%d) = %d \n",
		  j,SerialArray->getRawStride(j),j,getRawStride(j));
#endif

       // The left side uses the bases which are relative to a unit stride (a relatively simple case)!
          int Left_Size_Size  = (getLocalBase(j) - getBase(j)) * Array_Domain.Stride[j];

       // The bound is the funky case since the bases are relative to stride 1 and 
       // the bounds are relative to stride.
#if 0
       // int Right_Size_Size = (getBound(j) - getLocalBound(j)) * Array_Domain.Stride[j];
          int Right_Size_Size = 0;
               if (Is_A_Left_Partition == TRUE)
                    Right_Size_Size = (getBound(j) - getLocalBound(j)) * Array_Domain.Stride[j];
	       else
                {
                  int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
                  Right_Size_Size = getBound(j) - tempLocalBound;
                }
#else
       // int tempLocalBound  = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];

       // int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
       // int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

       // printf ("getBase(%d) = %d getLocalBase(%d) = %d getLocalBound(%d) = %d Array_Domain.Stride[%d] = %d \n",
       //      j,getBase(j),j,getLocalBase(j),j,getLocalBound(j),j,Array_Domain.Stride[j]);

       // int tempLocalBound = getBase(j) + (getLocalBound(j) - getBase(j)) * Array_Domain.Stride[j];
       // int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
          int tempLocalUnitStrideBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Array_Domain.Stride[j];
          int tempLocalBound = tempLocalUnitStrideBase + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];

          int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

       // printf ("tempLocalUnitStrideBase = %d \n",tempLocalUnitStrideBase);

          int Right_Size_Size = 0;

          if(SerialArray->isNullArray() == TRUE)
             {
            // printf ("found a null array (locally) so reset tempLocalBound to getLocalBase(%d) = %d \n",j,getLocalBase(j));
	       int nullArrayOnLeftEdge = isLeftNullArray(j);
               if (nullArrayOnLeftEdge == TRUE)
                  {
                 // printf ("Set tempLocalBound to base! \n");
                 // tempLocalBound  = getLocalBase(j);
                    tempLocalBound  = getBase(j);
                 // gUBnd(Array_Domain.BlockPartiArrayDomain,j);
	       
                  }
                 else
                  {
                 // printf ("Set tempLocalBound to bound! \n");
                 // tempLocalBound  = getBound(j);
                 // tempLocalBound  = getBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
                    tempLocalBound  = getBound(j);
                  }

               int rightEdgeSize = tempGlobalBound - tempLocalBound;
               Right_Size_Size = (rightEdgeSize == 0) ? 0 : rightEdgeSize + 1;
             }
            else
             {
            // printf ("NOT A NULL ARRAY (locally) \n");
               Right_Size_Size = (tempGlobalBound - tempLocalBound);
             }
	  
       // printf ("In allocate.C: first instance: j = %d tempLocalBound = %d tempGlobalBound = %d \n",j,tempLocalBound,tempGlobalBound);

       // bugfix (8/15/2000) account for stride
       // int Right_Size_Size = (tempGlobalBound - tempLocalBound) / Array_Domain.Stride[j];
       // int Right_Size_Size = (tempGlobalBound - tempLocalBound);
#endif

       // Do these depend upon the stride in anyway?
          Array_Domain.Left_Number_Of_Points [j] = (Left_Size_Size  >= 0) ? Left_Size_Size  : 0;
          Array_Domain.Right_Number_Of_Points[j] = (Right_Size_Size >= 0) ? Right_Size_Size : 0;

#if 0
          printf ("Left_Size_Size = %d Right_Size_Size = %d \n",Left_Size_Size,Right_Size_Size);
          printf ("tempLocalBound = %d tempGlobalBound = %d \n",tempLocalBound,tempGlobalBound);
	  printf ("Array_Domain.Left_Number_Of_Points [%d] = %d Array_Domain.Right_Number_Of_Points[%d] = %d \n",
               j,Array_Domain.Left_Number_Of_Points [j],
               j,Array_Domain.Right_Number_Of_Points[j]);
#endif

       // Comment out the Global Index since it should already be set properly
       // and this will just screw it up! We have to set it (I think)
#if 0
	  printf ("BEFORE RESET: Array_Domain.Global_Index [%d] = (%d,%d,%d,%s) \n",j,
               Array_Domain.Global_Index [j].getBase(),
               Array_Domain.Global_Index [j].getBound(),
               Array_Domain.Global_Index [j].getStride(),
               Array_Domain.Global_Index[j].getModeString());
#endif
       // Array_Domain.Global_Index [j] = Index (getBase(j),(getBound(j)-getBase(j))+1,1);
       // Use the range object instead
          Array_Domain.Global_Index [j] = Range (getRawBase(j),getRawBound(j),getRawStride(j));
#if 0
	  printf ("AFTER RESET: Array_Domain.Global_Index [%d] = (%d,%d,%d,%s) \n",j,
               Array_Domain.Global_Index [j].getBase(),
               Array_Domain.Global_Index [j].getBound(),
               Array_Domain.Global_Index [j].getStride(),
               Array_Domain.Global_Index[j].getModeString());
#endif

          APP_ASSERT (Array_Domain.Global_Index [j].getMode() != Null_Index);

          int ghostBoundaryWidth = Array_Domain.InternalGhostCellWidth[j];
          APP_ASSERT (ghostBoundaryWidth >= 0);

       // If the data is not contiguous on the global level then it could not 
       // be locally contiguous (unless the partitioning was just right --but 
       // we don't worry about that case) if this processor is on the left or 
       // right and the ghost bundaries are of non-zero width
       // then the left and right processors can not have:
          SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = Array_Domain.Is_Contiguous_Data;

       // Additionally if we have ghost boundaries then these are always hidden 
       // in a view and so the data can't be contiguous
          if (Array_Domain.InternalGhostCellWidth [j] > 0)
               SerialArray_Cannot_Have_Contiguous_Data = TRUE;

       // In this later case since we have to modify the parallel domain object to be consistant
          if (SerialArray_Cannot_Have_Contiguous_Data == TRUE)
             {
            // Note: we are changing the parallel array descriptor (not done much in this function)
               Array_Domain.Is_Contiguous_Data                               = FALSE;
               SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = FALSE;
             }

          int Local_Stride = SerialArray->Array_Descriptor.Array_Domain.Stride [j];

          if (j < Array_Domain.Domain_Dimension)
             {
            // Bugfix (17/10/96)
            // The Local_Mask_Index is set to NULL in the case of repartitioning
            // an array distributed first across one processor and then across 2 processors.
            // The second processor is a NullArray and then it has a valid local array (non nullarray).
            // But the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
            // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
            // setup.  The problem is that it is either not setup or sometimes setup incorrectly.
            // So we have to set it to a non-nullArray as a default.
               if (Array_Domain.Local_Mask_Index[j].getMode() == Null_Index)
                  {
                 // This resets the Local_Mask_Index to be consistant with what it usually is going into 
                 // this function. This provides a definition of Local_Mask_Index consistant with
                 // the global range of the array. We cannot let the Local_Mask_Index be a Null_Index
                 // since it would not allow the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
                 // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
                 // setup.
                 // Array_Domain.Local_Mask_Index[j].display("INSIDE OF ALLOCATE PARALLEL ARRAY -- CASE OF NULL_INDEX");
                 // Array_Domain.Local_Mask_Index[j] = Range (Array_Domain.Base[j],Array_Domain.Bound[j]);
                    Array_Domain.Local_Mask_Index[j] = Array_Domain.Global_Index[j];
                  }

               APP_ASSERT (Array_Domain.Local_Mask_Index[j].getMode() != Null_Index);
               APP_ASSERT (Array_Domain.Local_Mask_Index[j].getMode() != All_Index);

               bool Is_A_Left_Partition   = Array_Domain.isLeftPartition  (j);
               bool Is_A_Right_Partition  = Array_Domain.isRightPartition (j);
               bool Is_A_Middle_Partition = Array_Domain.isMiddlePartition(j);

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
               int base_offset  = getBase(j) - getLocalBase(j);

            // Bounds are relative to strides (non-unit strides)
            // int bound_offset = getLocalBound(j) - getBound(j);
            // Convert the bound to a unit stride bound so it can be compared to the value of getBound(j)
#if 0
               int tempLocalBound = 0;
               if (Is_A_Left_Partition == TRUE)
                    tempLocalBound = getLocalBound(j);
                 else
                    tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
#else
   //          int tempLocalBound  = getBase(j) + (getLocalBound(j) - getBase(j)) * Array_Domain.Stride[j];
   //          int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

            // I think these are already computed above!
               int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
               int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

            // printf ("In allocate.C: 2nd instance: j = %d tempLocalBound = %d tempGlobalBound = %d \n",j,tempLocalBound,tempGlobalBound);
#endif

            // Now that we have a unit stride bound we can do the subtraction
               int bound_offset = tempLocalBound - tempGlobalBound;

               int Original_Base_Offset_Of_View  = (Array_Domain.Is_A_View) ? (base_offset>0 ? base_offset : 0) : 0;
               int Original_Bound_Offset_Of_View = (Array_Domain.Is_A_View) ? (bound_offset>0 ? bound_offset : 0) : 0;

            // Need to account for the stride
            // int Count = ((getLocalBound(j) - getLocalBase(j))+1) -
            //             (Original_Base_Offset_Of_View + Original_Bound_Offset_Of_View);
               int Count = ((getLocalBound(j)-getLocalBase(j))+1) -
                           (Original_Base_Offset_Of_View + Original_Bound_Offset_Of_View);
            // Only could the left end once in adjusting for stride!
            // Count = 1 + (Count-1) * Array_Domain.Stride[j];

#if 0
               printf ("Array_Domain.Is_A_View = %s \n",(Array_Domain.Is_A_View == TRUE) ? "TRUE" : "FALSE");

               printf ("(compute Count): getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
               j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));

	       printf ("base_offset = %d  tempGlobalBound = %d tempLocalBound = %d bound_offset = %d \n",
                    base_offset,tempGlobalBound,tempLocalBound,bound_offset);
               printf ("SerialArray->Array_Descriptor.Array_Domain.getBase(%d) = %d \n",
                  j,SerialArray->Array_Descriptor.Array_Domain.getBase(j));
               printf ("SerialArray->Array_Descriptor.Array_Domain.getBound(%d) = %d \n",
                  j,SerialArray->Array_Descriptor.Array_Domain.getBound(j));
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

	       if (!Array_Domain.Is_A_View)
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
               printf ("In Allocate: Final Count = %d \n",Count);
#endif

               if (Count > 0)
                  {
                 // This should have already been set properly (so why reset it?)
                 // Actually it is not set properly all the time at least so we have to reset it.
#if 0
                    printf ("BEFORE 1st RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                 // This should be relative to stride 1 base/bound data since the local mask is relative to the local data directly (unit stride)
                 // Array_Domain.Local_Mask_Index[j] = Index(getLocalBase(j) + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);
		    int tempLocalBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Local_Stride;
                    Array_Domain.Local_Mask_Index[j] = Index(tempLocalBase + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);

#if 0
                    printf ("AFTER 1st RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif

                 // Make the SerialArray a view of the valid portion of the local partition
                    SerialArray->Array_Descriptor.Array_Domain.Is_A_View = TRUE;

                 /*
                 // ... (bug fix, 5/21/96,kdb) bases and bounds need to
                 // be adjusted by Original_Base_Offset_Of_View and
                 // Original_Bound_Offset_Of_View no matter what processor ...
                 */

                    SerialArray->Array_Descriptor.Array_Domain.Base [j] += Original_Base_Offset_Of_View;
                    SerialArray->Array_Descriptor.Array_Domain.Bound[j] -= Original_Bound_Offset_Of_View;
                 // ... bug fix (8/26/96, kdb) User_Base must reflect the view ...
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] += Original_Base_Offset_Of_View;

                    if (!Array_Domain.Is_A_View)
                       {
                         if (Is_A_Left_Partition)
                            {
                              SerialArray->Array_Descriptor.Array_Domain.Base[j] += ghostBoundaryWidth;
                           // ... bug fix (8/26/96, kdb) User_Base must reflect the ghost cell ...
                              SerialArray->Array_Descriptor.Array_Domain.User_Base[j] += ghostBoundaryWidth;
                            }

                         if (Is_A_Right_Partition)
                              SerialArray->Array_Descriptor.Array_Domain.Bound[j] -= ghostBoundaryWidth;
                       }

                 // Bugfix (10/19/95) if we modify the base and bound in the 
                 // descriptor then the data is no longer contiguous
                 // meaning that it is no longer binary conformable
                    if (ghostBoundaryWidth > 0)
                         SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = FALSE;
                  }
                 else
                  {
                    Generated_A_Null_Array = TRUE;
                  }
             }
            else // j >= Array_Domain.Domain_Dimension
             {
               int Count = (getLocalBound(j)-getLocalBase(j)) + 1;
               if (Count > 0)
                  {
#if 0
                    printf ("BEFORE 2nd RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                 // (6/28/2000) part of fix to permit allocation of strided array data
                 // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,1);
                 // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,Local_Stride);
		    int tempLocalBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Local_Stride;
                    Array_Domain.Local_Mask_Index[j] = Index(tempLocalBase, Count, Local_Stride);

#if 0
                    printf ("AFTER 2nd RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                  }
                 else
                  {
                 // build a null internalIndex for this case
                    Array_Domain.Local_Mask_Index[j] = Index (0,0,1);
                  }
             }

#if 1
       // ... add correct view offset and Scalar Offset ...
          if (j==0)
             {
#if 0
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
                    SerialArray->Array_Descriptor.Array_Domain.Base[0] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[0];
#else
            // bugfix (7/16/2000) The View_Offset is not computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
	            SerialArray->Array_Descriptor.Array_Domain.Base[0];
#endif
            // The Scalar_Offset is computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[0] =
                  - SerialArray->Array_Descriptor.Array_Domain.User_Base[0] *
	            SerialArray->Array_Descriptor.Array_Domain.Stride[0];
             }
            else
             {
#if 0
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
#else
            // The View_Offset is not computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
#endif
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] =
                    SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j-1] -
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
             }
#else
       // ... add correct view offset and Scalar Offset ...
          if (j==0)
             {
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
	            SerialArray->Array_Descriptor.Array_Domain.Base[0];
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[0] =
                  - SerialArray->Array_Descriptor.Array_Domain.User_Base[0];
             }
            else
             {
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
            // Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Size[temp-1];
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] =
                    SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j-1] -
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
             }
#endif
       // Error checking for same strides in parallel and serial array
          APP_ASSERT ( SerialArray->isNullArray() ||
                       (Array_Domain.Stride [j] == SerialArray->Array_Descriptor.Array_Domain.Stride [j]) );

#if 0
          for (int temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               printf ("Check values: SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
                    temp,SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[temp]);
             }
#endif

#if 0
       // New test (7/2/2000)
       // In general this test is not always valid (I think it was a mistake to add it - but
       // we might modify it in the future so I will leave it here for now (commented out))
       // This test is not working where the array on one processor is a NullArray (need to fix this test)
          if (SerialArray->isNullArray() == FALSE)
             {
            // recompute the partition information for this axis
               bool Is_A_Left_Partition   = Array_Domain.isLeftPartition  (j);
               bool Is_A_Right_Partition  = Array_Domain.isRightPartition (j);
               bool Is_A_Middle_Partition = Array_Domain.isMiddlePartition(j);

            // Error checking on left and right edges of the partition
               if (Is_A_Left_Partition)
                  {
                    if ( getRawBase(j) != SerialArray->getRawBase(j) )
                       {
                      // display("ERROR: getRawBase(j) != SerialArray->getRawBase(j)");
		         printf ("getRawBase(%d) = %d  SerialArray->getRawBase(%d) = %d \n",
                              j,getRawBase(j),j,SerialArray->getRawBase(j));
                       }
                    APP_ASSERT( getRawBase(j) == SerialArray->getRawBase(j) );
                  }

               if (Is_A_Right_Partition)
                  {
                    if ( getRawBound(j) != SerialArray->getRawBound(j) )
                       {
                      // display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
                      // SerialArray->Array_Descriptor.display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
		         printf ("getRawBound(%d) = %d  SerialArray->getRawBound(%d) = %d \n",
                              j,getRawBound(j),j,SerialArray->getRawBound(j));
                       }
                    APP_ASSERT( getRawBound(j) == SerialArray->getRawBound(j) );
                  }
             }
#endif
        } // end of j loop


     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
          if (j == 0)
             {
            // Start out true
               SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = TRUE;
#if 0
               printf ("Initial setting: SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = %s \n",
                    SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");
#endif
             }
            else
             {
            // Start out true
               SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = 
                    SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base && 
                    (SerialArray->Array_Descriptor.Array_Domain.Data_Base[j] == 
                     SerialArray->Array_Descriptor.Array_Domain.Data_Base[j-1]);
#if 0
               printf ("axis = %d Constant_Data_Base = %s \n",
                    j,SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");
#endif
             }
        }

#if 0
     printf ("Final value: SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = %s \n",
          SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");

     printf ("SerialArray->Array_Descriptor.Array_Domain.View_Offset = %d \n",
          SerialArray->Array_Descriptor.Array_Domain.View_Offset);
#endif

  // ... add View_Offset to Scalar_Offset (we can't do this inside the
  // loop above because View_Offset is a sum over all dimensions). Also
  // set View_Pointers now. ...
     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
          SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] +=
               SerialArray->Array_Descriptor.Array_Domain.View_Offset;
       // printf ("SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
       //      j,SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j]);
        }

  /* SERIAL_POINTER_LIST_INITIALIZATION_MACRO; */
     DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;

  // if we have generated a null view of a valid serial array then we have 
  // to make the descriptor conform to some specific rules
  // 1. A Null_Array has to have the Is_Contiguous_Data flag FALSE
  // 2. A Null_Array has to have the Base and Bound 0 and -1 (repectively)
  //    for ALL dimensions
  // 3. The Local_Mask_Index in the Parallel Descriptor must be a Null Index

     if (Generated_A_Null_Array == TRUE)
        {
       // We could call a function here to set the domain to represent a Null Array
          SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data   = FALSE;
          SerialArray->Array_Descriptor.Array_Domain.Is_A_View            = TRUE;
          SerialArray->Array_Descriptor.Array_Domain.Is_A_Null_Array      = TRUE;

          SerialArray->Array_Descriptor.Array_Domain.Constant_Unit_Stride = TRUE;

          int j;
          for (j=0; j < MAX_ARRAY_DIMENSION; j++)
             {
               Array_Domain.Local_Mask_Index   [j] = Index (0,0,1,Null_Index);
               SerialArray->Array_Descriptor.Array_Domain.Base     [j] =  0;
               SerialArray->Array_Descriptor.Array_Domain.Bound    [j] = -1;
               SerialArray->Array_Descriptor.Array_Domain.User_Base[j] =  0;
             }
        }
   }


#endif
 

#undef DOUBLEARRAY

#define FLOATARRAY
/*``#define'' CLASS_TEMPLATE*/  
/*``#define'' CLASS_TEMPLATE  template<class T``,''int Templated_Dimension>*/

/*``#define'' ARRAY_DESCRIPTOR_TYPE floatArray_Descriptor_Type*/
/*``#define'' ARRAY_DESCRIPTOR_TYPE Array_Descriptor_Type<T`,'Template_Dimension>*/

#if !defined(PPP)
// *********************************************************
// Hash tables can be used to to avoid redundent allocation
// and deallocation associated with the use of temporaries.
// Temporaries are created and saved for reuse when the same
// size temporary is required again. No smart system for aging
// of hashed memory is used to avoid the acumulation of reserved
// memory for reuse.  Also no measurable advantage of this
// feature has been observed.
// *********************************************************
// floatArray_Data_Hash_Table floatArray_Descriptor_Type::Hash_Table;
 floatArray_Data_Hash_Table floatArray_Descriptor_Type::Hash_Table;
#endif


// *****************************************************************************
// *****************************************************************************
// *********  NEW OPERATOR INITIALIZATION MEMBER FUNCTION DEFINITION  **********
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

void floatArray_Descriptor_Type::New_Function_Loop ()
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

//template<class T, int Template_Dimension>

void floatArray_Descriptor_Type::freeMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of floatArray_Descriptor_Type::freeMemoryInUse() \n");
#endif 

  // This function is useful in conjuction with the Purify (from Pure Software Inc.)
  // it frees memory allocated for use internally in A++ <type>Array objects.
  // This memory is used internally and is reported as "in use" by Purify
  // if it is not freed up using this function.  This function works with
  // similar functions for each A++ object to free up all of the A++ memory in
  // use internally.

#if !defined(PPP)
  // Bugfix (12/24/97)
  // We don't store the Array_Domain_Type::Array_Reference_Count_Array
  // in the floatArray_Descriptor_Type so we can't free it here
  // this is taken care of in the Array_Domain_Type::freeMemoryInUse()
  // member function!
  // free memory allocated for reference counting!
  // if (Array_Domain_Type::Array_Reference_Count_Array != NULL)
  //      free ((char*) Array_Domain_Type::Array_Reference_Count_Array);
#endif

  // free memory allocated for memory pools!
     for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++) 
          if (Memory_Block_List [i] != NULL)  
               free ((char*) (Memory_Block_List[i]));
   }

// *****************************************************************************
// *****************************************************************************
// ****************  INITIALIZATION SPECIFIC TO PARALLEL DATA  *****************
// *****************************************************************************
// *****************************************************************************

#if 0
   // I don't think this is called!
#if defined(PPP)
//template<class T, int Template_Dimension>

void floatArray_Descriptor_Type::
Initialize_Parallel_Parts_Of_Descriptor ( const floatArray_Descriptor_Type & X )
   {
     Array_Domain.Initialize_Parallel_Parts_Of_Domain (X.Array_Domain);
   }
#endif
#endif

// *****************************************************************************
// *****************************************************************************
// **************  BASE SPECIFICATION MEMBER FUNCTION DEFINITION  **************
// *****************************************************************************
// *****************************************************************************


void
floatArray_Descriptor_Type::setBase( int New_Base_For_All_Axes )
   {
  // NEW VERSION OF SETBASE
#if COMPILE_DEBUG_STATEMENTS
  // Enforce bound on Alt_Base depending on INT_MAX and INT_MIN
     static int Max_Base = INT_MAX;
     static int Min_Base = INT_MIN;

  // error checking!
     APP_ASSERT((New_Base_For_All_Axes > Min_Base) && (New_Base_For_All_Axes < Max_Base));
#endif

     int temp       = 0;  // iteration variable
     int Difference = 0;

#if defined(PPP)
  // APP_ASSERT(SerialArray != NULL);

  // Adjust the Data_Base on the LOCAL SerialArray relative to the change in the
  // GLOBAL base
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
        {
          Difference = New_Base_For_All_Axes - (Array_Domain.Data_Base[temp] +
                                                Array_Domain.Base[temp]);

          if (SerialArray != NULL)
             {
            // printf ("SHOULDN'T WE USE SerialArray->getBase(temp) instead of getBase(temp) \n");
	    /*
	    // ... (5/18/98, kdb) bug fix, getRawBase now corresponds to setBase
	    //   (getBase gets User_Base instead of Data_Base + Base) ...
	    */
               int New_Serial_Base = SerialArray->getRawBase(temp) + Difference;
            // int New_Serial_Base = SerialArray->getBase(temp) + Difference;
            // int New_Serial_Base = getBase(temp) + Difference;
            // printf ("New_Serial_Base = %d \n",New_Serial_Base);
               SerialArray->setBase (New_Serial_Base,temp);
             }
        }
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
#endif

     Array_Domain.setBase(New_Base_For_All_Axes);

  // Now initialize the View pointers (located in the 
  // descriptor -- since they are typed pointers)
#if defined(PPP)
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
     if (SerialArray != NULL)
        {
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
        }
#else
  // POINTER_LIST_INITIALIZATION_MACRO;
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
#endif
   }


void
floatArray_Descriptor_Type::setBase( int New_Base, int Axis )
   {
#if defined(PPP)
  // SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
     if (SerialArray != NULL)
        {
       // Call setBase on the SerialArray also
          int Difference = New_Base - (Array_Domain.Data_Base[Axis] + Array_Domain.Base[Axis]);

          int New_Serial_Base = SerialArray->getBase(Axis) + Difference;
          SerialArray->setBase (New_Serial_Base,Axis);

          Array_Domain.setBase (New_Base,Axis);

       // Now initialize the View pointers (located in the 
       // descriptor -- since they are typed pointers)
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;
        }
#else
     Array_Domain.setBase (New_Base,Axis);

  // Now initialize the View pointers (located in the 
  // descriptor -- since they are typed pointers)
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
#endif
   }
//====================================================================

// *****************************************************************************
// *****************************************************************************
// ********************  DISPLAY MEMBER FUNCTION DEFINITION  *******************
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

void
floatArray_Descriptor_Type::display(const char *Label ) const
   {
     int i=0;
     printf ("Array_Descriptor_Type::display() -- %s \n",Label);

     Array_Domain.display(Label);

#if defined(PPP)
     printf ("getLocalBase (output of getLocalBase member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalBase(i));
     printf ("\n");

     printf ("getLocalBound (output of getLocalBound member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalBound(i));
     printf ("\n");

     printf ("getLocalStride (output of getLocalStride member function)  = ");
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          printf (" %d ",getLocalStride(i));
     printf ("\n");
#endif

#if !defined(PPP)
     printf ("Array_Data                    = %p \n",Array_Data);
#endif
     printf ("ExpressionTemplateDataPointer = %p \n",ExpressionTemplateDataPointer);

#if !defined(PPP)
        printf ("Array_View_Pointer0 = %p (Array_Data-Array_View_Pointer0) = %d \n",
             Array_View_Pointer0,(Array_Data-Array_View_Pointer0));
#if MAX_ARRAY_DIMENSION>1
        printf ("Array_View_Pointer1 = %p (Array_View_Pointer0-Array_View_Pointer1) = %d \n",
             Array_View_Pointer1,(Array_View_Pointer0-Array_View_Pointer1));
#endif
#if MAX_ARRAY_DIMENSION>2
        printf ("Array_View_Pointer2 = %p (Array_View_Pointer1-Array_View_Pointer2) = %d \n",
             Array_View_Pointer2,(Array_View_Pointer1-Array_View_Pointer2));
#endif
#if MAX_ARRAY_DIMENSION>3
        printf ("Array_View_Pointer3 = %p (Array_View_Pointer2-Array_View_Pointer3) = %d \n",
             Array_View_Pointer3,(Array_View_Pointer2-Array_View_Pointer3));
#endif
#if MAX_ARRAY_DIMENSION>4
        printf ("Array_View_Pointer4 = %p (Array_View_Pointer3-Array_View_Pointer4) = %d \n",
             Array_View_Pointer4,(Array_View_Pointer3-Array_View_Pointer4));
#endif
#if MAX_ARRAY_DIMENSION>5
        printf ("Array_View_Pointer5 = %p (Array_View_Pointer4-Array_View_Pointer5) = %d \n",
             Array_View_Pointer5,(Array_View_Pointer4-Array_View_Pointer5));
#endif
#if MAX_ARRAY_DIMENSION>6
        printf ("Array_View_Pointer6 = %p (Array_View_Pointer5-Array_View_Pointer6) = %d \n",
             Array_View_Pointer6,(Array_View_Pointer5-Array_View_Pointer6));
#endif
#if MAX_ARRAY_DIMENSION>7
        printf ("Array_View_Pointer7 = %p (Array_View_Pointer6-Array_View_Pointer7) = %d \n",
             Array_View_Pointer7,(Array_View_Pointer6-Array_View_Pointer7));
#endif
#endif

  // The only additional thing to do is print out the values of the pointers 
  // in this class since that is the only difference between a descriptor and a domain.
   }

// *****************************************************************************
// *****************************************************************************
// **************************  DESTRUCTOR DEFINITIONS  *************************
// *****************************************************************************
// *****************************************************************************

//template<class T, int Template_Dimension>

floatArray_Descriptor_Type::~floatArray_Descriptor_Type ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Descriptor_Type::destructor! %p Array_ID = %d referenceCount = %d \n",
               this,Array_Domain.Array_ID(),referenceCount);
#endif

#if 0
#if defined(PPP)
     printf ("Why can't we assert that SerialArray == NULL (SerialArray = %s) in the ~floatArray_Descriptor_Type()? \n",
          (SerialArray == NULL) ? "NULL" : "VALID POINTER");
#else
     printf ("Why can't we assert that Array_Data == NULL (Array_Data = %s) in the ~floatArray_Descriptor_Type()? \n",
          (Array_Data == NULL) ? "NULL" : "VALID POINTER");
#endif
#endif

#if defined(USE_PADRE) && defined(PPP)
     if (Array_Domain.parallelPADRE_DescriptorPointer != NULL)
        {
       // printf ("In floatArray_Descriptor_Type::destructor(): delete the PADRE_Descriptor \n");
          Array_Domain.parallelPADRE_DescriptorPointer->decrementReferenceCount();
       // printf ("Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount() = %d \n",
       //      Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount());
          if (Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCount() < 
              Array_Domain.parallelPADRE_DescriptorPointer->getReferenceCountBase())
               delete Array_Domain.parallelPADRE_DescriptorPointer;
          Array_Domain.parallelPADRE_DescriptorPointer = NULL;
        }
#endif

  // Reset reference count!  This should always be the value of getReferenceCountBase() anyway
     referenceCount = getReferenceCountBase();

  // printf ("In ~floatArray_Descriptor_Type Array_ID on the stack is %d \n",SerialArray_Domain_Type::queryNextArrayID());

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Leaving ~Array_Descriptor_Type() -- referenceCount = %d \n",referenceCount);
#endif
   }

// *****************************************************************************
// *****************************************************************************
// *******************  CONSTRUCTOR INITIALIZATION SUPPORT  ********************
// *****************************************************************************
// *****************************************************************************


void
floatArray_Descriptor_Type::Preinitialize_Descriptor()
   {
  // At this point (when this function is called) the array data HAS NOT been allocated!

  // Set reference count!
     referenceCount = getReferenceCountBase();

  // We set these to NULL to avoid UMR (in purify) -- but these are RESET
  // after the pointer to the array data when it is known.
#if defined(PPP)
     floatSerialArray *Data_Pointer = SerialArray;
     if (Data_Pointer == NULL)
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
        }
       else
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
          DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO
        }
#else
     float *Data_Pointer = Array_Data;
     if (Data_Pointer == NULL)
        {
          DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
        }
       else
        {
          DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO;
        }
#endif

#if COMPILE_DEBUG_STATEMENTS
  // Test_Consistency ("Called from floatArray_Descriptor_Type::Preinitialize_Descriptor()");
  // printf ("In Preinitialize_Descriptor(): Commented out Test_Consistency since not enough data is available at this stage! \n");
#endif
   }

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void floatArray_Descriptor_Type::
Initialize_Descriptor(
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List,
     const Internal_Partitioning_Type* Internal_Partition )
   {
  // This function is required because sometimes the descriptor must be reinitialized
  // (this happens in the dynamic re-dimensioning of the array object for example).
  // This function is also called from within the array class adopt function.
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of floatArray_Descriptor_Type::Initialize_Descriptor(Number_Of_Valid_Dimensions=%d,int*) ",
               Number_Of_Valid_Dimensions);
        }
#endif

  // printf ("BEFORE Preinitialize_Descriptor() getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // APP_ASSERT (Array_Data == NULL);
  // setDataPointer(NULL);
  // resetRawDataReferenceCount();

  // Bugfix (11/6/2000) Move preinitialixation of the descriptor to after the initialization of the domain.
  // Preinitialize_Descriptor();

#if 0
  // We cannot assert this here since we want the Array_Domain.Initialize_Domain
  // to delete the old partitioning objects if they exist

#if defined(PPP) && !defined(USE_PADRE)
#if COMPILE_DEBUG_STATEMENTS
     if (Array_Domain.BlockPartiArrayDomain        != NULL)
          printf ("WARNING: In Initialize_Descriptor --- Array_Domain.BlockPartiArrayDomain        != NULL \n");

     if (Array_Domain.BlockPartiArrayDecomposition        != NULL)
          printf ("WARNING: In Initialize_Descriptor --- Array_Domain.BlockPartiArrayDecomposition != NULL \n");
#endif

     APP_ASSERT (Array_Domain.BlockPartiArrayDomain        == NULL);
     APP_ASSERT (Array_Domain.BlockPartiArrayDecomposition == NULL);
#endif
#endif

  // This appears to be the second initialization of the Array_Domain object!!!
  // We need to find out where this is called and see that the correct Array_Domain
  // constructor is called instead.  Upon investigation this "Initialization_Descriptor()"
  // function is not called by any of the Array_Descriptor constructors.
#if defined(PPP)
     if (Internal_Partition != NULL)
        {
          Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List,*Internal_Partition);
        }
       else
        {
          Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
        }
#else
  // Avoid compiler warning about unused input variable
     if (&Internal_Partition);

     Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
#endif

  // Bugfix (11/6/2000) Move preinitialixation of the descriptor to after the initialization of the domain.
     Preinitialize_Descriptor();

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
  // For P++ we only track the serial array objects not the serial array objects
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[Array_ID()] != NULL);
#ifdef DOUBLEARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_DOUBLE_ELEMENT_TYPE);
#endif
#ifdef FLOATARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_FLOAT_ELEMENT_TYPE);
#endif
#ifdef INTARRAY
          Diagnostic_Manager::diagnosticInfoArray[Array_ID()]->setTypeCode(APP_INT_ELEMENT_TYPE);
#endif
          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[Array_ID()] != NULL);
        }
#endif
   }

// Fixup_Descriptor_With_View( 
//     int Number_Of_Valid_Dimensions ,
//     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )

void
floatArray_Descriptor_Type::reshape( 
   int Number_Of_Valid_Dimensions ,
   const int* View_Sizes, const int* View_Bases)
   {
  // This function is only called by the array object's reshape member function
     Array_Domain.reshape (Number_Of_Valid_Dimensions, View_Sizes, View_Bases);
   }


// *****************************************************************************
// *****************************************************************************
// *************************  CONSTRUCTORS DEFINITIONS  ************************
// *****************************************************************************
// *****************************************************************************

//template<class T , int Template_Dimension>

floatArray_Descriptor_Type::
floatArray_Descriptor_Type() 
   : Array_Domain()
   {
  // This builds the NULL Array_Descriptor (used in NULL arrays)!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of floatArray_Descriptor_Type::Array_Descriptor_Type() this = %p \n",this);
       // printf ("WARNING: It is a ERROR to use this descriptor constructor except for NULL arrays! \n");
       // APP_ABORT();
        }
#endif

     setDataPointer(NULL);
#if !defined(PPP)
     resetRawDataReferenceCount();
#endif
  // printf ("In floatArray_Descriptor_Type::floatArray_Descriptor_Type() -- getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
     Preinitialize_Descriptor();

#if 0
  // Set reference count!
     referenceCount = getReferenceCountBase();

#if !defined(PPP)
     Array_Data = NULL;
  // DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
#else
     SerialArray = NULL;
#endif
  // We want to use this macro with both A++ and P++
  // ExpressionTemplateDataPointer = NULL;
     DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO
#endif

#if COMPILE_DEBUG_STATEMENTS
  // Can't run these tests in a NULL array
  // Test_Consistency ("Called from default constructor!");
#endif
   }

floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( const Internal_Index & I )
   : Array_Domain(I)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if (MAX_ARRAY_DIMENSION >= 2)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J )
   : Array_Domain(I,J)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K )
   : Array_Domain(I,J,K)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 4)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L )
   : Array_Domain(I,J,K,L)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M )
   : Array_Domain(I,J,K,L,M)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N )
   : Array_Domain(I,J,K,L,M,N)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 7)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O )
   : Array_Domain(I,J,K,L,M,N,O)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 8)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O, const Internal_Index & P )
   : Array_Domain(I,J,K,L,M,N,O,P)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

//template<class T , int Template_Dimension>

floatArray_Descriptor_Type::
floatArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   : Array_Domain (Number_Of_Valid_Dimensions,Integer_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
#if 0
  // Set reference count!
     referenceCount = getReferenceCountBase();
  // Initialize_Descriptor (Number_Of_Valid_Dimensions,Integer_List);
#endif
   }

#if defined(APP) || defined(PPP)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if (MAX_ARRAY_DIMENSION >= 2)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 4)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 7)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,O,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if (MAX_ARRAY_DIMENSION >= 8)
floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const Internal_Index & I, const Internal_Index & J,
     const Internal_Index & K, const Internal_Index & L,
     const Internal_Index & M, const Internal_Index & N,
     const Internal_Index & O, const Internal_Index & P,
     const Internal_Partitioning_Type & Internal_Partition)
   : Array_Domain(I,J,K,L,M,N,O,P,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif
#endif
 
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif


floatArray_Descriptor_Type::
floatArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List )
   : Array_Domain (Number_Of_Valid_Dimensions,Internal_Index_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }


floatArray_Descriptor_Type::
floatArray_Descriptor_Type( ARGUMENT_LIST_MACRO_INTEGER )
   : Array_Domain (VARIABLE_LIST_MACRO_INTEGER)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }

#if 0
//template<class T , int Template_Dimension>

floatArray_Descriptor_Type::
floatArray_Descriptor_Type(
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   : Array_Domain(MAX_ARRAY_DIMENSION,Integer_List)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if defined(PPP) || defined(APP)

floatArray_Descriptor_Type::
floatArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
     const Internal_Partitioning_Type & Internal_Partition )
   : Array_Domain (Number_Of_Valid_Dimensions,Internal_Index_List,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

#if defined(PPP)
// floatArray_Descriptor_Type::floatArray_Descriptor_Type(
//           int Input_Array_Size_I , int Input_Array_Size_J ,
//           int Input_Array_Size_K , int Input_Array_Size_L ,
//           const Partitioning_Type & Partition )

floatArray_Descriptor_Type::
floatArray_Descriptor_Type( 
     int Number_Of_Valid_Dimensions ,
     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List , 
     const Internal_Partitioning_Type & Internal_Partition )
   : Array_Domain(Number_Of_Valid_Dimensions,Integer_List,Internal_Partition)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }


floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const float* Data_Pointer,
     ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE )
   : Array_Domain(VARIABLE_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE)
   {
  // Avoid compiler warning about unused input variable
     if (&Data_Pointer);

     printf ("ERROR: Parallel floatArray_Descriptor_Type::floatArray_Descriptor_Type (const float* Data_Pointer,ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE) --- not implemented! \n");
     APP_ABORT();

  // We can't set the float pointer here  This must be done at a later stage
  // setDataPointer (Data_Pointer);
  // incrementRawDataReferenceCount();
     setDataPointer (NULL);
     Preinitialize_Descriptor();
   }
#else // end of PPP


floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( 
     const float* Data_Pointer, 
     ARGUMENT_LIST_MACRO_CONST_REF_RANGE )
   : Array_Domain(VARIABLE_LIST_MACRO_CONST_REF_RANGE)
   {
  // Mark this object as getting it's data from an external source
     Array_Domain.builtUsingExistingData = TRUE;

#if !defined(PPP)
  // Array_Data = Data_Pointer;
  // Cast away const here to preserve use of const data pointer
  // when building array object using a pointer to existing data
     setDataPointer ((float*)Data_Pointer);
     incrementRawDataReferenceCount();

     APP_ASSERT (Array_Domain.builtUsingExistingData == TRUE);
#else
     SerialArray = NULL;
     printf ("ERROR: floatArray_Descriptor_Type::floatArray_Descriptor_Type (const float* Data_Pointer,ARGUMENT_LIST_MACRO_CONST_REF_RANGE) should not be defined for P++ \n");
     APP_ABORT();
#endif
     Preinitialize_Descriptor();
   }
#endif // end of else PPP


floatArray_Descriptor_Type::
floatArray_Descriptor_Type(
     const Array_Domain_Type & X,
     bool AvoidBuildingIndirectAddressingView )
  // We call the Array_Domain copy constructor to do the preinitialization
   : Array_Domain(X,AvoidBuildingIndirectAddressingView)
   {
  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     
#if !defined(PPP)
  // Since this is a new array object is should have an initialize reference count on its
  // raw data.  This is required here because the reference counting mechanism reused the
  // value of zero for one existing reference and no references (this will be fixed soon).
     resetRawDataReferenceCount();
#endif

     Preinitialize_Descriptor();

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from floatArray_Descriptor_Type (Array_Domain_Type) \n");
#endif
   }

// ***************************************************************************************
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

// floatArray_Descriptor_Type::floatArray_Descriptor_Type 
//    ( const floatArray_Descriptor_Type & X , 
//      const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
// template<class T , int Template_Dimension>

floatArray_Descriptor_Type::
floatArray_Descriptor_Type (
     const Array_Domain_Type & X ,
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
   : Array_Domain (X,Index_List)
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
   {
      printf ("Inside of floatArray_Descriptor_Type::floatArray_Descriptor_Type ");
      printf ("( const Array_Domain_Type & X , Internal_Index** Index_List )");
      printf ("(this = %p)\n",this);
   }
#endif

  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     Preinitialize_Descriptor();

     APP_ASSERT(Array_ID() == X.Array_ID());

#if COMPILE_DEBUG_STATEMENTS
   Test_Consistency ("Called from floatArray_Descriptor_Type (Array_Domain_Type,Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type*) \n");
#endif
}

// ************************************************************************
// floatArray_Descriptor_Type constructors for indirect addressing
// ************************************************************************
// floatArray_Descriptor_Type ( const floatArray_Descriptor_Type & X , 
//      const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
//template<class T , int Template_Dimension>

floatArray_Descriptor_Type::
floatArray_Descriptor_Type ( const Array_Domain_Type & X , 
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
   : Array_Domain(X,Indirect_Index_List)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of floatArray_Descriptor_Type::floatArray_Descriptor_Type ( const Array_Domain_Type & X , Internal_Indirect_Addressing_Index* Indirect_Index_List ) (this = %p)\n",this);
#endif

  // Set reference count!
  // referenceCount = getReferenceCountBase();

  // This may cause data to be set to null initial values and later reset
     setDataPointer(NULL);
     Preinitialize_Descriptor();

     APP_ASSERT(Array_ID() == X.Array_ID());
   }


#ifdef USE_STRING_SPECIFIC_CODE

// ************************************************************************
// floatArray_Descriptor_Type constructors for initialization using a string
// ************************************************************************
// floatArray_Descriptor_Type::floatArray_Descriptor_Type ( const char* dataString )
floatArray_Descriptor_Type::floatArray_Descriptor_Type ( const AppString & dataString )
   : Array_Domain(dataString)
   {
     setDataPointer(NULL);
     Preinitialize_Descriptor();
   }
#endif

// *****************************************************************************
// *****************************************************************************
// ***********************  MEMBER FUNCTION DEFINITIONS  ***********************
// *****************************************************************************
// *****************************************************************************

// if defined(APP) || defined(PPP)

int
floatArray_Descriptor_Type::getGhostBoundaryWidth ( int Axis ) const
   {
     int width = Array_Domain.getGhostBoundaryWidth(Axis);
  // APP_ASSERT (width == Array_Domain.InternalGhostCellWidth[Axis]);
     return width;
   }
// endif


void floatArray_Descriptor_Type::setInternalGhostCellWidth ( 
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List )
   {
     Array_Domain.setInternalGhostCellWidth(Integer_List);
   }

#if defined(PPP) || defined(APP)

void floatArray_Descriptor_Type::
partition ( const Internal_Partitioning_Type & Internal_Partition )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::partition ( const Internal_Partitioning_Type & Internal_Partition ) \n");
#endif

#if defined(PPP) 
  // ... only actually partion for P++ ...
     Array_Domain.partition (Internal_Partition);
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Leaving floatArray_Descriptor_Type::partition \n");
#endif
   }
#endif // end of PPP or APP

#if defined(PPP) 
#if defined(USE_PADRE)
  // What PADRE function should we call here?
#else

DARRAY* floatArray_Descriptor_Type::
Build_BlockPartiArrayDomain ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::Build_BlockPartiArrayDomain \n");
#endif

     return Array_Domain.Build_BlockPartiArrayDomain();
   }
#endif // End of USE_PADRE not defined
#endif

// **********************************************************************************
//                           COPY CONSTRUCTOR
// **********************************************************************************

floatArray_Descriptor_Type::floatArray_Descriptor_Type ( const floatArray_Descriptor_Type & X, int Type_Of_Copy )
   : Array_Domain(X.Array_Domain,FALSE,Type_Of_Copy)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::floatArray_Descriptor_Type (const floatArray_Descriptor_Type & X,bool, int) \n");
#endif

  // This may cause data to be set to null initial values and later reset
  // Works with P++ (maybe not A++)
     setDataPointer(NULL);
     Preinitialize_Descriptor();

#if !defined(PPP)
#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from floatArray_Descriptor_Type COPY CONSTRUCTOR");
#endif
#endif
   }

// **********************************************************************************
//                           EQUALS OPERATOR
// **********************************************************************************
#if 0
// This is a more general template function but it does not work with many compilers!
//template<class T , int Template_Dimension>

//template<class S , int SecondDimension>

// floatArray_Descriptor_Type & floatArray_Descriptor_Type::
// operator=( const floatArray_Descriptor_Type & X )
floatArray_Descriptor_Type & floatArray_Descriptor_Type::
operator=( const floatArray_Descriptor_Type<S,SecondDimension> & X )
   {
  // The funtion body is the same as that of the operator= below!
     return *this;
   }
#endif

// operator=( const floatArray_Descriptor_Type & X )
//template<class T , int Template_Dimension>

floatArray_Descriptor_Type & floatArray_Descriptor_Type::
operator=( const floatArray_Descriptor_Type & X )
   {
  // At present this function is only called in the optimization of assignment and temporary use
  // in lazy_statement.C!  NOT TRUE! The Expression template option force frequent calls to this function!
#if COMPILE_DEBUG_STATEMENTS
#if !defined(PPP)
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type<T,int>::operator= (const floatArray_Descriptor_Type<T,int> & X) Array_Data = %p \n",Array_Data);
#endif
#endif

  // Set reference count!
     referenceCount = getReferenceCountBase();

  // For now we don't set any of the pointers in the Array_Descriptor functions
  // these are set in the array class member functions (we will fix this later)
     Array_Domain = X.Array_Domain;

  // Copy Pointers
  // We must be careful about mixing pointer types
  // Array_Data = X.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency ("Called from floatArray_Descriptor_Type::operator=");
#endif

#if COMPILE_DEBUG_STATEMENTS
#if !defined(PPP)
     if (APP_DEBUG > 0)
          printf ("Leaving floatArray_Descriptor_Type::operator= (const floatArray_Descriptor_Type & X) Array_Data = %p \n",Array_Data);
#endif
#endif
     return *this;
   }

#if 0
// Should be inlined since it is used in a lot of places within A++!
//template<class T , int Template_Dimension>

int floatArray_Descriptor_Type::Array_Size () const
   {
     return Array_Domain.Array_Size();
   }
#endif

#if defined(PPP)
//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::isLeftPartition( int Axis ) const
   {
     return Array_Domain.isLeftPartition(Axis);
   }

//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::isMiddlePartition( int Axis ) const
   {
     return Array_Domain.isMiddlePartition(Axis);
   }

//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::isRightPartition( int Axis ) const
   {
     return Array_Domain.isRightPartition(Axis);
   }

//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::isNonPartition( int Axis ) const
   {
     return Array_Domain.isNonPartition(Axis);
   }

#if defined(PPP)

bool
floatArray_Descriptor_Type::isLeftNullArray( int Axis ) const
   {
  // A Null array can be part of the left edge or the right edge of a distributed axis
  // This function is helpful in determining which it is.

     int result = FALSE;

#if 1
     APP_ASSERT (SerialArray != NULL);
     result = Array_Domain.isLeftNullArray(SerialArray->Array_Descriptor.Array_Domain,Axis);
#else
#if defined(USE_PADRE)
     APP_ASSERT (Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     result = Array_Domain.parallelPADRE_DescriptorPointer->isLeftPartition(Axis);
#else
  // printf ("In isLeftNullArray(): Axis = %d \n",Axis);

     APP_ASSERT (SerialArray != NULL);

     if ( SerialArray->isNullArray() == TRUE )
        {
       // printf ("In floatArray_Descriptor_Type::isLeftNullArray(axis): Domain_Dimension = %d \n",Array_Domain.Domain_Dimension);
          if ( Axis < Array_Domain.Domain_Dimension)
             {
               APP_ASSERT(Array_Domain.BlockPartiArrayDomain != NULL);
            // printf ("     gUBnd(BlockPartiArrayDomain,Axis) = %d \n",gUBnd(Array_Domain.BlockPartiArrayDomain,Axis));
            // printf ("     lalbnd(BlockPartiArrayDomain,Axis,Base[Axis],1) = %d \n",lalbnd(Array_Domain.BlockPartiArrayDomain,Axis,Array_Domain.Base[Axis],1));

               result = (gUBnd(Array_Domain.BlockPartiArrayDomain,Axis) < lalbnd(Array_Domain.BlockPartiArrayDomain,Axis,Array_Domain.Base[Axis],1));
             }
            else
             {
            // Along higher axes we want to force isLeftPartition == TRUE
               result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif
#endif

     return result;
   }


bool
floatArray_Descriptor_Type::isRightNullArray( int Axis ) const
   {
  // A Null array can be part of the left edge or the right edge of a distributed axis
  // This function is helpful in determining which it is.

     int result = FALSE;

#if 1
     APP_ASSERT (SerialArray != NULL);
     result = Array_Domain.isLeftNullArray(SerialArray->Array_Descriptor.Array_Domain,Axis);
#else
#if defined(USE_PADRE)
     APP_ASSERT (Array_Domain.parallelPADRE_DescriptorPointer != NULL);
     result = Array_Domain.parallelPADRE_DescriptorPointer->isLeftPartition(Axis);
#else
  // printf ("In isRightNullArray(): Axis = %d \n",Axis);
     if ( isNullArray() == TRUE )
        {
          if ( Axis < Array_Domain.Domain_Dimension)
             {
               result = !isLeftNullArray(Axis);
             }
            else
             {
            // Along higher axes we want to force isLeftPartition == TRUE
               result = TRUE;
             }
        }

  // USE_PADRE not defined
#endif
#endif

     return result;
   }
#endif

//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int floatArray_Descriptor_Type::findProcNum ( int* indexVals ) const
   {
  // Find number of processor where indexVals lives
     return Array_Domain.findProcNum(indexVals);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>
#if 0

int* floatArray_Descriptor_Type::setupProcessorList
   (const intSerialArray& I, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorList
                 (I,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,
                  localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
floatArray_Descriptor_Type::
setupProcessorList(
    int* I_A, int list_size, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorList
                 (I_A,list_size,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
floatArray_Descriptor_Type::
setupProcessorListOnPosition(
     const intSerialArray& I, int& numberOfSupplements, 
     int** supplementLocationsPtr, int** supplementDataPtr, 
     int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorListOnPosition
                  (I,numberOfSupplements,supplementLocationsPtr,
                   supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//---------------------------------------------------------------------
//template<class T , int Template_Dimension>

int*
floatArray_Descriptor_Type::
setupProcessorListOnPosition(
    int* I_A, int list_size, int& numberOfSupplements, 
    int** supplementLocationsPtr, int** supplementDataPtr, 
    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const
   {
     return Array_Domain.setupProcessorListOnPosition
                 (I_A,list_size,numberOfSupplements,supplementLocationsPtr,
                  supplementDataPtr,numberOfLocal,localLocationsPtr,localDataPtr);
   }
//end of setupProcessorList section that is turned off 
#endif
//---------------------------------------------------------------------
#endif

//---------------------------------------------------------------------
#if 0
//template<class T , int Template_Dimension>

int floatArray_Descriptor_Type::getRawDataSize( int Axis ) const
   {
     return Array_Domain.getRawDataSize(Axis);
   }

//template<class T , int Template_Dimension>

void floatArray_Descriptor_Type::getRawDataSize( int* Data_Size ) const
   {
     Array_Domain.getRawDataSize(Data_Size);
   }

//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::
isSameBase ( const floatArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameBase(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::
isSameBound ( const floatArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameBound(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::
isSameStride ( const floatArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameStride(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::
isSameLength ( const floatArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameLength(X.Array_Domain);
   }
#endif

#if defined(PPP)
//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::
isSameGhostBoundaryWidth ( const floatArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameGhostBoundaryWidth(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::
isSameDistribution ( const floatArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSameDistribution(X.Array_Domain);
   }
#endif

#if 0
//template<class T , int Template_Dimension>

bool floatArray_Descriptor_Type::
isSimilar ( const floatArray_Descriptor_Type & X ) const
   {
     return Array_Domain.isSimilar(X.Array_Domain);
   }
#endif

#if 0
// I think this is commented out because it is not required!
//template<class T , int Template_Dimension>

int floatArray_Descriptor_Type::
computeArrayDimension ( ARGUMENT_LIST_MACRO_INTEGER )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of floatArray_Descriptor_Type::computeArrayDimension (%d,%d,%d,%d) \n",
               VARIABLE_LIST_MACRO_INTEGER );
#endif

     APP_ASSERT(Array_Size_L >= 0);
     APP_ASSERT(Array_Size_K >= 0);
     APP_ASSERT(Array_Size_J >= 0);
     APP_ASSERT(Array_Size_I >= 0);

     APP_ASSERT(i >= 0);
     APP_ASSERT(j >= 0);
     APP_ASSERT(k >= 0);
     APP_ASSERT(l >= 0);
#if MAX_ARRAY_DIMENSION > 4
     APP_ASSERT(m >= 0);
#if MAX_ARRAY_DIMENSION > 5
     APP_ASSERT(n >= 0);
#if MAX_ARRAY_DIMENSION > 6
     APP_ASSERT(o >= 0);
#if MAX_ARRAY_DIMENSION > 7
     APP_ASSERT(p >= 0);
#endif
#endif
#endif
#endif

     int Array_Dimension = 0;
     if ( i > 0 ) Array_Dimension = 1;
     if ( j > 1 ) Array_Dimension = 2;
     if ( k > 1 ) Array_Dimension = 3;
     if ( l > 1 ) Array_Dimension = 4;
#if MAX_ARRAY_DIMENSION > 4
     if ( m > 1 ) Array_Dimension = 5;
#if MAX_ARRAY_DIMENSION > 5
     if ( n > 1 ) Array_Dimension = 6;
#if MAX_ARRAY_DIMENSION > 6
     if ( o > 1 ) Array_Dimension = 7;
#if MAX_ARRAY_DIMENSION > 7
     if ( o > 1 ) Array_Dimension = 8;
#if MAX_ARRAY_DIMENSION > 7
     APP_ABORT();
#endif
#endif
#endif
#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of floatArray_Descriptor_Type::computeArrayDimension (%d,%d,%d,%d) -- Array_Dimension = %d \n",
               Array_Size_I,Array_Size_J,Array_Size_K,Array_Size_L,Array_Dimension);
#endif

     return Array_Dimension;
   }
#endif

//template<class T , int Template_Dimension>

int floatArray_Descriptor_Type::
computeArrayDimension ( int* Array_Sizes )
   {
     return Array_Domain_Type::computeArrayDimension(Array_Sizes);
   }

//template<class T , int Template_Dimension>

int floatArray_Descriptor_Type::
computeArrayDimension ( const floatArray_Descriptor_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of floatArray_Descriptor_Type::computeArrayDimension () \n");
#endif

     return Array_Domain_Type::computeArrayDimension(X.Array_Domain);
   }

//template<class T , int Template_Dimension>

void floatArray_Descriptor_Type::
Build_Temporary_By_Example ( const floatArray_Descriptor_Type & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::Build_Temporary_By_Example(floatArray_Descriptor_Type) \n");
     X.Test_Consistency ("Test INPUT: Called from floatArray_Descriptor_Type::Build_Temporary_By_Example");
#endif

     int Original_Array_ID = Array_ID();
     int Example_Array_ID  = X.Array_ID();
     (*this) = X;

  // Restore the original array id
     Array_Domain.setArray_ID (Original_Array_ID);

  // Make sure we return a temporary (so set it so)
     setTemporary(TRUE);

  // ... (4/23/98, kdb) can't reset base here if using indirect addressing
  //  because i n d e x ing intArrays will be incorrect relative to Data_Base ...
     if (!Array_Domain.Uses_Indirect_Addressing)
          setBase(APP_Global_Array_Base);

#if COMPILE_DEBUG_STATEMENTS
     Test_Preliminary_Consistency
          ("Test OUTPUT: Called from floatArray_Descriptor_Type::Build_Temporary_By_Example(floatArray_Descriptor_Type)");
#endif
   }

#if defined(PPP)
//-------------------------------------------------------------------------
// This function is specific for use with P++ indirect addressing!
//template<class T , int Template_Dimension>

void floatArray_Descriptor_Type::
Compute_Local_Index_Arrays ( const floatArray_Descriptor_Type & Original_Descriptor )
   {

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
     {   
        printf ("Inside of floatArray_Descriptor_Type::Compute_Local_Index_Arrays()");
        printf (" (Array_ID = %d)\n",Array_ID());
     }   
#endif

     Array_Domain.Compute_Local_Index_Arrays(Original_Descriptor.Array_Domain);
   }   
//-------------------------------------------------------------------------
#if 0
// This function is specific for use with P++ indirect addressing!
//template<class T , int Template_Dimension>

void floatArray_Descriptor_Type::
Compute_Local_Index_Arrays ( const floatArray_Descriptor_Type & Original_Descriptor )
   {
     int i; // Index value used below

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::Compute_Local_Index_Arrays() (Array_ID = %d)\n",Array_ID());
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
          printf ("Inside of floatArray_Descriptor_Type::Compute_Local_Index_Arrays() -- Caching of local intArray usage not implemented yet! \n");
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
       // Indirect_Addressing_Cache_Descriptor *Cache_Descriptor = Indirect_Addressing_Cache.put (X);
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
               Replicated_Index_intArray[i] = Index_Array[i]->SerialArray;
               Index_Array[i]->SerialArray->incrementReferenceCount();
            // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");
               APP_ASSERT(Replicated_Index_intArray[i]->Array_Descriptor->Is_A_Temporary == FALSE);
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
               int Local_Base  = Original_Descriptor.Data_Base[i] + Original_Descriptor.Base[i];
               int Local_Bound = Original_Descriptor.Data_Base[i] + Original_Descriptor.Bound[i];

            // printf ("Local_Base = %d  Local_Bound = %d \n",Local_Base,Local_Bound);
               if (Valid_Local_Entries_Mask.Array_Descriptor->Is_A_Null_Array)
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
                 // display("THIS Descriptor");
                 // Original_Descriptor.display("ORIGINAL Descriptor");
                 // Replicated_Index_intArray[i]->display("Replicated_Index_intArray[i]");
                    Valid_Local_Entries_Mask.view("Valid_Local_Entries_Mask");
                    APP_ABORT();
                  }
             }
        }

  // Valid_Local_Entries_Mask.display("Inside of floatArray_Descriptor_Type::Compute_Local_Index_Arrays -- Valid_Entries_Mask");

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
               if ( sum ( (*Index_Array[i]->SerialArray) != (*Local_Index_Array[i]) ) != 0 )
                  {
                    printf ("ERROR: On a single processor the Local_Index_Array should match the Index_Array[i]->SerialArray \n");
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
//template<class T , int Template_Dimension>

void floatArray_Descriptor_Type::
Update_Parallel_Information_Using_Old_Descriptor ( const floatArray_Descriptor_Type & Old_Array_Descriptor )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of floatArray_Descriptor_Type::Update_Parallel_Information_Using_Old_Descriptor! \n");
#endif
  // Copy relevant data from old descriptor (this should be a function)

     Array_Domain.Update_Parallel_Information_Using_Old_Descriptor(Old_Array_Descriptor.Array_Domain);
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
//template<class T , int Template_Dimension>

void
floatArray_Descriptor_Type::Test_Preliminary_Consistency( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since floatArray_Descriptor_Type::Test_Preliminary_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of floatArray_Descriptor_Type::Test_Preliminary_Consistency! (Label = %s) (Descriptor_Dimension = %d) \n",
               Label,numberOfDimensions());
#endif

     Array_Domain.Test_Preliminary_Consistency(Label);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Leaving floatArray_Descriptor_Type::Test_Preliminary_Consistency! (Label = %s) \n",Label);
#endif
   }

//template<class T , int Template_Dimension>

void
floatArray_Descriptor_Type::Test_Consistency( const char *Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since floatArray_Descriptor_Type::Test_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Inside of floatArray_Descriptor_Type::Test_Consistency! (Label = %s) (Descriptor_Dimension = %d) \n",
               Label,numberOfDimensions());
#endif

#if defined(PPP)
     if (SerialArray != NULL)
        {
          if (getRawDataReferenceCount() < getRawDataReferenceCountBase())
               printf ("In floatArray_Descriptor_Type::Test_Consistency(): Array_ID() = %d getRawDataReferenceCount() = %d \n",
                    Array_ID(),getRawDataReferenceCount());
          APP_ASSERT (getRawDataReferenceCount() >= getRawDataReferenceCountBase());
        }
#else
     if (getRawDataReferenceCount() < 0)
          printf ("In floatArray_Descriptor_Type::Test_Consistency(): getRawDataReferenceCount() = %d \n",
               getRawDataReferenceCount());
     APP_ASSERT (getRawDataReferenceCount() >= 0);
#endif

#if 0
     printf ("sizeof(array_descriptor)             = %d \n",sizeof(array_descriptor) );
     printf ("sizeof(floatArray_Descriptor_Type) = %d \n",sizeof(floatArray_Descriptor_Type) );
#endif
#if defined(APP) || defined(SERIAL_APP)
     APP_ASSERT ( sizeof(array_descriptor) == sizeof(floatArray_Descriptor_Type) );
#endif

#if defined(PPP)
  // By definition of getRawDataReferenceCount() the following should be TRUE!
     if (SerialArray != NULL)
        {
          APP_ASSERT ( SerialArray->getReferenceCount() == getRawDataReferenceCount() );
        }
#endif

#if !defined(PPP)
  // Reference this variable so that purify can check that it is initialized!
     APP_ASSERT (Array_Data == NULL || Array_Data != NULL);
#endif

  // Reference this variable so that purify can check that it is initialized!
     APP_ASSERT (ExpressionTemplateDataPointer == NULL || ExpressionTemplateDataPointer != NULL);

  // I don't know what this should be tested against to verify correctness!
  // APP_ASSERT (ExpressionTemplateDataPointer == Array_Data + Array_Domain.ExpressionTemplateOffset);

  // I don't know what these should be tested against either!
     APP_ASSERT (Array_View_Pointer0 == NULL || Array_View_Pointer0 != NULL);
#if MAX_ARRAY_DIMENSION>1
     APP_ASSERT (Array_View_Pointer1 == NULL || Array_View_Pointer1 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>2
     APP_ASSERT (Array_View_Pointer2 == NULL || Array_View_Pointer2 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>3
     APP_ASSERT (Array_View_Pointer3 == NULL || Array_View_Pointer3 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>4
     APP_ASSERT (Array_View_Pointer4 == NULL || Array_View_Pointer4 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>5
     APP_ASSERT (Array_View_Pointer5 == NULL || Array_View_Pointer5 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>6
     APP_ASSERT (Array_View_Pointer6 == NULL || Array_View_Pointer6 != NULL);
#endif
#if MAX_ARRAY_DIMENSION>7
     APP_ASSERT (Array_View_Pointer7 == NULL || Array_View_Pointer7 != NULL);
#endif

  // Uncomment this line so that the array domain is tested!
     Array_Domain.Test_Consistency(Label);

#if 0
// We had to put this test into the array object's Test_Consistency member function! 
#if defined(USE_EXPRESSION_TEMPLATES)
     int ExpectedOffset = Array_Domain.Base[0];
     for (int i=1; i < MAX_ARRAY_DIMENSION; i++)
        {
          ExpectedOffset += Array_Domain.Base[i] * Array_Domain.Size[i-1];
        }
     APP_ASSERT (Array_Domain.ExpressionTemplateOffset == ExpectedOffset);
     if (Array_Data != NULL)
        {
          if (ExpressionTemplateDataPointer != Array_Data+ExpectedOffset)
             {
               printf ("Array_Data = %p \n",Array_Data);
               printf ("ExpressionTemplateDataPointer = %p \n",ExpressionTemplateDataPointer);
               printf ("ExpectedOffset = %d \n",ExpectedOffset);
               display("ERROR in floatArray_Descriptor_Type<T,Template_Dimension>::Test_Consistency()");
               APP_ABORT();
             }
          APP_ASSERT (ExpressionTemplateDataPointer == Array_Data+ExpectedOffset);
        }
#endif
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 5)
          printf ("Leaving floatArray_Descriptor_Type::Test_Consistency! (Label = %s) \n",Label);
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
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

// ****************************************************************************
// ****************************************************************************
// **************  ERROR CHECKING FUNCTION FOR INDEXING OPERATORS  ************
// ****************************************************************************
// ****************************************************************************

#if 0
//template<class T , int Template_Dimension>

void floatArray_Descriptor_Type::
Error_Checking_For_Index_Operators (
     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::Error_Checking_For_Index_Operators \n");
#endif

     Array_Domain.Error_Checking_For_Index_Operators(Internal_Index_List);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving floatArray_Descriptor_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }
#endif

#if 0
//template<class T , int Template_Dimension>

void floatArray_Descriptor_Type::
Error_Checking_For_Index_Operators (
     const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::Error_Checking_For_Index_Operators(Indirect_Index_List) \n");
#endif

     Array_Domain.Error_Checking_For_Index_Operators(Indirect_Index_List);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving floatArray_Descriptor_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }
#endif

#if 0
// **************************************************************************
// This function does the bounds checking for the scalar indexing of A++
// array objects.  Its purpose is to localize all the error checking for
// scalar indexing.
// **************************************************************************
// Old prototype
// void floatArray::Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const
//template<class T , int Template_Dimension>

void floatArray_Descriptor_Type::
Error_Checking_For_Scalar_Index_Operators ( 
     const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const
   {
     Array_Domain.Error_Checking_For_Scalar_Index_Operators(Integer_List);
   }
#endif

// *************************************************************************************************
// ********************  APPEND index_operator.C  **************************************************
// *************************************************************************************************

/* Actually this is a little too long for the AT&T compiler to inline! */
//template<class T , int Template_Dimension>

floatArray_Descriptor_Type* floatArray_Descriptor_Type::
Vectorizing_Descriptor ( const floatArray_Descriptor_Type & X )
   {
  // Build a descriptor to fill in!
     floatArray_Descriptor_Type* Return_Descriptor = new floatArray_Descriptor_Type;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
          printf ("Inside of floatArray_Descriptor_Type::Vectorizing_Descriptor ( const floatArray_Descriptor_Type & X ) (this = %p)\n",Return_Descriptor);
#endif

  // Avoid compiler warning about unused input variable
     if (&X);

     printf ("Inside of floatArray_Descriptor_Type::Vectorizing_Descriptor: not sure how to set it up! \n");
     APP_ABORT();

  // Return_Descriptor.Array_Domain = Vectorizing_Domain (X.Array_Domain);

     return Return_Descriptor;
   }


void
floatArray_Descriptor_Type::
Allocate_Array_Data ( bool Force_Memory_Allocation )
   {
  // This function allocates the internal data for the floatArray_Descriptor_Tyep object.  In A++
  // this allocates the the raw float data array.  In P++ it allocates the internal
  // A++ array for the current processor (using the sizes defined in the DARRAY
  // parallel descriptor).
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("*** Allocating the Array_Data (or getting it from the hash table)! \n");
#endif
 
#if defined(PPP)
  // The allocation of data in the parallel environment is especially complicated
  // so this is broken out as a separate function and is implemented only for P++.
     Allocate_Parallel_Array (Force_Memory_Allocation);
#else
     APP_ASSERT(Array_Data == NULL);
     float* Array_Data_Pointer = NULL;

// KCC gives an error about Expression_Tree_Node_Type not being a complete type
// this is because the general use of this function outside of A++ directly
// uses a subset of the header files used to compile A++.
#if !defined(USE_EXPRESSION_TEMPLATES)
  // if (!Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION || Force_Memory_Allocation)
     if (Force_Memory_Allocation)
#else
     if (Force_Memory_Allocation)
#endif
        {
          if (floatArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE)
             {
               Array_Data_Pointer = Hash_Table.Get_Primative_Array ( Array_Size() );
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call MDI_float_Allocate! \n");
#endif
#if !defined(USE_EXPRESSION_TEMPLATES)
               Array_Data_Pointer = MDI_float_Allocate ( (array_domain*)(&Array_Domain) );
#else
            // Need to allocate space without using the MDI layer
            // printf ("Need to allocate space without using the MDI layer! \n");
            // APP_ABORT();

               int Size = Array_Domain.Size[MAX_ARRAY_DIMENSION-1];
               if (Size > 0)
                  {
                    Array_Data_Pointer = (float*) APP_MALLOC ( Size * sizeof(double) );
  
                    if (Array_Data_Pointer == NULL)
                       {
                         printf ("HEAP ERROR: Array_Data_Pointer == NULL in void floatArray_Descriptor_Type::Allocate_Array_Data(bool) (Size = %d)! \n", Size);
                         APP_ABORT();
                       }
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (Size < 0)
                       {
                         printf ("ERROR: Size < 0 Size = %d in void floatArray_Descriptor_Type::Allocate_Array_Data(bool)! \n",Size);
                         APP_ABORT();
                       }
#endif
                    Array_Data_Pointer = NULL;
                  }
#endif
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Allocated Array_Data_Pointer = %p \n",Array_Data_Pointer);
#endif

  // Make the assignment of the new data pointer!
     Array_Data = Array_Data_Pointer;

  // printf ("In A++: setting up the Array_View_Pointer's \n");

  // Now that we have added tests into the Test_Consistancy function
  // (equivalent to that in the scalar indexing) we have to set these 
  // pointers as early as possible
     DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO

  // This test is repeated in the upper level array object's Test_Consistancy member function
     APP_ASSERT (Array_View_Pointer0 == Array_Data + Array_Domain.Scalar_Offset[0]);
     APP_ASSERT (Array_View_Pointer1 == Array_Data + Array_Domain.Scalar_Offset[1]);
     APP_ASSERT (Array_View_Pointer2 == Array_Data + Array_Domain.Scalar_Offset[2]);
     APP_ASSERT (Array_View_Pointer3 == Array_Data + Array_Domain.Scalar_Offset[3]);
     APP_ASSERT (Array_View_Pointer4 == Array_Data + Array_Domain.Scalar_Offset[4]);
     APP_ASSERT (Array_View_Pointer5 == Array_Data + Array_Domain.Scalar_Offset[5]);
#endif
   }
 
#if defined(PPP) && defined(USE_PADRE)

void floatArray_Descriptor_Type::
setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::setLocalDomainInPADRE_Descriptor(%p) \n",inputDomain);
#endif

     if (Array_Domain.parallelPADRE_DescriptorPointer != NULL)
          Array_Domain.parallelPADRE_DescriptorPointer->setLocalDomain(inputDomain);
   }
#endif

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" void MDI_float_Deallocate ( float* Data_Pointer , array_domain* Descriptor );
#endif

void
floatArray_Descriptor_Type::Delete_Array_Data ()
   {
#if defined(PPP)
     floatSerialArray* Data_Pointer = SerialArray;
#else
     float* Data_Pointer = Array_Data;
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Descriptor_Type::Delete_Array_Data Array_ID = %d getRawDataReferenceCount() = %d  Data_Pointer = %p \n",
               Array_ID(),getRawDataReferenceCount(),Data_Pointer);

     if (getRawDataReferenceCount() < 0)
          printf ("getRawDataReferenceCount() = %d \n",getRawDataReferenceCount());
#endif

     APP_ASSERT (getRawDataReferenceCount() >= 0);
 
  // P++ objects must always try to delete their SerialArray objects because then the
  // SerialArray reference count will be decremented (at least) or the SerialArray
  // deleted (at most) within the delete operator.  However we find this feature of the delete
  // operator to be a little too clever and so we should consider removing  from the 
  // decrementation of the reference count from the delete operator and have it done by the user
  // instead.  this would mean that the user would have to take care of incrementing and decrementing
  // the reference count instead of just the incrementing (this is more symetric -- i.e. superior).
  // A++ and SerialArray objects should always decrement their
  // reference count (and then delete the raw data if the reference coutn is less
  // than ZERO) here since there destructor has called this function (for that purpose).
  // Within this system the delete operator takes care of decrementing the reference count!
#if defined(PPP)
  // printf ("In floatArray_Descriptor_Type::Delete_Array_Data shouldn't we call decrementRawDataReferenceCount()??? \n");
  // APP_ABORT();
  // APP_ASSERT (getRawDataReferenceCount() >= 0);
     APP_ASSERT (getRawDataReferenceCount() >= getReferenceCountBase());
  // decrementRawDataReferenceCount();
     bool Delete_Raw_Data = TRUE;
#else
  // APP_ASSERT (getRawDataReferenceCount() >= 0);
     APP_ASSERT (getRawDataReferenceCount() >= getReferenceCountBase());
     decrementRawDataReferenceCount();
  // int Reference_Count_Lower_Bound = 0;
  // bool Delete_Raw_Data = ( getRawDataReferenceCount() < Reference_Count_Lower_Bound );
     bool Delete_Raw_Data = ( getRawDataReferenceCount() < getReferenceCountBase() );
#endif
 
     if ( Delete_Raw_Data )
        {
#if COMPILE_DEBUG_STATEMENTS
          if ( (APP_DEBUG > 1) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Deleting the data since reference count was == getReferenceCountBase()! \n");
#endif
          if (Data_Pointer != NULL)
             {
            // In P++ we delete the reference counted SerialArray and in A++ and Serial_A++ (the lower level)
            // we can optionally save the data for reuse (through a hash table mechanism).
#if defined(PPP)
#if defined(USE_PADRE)
            // We have to remove the references in PADRE to the P++ SerialArray_Domain objects
            // before we remove the SerialArray objects which contain the SerialArray_Domain objects.
               setLocalDomainInPADRE_Descriptor(NULL);
#endif
            // The destructor we decrement the referenceCount in the SerialArray
               Data_Pointer->decrementReferenceCount();
               if (Data_Pointer->getReferenceCount() < getReferenceCountBase())
                    delete Data_Pointer;
               Data_Pointer = NULL;
#else
            // This is done above before the if test -- it is a bug to do it here!
            // decrementRawDataReferenceCount();
               if (floatArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE)
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 2)
                         printf ("Calling Hash_Table.Put_Primative_Array_Into_Storage \n");
#endif
                    Hash_Table.Put_Primative_Array_Into_Storage ( Data_Pointer , Array_Size() );
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("Calling MDI_float_Deallocate! Data_Pointer = %p \n",Data_Pointer);
#endif
                 // MDI_float_Deallocate ( Data_Pointer , (array_domain*)Array_Descriptor.Array_Domain );
                 // MDI_float_Deallocate ( Data_Pointer , &((array_domain)Array_Descriptor.Array_Domain) );
#if !defined(USE_EXPRESSION_TEMPLATES)
                    MDI_float_Deallocate ( Data_Pointer , (array_domain*)(&Array_Domain) );
#else
                 // printf ("Need to free space without calling the MDI layer \n");
                 // APP_ABORT();
                    APP_ASSERT (Data_Pointer != NULL);
                    free (Data_Pointer);
#endif
                  }
#endif
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Data_Pointer == NULL in floatArray::Delete_Array_Data! \n");
#endif
             }
        }
#if COMPILE_DEBUG_STATEMENTS
       else
        {
          if ( (APP_DEBUG > 1) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Skip deallocation of array data since Array_ID = %d getRawDataReferenceCount() = %d Data_Pointer = %p \n",
                    Array_ID(),getRawDataReferenceCount(),Data_Pointer);
        }
#endif
 
#if defined(PPP)
     SerialArray = NULL;
#else
     Array_Data = NULL;
#endif
   }


#if 0


void
floatArray_Descriptor_Type::postAllocationSupport()
   {
  // Support function for allocation of P++ array objects
  // This function modifies the serialArray descriptor to 
  // make it a proper view of the serial array excluding 
  // the ghost boundaries

     bool Generated_A_Null_Array              = FALSE;
     int SerialArray_Cannot_Have_Contiguous_Data = FALSE;

  // Need to set the upper bound on the iteration to MAX_ARRAY_DIMENSION
  // since all the variables for each dimension must be initialized.
     int j;
     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
#if 0
          printf ("(after setBase): getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
               j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));
	  printf ("(after setBase): SerialArray->getRawBase(%d) = %d  getRawBase(%d) = %d  SerialArray->getRawBound(%d) = %d getRawBound(%d) = %d \n",
		  j,SerialArray->getRawBase(j),j,getRawBase(j),j,SerialArray->getRawBound(j),j,getRawBound(j));
#endif

       // This function is used tohide the use of A++/P++ specific range objects and 
       // provide an interface that both the PADRE and nonPADRE versions can call.
       // void adjustSerialDomainForStridedAllocation ( int axis,  const Array_Domain_Type & globalDomain );
          if ( (Array_Domain.Stride [j] > 1) && (SerialArray->isNullArray() == FALSE) )
             {
            // printf ("##### Calling Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");
               SerialArray->Array_Descriptor.Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain);
            // printf ("##### DONE: Array_Domain.adjustSerialDomainForStridedAllocation(j,Array_Domain) \n");
             }

#if 0
#if 0
          printf ("Bases[%d] = %d \n",j,Bases[j]);
          printf ("Array_Domain.getDataBaseVariable(%d) = %d \n",
                   j,Array_Domain.getDataBaseVariable(j));
          printf ("AFTER SerialArray->setBase (%d) --- SerialArray->getRawBase(%d) = %d \n",
                   Bases[j]+Array_Domain.getDataBaseVariable(j),j,SerialArray->getRawBase(j));
          printf ("SerialArray->getDataBaseVariable(%d) = %d \n",
                   j,SerialArray->Array_Descriptor.Array_Domain.getDataBaseVariable(j));
#endif

       // This could be implemented without the function call overhead
       // represented here but this simplifies the initial implementation for now

	  printf ("In Allocate.C: getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
		  j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));
	  printf ("In Allocate.C: getLocalStride(%d) = %d  getStride(%d) = %d \n",
		  j,getLocalStride(j),j,getStride(j));

	  printf ("In Allocate.C: SerialArray->getRawBase(%d) = %d  getRawBase(%d) = %d  SerialArray->getRawBound(%d) = %d getRawBound(%d) = %d \n",
		  j,SerialArray->getRawBase(j),j,getRawBase(j),j,SerialArray->getRawBound(j),j,getRawBound(j));
	  printf ("In Allocate.C: SerialArray->getRawStride(%d) = %d  getRawStride(%d) = %d \n",
		  j,SerialArray->getRawStride(j),j,getRawStride(j));
#endif

       // The left side uses the bases which are relative to a unit stride (a relatively simple case)!
          int Left_Size_Size  = (getLocalBase(j) - getBase(j)) * Array_Domain.Stride[j];

       // The bound is the funky case since the bases are relative to stride 1 and 
       // the bounds are relative to stride.
#if 0
       // int Right_Size_Size = (getBound(j) - getLocalBound(j)) * Array_Domain.Stride[j];
          int Right_Size_Size = 0;
               if (Is_A_Left_Partition == TRUE)
                    Right_Size_Size = (getBound(j) - getLocalBound(j)) * Array_Domain.Stride[j];
	       else
                {
                  int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
                  Right_Size_Size = getBound(j) - tempLocalBound;
                }
#else
       // int tempLocalBound  = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];

       // int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
       // int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

       // printf ("getBase(%d) = %d getLocalBase(%d) = %d getLocalBound(%d) = %d Array_Domain.Stride[%d] = %d \n",
       //      j,getBase(j),j,getLocalBase(j),j,getLocalBound(j),j,Array_Domain.Stride[j]);

       // int tempLocalBound = getBase(j) + (getLocalBound(j) - getBase(j)) * Array_Domain.Stride[j];
       // int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
          int tempLocalUnitStrideBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Array_Domain.Stride[j];
          int tempLocalBound = tempLocalUnitStrideBase + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];

          int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

       // printf ("tempLocalUnitStrideBase = %d \n",tempLocalUnitStrideBase);

          int Right_Size_Size = 0;

          if(SerialArray->isNullArray() == TRUE)
             {
            // printf ("found a null array (locally) so reset tempLocalBound to getLocalBase(%d) = %d \n",j,getLocalBase(j));
	       int nullArrayOnLeftEdge = isLeftNullArray(j);
               if (nullArrayOnLeftEdge == TRUE)
                  {
                 // printf ("Set tempLocalBound to base! \n");
                 // tempLocalBound  = getLocalBase(j);
                    tempLocalBound  = getBase(j);
                 // gUBnd(Array_Domain.BlockPartiArrayDomain,j);
	       
                  }
                 else
                  {
                 // printf ("Set tempLocalBound to bound! \n");
                 // tempLocalBound  = getBound(j);
                 // tempLocalBound  = getBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
                    tempLocalBound  = getBound(j);
                  }

               int rightEdgeSize = tempGlobalBound - tempLocalBound;
               Right_Size_Size = (rightEdgeSize == 0) ? 0 : rightEdgeSize + 1;
             }
            else
             {
            // printf ("NOT A NULL ARRAY (locally) \n");
               Right_Size_Size = (tempGlobalBound - tempLocalBound);
             }
	  
       // printf ("In allocate.C: first instance: j = %d tempLocalBound = %d tempGlobalBound = %d \n",j,tempLocalBound,tempGlobalBound);

       // bugfix (8/15/2000) account for stride
       // int Right_Size_Size = (tempGlobalBound - tempLocalBound) / Array_Domain.Stride[j];
       // int Right_Size_Size = (tempGlobalBound - tempLocalBound);
#endif

       // Do these depend upon the stride in anyway?
          Array_Domain.Left_Number_Of_Points [j] = (Left_Size_Size  >= 0) ? Left_Size_Size  : 0;
          Array_Domain.Right_Number_Of_Points[j] = (Right_Size_Size >= 0) ? Right_Size_Size : 0;

#if 0
          printf ("Left_Size_Size = %d Right_Size_Size = %d \n",Left_Size_Size,Right_Size_Size);
          printf ("tempLocalBound = %d tempGlobalBound = %d \n",tempLocalBound,tempGlobalBound);
	  printf ("Array_Domain.Left_Number_Of_Points [%d] = %d Array_Domain.Right_Number_Of_Points[%d] = %d \n",
               j,Array_Domain.Left_Number_Of_Points [j],
               j,Array_Domain.Right_Number_Of_Points[j]);
#endif

       // Comment out the Global Index since it should already be set properly
       // and this will just screw it up! We have to set it (I think)
#if 0
	  printf ("BEFORE RESET: Array_Domain.Global_Index [%d] = (%d,%d,%d,%s) \n",j,
               Array_Domain.Global_Index [j].getBase(),
               Array_Domain.Global_Index [j].getBound(),
               Array_Domain.Global_Index [j].getStride(),
               Array_Domain.Global_Index[j].getModeString());
#endif
       // Array_Domain.Global_Index [j] = Index (getBase(j),(getBound(j)-getBase(j))+1,1);
       // Use the range object instead
          Array_Domain.Global_Index [j] = Range (getRawBase(j),getRawBound(j),getRawStride(j));
#if 0
	  printf ("AFTER RESET: Array_Domain.Global_Index [%d] = (%d,%d,%d,%s) \n",j,
               Array_Domain.Global_Index [j].getBase(),
               Array_Domain.Global_Index [j].getBound(),
               Array_Domain.Global_Index [j].getStride(),
               Array_Domain.Global_Index[j].getModeString());
#endif

          APP_ASSERT (Array_Domain.Global_Index [j].getMode() != Null_Index);

          int ghostBoundaryWidth = Array_Domain.InternalGhostCellWidth[j];
          APP_ASSERT (ghostBoundaryWidth >= 0);

       // If the data is not contiguous on the global level then it could not 
       // be locally contiguous (unless the partitioning was just right --but 
       // we don't worry about that case) if this processor is on the left or 
       // right and the ghost bundaries are of non-zero width
       // then the left and right processors can not have:
          SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = Array_Domain.Is_Contiguous_Data;

       // Additionally if we have ghost boundaries then these are always hidden 
       // in a view and so the data can't be contiguous
          if (Array_Domain.InternalGhostCellWidth [j] > 0)
               SerialArray_Cannot_Have_Contiguous_Data = TRUE;

       // In this later case since we have to modify the parallel domain object to be consistant
          if (SerialArray_Cannot_Have_Contiguous_Data == TRUE)
             {
            // Note: we are changing the parallel array descriptor (not done much in this function)
               Array_Domain.Is_Contiguous_Data                               = FALSE;
               SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = FALSE;
             }

          int Local_Stride = SerialArray->Array_Descriptor.Array_Domain.Stride [j];

          if (j < Array_Domain.Domain_Dimension)
             {
            // Bugfix (17/10/96)
            // The Local_Mask_Index is set to NULL in the case of repartitioning
            // an array distributed first across one processor and then across 2 processors.
            // The second processor is a NullArray and then it has a valid local array (non nullarray).
            // But the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
            // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
            // setup.  The problem is that it is either not setup or sometimes setup incorrectly.
            // So we have to set it to a non-nullArray as a default.
               if (Array_Domain.Local_Mask_Index[j].getMode() == Null_Index)
                  {
                 // This resets the Local_Mask_Index to be consistant with what it usually is going into 
                 // this function. This provides a definition of Local_Mask_Index consistant with
                 // the global range of the array. We cannot let the Local_Mask_Index be a Null_Index
                 // since it would not allow the isLeftPartition isRightPartition isMiddlePartition call the isNonPartition
                 // member functions and the isNonPartition relies upon the Local_Mask_Index[0] to be properly
                 // setup.
                 // Array_Domain.Local_Mask_Index[j].display("INSIDE OF ALLOCATE PARALLEL ARRAY -- CASE OF NULL_INDEX");
                 // Array_Domain.Local_Mask_Index[j] = Range (Array_Domain.Base[j],Array_Domain.Bound[j]);
                    Array_Domain.Local_Mask_Index[j] = Array_Domain.Global_Index[j];
                  }

               APP_ASSERT (Array_Domain.Local_Mask_Index[j].getMode() != Null_Index);
               APP_ASSERT (Array_Domain.Local_Mask_Index[j].getMode() != All_Index);

               bool Is_A_Left_Partition   = Array_Domain.isLeftPartition  (j);
               bool Is_A_Right_Partition  = Array_Domain.isRightPartition (j);
               bool Is_A_Middle_Partition = Array_Domain.isMiddlePartition(j);

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
               int base_offset  = getBase(j) - getLocalBase(j);

            // Bounds are relative to strides (non-unit strides)
            // int bound_offset = getLocalBound(j) - getBound(j);
            // Convert the bound to a unit stride bound so it can be compared to the value of getBound(j)
#if 0
               int tempLocalBound = 0;
               if (Is_A_Left_Partition == TRUE)
                    tempLocalBound = getLocalBound(j);
                 else
                    tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
#else
   //          int tempLocalBound  = getBase(j) + (getLocalBound(j) - getBase(j)) * Array_Domain.Stride[j];
   //          int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

            // I think these are already computed above!
               int tempLocalBound = getLocalBase(j) + (getLocalBound(j) - getLocalBase(j)) * Array_Domain.Stride[j];
               int tempGlobalBound = getBase(j) + (getBound(j) - getBase(j)) * Array_Domain.Stride[j];

            // printf ("In allocate.C: 2nd instance: j = %d tempLocalBound = %d tempGlobalBound = %d \n",j,tempLocalBound,tempGlobalBound);
#endif

            // Now that we have a unit stride bound we can do the subtraction
               int bound_offset = tempLocalBound - tempGlobalBound;

               int Original_Base_Offset_Of_View  = (Array_Domain.Is_A_View) ? (base_offset>0 ? base_offset : 0) : 0;
               int Original_Bound_Offset_Of_View = (Array_Domain.Is_A_View) ? (bound_offset>0 ? bound_offset : 0) : 0;

            // Need to account for the stride
            // int Count = ((getLocalBound(j) - getLocalBase(j))+1) -
            //             (Original_Base_Offset_Of_View + Original_Bound_Offset_Of_View);
               int Count = ((getLocalBound(j)-getLocalBase(j))+1) -
                           (Original_Base_Offset_Of_View + Original_Bound_Offset_Of_View);
            // Only could the left end once in adjusting for stride!
            // Count = 1 + (Count-1) * Array_Domain.Stride[j];

#if 0
               printf ("Array_Domain.Is_A_View = %s \n",(Array_Domain.Is_A_View == TRUE) ? "TRUE" : "FALSE");

               printf ("(compute Count): getLocalBase(%d) = %d  getBase(%d) = %d  getLocalBound(%d) = %d getBound(%d) = %d \n",
               j,getLocalBase(j),j,getBase(j),j,getLocalBound(j),j,getBound(j));

	       printf ("base_offset = %d  tempGlobalBound = %d tempLocalBound = %d bound_offset = %d \n",
                    base_offset,tempGlobalBound,tempLocalBound,bound_offset);
               printf ("SerialArray->Array_Descriptor.Array_Domain.getBase(%d) = %d \n",
                  j,SerialArray->Array_Descriptor.Array_Domain.getBase(j));
               printf ("SerialArray->Array_Descriptor.Array_Domain.getBound(%d) = %d \n",
                  j,SerialArray->Array_Descriptor.Array_Domain.getBound(j));
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

	       if (!Array_Domain.Is_A_View)
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
               printf ("In Allocate: Final Count = %d \n",Count);
#endif

               if (Count > 0)
                  {
                 // This should have already been set properly (so why reset it?)
                 // Actually it is not set properly all the time at least so we have to reset it.
#if 0
                    printf ("BEFORE 1st RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                 // This should be relative to stride 1 base/bound data since the local mask is relative to the local data directly (unit stride)
                 // Array_Domain.Local_Mask_Index[j] = Index(getLocalBase(j) + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);
		    int tempLocalBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Local_Stride;
                    Array_Domain.Local_Mask_Index[j] = Index(tempLocalBase + Original_Base_Offset_Of_View + Start_Offset, Count, Local_Stride);

#if 0
                    printf ("AFTER 1st RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif

                 // Make the SerialArray a view of the valid portion of the local partition
                    SerialArray->Array_Descriptor.Array_Domain.Is_A_View = TRUE;

                 /*
                 // ... (bug fix, 5/21/96,kdb) bases and bounds need to
                 // be adjusted by Original_Base_Offset_Of_View and
                 // Original_Bound_Offset_Of_View no matter what processor ...
                 */

                    SerialArray->Array_Descriptor.Array_Domain.Base [j] += Original_Base_Offset_Of_View;
                    SerialArray->Array_Descriptor.Array_Domain.Bound[j] -= Original_Bound_Offset_Of_View;
                 // ... bug fix (8/26/96, kdb) User_Base must reflect the view ...
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] += Original_Base_Offset_Of_View;

                    if (!Array_Domain.Is_A_View)
                       {
                         if (Is_A_Left_Partition)
                            {
                              SerialArray->Array_Descriptor.Array_Domain.Base[j] += ghostBoundaryWidth;
                           // ... bug fix (8/26/96, kdb) User_Base must reflect the ghost cell ...
                              SerialArray->Array_Descriptor.Array_Domain.User_Base[j] += ghostBoundaryWidth;
                            }

                         if (Is_A_Right_Partition)
                              SerialArray->Array_Descriptor.Array_Domain.Bound[j] -= ghostBoundaryWidth;
                       }

                 // Bugfix (10/19/95) if we modify the base and bound in the 
                 // descriptor then the data is no longer contiguous
                 // meaning that it is no longer binary conformable
                    if (ghostBoundaryWidth > 0)
                         SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data = FALSE;
                  }
                 else
                  {
                    Generated_A_Null_Array = TRUE;
                  }
             }
            else // j >= Array_Domain.Domain_Dimension
             {
               int Count = (getLocalBound(j)-getLocalBase(j)) + 1;
               if (Count > 0)
                  {
#if 0
                    printf ("BEFORE 2nd RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                 // (6/28/2000) part of fix to permit allocation of strided array data
                 // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,1);
                 // Array_Domain.Local_Mask_Index[j] = Index (getLocalBase(j),Count,Local_Stride);
		    int tempLocalBase = getBase(j) + (getLocalBase(j) - getBase(j)) * Local_Stride;
                    Array_Domain.Local_Mask_Index[j] = Index(tempLocalBase, Count, Local_Stride);

#if 0
                    printf ("AFTER 2nd RESET: Array_Domain.Local_Mask_Index [%d] = (%d,%d,%d) \n",j,
                         Array_Domain.Local_Mask_Index [j].getBase(),
                         Array_Domain.Local_Mask_Index [j].getBound(),
                         Array_Domain.Local_Mask_Index [j].getStride());
#endif
                  }
                 else
                  {
                 // build a null internalIndex for this case
                    Array_Domain.Local_Mask_Index[j] = Index (0,0,1);
                  }
             }

#if 1
       // ... add correct view offset and Scalar Offset ...
          if (j==0)
             {
#if 0
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
                    SerialArray->Array_Descriptor.Array_Domain.Base[0] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[0];
#else
            // bugfix (7/16/2000) The View_Offset is not computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
	            SerialArray->Array_Descriptor.Array_Domain.Base[0];
#endif
            // The Scalar_Offset is computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[0] =
                  - SerialArray->Array_Descriptor.Array_Domain.User_Base[0] *
	            SerialArray->Array_Descriptor.Array_Domain.Stride[0];
             }
            else
             {
#if 0
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
#else
            // The View_Offset is not computed with a stride!
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
#endif
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] =
                    SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j-1] -
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Stride[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
             }
#else
       // ... add correct view offset and Scalar Offset ...
          if (j==0)
             {
               SerialArray->Array_Descriptor.Array_Domain.View_Offset =
	            SerialArray->Array_Descriptor.Array_Domain.Base[0];
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[0] =
                  - SerialArray->Array_Descriptor.Array_Domain.User_Base[0];
             }
            else
             {
               SerialArray->Array_Descriptor.Array_Domain.View_Offset +=
                    SerialArray->Array_Descriptor.Array_Domain.Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
            // Scalar_Offset[temp] = Scalar_Offset[temp-1] - User_Base[temp] * Size[temp-1];
               SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] =
                    SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j-1] -
                    SerialArray->Array_Descriptor.Array_Domain.User_Base[j] *
                    SerialArray->Array_Descriptor.Array_Domain.Size[j-1];
             }
#endif
       // Error checking for same strides in parallel and serial array
          APP_ASSERT ( SerialArray->isNullArray() ||
                       (Array_Domain.Stride [j] == SerialArray->Array_Descriptor.Array_Domain.Stride [j]) );

#if 0
          for (int temp=0; temp < MAX_ARRAY_DIMENSION; temp++)
             {
               printf ("Check values: SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
                    temp,SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[temp]);
             }
#endif

#if 0
       // New test (7/2/2000)
       // In general this test is not always valid (I think it was a mistake to add it - but
       // we might modify it in the future so I will leave it here for now (commented out))
       // This test is not working where the array on one processor is a NullArray (need to fix this test)
          if (SerialArray->isNullArray() == FALSE)
             {
            // recompute the partition information for this axis
               bool Is_A_Left_Partition   = Array_Domain.isLeftPartition  (j);
               bool Is_A_Right_Partition  = Array_Domain.isRightPartition (j);
               bool Is_A_Middle_Partition = Array_Domain.isMiddlePartition(j);

            // Error checking on left and right edges of the partition
               if (Is_A_Left_Partition)
                  {
                    if ( getRawBase(j) != SerialArray->getRawBase(j) )
                       {
                      // display("ERROR: getRawBase(j) != SerialArray->getRawBase(j)");
		         printf ("getRawBase(%d) = %d  SerialArray->getRawBase(%d) = %d \n",
                              j,getRawBase(j),j,SerialArray->getRawBase(j));
                       }
                    APP_ASSERT( getRawBase(j) == SerialArray->getRawBase(j) );
                  }

               if (Is_A_Right_Partition)
                  {
                    if ( getRawBound(j) != SerialArray->getRawBound(j) )
                       {
                      // display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
                      // SerialArray->Array_Descriptor.display("ERROR: getRawBound(j) != SerialArray->getRawBound(j)");
		         printf ("getRawBound(%d) = %d  SerialArray->getRawBound(%d) = %d \n",
                              j,getRawBound(j),j,SerialArray->getRawBound(j));
                       }
                    APP_ASSERT( getRawBound(j) == SerialArray->getRawBound(j) );
                  }
             }
#endif
        } // end of j loop


     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
          if (j == 0)
             {
            // Start out true
               SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = TRUE;
#if 0
               printf ("Initial setting: SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = %s \n",
                    SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");
#endif
             }
            else
             {
            // Start out true
               SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = 
                    SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base && 
                    (SerialArray->Array_Descriptor.Array_Domain.Data_Base[j] == 
                     SerialArray->Array_Descriptor.Array_Domain.Data_Base[j-1]);
#if 0
               printf ("axis = %d Constant_Data_Base = %s \n",
                    j,SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");
#endif
             }
        }

#if 0
     printf ("Final value: SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base = %s \n",
          SerialArray->Array_Descriptor.Array_Domain.Constant_Data_Base ? "TRUE" : "FALSE");

     printf ("SerialArray->Array_Descriptor.Array_Domain.View_Offset = %d \n",
          SerialArray->Array_Descriptor.Array_Domain.View_Offset);
#endif

  // ... add View_Offset to Scalar_Offset (we can't do this inside the
  // loop above because View_Offset is a sum over all dimensions). Also
  // set View_Pointers now. ...
     for (j=0; j < MAX_ARRAY_DIMENSION; j++)
        {
          SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j] +=
               SerialArray->Array_Descriptor.Array_Domain.View_Offset;
       // printf ("SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[%d] = %d \n",
       //      j,SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[j]);
        }

  /* SERIAL_POINTER_LIST_INITIALIZATION_MACRO; */
     DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO;

  // if we have generated a null view of a valid serial array then we have 
  // to make the descriptor conform to some specific rules
  // 1. A Null_Array has to have the Is_Contiguous_Data flag FALSE
  // 2. A Null_Array has to have the Base and Bound 0 and -1 (repectively)
  //    for ALL dimensions
  // 3. The Local_Mask_Index in the Parallel Descriptor must be a Null Index

     if (Generated_A_Null_Array == TRUE)
        {
       // We could call a function here to set the domain to represent a Null Array
          SerialArray->Array_Descriptor.Array_Domain.Is_Contiguous_Data   = FALSE;
          SerialArray->Array_Descriptor.Array_Domain.Is_A_View            = TRUE;
          SerialArray->Array_Descriptor.Array_Domain.Is_A_Null_Array      = TRUE;

          SerialArray->Array_Descriptor.Array_Domain.Constant_Unit_Stride = TRUE;

          int j;
          for (j=0; j < MAX_ARRAY_DIMENSION; j++)
             {
               Array_Domain.Local_Mask_Index   [j] = Index (0,0,1,Null_Index);
               SerialArray->Array_Descriptor.Array_Domain.Base     [j] =  0;
               SerialArray->Array_Descriptor.Array_Domain.Bound    [j] = -1;
               SerialArray->Array_Descriptor.Array_Domain.User_Base[j] =  0;
             }
        }
   }


#endif
 

#undef FLOATARRAY







#define INTARRAY

int intArray_Descriptor_Type::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Descriptor_Type* intArray_Descriptor_Type::Current_Link                      = NULL;

int intArray_Descriptor_Type::Memory_Block_Index                = 0;

const int intArray_Descriptor_Type::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Descriptor_Type::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Descriptor_Type::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Descriptor_Type::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
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

     if (Size != sizeof(intArray_Descriptor_Type))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Descriptor_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Descriptor_Type::operator new: Calling malloc because Size(%d) != sizeof(intArray_Descriptor_Type)(%d) \n",Size,sizeof(intArray_Descriptor_Type));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Descriptor_Type::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Descriptor_Type)(%d) \n",Size,sizeof(intArray_Descriptor_Type));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Descriptor_Type*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Descriptor_Type) );
#else
               Current_Link = (intArray_Descriptor_Type*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Descriptor_Type) ];
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

     intArray_Descriptor_Type* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Descriptor_Type::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Descriptor_Type::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Descriptor_Type::operator delete: Size(%d)  sizeof(intArray_Descriptor_Type)(%d) \n",sizeOfObject,sizeof(intArray_Descriptor_Type));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Descriptor_Type))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Descriptor_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Descriptor_Type::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Descriptor_Type)(%d) \n",sizeOfObject,sizeof(intArray_Descriptor_Type));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Descriptor_Type *New_Link = (intArray_Descriptor_Type*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Descriptor_Type::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Descriptor_Type)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Descriptor_Type));
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
               printf ("ERROR: In intArray_Descriptor_Type::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Descriptor_Type::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#undef INTARRAY

#define DOUBLEARRAY 

int doubleArray_Descriptor_Type::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Descriptor_Type* doubleArray_Descriptor_Type::Current_Link                      = NULL;

int doubleArray_Descriptor_Type::Memory_Block_Index                = 0;

const int doubleArray_Descriptor_Type::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Descriptor_Type::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Descriptor_Type::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Descriptor_Type::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
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

     if (Size != sizeof(doubleArray_Descriptor_Type))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Descriptor_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Descriptor_Type::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Descriptor_Type)(%d) \n",Size,sizeof(doubleArray_Descriptor_Type));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Descriptor_Type::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Descriptor_Type)(%d) \n",Size,sizeof(doubleArray_Descriptor_Type));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Descriptor_Type*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Descriptor_Type) );
#else
               Current_Link = (doubleArray_Descriptor_Type*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Descriptor_Type) ];
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

     doubleArray_Descriptor_Type* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Descriptor_Type::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Descriptor_Type::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Descriptor_Type::operator delete: Size(%d)  sizeof(doubleArray_Descriptor_Type)(%d) \n",sizeOfObject,sizeof(doubleArray_Descriptor_Type));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Descriptor_Type))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Descriptor_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Descriptor_Type::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Descriptor_Type)(%d) \n",sizeOfObject,sizeof(doubleArray_Descriptor_Type));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Descriptor_Type *New_Link = (doubleArray_Descriptor_Type*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Descriptor_Type::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Descriptor_Type)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Descriptor_Type));
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
               printf ("ERROR: In doubleArray_Descriptor_Type::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Descriptor_Type::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#undef DOUBLEARRAY

#define FLOATARRAY

int floatArray_Descriptor_Type::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Descriptor_Type* floatArray_Descriptor_Type::Current_Link                      = NULL;

int floatArray_Descriptor_Type::Memory_Block_Index                = 0;

const int floatArray_Descriptor_Type::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Descriptor_Type::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Descriptor_Type::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Descriptor_Type::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
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

     if (Size != sizeof(floatArray_Descriptor_Type))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Descriptor_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Descriptor_Type::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Descriptor_Type)(%d) \n",Size,sizeof(floatArray_Descriptor_Type));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Descriptor_Type::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Descriptor_Type)(%d) \n",Size,sizeof(floatArray_Descriptor_Type));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Descriptor_Type*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Descriptor_Type) );
#else
               Current_Link = (floatArray_Descriptor_Type*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Descriptor_Type) ];
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

     floatArray_Descriptor_Type* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Descriptor_Type::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Descriptor_Type::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Descriptor_Type::operator delete: Size(%d)  sizeof(floatArray_Descriptor_Type)(%d) \n",sizeOfObject,sizeof(floatArray_Descriptor_Type));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Descriptor_Type))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Descriptor_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Descriptor_Type::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Descriptor_Type)(%d) \n",sizeOfObject,sizeof(floatArray_Descriptor_Type));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Descriptor_Type *New_Link = (floatArray_Descriptor_Type*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Descriptor_Type::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Descriptor_Type)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Descriptor_Type));
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
               printf ("ERROR: In floatArray_Descriptor_Type::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Descriptor_Type::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#undef FLOATARRAY




















