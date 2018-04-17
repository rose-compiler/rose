// Allow repeated includes of descriptor.h without error
#ifndef _APP_DESCRIPTOR_H
#define _APP_DESCRIPTOR_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif



























/*Typed_Array_Descriptor_Class_Definition()*/

#define INTARRAY
// pragma implementation "A++.h"
// pragma implementation "descriptor.h"

// Typedefs used to simplify (and make more clear) the dimension 
// independent function interface of the A++/P++ implementation.
typedef int* Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Index* Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Indirect_Addressing_Index* Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];

typedef int Integer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Index Index_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];

#if 0
#if defined(PPP)
// for P++ we use a Parallel_Access_Descriptor designed jointly 
// with Carl Kesselman - Dan Quinlan - Joel Saltz
// class intArray_Descriptor_Type : Parallel_Access_Descriptor
#else
// for A++ we don't need the Parallel_Access_Descriptor
//class intArray_Descriptor_Type
#endif
#endif

#if defined(PPP)
class Array_Conformability_Info_Type;
#endif

// Avoid unknown type for pointers in the Initialize_Descriptor function
class Internal_Partitioning_Type;

//template<class T, int Templated_Dimension>

class intArray_Descriptor_Type
   {
  // **********************************************************************************************
  // We might only require one descriptor (instead of one for each different type of array object)!
  // This is because we no longer require the use of the Original_Array field in this class!
  // **********************************************************************************************

     friend class Where_Statement_Support;
  // friend class doubleArray;
  // friend class floatArray;
  // friend class intArray;
#if defined(PPP)
     friend double sum ( const doubleArray & X );
     friend float  sum ( const floatArray  & X );
     friend int    sum ( const intArray    & X );
#endif

  // friend doubleArray sum ( const doubleArray & X , int Axis );
  // friend floatArray  sum ( const floatArray  & X , int Axis );
  // friend intArray    sum ( const intArray    & X , int Axis );

     friend doubleArray & transpose ( const doubleArray & X );
     friend floatArray  & transpose ( const floatArray  & X );
     friend intArray    & transpose ( const intArray    & X );

  // Allow access to private variables Top_Of_Stack and Bottom_Of_Stack
     friend class MemoryManagerType;

#if defined(SERIAL_APP) && !defined(PPP)
  // error make doubleArray a friend
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;

  // friend class Partitioning_Type;

  // friend doubleArray sum ( const doubleArray & X , int Axis );
  // friend floatArray  sum ( const floatArray  & X , int Axis );
  // friend intArray    sum ( const intArray    & X , int Axis );
#endif

#if defined(APP) || defined(PPP)
  // error make doubleSerialArray a friend
     friend class Array_Domain_Type;
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class Internal_Partitioning_Type;
  // friend class Partitioning_Type;
#endif

#if defined(PPP)
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;
     friend class Array_Conformability_Info_Type;
#endif

     public:
     // The purpose of use a domain object is to have a non-templated object that
     // any array can easily point to and/or reuse or copy from.  It is in reality what
     // we used to call the Array_Descriptor.  But after we moved the pointers into the
     // Array_Descriptor_Type there remained a need for a form of type independent
     // object that would do what Array_Descriptor used to do.  So we now call this
     // object Array_Domain_Type and we reuse all the same code (and most of the same comments).

     // This is where the data previously in the Array_Descriptor_Type is located
        Array_Domain_Type Array_Domain;

     // Data required for the "new" and "delete" operators!
     // These are not used so much now that the array objects have array descriptors
     // as member data (instead of just pointers to the array descriptors).
     private:
        static intArray_Descriptor_Type *Current_Link;
        intArray_Descriptor_Type        *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

     public:
#if defined(PPP)
     // Pointer to A++ Serial array
     // intSerialArray* SerialArray;
     // SerialArray_Descriptor_Type* SerialArray;
     // SerialArray_Descriptor_Type<T,Templated_Dimension> *SerialArray;
        intSerialArray *SerialArray;

     // Access functions for the serialArray object
        inline const intSerialArray & getSerialArray () const
           { return *SerialArray; }
        inline intSerialArray* getSerialArrayPointer () const
           { return SerialArray; }

        inline void setDataPointer ( intSerialArray *inputPointer )
           { SerialArray = inputPointer; }
#else
     // Pointer to raw data
     // T* APP_RESTRICT_MACRO Array_Data;
     // The type_macro embeds the use of the pointer type with the 
     // use of the restrict keyword (which is not a part of standard C++ or C -- yet)
        int* Array_Data;

        inline int* getArrayData () const
           { return Array_Data; }
        inline void setDataPointer ( int* inputPointer )
           { Array_Data = inputPointer; }
#endif

     // These pointers support the use of views. we require one per dimension
     // because a higher dimensional array object can have a view which is
     // of a lower dimension and then it is best (i.e. makes for the best interface)
     // to allow it to be treated as a lower dimensional array object.

        int* Array_View_Pointer0;
#if MAX_ARRAY_DIMENSION>1
        int* Array_View_Pointer1;
#endif
#if MAX_ARRAY_DIMENSION>2
        int* Array_View_Pointer2;
#endif
#if MAX_ARRAY_DIMENSION>3
        int* Array_View_Pointer3;
#endif
#if MAX_ARRAY_DIMENSION>4
        int* Array_View_Pointer4;
#endif
#if MAX_ARRAY_DIMENSION>5
        int* Array_View_Pointer5;
#endif
#if MAX_ARRAY_DIMENSION>6
        int* Array_View_Pointer6;
#endif
#if MAX_ARRAY_DIMENSION>7
        int* Array_View_Pointer7;
#endif

     // We have to make this available in the non expression template version since
     // it is a handy mechanism for the ROSE preprocessor to use.
     // Basically the initial versions of the rose preprocessor can mimic the
     // expression template implementation (in some ways) but be more efficient.
     // Later versions can be more efficient still.
     // Pointer used for indexing into data within the expression template version
     // This makes the subscript computation within the expression template
     // implementation as simple as possible.
     // T* ExpressionTemplateDataPointer;
        int* ExpressionTemplateDataPointer;

#if defined(USE_EXPRESSION_TEMPLATES)
        void setupIndirectAddressingSupport() const
           { Array_Domain.setupIndirectAddressingSupport(); }

        int computeSubscript ( int axis, int i ) const
           { return Array_Domain.computeSubscript(axis,i); }

        int indirectOffset_1D ( int i ) const
           { return Array_Domain.indirectOffset_1D(i); }

#if (MAX_ARRAY_DIMENSION >= 2)
        int indirectOffset_2D ( int i ) const
           { return Array_Domain.indirectOffset_2D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        int indirectOffset_3D ( int i ) const
           { return Array_Domain.indirectOffset_3D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        int indirectOffset_4D ( int i ) const
           { return Array_Domain.indirectOffset_4D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        int indirectOffset_5D ( int i ) const
           { return Array_Domain.indirectOffset_5D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        int indirectOffset_6D ( int i ) const
           { return Array_Domain.indirectOffset_6D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        int indirectOffset_7D ( int i ) const
           { return Array_Domain.indirectOffset_7D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        int indirectOffset_8D ( int i ) const
           { return Array_Domain.indirectOffset_8D(i); }
#endif

     // Access to pointer specific to expression template indexing
        inline int* getExpressionTemplateDataPointer () const 
            { // APP_ASSERT(ExpressionTemplateDataPointer != NULL);
              return ExpressionTemplateDataPointer; 
            }

// end of defined(USE_EXPRESSION_TEMPLATES)
#endif

        inline int* getDataPointer () const
           {
#if defined(PPP)
          // P++ calls the A++ getArrayPointer function
             return SerialArray->getDataPointer();
#else
          // A++ just returns the data
             return Array_Data;
#endif
           }

     // *************************************************
     // Reference counting data and functions
     private:
          int referenceCount;

     public:
       // Use of reference count access is made available for debugging
       // We don't want this access function to be returning a reference!!!
       // Mostly because it is very dangerous as a point of style!!!
       // inline int& getReferenceCount() { return referenceCount; }   
          inline int getReferenceCount() { return referenceCount; }   

       // Support functions for reference counting (inlined for performance)
          inline void incrementReferenceCount() { referenceCount++; }   
          inline void decrementReferenceCount() { referenceCount--; }   
       // inline void referenceCountedDelete () { delete this; }

          inline void resetRawDataReferenceCount() const
             {
            // When we steal the data and reuse it from a temporary we have to delete it
            // which decrements the reference count to -1 (typically) and then we copy the
            // data pointers.  But we have to reset the reference cout to ZERO as well.
            // This function allows that operation to be insulated from the details of how the
            // reference count is implemented.  It is used in the A++ operator= ( arrayobject )
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::resetRawDataReferenceCount() \n");
#endif

#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
            // This has to be a bug since assignement to a function makes no sense
            // unless the the function returns a reference! (but it DOES return a reference)
            // But the point is that from the P++ view the RawData at the P++ array object is
            // suposed to be the referenceCount of the SerialArray object (not the SerialArray Data's
            // referenceCount)
            // SerialArray->Array_Descriptor.getReferenceCount() = 0;
               SerialArray->resetReferenceCount();
               APP_ASSERT (SerialArray->getReferenceCount() == getReferenceCountBase());
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID] = 0;
               Array_Domain.resetRawDataReferenceCount();
#endif
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
#if defined(SERIAL_APP) || defined(PPP)
               return SerialArray_Domain_Type::getRawDataReferenceCountBase(); 
#else
               return Array_Domain_Type::getRawDataReferenceCountBase(); 
#endif
             }

          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return Array_Domain_Type::getReferenceCountBase(); 
             }

          inline void resetReferenceCount()
             {
           // reset the Reference Count in the array object!
              referenceCount = getReferenceCountBase();
             }

          inline int getRawDataReferenceCount() const
             {
#if defined(PPP)
            // Within P++ we can just use the reference count already provided with the A++ array
            // objects.  This avoids providing two mechanisms for reference counting the SerialArray
            // (A++) objects used within P++.
               APP_ASSERT (SerialArray != NULL);
               return SerialArray->getReferenceCount();
#else
            // Within A++ we cna't store the reference count with the array data so we have to
            // provide a seperate array of reference counts indexed by the Array_IDs.
            // APP_ASSERT( Array_Domain_Type::Array_Reference_Count_Array_Length > Array_Domain.Array_ID );
            // return Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID];
               return Array_Domain.getRawDataReferenceCount();
#endif
             }

          inline void incrementRawDataReferenceCount() const
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::incrementRawDataReferenceCount() \n");
#endif
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
               SerialArray->incrementReferenceCount();
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID]++;
               Array_Domain.incrementRawDataReferenceCount();
#endif
             }

          inline void decrementRawDataReferenceCount() const
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::decrementRawDataReferenceCount() \n");
#endif
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
               SerialArray->decrementReferenceCount();
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID]--;
               Array_Domain.decrementRawDataReferenceCount();
#endif
             }

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

     // *************************************************

     private:
#if (!defined(PPP))
     // Hash Table for array data (raw memory)! Intended to be more efficient on the Cray!
     // The use of the hash table for cacheing raw memory avoids calles to malloc!
        static intArray_Data_Hash_Table Hash_Table;
#endif

     private:
     // Bug fix (5/3/94) fix for time dependent problems that can generate millions of 
     // array operations and in so doing make Array_Reference_Count_Array TOO long!
     // This allows reuse of Array_IDs and so avoids 
     // the unbounded growth of the Array_Reference_Count_Array
     // static Array_ID_Stack Stack_Of_Array_IDs;

     public:
#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
     // This is a mechanism for detection of the last array being deleted so that
     // if specialized debugging is turned on -- A++ internalmemory can be deleted.
        static int getNumberOfArraysInUse() { return Array_Domain_Type::getNumberOfArraysInUse(); }
#endif
        friend void GlobalMemoryRelease();

        void Test_Preliminary_Consistency( const char *Label ) const;
        void Test_Consistency( const char *Label ) const;

     private:
     // This function allocates the array data for the serial and parallel 
     // Array_Descriptor_Type objects. It is the only function which allocates the array data
     // however in the case of the parallel allocation of data this function calls the
     // Allocate_Parallel_Array() function internally.
        void Allocate_Array_Data ( bool Force_Memory_Allocation );
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation );
        void Delete_Array_Data ();

     // friend void APP_Push_Array_ID ( int x );
     // friend inline int  APP_Pop_Array_ID ();

#if !defined(PPP)
        static void Allocate_Larger_Reference_Count_Array ( int New_Size );
#endif
        static void New_Function_Loop ();

        void Initialize_View_Of_Target ( int Axis , int x , 
                                         const intArray_Descriptor_Type & Target_Descriptor );
        void Initialize_View_Of_Target ( int Axis , const Internal_Index & X , 
                                         const intArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , int x , 
                                         const intArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , const Internal_Index & X , 
                                         const intArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , const intArray & X , 
                                         const intArray_Descriptor_Type & Target_Descriptor );
#if defined(PPP)
        void Compute_Local_Index_Arrays ( const intArray_Descriptor_Type & Original_Descriptor );
        void Initialize_Parallel_Parts_Of_Descriptor ( const intArray_Descriptor_Type & X );

     // static int Get_Processor_Position ( int Lhs_Left_Number_Of_Points  , int Rhs_Left_Number_Of_Points  ,
     //                                     int Lhs_Right_Number_Of_Points , int Rhs_Right_Number_Of_Points );
        int Get_Processor_Position ( int Axis ) const;
#endif

#if 0
#if !defined(USE_SAGE)
     // I don't know why Sage has a problem with this -- the input arguments seem to be correct!
        inline void Fixup_Array_Descriptor_For_Copy_Constructor ( const intArray_Descriptor_Type & X )
           { Array_Domain.Fixup_Array_Domain_For_Copy_Constructor(X.Array_Domain); }
#endif
#endif

     public:
     // Used to free memory-in-use internally in intArray_Descriptor_Type objects! 
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();

#ifdef INLINE_FUNCTIONS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     // ********************************************************************************
     // ********************************************************************************
     // *******************  intArray_Descriptor_Type DESTRUCTORS  ********************
     // ********************************************************************************
     // ********************************************************************************

       ~intArray_Descriptor_Type();

     // ********************************************************************************
     // ********************************************************************************
     // *************************  CONSTRUCTOR INITIALIZATION  *************************
     // ********************************************************************************
     // ********************************************************************************

     // This is the only initialization member function required!
     // void Initialize_Descriptor();
        void Preinitialize_Descriptor();

     // void Initialize_Descriptor ( const intArray_Descriptor_Type & X ,
     //   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List, 
     //   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_Non_Indexed_Dimension ( int x , const intArray_Descriptor_Type & X );
     // void Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension ( int x , const intArray_Descriptor_Type & X );
     // void Preinitialize_Descriptor ( const intArray_Descriptor_Type & X );
// if !defined(USE_SAGE)
     // This yields a warning from Sage
        void Initialize_Descriptor ( int Number_Of_Valid_Dimensions ,
                                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List,
                                     const Internal_Partitioning_Type* Internal_Partition = NULL );
     // void Fixup_Descriptor_With_View 
        void reshape
	   ( int Number_Of_Valid_Dimensions ,
	     const int* View_Sizes, const int* View_Bases );
        // {
        //   referenceCount = 0;
        //   Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
        // }
// else
     // This function is not implemented in descriptor.C
     // void Initialize_Descriptor ( int Number_Of_Valid_Dimensions ,
     //                              const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
// endif
     // void Initialize_Descriptor ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
     // void Initialize_Descriptor
     //           ( const intArray_Descriptor_Type & X ,
     //             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // void Initialize_Descriptor
     //           ( const intArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_Descriptor
     //           ( const intArray_Descriptor_Type & X ,
     //             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List, 
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_IndirectAddressing_Descriptor_Using_IndirectAddressing
     //           ( const intArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
     // void Initialize_NonindirectAddressing_Descriptor_Using_IndirectAddressing
     //           ( const intArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );


     // ********************************************************************************
     // ********************************************************************************
     // *******************  intArray_Descriptor_Type CONSTRUCTORS  *******************
     // ********************************************************************************
     // ********************************************************************************

     // ********************************************************************************
     // ************************** A++ Constructors ************************************
     // ********************************************************************************
        intArray_Descriptor_Type();
        intArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size );
        intArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // This takes the optional bool for memory allocation. We may want to have this later.
     // intArray_Descriptor_Type ( ARGUMENT_LIST_MACRO_INTEGER , bool Force_Memory_Allocation );
        intArray_Descriptor_Type ( ARGUMENT_LIST_MACRO_INTEGER );

     // ****************************   COPY CONSTRUCTOR  *******************************
     // intArray_Descriptor_Type ( const intArray_Descriptor_Type & X );
#if 0
     // The KCC compiler can accept this as correct code
     // but the Sun C++ compiler must have templates defined at global scope
     // so we have had to dumb this use of templates down somewhat
        template<class S, int SecondDimension>
        intArray_Descriptor_Type ( const intArray_Descriptor_Type<S,SecondDimension> & X );
#else
     // Sun C++ version
     // intArray_Descriptor_Type ( const intArray_Descriptor_Type & X );
        intArray_Descriptor_Type ( const intArray_Descriptor_Type & X, int Type_Of_Copy = DEEPCOPY );
     // intArray_Descriptor_Type ( const intArray_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );
     // intArray_Descriptor_Type ( const Array_Descriptor_Type<float ,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type ( const Array_Descriptor_Type<int   ,MAX_ARRAY_DIMENSION> & X );
#endif

     // Array_Descriptor_Type ( const Array_Descriptor_Type & X,
     //                           Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type* Index_List );
     // Array_Descriptor_Type ( const Array_Descriptor_Type & X, 
     //                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        intArray_Descriptor_Type ( const Array_Domain_Type & X, 
                                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

     // intArray_Descriptor_Type ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // intArray_Descriptor_Type ( const intArray_Descriptor_Type & X,
     //                           const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
        intArray_Descriptor_Type ( const Array_Domain_Type & X,
                                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

  // We need these because the array object constructors take a different
  // number of parameteres to represent the size in each axis and we
  // have to call constructors for the member objects within the array objects
  // within the preinitialization lists for the array objects.  So we need 
  // constructors for the Array_Descriptor_Type and Array_Domain_Type objects
  // that have a similar interface to the constructors in the array class interface.
  // This simplifies the preinitialization lists.

     intArray_Descriptor_Type ( const Internal_Index & I );
#if (MAX_ARRAY_DIMENSION >= 2)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O, const Internal_Index & P );
#endif
 
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

     intArray_Descriptor_Type( const Array_Domain_Type & X, 
                                bool AvoidBuildingIndirectAddressingView );

  // Constructor for initialization from a string
  // intArray_Descriptor_Type( const char* );
     intArray_Descriptor_Type( const AppString & X );

     // ********************************************************************************
     // ************************ A++ and P++ Constructors ******************************
     // ********************************************************************************
#if defined(APP) || defined(PPP)
        intArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                 const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
                                 const Internal_Partitioning_Type & Internal_Partition );

        intArray_Descriptor_Type( int Number_Of_Valid_Dimensions ,
                                 const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
                                 const Internal_Partitioning_Type & Internal_Partition );

     // This is a redundent constructor!
     // intArray_Descriptor_Type( const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
     //                          const Internal_Partitioning_Type & Internal_Partition );

     // This is a redundent initialization function!
     // void Initialize_Descriptor ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
     //                              const Internal_Partitioning_Type & Internal_Partition );
     intArray_Descriptor_Type ( const Internal_Index & I , 
                                 const Internal_Partitioning_Type & Internal_Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
     intArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O, const Internal_Index & P,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
 
// Error checking
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// endif for defined(APP) || defined(PPP)
#endif

     // ********************************************************************************
     // ********************* Existing Data Constructors *******************************
     // ********************************************************************************
#if defined(PPP)
        intArray_Descriptor_Type ( 
             const int* Data_Pointer, 
             ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE );
#else
        intArray_Descriptor_Type ( const int* Data_Pointer, ARGUMENT_LIST_MACRO_CONST_REF_RANGE );

     // This is a redundent constructor
     // intArray_Descriptor_Type ( const int* Data_Pointer, ARGUMENT_LIST_MACRO_INTEGER);
#endif

#if 0
     // These support the adopt type features for the array objects
     // they are used in the preinitialization lists for the array object constructors
#if defined(PPP)
        intArray_Descriptor_Type( const intSerialArray* SerialArray_Pointer ,
             intArray_Descriptor_Type*
             Array_Descriptor_Pointer, Operand_Storage* Array_Storage_Pointer );
#else
        intArray_Descriptor_Type( const int* Array_Data_Pointer ,
             intArray_Descriptor_Type*
             Array_Descriptor_Pointer , Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if defined(PPP)
#if !defined(USE_PADRE)
        DARRAY* Build_BlockPartiArrayDomain ();
#endif
#if 0
        static void fixupLocalBase ( 
                              intArray_Descriptor_Type & New_Parallel_Descriptor,
                        intSerialArray_Descriptor_Type & New_Serial_Descriptor,
                  const       intArray_Descriptor_Type & Old_Parallel_Descriptor,
                  const intSerialArray_Descriptor_Type & Old_Serial_Descriptor );
        void Update_Parallel_Information_Using_Old_Descriptor ( const intArray_Descriptor_Type & Old_Array_Descriptor );
#endif
#endif

#if defined(APP) || defined(PPP)
        void partition ( const Internal_Partitioning_Type & Internal_Partition );
#endif

#if !defined(USE_SAGE)
     // this yeilds a warning in Sage!

     // function interfaces must be dimension independent to permit arbitrary dimension A++/P++ array objects
     // void Error_Checking_For_Index_Operators (
     //   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndexParameterList, 
     //   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndirectIndexParameterList );
        void Error_Checking_For_Index_Operators (
          const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const
           { Array_Domain.Error_Checking_For_Index_Operators(Internal_Index_List); }
        void Error_Checking_For_Index_Operators (
          const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List ) const
           { Array_Domain.Error_Checking_For_Index_Operators(Indirect_Index_List); }

        void Error_Checking_For_Scalar_Index_Operators ( const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const
           { Array_Domain.Error_Checking_For_Scalar_Index_Operators(Integer_List); }
#endif

     // template<class S, int SecondDimension>
     // intArray_Descriptor_Type & operator= ( const intArray_Descriptor_Type<S,SecondDimension> & X );

        intArray_Descriptor_Type & operator= ( const intArray_Descriptor_Type & X );
     // intArray_Descriptor_Type & operator= ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type & operator= ( const Array_Descriptor_Type<float ,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type & operator= ( const Array_Descriptor_Type<int   ,MAX_ARRAY_DIMENSION> & X );

     // This builds a descriptor to represent a 1D (linear) view any data described by the input descriptor.
        static intArray_Descriptor_Type *Vectorizing_Descriptor ( const intArray_Descriptor_Type & X );

        void Build_Temporary_By_Example ( const intArray_Descriptor_Type & X );

     // void Convert_View_To_Nonview ();

#if defined(USE_EXPRESSION_TEMPLATES)
     // Functions specific to the expression template implementation
        int size() const { return Array_Size(); };
     // int conformsTo () const { return TRUE; };
        int conformsTo ( const Array_Descriptor_Type & X ) const { return TRUE; };
        
     /* SUGGESTION:  Since the expression templates have different demands on
     //              the scalar indexing we should consider combining the 
     //              operations to simplify the operations below (i.e multiply out
     //              offset and seperate the variant parts dependent upon i,j,k, etc. and the
     //              invariant parts which could be stored seperately).
     */
        inline int offset(int i) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i);
        }
        inline int offset0(int i) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i);
        }
#if (MAX_ARRAY_DIMENSION >= 2)
        inline int offset(int i, int j) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j);
        }
        inline int offset0(int i, int j) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        inline int offset(int i, int j, int k) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k);
        }
        inline int offset0(int i, int j, int k) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        inline int offset(int i, int j, int k, int l) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l);
        }
        inline int offset0(int i, int j, int k, int l) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        inline int offset(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m);
        }
        inline int offset0(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        inline int offset(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        inline int offset(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n,o);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n,o);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        inline int offset(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n,o,p);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n,o,p);
        }
#endif
      
     // Indexing function. Used to find the ith item - taking account of strides.
     // if defined(USE_EXPRESSION_TEMPLATES)
#endif

        void setBase ( int New_Base_For_All_Axes );
        void setBase ( int New_Base, int Axis );

        inline int Array_Size () const { return Array_Domain.Array_Size(); }
#if !defined(USE_SAGE)
        inline void getRawDataSize ( int* Sizes ) const { Array_Domain.getRawDataSize(Sizes); }
#endif
        inline Range dimension ( int Axis ) const { return Array_Domain.dimension(Axis); }
        inline Range getFullRange ( int Axis ) const { return Array_Domain.getFullRange(Axis); }
        inline int getBase   ( int Axis ) const { return Array_Domain.getBase(Axis); }
        inline int getBound  ( int Axis ) const { return Array_Domain.getBound(Axis); }
        inline int getStride ( int Axis ) const { return Array_Domain.getStride(Axis); }
        inline int getLength ( int Axis ) const { return Array_Domain.getLength(Axis); }
        inline int getRawDataSize ( int Axis ) const { return Array_Domain.getRawDataSize(Axis); }
        inline int getRawBase   ( int Axis ) const   { return Array_Domain.getRawBase(Axis); }
        inline int getRawBound  ( int Axis ) const   { return Array_Domain.getRawBound(Axis); }
        inline int getRawStride ( int Axis ) const   { return Array_Domain.getRawStride(Axis); }
        inline int getDataBase ( int Axis ) const   
	   { return Array_Domain.getDataBaseVariable (Axis); }
        inline int numberOfDimensions () const { return Array_Domain.numberOfDimensions(); }
        inline int internalNumberOfDimensions () const { return Array_Domain.internalNumberOfDimensions(); }
        inline bool usesIndirectAddressing () const { return Array_Domain.usesIndirectAddressing(); }
        inline void setTemporary( bool x ) const { Array_Domain.setTemporary(x); }
        inline bool isTemporary() const { return Array_Domain.isTemporary(); }
        inline bool isNullArray() const { return Array_Domain.isNullArray(); }
        inline bool isView() const { return Array_Domain.isView(); }
        inline bool isContiguousData() const { return Array_Domain.isContiguousData(); }
        inline int Array_ID() const { return Array_Domain.Array_ID(); }

#if defined(APP) || defined(PPP)
// ********************************************************************
// ********************************************************************
// These functions are specific to P++ and provide informationabout the
// local partition instead of the global distributed array.
// ********************************************************************
// ********************************************************************

#if defined(PPP)
     // Added (7/26/2000)
        inline Internal_Index getLocalMaskIndex  (int i) const { return Array_Domain.getLocalMaskIndex(i); }
        inline Internal_Index getGlobalMaskIndex (int i) const { return Array_Domain.getGlobalMaskIndex(i); }

     // Added (12/13/2000)
        inline int getLeftNumberOfPoints  ( int Axis ) const { return Array_Domain.getLeftNumberOfPoints(Axis);  }
        inline int getRightNumberOfPoints ( int Axis ) const { return Array_Domain.getRightNumberOfPoints(Axis); }
#endif

        inline Range localDimension( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->dimension( Axis );
#else
             return dimension( Axis );
#endif
           }

        inline int getLocalBase( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getBase(Axis);
#else
             return getBase(Axis);
#endif
           }

        inline int getLocalBound( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getBound(Axis);
#else
             return getBound(Axis);
#endif
           }

        inline int getLocalRawBound( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getRawBound(Axis);
#else
             return getRawBound(Axis);
#endif
           }

        inline int getLocalStride( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getStride(Axis);
#else
             return getStride(Axis);
#endif
           }

        inline int getLocalRawStride( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getRawStride(Axis);
#else
             return getRawStride(Axis);
#endif
           }

        inline int getLocalLength( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getLength(Axis);
#else
             return getLength(Axis);
#endif
           }
#endif // End of APP || PPP

#if !defined(USE_SAGE)
     // These generate warning in Sage!
     // Dimension independent query functions
        bool isSameBase   ( const intArray_Descriptor_Type & X ) const { return Array_Domain.isSameBase(X.Array_Domain); }
        bool isSameBound  ( const intArray_Descriptor_Type & X ) const { return Array_Domain.isSameBound(X.Array_Domain); }
        bool isSameStride ( const intArray_Descriptor_Type & X ) const { return Array_Domain.isSameStride(X.Array_Domain); }
        bool isSameLength ( const intArray_Descriptor_Type & X ) const { return Array_Domain.isSameLength(X.Array_Domain); }
#if defined(PPP)
        bool isSameGhostBoundaryWidth (const intArray_Descriptor_Type & X ) const;
        bool isSameDistribution ( const intArray_Descriptor_Type & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const intArray_Descriptor_Type & X ) const { return Array_Domain.isSimilar(X.Array_Domain); }
#endif

     // void expandGhostBoundariesOfRawData ( intSerialArray_Descriptor_Type & X, 
     //                                       ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
     // void expandGhostBoundariesOfRawData ( intSerialArray_Descriptor_Type & X, 
     //                                       Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

        int getGhostBoundaryWidth ( int Axis ) const;
        void setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        void resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE );
        void reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE );

#if defined(PPP)
     // Is a P++ array partitioned along an axis
#if 0
        static bool isPartitioned ( const intArray_Descriptor_Type & Parallel_Descriptor, 
                                       const intSerialArray_Descriptor_Type & Serial_Descriptor, int Axis );
#endif

     private:
        bool isLeftPartition  ( int Axis ) const;
        bool isMiddlePartition( int Axis ) const;
        bool isRightPartition ( int Axis ) const;

     // Case of array object not present on local processor 
     // Since not all processors have to have a partition of every array object
        bool isNonPartition   ( int Axis ) const;

     // Used in allocate.C to determin position of null array objects in distributions
        bool isLeftNullArray ( int Axis ) const;
        bool isRightNullArray( int Axis ) const;

     public:
	int findProcNum (int*) const;
	int* setupProcessorList 
	   (const intSerialArray& I, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (const intSerialArray& I, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorList 
	   (int* I_A,  int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (int* I_A, int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
#endif

     // These are intended for use in the debugging of A++/P++ arrays
     // static int computeArrayDimension ( int Array_Size_I, int Array_Size_J, int Array_Size_K, int Array_Size_L );
        static int computeArrayDimension ( int* Array_Sizes );
        static int computeArrayDimension ( const intArray_Descriptor_Type & X );

        void display ( const char *Label = "" ) const;

#if defined(PPP)
     // These functions are used to get or make the Array_Conformability_Info_Type objects
     // passed between subexpressions in the evaluation of expressions of parallel array objects
        static Array_Conformability_Info_Type* getArray_Conformability_Info ();
#endif

#if !defined(PPP)
        static void Put_Descriptor_Into_Cache ( intArray_Descriptor_Type* & X );

        static intArray_Descriptor_Type *Get_Descriptor_From_Cache ( 
             int Array_Size_I , int Array_Size_J , int Array_Size_K , int Array_Size_L );
        static intArray_Descriptor_Type *Get_Descriptor_From_Cache ( const intArray_Descriptor_Type & X );
        static intArray_Descriptor_Type *Get_Descriptor_From_Cache ();
        intArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I );
        intArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J );
        intArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J , 
                                                             const Internal_Index & K );
        intArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J , 
                                                             const Internal_Index & K , const Internal_Index & L );
#endif

   // functions specific to expression templates
      bool continuousData() const { return Array_Domain.continuousData(); };
   };


#undef INTARRAY

#define DOUBLEARRAY
// pragma implementation "A++.h"
// pragma implementation "descriptor.h"

// Typedefs used to simplify (and make more clear) the dimension 
// independent function interface of the A++/P++ implementation.
typedef int* Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Index* Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Indirect_Addressing_Index* Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];

typedef int Integer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Index Index_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];

#if 0
#if defined(PPP)
// for P++ we use a Parallel_Access_Descriptor designed jointly 
// with Carl Kesselman - Dan Quinlan - Joel Saltz
// class doubleArray_Descriptor_Type : Parallel_Access_Descriptor
#else
// for A++ we don't need the Parallel_Access_Descriptor
//class doubleArray_Descriptor_Type
#endif
#endif

#if defined(PPP)
class Array_Conformability_Info_Type;
#endif

// Avoid unknown type for pointers in the Initialize_Descriptor function
class Internal_Partitioning_Type;

//template<class T, int Templated_Dimension>

class doubleArray_Descriptor_Type
   {
  // **********************************************************************************************
  // We might only require one descriptor (instead of one for each different type of array object)!
  // This is because we no longer require the use of the Original_Array field in this class!
  // **********************************************************************************************

     friend class Where_Statement_Support;
  // friend class doubleArray;
  // friend class floatArray;
  // friend class intArray;
#if defined(PPP)
     friend double sum ( const doubleArray & X );
     friend float  sum ( const floatArray  & X );
     friend int    sum ( const intArray    & X );
#endif

  // friend doubleArray sum ( const doubleArray & X , int Axis );
  // friend floatArray  sum ( const floatArray  & X , int Axis );
  // friend intArray    sum ( const intArray    & X , int Axis );

     friend doubleArray & transpose ( const doubleArray & X );
     friend floatArray  & transpose ( const floatArray  & X );
     friend intArray    & transpose ( const intArray    & X );

  // Allow access to private variables Top_Of_Stack and Bottom_Of_Stack
     friend class MemoryManagerType;

#if defined(SERIAL_APP) && !defined(PPP)
  // error make doubleArray a friend
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;

  // friend class Partitioning_Type;

  // friend doubleArray sum ( const doubleArray & X , int Axis );
  // friend floatArray  sum ( const floatArray  & X , int Axis );
  // friend intArray    sum ( const intArray    & X , int Axis );
#endif

#if defined(APP) || defined(PPP)
  // error make doubleSerialArray a friend
     friend class Array_Domain_Type;
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class Internal_Partitioning_Type;
  // friend class Partitioning_Type;
#endif

#if defined(PPP)
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;
     friend class Array_Conformability_Info_Type;
#endif

     public:
     // The purpose of use a domain object is to have a non-templated object that
     // any array can easily point to and/or reuse or copy from.  It is in reality what
     // we used to call the Array_Descriptor.  But after we moved the pointers into the
     // Array_Descriptor_Type there remained a need for a form of type independent
     // object that would do what Array_Descriptor used to do.  So we now call this
     // object Array_Domain_Type and we reuse all the same code (and most of the same comments).

     // This is where the data previously in the Array_Descriptor_Type is located
        Array_Domain_Type Array_Domain;

     // Data required for the "new" and "delete" operators!
     // These are not used so much now that the array objects have array descriptors
     // as member data (instead of just pointers to the array descriptors).
     private:
        static doubleArray_Descriptor_Type *Current_Link;
        doubleArray_Descriptor_Type        *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

     public:
#if defined(PPP)
     // Pointer to A++ Serial array
     // doubleSerialArray* SerialArray;
     // SerialArray_Descriptor_Type* SerialArray;
     // SerialArray_Descriptor_Type<T,Templated_Dimension> *SerialArray;
        doubleSerialArray *SerialArray;

     // Access functions for the serialArray object
        inline const doubleSerialArray & getSerialArray () const
           { return *SerialArray; }
        inline doubleSerialArray* getSerialArrayPointer () const
           { return SerialArray; }

        inline void setDataPointer ( doubleSerialArray *inputPointer )
           { SerialArray = inputPointer; }
#else
     // Pointer to raw data
     // T* APP_RESTRICT_MACRO Array_Data;
     // The type_macro embeds the use of the pointer type with the 
     // use of the restrict keyword (which is not a part of standard C++ or C -- yet)
        double* Array_Data;

        inline double* getArrayData () const
           { return Array_Data; }
        inline void setDataPointer ( double* inputPointer )
           { Array_Data = inputPointer; }
#endif

     // These pointers support the use of views. we require one per dimension
     // because a higher dimensional array object can have a view which is
     // of a lower dimension and then it is best (i.e. makes for the best interface)
     // to allow it to be treated as a lower dimensional array object.

        double* Array_View_Pointer0;
#if MAX_ARRAY_DIMENSION>1
        double* Array_View_Pointer1;
#endif
#if MAX_ARRAY_DIMENSION>2
        double* Array_View_Pointer2;
#endif
#if MAX_ARRAY_DIMENSION>3
        double* Array_View_Pointer3;
#endif
#if MAX_ARRAY_DIMENSION>4
        double* Array_View_Pointer4;
#endif
#if MAX_ARRAY_DIMENSION>5
        double* Array_View_Pointer5;
#endif
#if MAX_ARRAY_DIMENSION>6
        double* Array_View_Pointer6;
#endif
#if MAX_ARRAY_DIMENSION>7
        double* Array_View_Pointer7;
#endif

     // We have to make this available in the non expression template version since
     // it is a handy mechanism for the ROSE preprocessor to use.
     // Basically the initial versions of the rose preprocessor can mimic the
     // expression template implementation (in some ways) but be more efficient.
     // Later versions can be more efficient still.
     // Pointer used for indexing into data within the expression template version
     // This makes the subscript computation within the expression template
     // implementation as simple as possible.
     // T* ExpressionTemplateDataPointer;
        double* ExpressionTemplateDataPointer;

#if defined(USE_EXPRESSION_TEMPLATES)
        void setupIndirectAddressingSupport() const
           { Array_Domain.setupIndirectAddressingSupport(); }

        int computeSubscript ( int axis, int i ) const
           { return Array_Domain.computeSubscript(axis,i); }

        int indirectOffset_1D ( int i ) const
           { return Array_Domain.indirectOffset_1D(i); }

#if (MAX_ARRAY_DIMENSION >= 2)
        int indirectOffset_2D ( int i ) const
           { return Array_Domain.indirectOffset_2D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        int indirectOffset_3D ( int i ) const
           { return Array_Domain.indirectOffset_3D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        int indirectOffset_4D ( int i ) const
           { return Array_Domain.indirectOffset_4D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        int indirectOffset_5D ( int i ) const
           { return Array_Domain.indirectOffset_5D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        int indirectOffset_6D ( int i ) const
           { return Array_Domain.indirectOffset_6D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        int indirectOffset_7D ( int i ) const
           { return Array_Domain.indirectOffset_7D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        int indirectOffset_8D ( int i ) const
           { return Array_Domain.indirectOffset_8D(i); }
#endif

     // Access to pointer specific to expression template indexing
        inline double* getExpressionTemplateDataPointer () const 
            { // APP_ASSERT(ExpressionTemplateDataPointer != NULL);
              return ExpressionTemplateDataPointer; 
            }

// end of defined(USE_EXPRESSION_TEMPLATES)
#endif

        inline double* getDataPointer () const
           {
#if defined(PPP)
          // P++ calls the A++ getArrayPointer function
             return SerialArray->getDataPointer();
#else
          // A++ just returns the data
             return Array_Data;
#endif
           }

     // *************************************************
     // Reference counting data and functions
     private:
          int referenceCount;

     public:
       // Use of reference count access is made available for debugging
       // We don't want this access function to be returning a reference!!!
       // Mostly because it is very dangerous as a point of style!!!
       // inline int& getReferenceCount() { return referenceCount; }   
          inline int getReferenceCount() { return referenceCount; }   

       // Support functions for reference counting (inlined for performance)
          inline void incrementReferenceCount() { referenceCount++; }   
          inline void decrementReferenceCount() { referenceCount--; }   
       // inline void referenceCountedDelete () { delete this; }

          inline void resetRawDataReferenceCount() const
             {
            // When we steal the data and reuse it from a temporary we have to delete it
            // which decrements the reference count to -1 (typically) and then we copy the
            // data pointers.  But we have to reset the reference cout to ZERO as well.
            // This function allows that operation to be insulated from the details of how the
            // reference count is implemented.  It is used in the A++ operator= ( arrayobject )
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::resetRawDataReferenceCount() \n");
#endif

#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
            // This has to be a bug since assignement to a function makes no sense
            // unless the the function returns a reference! (but it DOES return a reference)
            // But the point is that from the P++ view the RawData at the P++ array object is
            // suposed to be the referenceCount of the SerialArray object (not the SerialArray Data's
            // referenceCount)
            // SerialArray->Array_Descriptor.getReferenceCount() = 0;
               SerialArray->resetReferenceCount();
               APP_ASSERT (SerialArray->getReferenceCount() == getReferenceCountBase());
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID] = 0;
               Array_Domain.resetRawDataReferenceCount();
#endif
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
#if defined(SERIAL_APP) || defined(PPP)
               return SerialArray_Domain_Type::getRawDataReferenceCountBase(); 
#else
               return Array_Domain_Type::getRawDataReferenceCountBase(); 
#endif
             }

          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return Array_Domain_Type::getReferenceCountBase(); 
             }

          inline void resetReferenceCount()
             {
           // reset the Reference Count in the array object!
              referenceCount = getReferenceCountBase();
             }

          inline int getRawDataReferenceCount() const
             {
#if defined(PPP)
            // Within P++ we can just use the reference count already provided with the A++ array
            // objects.  This avoids providing two mechanisms for reference counting the SerialArray
            // (A++) objects used within P++.
               APP_ASSERT (SerialArray != NULL);
               return SerialArray->getReferenceCount();
#else
            // Within A++ we cna't store the reference count with the array data so we have to
            // provide a seperate array of reference counts indexed by the Array_IDs.
            // APP_ASSERT( Array_Domain_Type::Array_Reference_Count_Array_Length > Array_Domain.Array_ID );
            // return Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID];
               return Array_Domain.getRawDataReferenceCount();
#endif
             }

          inline void incrementRawDataReferenceCount() const
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::incrementRawDataReferenceCount() \n");
#endif
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
               SerialArray->incrementReferenceCount();
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID]++;
               Array_Domain.incrementRawDataReferenceCount();
#endif
             }

          inline void decrementRawDataReferenceCount() const
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::decrementRawDataReferenceCount() \n");
#endif
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
               SerialArray->decrementReferenceCount();
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID]--;
               Array_Domain.decrementRawDataReferenceCount();
#endif
             }

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

     // *************************************************

     private:
#if (!defined(PPP))
     // Hash Table for array data (raw memory)! Intended to be more efficient on the Cray!
     // The use of the hash table for cacheing raw memory avoids calles to malloc!
        static doubleArray_Data_Hash_Table Hash_Table;
#endif

     private:
     // Bug fix (5/3/94) fix for time dependent problems that can generate millions of 
     // array operations and in so doing make Array_Reference_Count_Array TOO long!
     // This allows reuse of Array_IDs and so avoids 
     // the unbounded growth of the Array_Reference_Count_Array
     // static Array_ID_Stack Stack_Of_Array_IDs;

     public:
#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
     // This is a mechanism for detection of the last array being deleted so that
     // if specialized debugging is turned on -- A++ internalmemory can be deleted.
        static int getNumberOfArraysInUse() { return Array_Domain_Type::getNumberOfArraysInUse(); }
#endif
        friend void GlobalMemoryRelease();

        void Test_Preliminary_Consistency( const char *Label ) const;
        void Test_Consistency( const char *Label ) const;

     private:
     // This function allocates the array data for the serial and parallel 
     // Array_Descriptor_Type objects. It is the only function which allocates the array data
     // however in the case of the parallel allocation of data this function calls the
     // Allocate_Parallel_Array() function internally.
        void Allocate_Array_Data ( bool Force_Memory_Allocation );
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation );
        void Delete_Array_Data ();

     // friend void APP_Push_Array_ID ( int x );
     // friend inline int  APP_Pop_Array_ID ();

#if !defined(PPP)
        static void Allocate_Larger_Reference_Count_Array ( int New_Size );
#endif
        static void New_Function_Loop ();

        void Initialize_View_Of_Target ( int Axis , int x , 
                                         const doubleArray_Descriptor_Type & Target_Descriptor );
        void Initialize_View_Of_Target ( int Axis , const Internal_Index & X , 
                                         const doubleArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , int x , 
                                         const doubleArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , const Internal_Index & X , 
                                         const doubleArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , const intArray & X , 
                                         const doubleArray_Descriptor_Type & Target_Descriptor );
#if defined(PPP)
        void Compute_Local_Index_Arrays ( const doubleArray_Descriptor_Type & Original_Descriptor );
        void Initialize_Parallel_Parts_Of_Descriptor ( const doubleArray_Descriptor_Type & X );

     // static int Get_Processor_Position ( int Lhs_Left_Number_Of_Points  , int Rhs_Left_Number_Of_Points  ,
     //                                     int Lhs_Right_Number_Of_Points , int Rhs_Right_Number_Of_Points );
        int Get_Processor_Position ( int Axis ) const;
#endif

#if 0
#if !defined(USE_SAGE)
     // I don't know why Sage has a problem with this -- the input arguments seem to be correct!
        inline void Fixup_Array_Descriptor_For_Copy_Constructor ( const doubleArray_Descriptor_Type & X )
           { Array_Domain.Fixup_Array_Domain_For_Copy_Constructor(X.Array_Domain); }
#endif
#endif

     public:
     // Used to free memory-in-use internally in doubleArray_Descriptor_Type objects! 
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();

#ifdef INLINE_FUNCTIONS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     // ********************************************************************************
     // ********************************************************************************
     // *******************  doubleArray_Descriptor_Type DESTRUCTORS  ********************
     // ********************************************************************************
     // ********************************************************************************

       ~doubleArray_Descriptor_Type();

     // ********************************************************************************
     // ********************************************************************************
     // *************************  CONSTRUCTOR INITIALIZATION  *************************
     // ********************************************************************************
     // ********************************************************************************

     // This is the only initialization member function required!
     // void Initialize_Descriptor();
        void Preinitialize_Descriptor();

     // void Initialize_Descriptor ( const doubleArray_Descriptor_Type & X ,
     //   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List, 
     //   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_Non_Indexed_Dimension ( int x , const doubleArray_Descriptor_Type & X );
     // void Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension ( int x , const doubleArray_Descriptor_Type & X );
     // void Preinitialize_Descriptor ( const doubleArray_Descriptor_Type & X );
// if !defined(USE_SAGE)
     // This yields a warning from Sage
        void Initialize_Descriptor ( int Number_Of_Valid_Dimensions ,
                                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List,
                                     const Internal_Partitioning_Type* Internal_Partition = NULL );
     // void Fixup_Descriptor_With_View 
        void reshape
	   ( int Number_Of_Valid_Dimensions ,
	     const int* View_Sizes, const int* View_Bases );
        // {
        //   referenceCount = 0;
        //   Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
        // }
// else
     // This function is not implemented in descriptor.C
     // void Initialize_Descriptor ( int Number_Of_Valid_Dimensions ,
     //                              const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
// endif
     // void Initialize_Descriptor ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
     // void Initialize_Descriptor
     //           ( const doubleArray_Descriptor_Type & X ,
     //             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // void Initialize_Descriptor
     //           ( const doubleArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_Descriptor
     //           ( const doubleArray_Descriptor_Type & X ,
     //             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List, 
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_IndirectAddressing_Descriptor_Using_IndirectAddressing
     //           ( const doubleArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
     // void Initialize_NonindirectAddressing_Descriptor_Using_IndirectAddressing
     //           ( const doubleArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );


     // ********************************************************************************
     // ********************************************************************************
     // *******************  doubleArray_Descriptor_Type CONSTRUCTORS  *******************
     // ********************************************************************************
     // ********************************************************************************

     // ********************************************************************************
     // ************************** A++ Constructors ************************************
     // ********************************************************************************
        doubleArray_Descriptor_Type();
        doubleArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size );
        doubleArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // This takes the optional bool for memory allocation. We may want to have this later.
     // doubleArray_Descriptor_Type ( ARGUMENT_LIST_MACRO_INTEGER , bool Force_Memory_Allocation );
        doubleArray_Descriptor_Type ( ARGUMENT_LIST_MACRO_INTEGER );

     // ****************************   COPY CONSTRUCTOR  *******************************
     // doubleArray_Descriptor_Type ( const doubleArray_Descriptor_Type & X );
#if 0
     // The KCC compiler can accept this as correct code
     // but the Sun C++ compiler must have templates defined at global scope
     // so we have had to dumb this use of templates down somewhat
        template<class S, int SecondDimension>
        doubleArray_Descriptor_Type ( const doubleArray_Descriptor_Type<S,SecondDimension> & X );
#else
     // Sun C++ version
     // doubleArray_Descriptor_Type ( const doubleArray_Descriptor_Type & X );
        doubleArray_Descriptor_Type ( const doubleArray_Descriptor_Type & X, int Type_Of_Copy = DEEPCOPY );
     // doubleArray_Descriptor_Type ( const doubleArray_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );
     // doubleArray_Descriptor_Type ( const Array_Descriptor_Type<float ,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type ( const Array_Descriptor_Type<int   ,MAX_ARRAY_DIMENSION> & X );
#endif

     // Array_Descriptor_Type ( const Array_Descriptor_Type & X,
     //                           Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type* Index_List );
     // Array_Descriptor_Type ( const Array_Descriptor_Type & X, 
     //                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        doubleArray_Descriptor_Type ( const Array_Domain_Type & X, 
                                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

     // doubleArray_Descriptor_Type ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // doubleArray_Descriptor_Type ( const doubleArray_Descriptor_Type & X,
     //                           const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
        doubleArray_Descriptor_Type ( const Array_Domain_Type & X,
                                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

  // We need these because the array object constructors take a different
  // number of parameteres to represent the size in each axis and we
  // have to call constructors for the member objects within the array objects
  // within the preinitialization lists for the array objects.  So we need 
  // constructors for the Array_Descriptor_Type and Array_Domain_Type objects
  // that have a similar interface to the constructors in the array class interface.
  // This simplifies the preinitialization lists.

     doubleArray_Descriptor_Type ( const Internal_Index & I );
#if (MAX_ARRAY_DIMENSION >= 2)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O, const Internal_Index & P );
#endif
 
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

     doubleArray_Descriptor_Type( const Array_Domain_Type & X, 
                                bool AvoidBuildingIndirectAddressingView );

  // Constructor for initialization from a string
  // doubleArray_Descriptor_Type( const char* );
     doubleArray_Descriptor_Type( const AppString & X );

     // ********************************************************************************
     // ************************ A++ and P++ Constructors ******************************
     // ********************************************************************************
#if defined(APP) || defined(PPP)
        doubleArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                 const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
                                 const Internal_Partitioning_Type & Internal_Partition );

        doubleArray_Descriptor_Type( int Number_Of_Valid_Dimensions ,
                                 const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
                                 const Internal_Partitioning_Type & Internal_Partition );

     // This is a redundent constructor!
     // doubleArray_Descriptor_Type( const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
     //                          const Internal_Partitioning_Type & Internal_Partition );

     // This is a redundent initialization function!
     // void Initialize_Descriptor ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
     //                              const Internal_Partitioning_Type & Internal_Partition );
     doubleArray_Descriptor_Type ( const Internal_Index & I , 
                                 const Internal_Partitioning_Type & Internal_Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
     doubleArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O, const Internal_Index & P,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
 
// Error checking
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// endif for defined(APP) || defined(PPP)
#endif

     // ********************************************************************************
     // ********************* Existing Data Constructors *******************************
     // ********************************************************************************
#if defined(PPP)
        doubleArray_Descriptor_Type ( 
             const double* Data_Pointer, 
             ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE );
#else
        doubleArray_Descriptor_Type ( const double* Data_Pointer, ARGUMENT_LIST_MACRO_CONST_REF_RANGE );

     // This is a redundent constructor
     // doubleArray_Descriptor_Type ( const double* Data_Pointer, ARGUMENT_LIST_MACRO_INTEGER);
#endif

#if 0
     // These support the adopt type features for the array objects
     // they are used in the preinitialization lists for the array object constructors
#if defined(PPP)
        doubleArray_Descriptor_Type( const doubleSerialArray* SerialArray_Pointer ,
             doubleArray_Descriptor_Type*
             Array_Descriptor_Pointer, Operand_Storage* Array_Storage_Pointer );
#else
        doubleArray_Descriptor_Type( const double* Array_Data_Pointer ,
             doubleArray_Descriptor_Type*
             Array_Descriptor_Pointer , Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if defined(PPP)
#if !defined(USE_PADRE)
        DARRAY* Build_BlockPartiArrayDomain ();
#endif
#if 0
        static void fixupLocalBase ( 
                              doubleArray_Descriptor_Type & New_Parallel_Descriptor,
                        doubleSerialArray_Descriptor_Type & New_Serial_Descriptor,
                  const       doubleArray_Descriptor_Type & Old_Parallel_Descriptor,
                  const doubleSerialArray_Descriptor_Type & Old_Serial_Descriptor );
        void Update_Parallel_Information_Using_Old_Descriptor ( const doubleArray_Descriptor_Type & Old_Array_Descriptor );
#endif
#endif

#if defined(APP) || defined(PPP)
        void partition ( const Internal_Partitioning_Type & Internal_Partition );
#endif

#if !defined(USE_SAGE)
     // this yeilds a warning in Sage!

     // function interfaces must be dimension independent to permit arbitrary dimension A++/P++ array objects
     // void Error_Checking_For_Index_Operators (
     //   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndexParameterList, 
     //   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndirectIndexParameterList );
        void Error_Checking_For_Index_Operators (
          const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const
           { Array_Domain.Error_Checking_For_Index_Operators(Internal_Index_List); }
        void Error_Checking_For_Index_Operators (
          const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List ) const
           { Array_Domain.Error_Checking_For_Index_Operators(Indirect_Index_List); }

        void Error_Checking_For_Scalar_Index_Operators ( const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const
           { Array_Domain.Error_Checking_For_Scalar_Index_Operators(Integer_List); }
#endif

     // template<class S, int SecondDimension>
     // doubleArray_Descriptor_Type & operator= ( const doubleArray_Descriptor_Type<S,SecondDimension> & X );

        doubleArray_Descriptor_Type & operator= ( const doubleArray_Descriptor_Type & X );
     // doubleArray_Descriptor_Type & operator= ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type & operator= ( const Array_Descriptor_Type<float ,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type & operator= ( const Array_Descriptor_Type<int   ,MAX_ARRAY_DIMENSION> & X );

     // This builds a descriptor to represent a 1D (linear) view any data described by the input descriptor.
        static doubleArray_Descriptor_Type *Vectorizing_Descriptor ( const doubleArray_Descriptor_Type & X );

        void Build_Temporary_By_Example ( const doubleArray_Descriptor_Type & X );

     // void Convert_View_To_Nonview ();

#if defined(USE_EXPRESSION_TEMPLATES)
     // Functions specific to the expression template implementation
        int size() const { return Array_Size(); };
     // int conformsTo () const { return TRUE; };
        int conformsTo ( const Array_Descriptor_Type & X ) const { return TRUE; };
        
     /* SUGGESTION:  Since the expression templates have different demands on
     //              the scalar indexing we should consider combining the 
     //              operations to simplify the operations below (i.e multiply out
     //              offset and seperate the variant parts dependent upon i,j,k, etc. and the
     //              invariant parts which could be stored seperately).
     */
        inline int offset(int i) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i);
        }
        inline int offset0(int i) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i);
        }
#if (MAX_ARRAY_DIMENSION >= 2)
        inline int offset(int i, int j) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j);
        }
        inline int offset0(int i, int j) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        inline int offset(int i, int j, int k) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k);
        }
        inline int offset0(int i, int j, int k) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        inline int offset(int i, int j, int k, int l) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l);
        }
        inline int offset0(int i, int j, int k, int l) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        inline int offset(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m);
        }
        inline int offset0(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        inline int offset(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        inline int offset(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n,o);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n,o);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        inline int offset(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n,o,p);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n,o,p);
        }
#endif
      
     // Indexing function. Used to find the ith item - taking account of strides.
     // if defined(USE_EXPRESSION_TEMPLATES)
#endif

        void setBase ( int New_Base_For_All_Axes );
        void setBase ( int New_Base, int Axis );

        inline int Array_Size () const { return Array_Domain.Array_Size(); }
#if !defined(USE_SAGE)
        inline void getRawDataSize ( int* Sizes ) const { Array_Domain.getRawDataSize(Sizes); }
#endif
        inline Range dimension ( int Axis ) const { return Array_Domain.dimension(Axis); }
        inline Range getFullRange ( int Axis ) const { return Array_Domain.getFullRange(Axis); }
        inline int getBase   ( int Axis ) const { return Array_Domain.getBase(Axis); }
        inline int getBound  ( int Axis ) const { return Array_Domain.getBound(Axis); }
        inline int getStride ( int Axis ) const { return Array_Domain.getStride(Axis); }
        inline int getLength ( int Axis ) const { return Array_Domain.getLength(Axis); }
        inline int getRawDataSize ( int Axis ) const { return Array_Domain.getRawDataSize(Axis); }
        inline int getRawBase   ( int Axis ) const   { return Array_Domain.getRawBase(Axis); }
        inline int getRawBound  ( int Axis ) const   { return Array_Domain.getRawBound(Axis); }
        inline int getRawStride ( int Axis ) const   { return Array_Domain.getRawStride(Axis); }
        inline int getDataBase ( int Axis ) const   
	   { return Array_Domain.getDataBaseVariable (Axis); }
        inline int numberOfDimensions () const { return Array_Domain.numberOfDimensions(); }
        inline int internalNumberOfDimensions () const { return Array_Domain.internalNumberOfDimensions(); }
        inline bool usesIndirectAddressing () const { return Array_Domain.usesIndirectAddressing(); }
        inline void setTemporary( bool x ) const { Array_Domain.setTemporary(x); }
        inline bool isTemporary() const { return Array_Domain.isTemporary(); }
        inline bool isNullArray() const { return Array_Domain.isNullArray(); }
        inline bool isView() const { return Array_Domain.isView(); }
        inline bool isContiguousData() const { return Array_Domain.isContiguousData(); }
        inline int Array_ID() const { return Array_Domain.Array_ID(); }

#if defined(APP) || defined(PPP)
// ********************************************************************
// ********************************************************************
// These functions are specific to P++ and provide informationabout the
// local partition instead of the global distributed array.
// ********************************************************************
// ********************************************************************

#if defined(PPP)
     // Added (7/26/2000)
        inline Internal_Index getLocalMaskIndex  (int i) const { return Array_Domain.getLocalMaskIndex(i); }
        inline Internal_Index getGlobalMaskIndex (int i) const { return Array_Domain.getGlobalMaskIndex(i); }

     // Added (12/13/2000)
        inline int getLeftNumberOfPoints  ( int Axis ) const { return Array_Domain.getLeftNumberOfPoints(Axis);  }
        inline int getRightNumberOfPoints ( int Axis ) const { return Array_Domain.getRightNumberOfPoints(Axis); }
#endif

        inline Range localDimension( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->dimension( Axis );
#else
             return dimension( Axis );
#endif
           }

        inline int getLocalBase( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getBase(Axis);
#else
             return getBase(Axis);
#endif
           }

        inline int getLocalBound( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getBound(Axis);
#else
             return getBound(Axis);
#endif
           }

        inline int getLocalRawBound( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getRawBound(Axis);
#else
             return getRawBound(Axis);
#endif
           }

        inline int getLocalStride( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getStride(Axis);
#else
             return getStride(Axis);
#endif
           }

        inline int getLocalRawStride( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getRawStride(Axis);
#else
             return getRawStride(Axis);
#endif
           }

        inline int getLocalLength( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getLength(Axis);
#else
             return getLength(Axis);
#endif
           }
#endif // End of APP || PPP

#if !defined(USE_SAGE)
     // These generate warning in Sage!
     // Dimension independent query functions
        bool isSameBase   ( const doubleArray_Descriptor_Type & X ) const { return Array_Domain.isSameBase(X.Array_Domain); }
        bool isSameBound  ( const doubleArray_Descriptor_Type & X ) const { return Array_Domain.isSameBound(X.Array_Domain); }
        bool isSameStride ( const doubleArray_Descriptor_Type & X ) const { return Array_Domain.isSameStride(X.Array_Domain); }
        bool isSameLength ( const doubleArray_Descriptor_Type & X ) const { return Array_Domain.isSameLength(X.Array_Domain); }
#if defined(PPP)
        bool isSameGhostBoundaryWidth (const doubleArray_Descriptor_Type & X ) const;
        bool isSameDistribution ( const doubleArray_Descriptor_Type & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const doubleArray_Descriptor_Type & X ) const { return Array_Domain.isSimilar(X.Array_Domain); }
#endif

     // void expandGhostBoundariesOfRawData ( doubleSerialArray_Descriptor_Type & X, 
     //                                       ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
     // void expandGhostBoundariesOfRawData ( doubleSerialArray_Descriptor_Type & X, 
     //                                       Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

        int getGhostBoundaryWidth ( int Axis ) const;
        void setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        void resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE );
        void reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE );

#if defined(PPP)
     // Is a P++ array partitioned along an axis
#if 0
        static bool isPartitioned ( const doubleArray_Descriptor_Type & Parallel_Descriptor, 
                                       const doubleSerialArray_Descriptor_Type & Serial_Descriptor, int Axis );
#endif

     private:
        bool isLeftPartition  ( int Axis ) const;
        bool isMiddlePartition( int Axis ) const;
        bool isRightPartition ( int Axis ) const;

     // Case of array object not present on local processor 
     // Since not all processors have to have a partition of every array object
        bool isNonPartition   ( int Axis ) const;

     // Used in allocate.C to determin position of null array objects in distributions
        bool isLeftNullArray ( int Axis ) const;
        bool isRightNullArray( int Axis ) const;

     public:
	int findProcNum (int*) const;
	int* setupProcessorList 
	   (const intSerialArray& I, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (const intSerialArray& I, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorList 
	   (int* I_A,  int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (int* I_A, int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
#endif

     // These are intended for use in the debugging of A++/P++ arrays
     // static int computeArrayDimension ( int Array_Size_I, int Array_Size_J, int Array_Size_K, int Array_Size_L );
        static int computeArrayDimension ( int* Array_Sizes );
        static int computeArrayDimension ( const doubleArray_Descriptor_Type & X );

        void display ( const char *Label = "" ) const;

#if defined(PPP)
     // These functions are used to get or make the Array_Conformability_Info_Type objects
     // passed between subexpressions in the evaluation of expressions of parallel array objects
        static Array_Conformability_Info_Type* getArray_Conformability_Info ();
#endif

#if !defined(PPP)
        static void Put_Descriptor_Into_Cache ( doubleArray_Descriptor_Type* & X );

        static doubleArray_Descriptor_Type *Get_Descriptor_From_Cache ( 
             int Array_Size_I , int Array_Size_J , int Array_Size_K , int Array_Size_L );
        static doubleArray_Descriptor_Type *Get_Descriptor_From_Cache ( const doubleArray_Descriptor_Type & X );
        static doubleArray_Descriptor_Type *Get_Descriptor_From_Cache ();
        doubleArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I );
        doubleArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J );
        doubleArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J , 
                                                             const Internal_Index & K );
        doubleArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J , 
                                                             const Internal_Index & K , const Internal_Index & L );
#endif

   // functions specific to expression templates
      bool continuousData() const { return Array_Domain.continuousData(); };
   };


#undef DOUBLEARRAY

#define FLOATARRAY
// pragma implementation "A++.h"
// pragma implementation "descriptor.h"

// Typedefs used to simplify (and make more clear) the dimension 
// independent function interface of the A++/P++ implementation.
typedef int* Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Index* Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Indirect_Addressing_Index* Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];

typedef int Integer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Index Index_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];

#if 0
#if defined(PPP)
// for P++ we use a Parallel_Access_Descriptor designed jointly 
// with Carl Kesselman - Dan Quinlan - Joel Saltz
// class floatArray_Descriptor_Type : Parallel_Access_Descriptor
#else
// for A++ we don't need the Parallel_Access_Descriptor
//class floatArray_Descriptor_Type
#endif
#endif

#if defined(PPP)
class Array_Conformability_Info_Type;
#endif

// Avoid unknown type for pointers in the Initialize_Descriptor function
class Internal_Partitioning_Type;

//template<class T, int Templated_Dimension>

class floatArray_Descriptor_Type
   {
  // **********************************************************************************************
  // We might only require one descriptor (instead of one for each different type of array object)!
  // This is because we no longer require the use of the Original_Array field in this class!
  // **********************************************************************************************

     friend class Where_Statement_Support;
  // friend class doubleArray;
  // friend class floatArray;
  // friend class intArray;
#if defined(PPP)
     friend double sum ( const doubleArray & X );
     friend float  sum ( const floatArray  & X );
     friend int    sum ( const intArray    & X );
#endif

  // friend doubleArray sum ( const doubleArray & X , int Axis );
  // friend floatArray  sum ( const floatArray  & X , int Axis );
  // friend intArray    sum ( const intArray    & X , int Axis );

     friend doubleArray & transpose ( const doubleArray & X );
     friend floatArray  & transpose ( const floatArray  & X );
     friend intArray    & transpose ( const intArray    & X );

  // Allow access to private variables Top_Of_Stack and Bottom_Of_Stack
     friend class MemoryManagerType;

#if defined(SERIAL_APP) && !defined(PPP)
  // error make doubleArray a friend
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;

  // friend class Partitioning_Type;

  // friend doubleArray sum ( const doubleArray & X , int Axis );
  // friend floatArray  sum ( const floatArray  & X , int Axis );
  // friend intArray    sum ( const intArray    & X , int Axis );
#endif

#if defined(APP) || defined(PPP)
  // error make doubleSerialArray a friend
     friend class Array_Domain_Type;
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class Internal_Partitioning_Type;
  // friend class Partitioning_Type;
#endif

#if defined(PPP)
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;
     friend class Array_Conformability_Info_Type;
#endif

     public:
     // The purpose of use a domain object is to have a non-templated object that
     // any array can easily point to and/or reuse or copy from.  It is in reality what
     // we used to call the Array_Descriptor.  But after we moved the pointers into the
     // Array_Descriptor_Type there remained a need for a form of type independent
     // object that would do what Array_Descriptor used to do.  So we now call this
     // object Array_Domain_Type and we reuse all the same code (and most of the same comments).

     // This is where the data previously in the Array_Descriptor_Type is located
        Array_Domain_Type Array_Domain;

     // Data required for the "new" and "delete" operators!
     // These are not used so much now that the array objects have array descriptors
     // as member data (instead of just pointers to the array descriptors).
     private:
        static floatArray_Descriptor_Type *Current_Link;
        floatArray_Descriptor_Type        *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

     public:
#if defined(PPP)
     // Pointer to A++ Serial array
     // floatSerialArray* SerialArray;
     // SerialArray_Descriptor_Type* SerialArray;
     // SerialArray_Descriptor_Type<T,Templated_Dimension> *SerialArray;
        floatSerialArray *SerialArray;

     // Access functions for the serialArray object
        inline const floatSerialArray & getSerialArray () const
           { return *SerialArray; }
        inline floatSerialArray* getSerialArrayPointer () const
           { return SerialArray; }

        inline void setDataPointer ( floatSerialArray *inputPointer )
           { SerialArray = inputPointer; }
#else
     // Pointer to raw data
     // T* APP_RESTRICT_MACRO Array_Data;
     // The type_macro embeds the use of the pointer type with the 
     // use of the restrict keyword (which is not a part of standard C++ or C -- yet)
        float* Array_Data;

        inline float* getArrayData () const
           { return Array_Data; }
        inline void setDataPointer ( float* inputPointer )
           { Array_Data = inputPointer; }
#endif

     // These pointers support the use of views. we require one per dimension
     // because a higher dimensional array object can have a view which is
     // of a lower dimension and then it is best (i.e. makes for the best interface)
     // to allow it to be treated as a lower dimensional array object.

        float* Array_View_Pointer0;
#if MAX_ARRAY_DIMENSION>1
        float* Array_View_Pointer1;
#endif
#if MAX_ARRAY_DIMENSION>2
        float* Array_View_Pointer2;
#endif
#if MAX_ARRAY_DIMENSION>3
        float* Array_View_Pointer3;
#endif
#if MAX_ARRAY_DIMENSION>4
        float* Array_View_Pointer4;
#endif
#if MAX_ARRAY_DIMENSION>5
        float* Array_View_Pointer5;
#endif
#if MAX_ARRAY_DIMENSION>6
        float* Array_View_Pointer6;
#endif
#if MAX_ARRAY_DIMENSION>7
        float* Array_View_Pointer7;
#endif

     // We have to make this available in the non expression template version since
     // it is a handy mechanism for the ROSE preprocessor to use.
     // Basically the initial versions of the rose preprocessor can mimic the
     // expression template implementation (in some ways) but be more efficient.
     // Later versions can be more efficient still.
     // Pointer used for indexing into data within the expression template version
     // This makes the subscript computation within the expression template
     // implementation as simple as possible.
     // T* ExpressionTemplateDataPointer;
        float* ExpressionTemplateDataPointer;

#if defined(USE_EXPRESSION_TEMPLATES)
        void setupIndirectAddressingSupport() const
           { Array_Domain.setupIndirectAddressingSupport(); }

        int computeSubscript ( int axis, int i ) const
           { return Array_Domain.computeSubscript(axis,i); }

        int indirectOffset_1D ( int i ) const
           { return Array_Domain.indirectOffset_1D(i); }

#if (MAX_ARRAY_DIMENSION >= 2)
        int indirectOffset_2D ( int i ) const
           { return Array_Domain.indirectOffset_2D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        int indirectOffset_3D ( int i ) const
           { return Array_Domain.indirectOffset_3D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        int indirectOffset_4D ( int i ) const
           { return Array_Domain.indirectOffset_4D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        int indirectOffset_5D ( int i ) const
           { return Array_Domain.indirectOffset_5D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        int indirectOffset_6D ( int i ) const
           { return Array_Domain.indirectOffset_6D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        int indirectOffset_7D ( int i ) const
           { return Array_Domain.indirectOffset_7D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        int indirectOffset_8D ( int i ) const
           { return Array_Domain.indirectOffset_8D(i); }
#endif

     // Access to pointer specific to expression template indexing
        inline float* getExpressionTemplateDataPointer () const 
            { // APP_ASSERT(ExpressionTemplateDataPointer != NULL);
              return ExpressionTemplateDataPointer; 
            }

// end of defined(USE_EXPRESSION_TEMPLATES)
#endif

        inline float* getDataPointer () const
           {
#if defined(PPP)
          // P++ calls the A++ getArrayPointer function
             return SerialArray->getDataPointer();
#else
          // A++ just returns the data
             return Array_Data;
#endif
           }

     // *************************************************
     // Reference counting data and functions
     private:
          int referenceCount;

     public:
       // Use of reference count access is made available for debugging
       // We don't want this access function to be returning a reference!!!
       // Mostly because it is very dangerous as a point of style!!!
       // inline int& getReferenceCount() { return referenceCount; }   
          inline int getReferenceCount() { return referenceCount; }   

       // Support functions for reference counting (inlined for performance)
          inline void incrementReferenceCount() { referenceCount++; }   
          inline void decrementReferenceCount() { referenceCount--; }   
       // inline void referenceCountedDelete () { delete this; }

          inline void resetRawDataReferenceCount() const
             {
            // When we steal the data and reuse it from a temporary we have to delete it
            // which decrements the reference count to -1 (typically) and then we copy the
            // data pointers.  But we have to reset the reference cout to ZERO as well.
            // This function allows that operation to be insulated from the details of how the
            // reference count is implemented.  It is used in the A++ operator= ( arrayobject )
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::resetRawDataReferenceCount() \n");
#endif

#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
            // This has to be a bug since assignement to a function makes no sense
            // unless the the function returns a reference! (but it DOES return a reference)
            // But the point is that from the P++ view the RawData at the P++ array object is
            // suposed to be the referenceCount of the SerialArray object (not the SerialArray Data's
            // referenceCount)
            // SerialArray->Array_Descriptor.getReferenceCount() = 0;
               SerialArray->resetReferenceCount();
               APP_ASSERT (SerialArray->getReferenceCount() == getReferenceCountBase());
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID] = 0;
               Array_Domain.resetRawDataReferenceCount();
#endif
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
#if defined(SERIAL_APP) || defined(PPP)
               return SerialArray_Domain_Type::getRawDataReferenceCountBase(); 
#else
               return Array_Domain_Type::getRawDataReferenceCountBase(); 
#endif
             }

          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return Array_Domain_Type::getReferenceCountBase(); 
             }

          inline void resetReferenceCount()
             {
           // reset the Reference Count in the array object!
              referenceCount = getReferenceCountBase();
             }

          inline int getRawDataReferenceCount() const
             {
#if defined(PPP)
            // Within P++ we can just use the reference count already provided with the A++ array
            // objects.  This avoids providing two mechanisms for reference counting the SerialArray
            // (A++) objects used within P++.
               APP_ASSERT (SerialArray != NULL);
               return SerialArray->getReferenceCount();
#else
            // Within A++ we cna't store the reference count with the array data so we have to
            // provide a seperate array of reference counts indexed by the Array_IDs.
            // APP_ASSERT( Array_Domain_Type::Array_Reference_Count_Array_Length > Array_Domain.Array_ID );
            // return Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID];
               return Array_Domain.getRawDataReferenceCount();
#endif
             }

          inline void incrementRawDataReferenceCount() const
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::incrementRawDataReferenceCount() \n");
#endif
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
               SerialArray->incrementReferenceCount();
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID]++;
               Array_Domain.incrementRawDataReferenceCount();
#endif
             }

          inline void decrementRawDataReferenceCount() const
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Inside of Array_Descriptor_Type::decrementRawDataReferenceCount() \n");
#endif
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (SerialArray != NULL);
               SerialArray->decrementReferenceCount();
#else
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Domain.Array_ID]--;
               Array_Domain.decrementRawDataReferenceCount();
#endif
             }

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

     // *************************************************

     private:
#if (!defined(PPP))
     // Hash Table for array data (raw memory)! Intended to be more efficient on the Cray!
     // The use of the hash table for cacheing raw memory avoids calles to malloc!
        static floatArray_Data_Hash_Table Hash_Table;
#endif

     private:
     // Bug fix (5/3/94) fix for time dependent problems that can generate millions of 
     // array operations and in so doing make Array_Reference_Count_Array TOO long!
     // This allows reuse of Array_IDs and so avoids 
     // the unbounded growth of the Array_Reference_Count_Array
     // static Array_ID_Stack Stack_Of_Array_IDs;

     public:
#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
     // This is a mechanism for detection of the last array being deleted so that
     // if specialized debugging is turned on -- A++ internalmemory can be deleted.
        static int getNumberOfArraysInUse() { return Array_Domain_Type::getNumberOfArraysInUse(); }
#endif
        friend void GlobalMemoryRelease();

        void Test_Preliminary_Consistency( const char *Label ) const;
        void Test_Consistency( const char *Label ) const;

     private:
     // This function allocates the array data for the serial and parallel 
     // Array_Descriptor_Type objects. It is the only function which allocates the array data
     // however in the case of the parallel allocation of data this function calls the
     // Allocate_Parallel_Array() function internally.
        void Allocate_Array_Data ( bool Force_Memory_Allocation );
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation );
        void Delete_Array_Data ();

     // friend void APP_Push_Array_ID ( int x );
     // friend inline int  APP_Pop_Array_ID ();

#if !defined(PPP)
        static void Allocate_Larger_Reference_Count_Array ( int New_Size );
#endif
        static void New_Function_Loop ();

        void Initialize_View_Of_Target ( int Axis , int x , 
                                         const floatArray_Descriptor_Type & Target_Descriptor );
        void Initialize_View_Of_Target ( int Axis , const Internal_Index & X , 
                                         const floatArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , int x , 
                                         const floatArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , const Internal_Index & X , 
                                         const floatArray_Descriptor_Type & Target_Descriptor );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , const intArray & X , 
                                         const floatArray_Descriptor_Type & Target_Descriptor );
#if defined(PPP)
        void Compute_Local_Index_Arrays ( const floatArray_Descriptor_Type & Original_Descriptor );
        void Initialize_Parallel_Parts_Of_Descriptor ( const floatArray_Descriptor_Type & X );

     // static int Get_Processor_Position ( int Lhs_Left_Number_Of_Points  , int Rhs_Left_Number_Of_Points  ,
     //                                     int Lhs_Right_Number_Of_Points , int Rhs_Right_Number_Of_Points );
        int Get_Processor_Position ( int Axis ) const;
#endif

#if 0
#if !defined(USE_SAGE)
     // I don't know why Sage has a problem with this -- the input arguments seem to be correct!
        inline void Fixup_Array_Descriptor_For_Copy_Constructor ( const floatArray_Descriptor_Type & X )
           { Array_Domain.Fixup_Array_Domain_For_Copy_Constructor(X.Array_Domain); }
#endif
#endif

     public:
     // Used to free memory-in-use internally in floatArray_Descriptor_Type objects! 
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();

#ifdef INLINE_FUNCTIONS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     // ********************************************************************************
     // ********************************************************************************
     // *******************  floatArray_Descriptor_Type DESTRUCTORS  ********************
     // ********************************************************************************
     // ********************************************************************************

       ~floatArray_Descriptor_Type();

     // ********************************************************************************
     // ********************************************************************************
     // *************************  CONSTRUCTOR INITIALIZATION  *************************
     // ********************************************************************************
     // ********************************************************************************

     // This is the only initialization member function required!
     // void Initialize_Descriptor();
        void Preinitialize_Descriptor();

     // void Initialize_Descriptor ( const floatArray_Descriptor_Type & X ,
     //   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List, 
     //   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_Non_Indexed_Dimension ( int x , const floatArray_Descriptor_Type & X );
     // void Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension ( int x , const floatArray_Descriptor_Type & X );
     // void Preinitialize_Descriptor ( const floatArray_Descriptor_Type & X );
// if !defined(USE_SAGE)
     // This yields a warning from Sage
        void Initialize_Descriptor ( int Number_Of_Valid_Dimensions ,
                                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List,
                                     const Internal_Partitioning_Type* Internal_Partition = NULL );
     // void Fixup_Descriptor_With_View 
        void reshape
	   ( int Number_Of_Valid_Dimensions ,
	     const int* View_Sizes, const int* View_Bases );
        // {
        //   referenceCount = 0;
        //   Array_Domain.Initialize_Domain (Number_Of_Valid_Dimensions,Integer_List);
        // }
// else
     // This function is not implemented in descriptor.C
     // void Initialize_Descriptor ( int Number_Of_Valid_Dimensions ,
     //                              const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
// endif
     // void Initialize_Descriptor ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
     // void Initialize_Descriptor
     //           ( const floatArray_Descriptor_Type & X ,
     //             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // void Initialize_Descriptor
     //           ( const floatArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_Descriptor
     //           ( const floatArray_Descriptor_Type & X ,
     //             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List, 
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

     // void Initialize_IndirectAddressing_Descriptor_Using_IndirectAddressing
     //           ( const floatArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
     // void Initialize_NonindirectAddressing_Descriptor_Using_IndirectAddressing
     //           ( const floatArray_Descriptor_Type & X ,
     //             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );


     // ********************************************************************************
     // ********************************************************************************
     // *******************  floatArray_Descriptor_Type CONSTRUCTORS  *******************
     // ********************************************************************************
     // ********************************************************************************

     // ********************************************************************************
     // ************************** A++ Constructors ************************************
     // ********************************************************************************
        floatArray_Descriptor_Type();
        floatArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size );
        floatArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // This takes the optional bool for memory allocation. We may want to have this later.
     // floatArray_Descriptor_Type ( ARGUMENT_LIST_MACRO_INTEGER , bool Force_Memory_Allocation );
        floatArray_Descriptor_Type ( ARGUMENT_LIST_MACRO_INTEGER );

     // ****************************   COPY CONSTRUCTOR  *******************************
     // floatArray_Descriptor_Type ( const floatArray_Descriptor_Type & X );
#if 0
     // The KCC compiler can accept this as correct code
     // but the Sun C++ compiler must have templates defined at global scope
     // so we have had to dumb this use of templates down somewhat
        template<class S, int SecondDimension>
        floatArray_Descriptor_Type ( const floatArray_Descriptor_Type<S,SecondDimension> & X );
#else
     // Sun C++ version
     // floatArray_Descriptor_Type ( const floatArray_Descriptor_Type & X );
        floatArray_Descriptor_Type ( const floatArray_Descriptor_Type & X, int Type_Of_Copy = DEEPCOPY );
     // floatArray_Descriptor_Type ( const floatArray_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );
     // floatArray_Descriptor_Type ( const Array_Descriptor_Type<float ,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type ( const Array_Descriptor_Type<int   ,MAX_ARRAY_DIMENSION> & X );
#endif

     // Array_Descriptor_Type ( const Array_Descriptor_Type & X,
     //                           Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type* Index_List );
     // Array_Descriptor_Type ( const Array_Descriptor_Type & X, 
     //                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        floatArray_Descriptor_Type ( const Array_Domain_Type & X, 
                                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

     // floatArray_Descriptor_Type ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // floatArray_Descriptor_Type ( const floatArray_Descriptor_Type & X,
     //                           const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
        floatArray_Descriptor_Type ( const Array_Domain_Type & X,
                                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

  // We need these because the array object constructors take a different
  // number of parameteres to represent the size in each axis and we
  // have to call constructors for the member objects within the array objects
  // within the preinitialization lists for the array objects.  So we need 
  // constructors for the Array_Descriptor_Type and Array_Domain_Type objects
  // that have a similar interface to the constructors in the array class interface.
  // This simplifies the preinitialization lists.

     floatArray_Descriptor_Type ( const Internal_Index & I );
#if (MAX_ARRAY_DIMENSION >= 2)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O, const Internal_Index & P );
#endif
 
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

     floatArray_Descriptor_Type( const Array_Domain_Type & X, 
                                bool AvoidBuildingIndirectAddressingView );

  // Constructor for initialization from a string
  // floatArray_Descriptor_Type( const char* );
     floatArray_Descriptor_Type( const AppString & X );

     // ********************************************************************************
     // ************************ A++ and P++ Constructors ******************************
     // ********************************************************************************
#if defined(APP) || defined(PPP)
        floatArray_Descriptor_Type( int Number_Of_Valid_Dimensions , 
                                 const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
                                 const Internal_Partitioning_Type & Internal_Partition );

        floatArray_Descriptor_Type( int Number_Of_Valid_Dimensions ,
                                 const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
                                 const Internal_Partitioning_Type & Internal_Partition );

     // This is a redundent constructor!
     // floatArray_Descriptor_Type( const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
     //                          const Internal_Partitioning_Type & Internal_Partition );

     // This is a redundent initialization function!
     // void Initialize_Descriptor ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
     //                              const Internal_Partitioning_Type & Internal_Partition );
     floatArray_Descriptor_Type ( const Internal_Index & I , 
                                 const Internal_Partitioning_Type & Internal_Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
     floatArray_Descriptor_Type ( const Internal_Index & I, const Internal_Index & J,
                                 const Internal_Index & K, const Internal_Index & L,
                                 const Internal_Index & M, const Internal_Index & N,
                                 const Internal_Index & O, const Internal_Index & P,
                                 const Internal_Partitioning_Type & Internal_Partition );
#endif
 
// Error checking
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// endif for defined(APP) || defined(PPP)
#endif

     // ********************************************************************************
     // ********************* Existing Data Constructors *******************************
     // ********************************************************************************
#if defined(PPP)
        floatArray_Descriptor_Type ( 
             const float* Data_Pointer, 
             ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE );
#else
        floatArray_Descriptor_Type ( const float* Data_Pointer, ARGUMENT_LIST_MACRO_CONST_REF_RANGE );

     // This is a redundent constructor
     // floatArray_Descriptor_Type ( const float* Data_Pointer, ARGUMENT_LIST_MACRO_INTEGER);
#endif

#if 0
     // These support the adopt type features for the array objects
     // they are used in the preinitialization lists for the array object constructors
#if defined(PPP)
        floatArray_Descriptor_Type( const floatSerialArray* SerialArray_Pointer ,
             floatArray_Descriptor_Type*
             Array_Descriptor_Pointer, Operand_Storage* Array_Storage_Pointer );
#else
        floatArray_Descriptor_Type( const float* Array_Data_Pointer ,
             floatArray_Descriptor_Type*
             Array_Descriptor_Pointer , Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if defined(PPP)
#if !defined(USE_PADRE)
        DARRAY* Build_BlockPartiArrayDomain ();
#endif
#if 0
        static void fixupLocalBase ( 
                              floatArray_Descriptor_Type & New_Parallel_Descriptor,
                        floatSerialArray_Descriptor_Type & New_Serial_Descriptor,
                  const       floatArray_Descriptor_Type & Old_Parallel_Descriptor,
                  const floatSerialArray_Descriptor_Type & Old_Serial_Descriptor );
        void Update_Parallel_Information_Using_Old_Descriptor ( const floatArray_Descriptor_Type & Old_Array_Descriptor );
#endif
#endif

#if defined(APP) || defined(PPP)
        void partition ( const Internal_Partitioning_Type & Internal_Partition );
#endif

#if !defined(USE_SAGE)
     // this yeilds a warning in Sage!

     // function interfaces must be dimension independent to permit arbitrary dimension A++/P++ array objects
     // void Error_Checking_For_Index_Operators (
     //   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndexParameterList, 
     //   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndirectIndexParameterList );
        void Error_Checking_For_Index_Operators (
          const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const
           { Array_Domain.Error_Checking_For_Index_Operators(Internal_Index_List); }
        void Error_Checking_For_Index_Operators (
          const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List ) const
           { Array_Domain.Error_Checking_For_Index_Operators(Indirect_Index_List); }

        void Error_Checking_For_Scalar_Index_Operators ( const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const
           { Array_Domain.Error_Checking_For_Scalar_Index_Operators(Integer_List); }
#endif

     // template<class S, int SecondDimension>
     // floatArray_Descriptor_Type & operator= ( const floatArray_Descriptor_Type<S,SecondDimension> & X );

        floatArray_Descriptor_Type & operator= ( const floatArray_Descriptor_Type & X );
     // floatArray_Descriptor_Type & operator= ( const Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type & operator= ( const Array_Descriptor_Type<float ,MAX_ARRAY_DIMENSION> & X );
     // Array_Descriptor_Type & operator= ( const Array_Descriptor_Type<int   ,MAX_ARRAY_DIMENSION> & X );

     // This builds a descriptor to represent a 1D (linear) view any data described by the input descriptor.
        static floatArray_Descriptor_Type *Vectorizing_Descriptor ( const floatArray_Descriptor_Type & X );

        void Build_Temporary_By_Example ( const floatArray_Descriptor_Type & X );

     // void Convert_View_To_Nonview ();

#if defined(USE_EXPRESSION_TEMPLATES)
     // Functions specific to the expression template implementation
        int size() const { return Array_Size(); };
     // int conformsTo () const { return TRUE; };
        int conformsTo ( const Array_Descriptor_Type & X ) const { return TRUE; };
        
     /* SUGGESTION:  Since the expression templates have different demands on
     //              the scalar indexing we should consider combining the 
     //              operations to simplify the operations below (i.e multiply out
     //              offset and seperate the variant parts dependent upon i,j,k, etc. and the
     //              invariant parts which could be stored seperately).
     */
        inline int offset(int i) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i);
        }
        inline int offset0(int i) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i);
        }
#if (MAX_ARRAY_DIMENSION >= 2)
        inline int offset(int i, int j) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j);
        }
        inline int offset0(int i, int j) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        inline int offset(int i, int j, int k) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k);
        }
        inline int offset0(int i, int j, int k) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        inline int offset(int i, int j, int k, int l) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l);
        }
        inline int offset0(int i, int j, int k, int l) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        inline int offset(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m);
        }
        inline int offset0(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        inline int offset(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        inline int offset(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n,o);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n,o);
        }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        inline int offset(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset(i,j,k,l,m,n,o,p);
        }
        inline int offset0(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
        {
          return Array_Domain.offset0(i,j,k,l,m,n,o,p);
        }
#endif
      
     // Indexing function. Used to find the ith item - taking account of strides.
     // if defined(USE_EXPRESSION_TEMPLATES)
#endif

        void setBase ( int New_Base_For_All_Axes );
        void setBase ( int New_Base, int Axis );

        inline int Array_Size () const { return Array_Domain.Array_Size(); }
#if !defined(USE_SAGE)
        inline void getRawDataSize ( int* Sizes ) const { Array_Domain.getRawDataSize(Sizes); }
#endif
        inline Range dimension ( int Axis ) const { return Array_Domain.dimension(Axis); }
        inline Range getFullRange ( int Axis ) const { return Array_Domain.getFullRange(Axis); }
        inline int getBase   ( int Axis ) const { return Array_Domain.getBase(Axis); }
        inline int getBound  ( int Axis ) const { return Array_Domain.getBound(Axis); }
        inline int getStride ( int Axis ) const { return Array_Domain.getStride(Axis); }
        inline int getLength ( int Axis ) const { return Array_Domain.getLength(Axis); }
        inline int getRawDataSize ( int Axis ) const { return Array_Domain.getRawDataSize(Axis); }
        inline int getRawBase   ( int Axis ) const   { return Array_Domain.getRawBase(Axis); }
        inline int getRawBound  ( int Axis ) const   { return Array_Domain.getRawBound(Axis); }
        inline int getRawStride ( int Axis ) const   { return Array_Domain.getRawStride(Axis); }
        inline int getDataBase ( int Axis ) const   
	   { return Array_Domain.getDataBaseVariable (Axis); }
        inline int numberOfDimensions () const { return Array_Domain.numberOfDimensions(); }
        inline int internalNumberOfDimensions () const { return Array_Domain.internalNumberOfDimensions(); }
        inline bool usesIndirectAddressing () const { return Array_Domain.usesIndirectAddressing(); }
        inline void setTemporary( bool x ) const { Array_Domain.setTemporary(x); }
        inline bool isTemporary() const { return Array_Domain.isTemporary(); }
        inline bool isNullArray() const { return Array_Domain.isNullArray(); }
        inline bool isView() const { return Array_Domain.isView(); }
        inline bool isContiguousData() const { return Array_Domain.isContiguousData(); }
        inline int Array_ID() const { return Array_Domain.Array_ID(); }

#if defined(APP) || defined(PPP)
// ********************************************************************
// ********************************************************************
// These functions are specific to P++ and provide informationabout the
// local partition instead of the global distributed array.
// ********************************************************************
// ********************************************************************

#if defined(PPP)
     // Added (7/26/2000)
        inline Internal_Index getLocalMaskIndex  (int i) const { return Array_Domain.getLocalMaskIndex(i); }
        inline Internal_Index getGlobalMaskIndex (int i) const { return Array_Domain.getGlobalMaskIndex(i); }

     // Added (12/13/2000)
        inline int getLeftNumberOfPoints  ( int Axis ) const { return Array_Domain.getLeftNumberOfPoints(Axis);  }
        inline int getRightNumberOfPoints ( int Axis ) const { return Array_Domain.getRightNumberOfPoints(Axis); }
#endif

        inline Range localDimension( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->dimension( Axis );
#else
             return dimension( Axis );
#endif
           }

        inline int getLocalBase( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getBase(Axis);
#else
             return getBase(Axis);
#endif
           }

        inline int getLocalBound( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getBound(Axis);
#else
             return getBound(Axis);
#endif
           }

        inline int getLocalRawBound( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getRawBound(Axis);
#else
             return getRawBound(Axis);
#endif
           }

        inline int getLocalStride( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getStride(Axis);
#else
             return getStride(Axis);
#endif
           }

        inline int getLocalRawStride( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getRawStride(Axis);
#else
             return getRawStride(Axis);
#endif
           }

        inline int getLocalLength( int Axis ) const
           {
#if defined(PPP)
             APP_ASSERT(SerialArray != NULL);
             return SerialArray->getLength(Axis);
#else
             return getLength(Axis);
#endif
           }
#endif // End of APP || PPP

#if !defined(USE_SAGE)
     // These generate warning in Sage!
     // Dimension independent query functions
        bool isSameBase   ( const floatArray_Descriptor_Type & X ) const { return Array_Domain.isSameBase(X.Array_Domain); }
        bool isSameBound  ( const floatArray_Descriptor_Type & X ) const { return Array_Domain.isSameBound(X.Array_Domain); }
        bool isSameStride ( const floatArray_Descriptor_Type & X ) const { return Array_Domain.isSameStride(X.Array_Domain); }
        bool isSameLength ( const floatArray_Descriptor_Type & X ) const { return Array_Domain.isSameLength(X.Array_Domain); }
#if defined(PPP)
        bool isSameGhostBoundaryWidth (const floatArray_Descriptor_Type & X ) const;
        bool isSameDistribution ( const floatArray_Descriptor_Type & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const floatArray_Descriptor_Type & X ) const { return Array_Domain.isSimilar(X.Array_Domain); }
#endif

     // void expandGhostBoundariesOfRawData ( floatSerialArray_Descriptor_Type & X, 
     //                                       ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
     // void expandGhostBoundariesOfRawData ( floatSerialArray_Descriptor_Type & X, 
     //                                       Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

        int getGhostBoundaryWidth ( int Axis ) const;
        void setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        void resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE );
        void reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE );

#if defined(PPP)
     // Is a P++ array partitioned along an axis
#if 0
        static bool isPartitioned ( const floatArray_Descriptor_Type & Parallel_Descriptor, 
                                       const floatSerialArray_Descriptor_Type & Serial_Descriptor, int Axis );
#endif

     private:
        bool isLeftPartition  ( int Axis ) const;
        bool isMiddlePartition( int Axis ) const;
        bool isRightPartition ( int Axis ) const;

     // Case of array object not present on local processor 
     // Since not all processors have to have a partition of every array object
        bool isNonPartition   ( int Axis ) const;

     // Used in allocate.C to determin position of null array objects in distributions
        bool isLeftNullArray ( int Axis ) const;
        bool isRightNullArray( int Axis ) const;

     public:
	int findProcNum (int*) const;
	int* setupProcessorList 
	   (const intSerialArray& I, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (const intSerialArray& I, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorList 
	   (int* I_A,  int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (int* I_A, int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
#endif

     // These are intended for use in the debugging of A++/P++ arrays
     // static int computeArrayDimension ( int Array_Size_I, int Array_Size_J, int Array_Size_K, int Array_Size_L );
        static int computeArrayDimension ( int* Array_Sizes );
        static int computeArrayDimension ( const floatArray_Descriptor_Type & X );

        void display ( const char *Label = "" ) const;

#if defined(PPP)
     // These functions are used to get or make the Array_Conformability_Info_Type objects
     // passed between subexpressions in the evaluation of expressions of parallel array objects
        static Array_Conformability_Info_Type* getArray_Conformability_Info ();
#endif

#if !defined(PPP)
        static void Put_Descriptor_Into_Cache ( floatArray_Descriptor_Type* & X );

        static floatArray_Descriptor_Type *Get_Descriptor_From_Cache ( 
             int Array_Size_I , int Array_Size_J , int Array_Size_K , int Array_Size_L );
        static floatArray_Descriptor_Type *Get_Descriptor_From_Cache ( const floatArray_Descriptor_Type & X );
        static floatArray_Descriptor_Type *Get_Descriptor_From_Cache ();
        floatArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I );
        floatArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J );
        floatArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J , 
                                                             const Internal_Index & K );
        floatArray_Descriptor_Type *Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J , 
                                                             const Internal_Index & K , const Internal_Index & L );
#endif

   // functions specific to expression templates
      bool continuousData() const { return Array_Domain.continuousData(); };
   };


#undef FLOATARRAY


#endif  /* !defined(_APP_DESCRIPTOR_H) */





