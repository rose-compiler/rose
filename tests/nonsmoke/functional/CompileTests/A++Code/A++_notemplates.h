// Allow repeated includes of array.h without error 
#ifndef _APP_ARRAY_H
#define _APP_ARRAY_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#pragma implementation "A++_headers.h"
#endif

// Turn ON the APP macro
#define APP

// the collection of header files required 
// before the A++ objects can be defined are 
// contained in A++_headers.h.  This simplifies
// the implementation.  Depending on if we 
// are compiling A++ or and A++ application
// the collection of required files are different.
#include <A++_headers.h>

















/* MACRO EXPANSION BEGINS HERE */
#define DOUBLEARRAY
// Default base and bound object for use in doubleArray constructor
extern Range APP_Unit_Range;



// Forward class declarations
class doubleArray_Function_Steal_Data;
class doubleArray_Function_0;
class doubleArray_Function_1;
class doubleArray_Function_2;
class doubleArray_Function_3;
class doubleArray_Function_4;
class doubleArray_Function_5;
class doubleArray_Function_6;
class doubleArray_Function_7;
class doubleArray_Function_8;
class doubleArray_Function_9;
class doubleArray_Function_10;
class doubleArray_Function_11;
class doubleArray_Function_12;
class doubleArray_Function_13;
class doubleArray_Function_14;
class doubleArray_Function_15;
class doubleArray_Function_16;

class doubleArray_Aggregate_Operator;

// Forward class declarations
class floatArray_Function_Steal_Data;
class floatArray_Function_0;
class floatArray_Function_1;
class floatArray_Function_2;
class floatArray_Function_3;
class floatArray_Function_4;
class floatArray_Function_5;
class floatArray_Function_6;
class floatArray_Function_7;
class floatArray_Function_8;
class floatArray_Function_9;
class floatArray_Function_10;
class floatArray_Function_11;
class floatArray_Function_12;
class floatArray_Function_13;
class floatArray_Function_14;
class floatArray_Function_15;
class floatArray_Function_16;

class floatArray_Aggregate_Operator;

// Forward class declarations
class intArray_Function_Steal_Data;
class intArray_Function_0;
class intArray_Function_1;
class intArray_Function_2;
class intArray_Function_3;
class intArray_Function_4;
class intArray_Function_5;
class intArray_Function_6;
class intArray_Function_7;
class intArray_Function_8;
class intArray_Function_9;
class intArray_Function_10;
class intArray_Function_11;
class intArray_Function_12;
class intArray_Function_13;
class intArray_Function_14;
class intArray_Function_15;
class intArray_Function_16;

class intArray_Aggregate_Operator;


#if defined(APP) || defined(PPP)
class Array_Conformability_Info_Type;
class Partitioning_Type;
#endif


#if defined(PPP)
// PVM function prototype
// Bugfix (6/20/95) To remove the dependence on pvm in the P++ implementation
// we pass the Operation_Type (an int) instead of a PVM function pointer.
// So this typedef should not be required anymore
// extern "C" { typedef void (Reduction_Function_Type) ( int*, void*, void*, int*, int*); };
#endif

// we use the case where the DOUBLEARRAY is defined so that
// this will be placed first in the file once it is expanded by m4!
// if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)
// if defined(DOUBLEARRAY) && (defined(APP) || defined(PPP))
#if defined(DOUBLEARRAY)
// error "USE_EXPRESSION_TEMPLATES is defined!"
class BaseArray
   {
  // Base class for all array objects.  This base class may eventualy be filled with
  // most of the member function that I presently in the A++/P++ objects themselves.
  // This would simplify the definition of A++/P++ and in the templated version it might
  // simplify the instantiation of the templated member functions.

       // this might be a useful mechanism to avoid any future requirements of runtime typing
       // but we avoid using it unless there is no other way to handle this.
       // int TypeInfo;  // A mechanism to record the derived type -- a poor design mechanism

     public:
#if 1
       // virtual ~BaseArray();
       // virtual ~BaseArray() = 0;
          BaseArray();
#endif

// We comment out these pure virtual functions to test a bug
#if 0
       // This is a list of most of the functions in the array objects
       // I think that most of these could placed into the base class
       // however for the moment I have only made them pure virtual functions.
       // some of these (like the getDomainPointer - could not be implemented within the
       // base class since they make calls to the typed (double, float, int) ArrayDescriptorType
       // object which is only contained in the derived class).
          virtual int getReferenceCount() const = 0;
          virtual void incrementReferenceCount() = 0;
          virtual void decrementReferenceCount() = 0;
          virtual void resetRawDataReferenceCount() const = 0;
          virtual int getRawDataReferenceCount() const = 0;
          virtual void incrementRawDataReferenceCount() const = 0;
          virtual void decrementRawDataReferenceCount() const = 0;
          virtual void ReferenceCountedDelete () = 0;
       // virtual static void New_Function_Loop () = 0;
       // virtual static void freeMemoryInUse() = 0;
       // virtual void *operator new    (size_t) = 0;
       // virtual void operator  delete (void*) = 0;
          virtual void Initialize_Array ( 
               int Number_Of_Valid_Dimensions ,
               const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
               bool Force_Memory_Allocation ) = 0;
       // virtual void Initialization_From_Range_List ( 
       //      int Number_Of_Valid_Dimensions,
       //      const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Range_List ) = 0;
          virtual int getDataOffset ( int Axis ) const = 0;
          virtual Array_Domain_Type* getDomainPointer () = 0;
       // virtual Array_Descriptor_Type *& getArrayDomainReference () const = 0;
          virtual bool Is_Built_By_Defered_Evaluation() const = 0;
          virtual void setTemporary( bool x ) const = 0;
          virtual bool isTemporary() const = 0;
          virtual bool isNullArray() const = 0;
          virtual bool isView()      const = 0;
          virtual bool isContiguousData() const = 0;

       // I have commented this out to test a bugfix!
       // virtual void Test_Consistency ( const char *Label = "" ) const = 0;

          virtual bool usesIndirectAddressing() const = 0; 
#if !defined(PPP)
          virtual void Fixup_User_Base
              (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List,
               const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List) = 0;
#endif
          virtual int getBase() const = 0;
          virtual int getRawBase() const = 0;
          virtual int getBase( int Axis ) const = 0;
          virtual int getStride( int Axis ) const = 0;
          virtual int getBound( int Axis ) const = 0;
          virtual int getLength( int Axis ) const = 0;
          virtual int getRawDataSize( int Axis ) const = 0;
          virtual int getRawBase( int Axis ) const = 0;
          virtual int getRawStride( int Axis ) const = 0;
          virtual int getRawBound( int Axis ) const = 0;
          virtual int getDataBase( int Axis ) const = 0;
          virtual int numberOfDimensions () const = 0;
          virtual Range dimension( int Axis ) const = 0;
          virtual Range getFullRange( int Axis ) const = 0;
#if defined(APP) || defined(PPP)
       // P++ specific functions report on the local partition
          virtual int getLocalBase( int Axis ) const = 0;
          virtual int getLocalStride( int Axis ) const = 0;
          virtual int getLocalBound( int Axis ) const = 0;
          virtual int getLocalLength( int Axis ) const = 0;
          virtual int getLocalRawBase( int Axis ) const = 0;
          virtual int getLocalRawStride( int Axis ) const = 0;
          virtual int getLocalRawBound( int Axis ) const = 0;
          virtual Range localDimension( int Axis ) const = 0;
#endif
          virtual Range getLocalFullRange( int Axis ) const = 0;
          virtual int elementCount() const = 0;
          virtual int cols() const = 0;
          virtual int rows() const = 0;

       // note that redim reshape resize may be functions that might have
       // to be defined only within the derived class.  Some support might
       // be possible from the base class if so this this would be worthwhile.

          virtual void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const = 0;
          virtual void Delete_Array_Data () = 0;
          virtual void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) = 0;
#endif

#if 1   
       // Bugs in the Sun C++ compiler have routinely caused the base class member functions
       // to be called instead of the derived class.  One solution might be to have the
       // Array_ID variable stored in the base class so that it can always be accessed from 
       // the base class.  Since the other function is only defined for APP and PPP
       // I think the definition of some virtual function here is required.
       // virtual int Array_ID () const = 0;
          virtual int Dummy_Function_Array_ID () const
             { 
               printf ("ERROR: Virtual Base Class called: BaseArray::Array_ID() \n");
               APP_ABORT();
               return -1000;
             }
#endif

#if defined(APP) || defined(PPP)
#if defined(USE_PADRE)
       // virtual void partition( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition ) = 0;
          virtual void partition( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition )
             {
            // Avoid compiler warning about unused variable
               if (&Partition);

               printf ("ERROR: Virtual Base Class called: BaseArray::partition(PADRE_Distribution<*,*,*>) \n");
               APP_ABORT();
             }
#endif
// We comment out these pure virtual functions to test a bug
#if 0
          virtual Partitioning_Type getPartition () const = 0;
          virtual void displayPartitioning (const char *Label = "") = 0;
          virtual int getInternalGhostCellWidth ( int Axis ) const = 0;
          virtual int getExternalGhostCellWidth ( int Axis ) const = 0;
          virtual void updateGhostBoundaries() const = 0;
#endif
#endif

#if 0
       // This function exists in both bases classes of an array object and 
       // creates an ambiguous reference to the buildListOfArrayOperands member 
       // function of a doubleArray or floatArray or intArray.
          void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
             {
               printf ("Inside of BaseArray::buildListOfArrayOperands(list<BaseArray*> & arrayList) \n");
               abort();
            // APP_ABORT();
            // There is a const problem here!
            // arrayList.push_back (this);
             }
#endif
   };
#endif

#if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)
/*
///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    InArray
//
// DESCRIPTION
//    A version of Indexable that container classes, like Arrays, can
//    inherit from in order to become indexable as well.
//
///////////////////////////////////////////////////////////////////////////
*/

template<class T>
class InArray : public Indexable<T, InArray<T> > 
   {
     public:
        typedef T RetT;

     protected:

        InArray ()  
           { 
             printf ("ERROR: inside of InArray::InArray() \n"); 
             APP_ABORT(); 
           }

     // We pass FALSE to the Array_Descriptor constructor because we want 
     // to have AvoidBuildingIndirectAddressingView = FALSE internally
        InArray(const Array_Domain_Type & Domain)
           : Array_Descriptor(Domain,FALSE) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}

     // I don't think we need this now!
        InArray ( const doubleArray* InputArrayPointer ) {}
        InArray ( const floatArray* InputArrayPointer ) {}
        InArray ( const intArray* InputArrayPointer ) {}

     public:

        inline bool usesIndirectAddressing() const APP_RESTRICT_MACRO;
        inline int internalNumberOfDimensions() const APP_RESTRICT_MACRO;
        inline bool containsArrayID ( int lhs_id ) const APP_RESTRICT_MACRO;

        void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<doubleArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<floatArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<intArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<intArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<BaseArray*> & arrayList) \n");
           }

        void setupIndirectAddressingSupport() const APP_RESTRICT_MACRO
           { Array_Descriptor.setupIndirectAddressingSupport(); }

        T expand(int i) const APP_RESTRICT_MACRO;
        T indirect_expand_1D(int i) const APP_RESTRICT_MACRO;
#if (MAX_ARRAY_DIMENSION >= 2)
        T expand(int i, int j) const APP_RESTRICT_MACRO;
        T indirect_expand_2D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        T expand(int i, int j, int k) const APP_RESTRICT_MACRO;
        T indirect_expand_3D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        T expand(int i, int j, int k, int l) const APP_RESTRICT_MACRO;
        T indirect_expand_4D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        T expand(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO;
        T indirect_expand_5D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        T expand(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO;
        T indirect_expand_6D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        T expand(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO;
        T indirect_expand_7D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        T expand(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO;
        T indirect_expand_8D(int i) const APP_RESTRICT_MACRO;
#endif

        const Array_Domain_Type &shape() const APP_RESTRICT_MACRO;
        const Array_Domain_Type *shapePtr() const APP_RESTRICT_MACRO;

public:
     // To avoid access to relavanet data through a pointer which makes the expression 
     // template implementation less efficient we put all the pointers and the traditiona
     // descriptor data into the Array_Descriptor which has been templated to handle
     // the different types of data (pointers of different types).
        TArray_Descriptor_Type Array_Descriptor;
   };

// endif for "if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)" above
#endif


// We use this as a mechanism to define the base class differently when we use expression templates
#if defined(USE_EXPRESSION_TEMPLATES)
#define EXPRESSION_TEMPLATES_DERIVED_double_CLASS_SUPPORT : public BaseArray , public InArray<double>
#else
// We modify A++/P++ to have a common base class for all array objects - this permits
// many additional design features such as a mechanism for forming references to array 
// object independent of the type of the array.
// #define EXPRESSION_TEMPLATES_DERIVED_double_CLASS_SUPPORT
#undef EXPRESSION_TEMPLATES_DERIVED_double_CLASS_SUPPORT
#define EXPRESSION_TEMPLATES_DERIVED_double_CLASS_SUPPORT : public BaseArray
#endif


// #################################################################
// ######  Definition of the principal object within A++/P++  ######
// #################################################################

class doubleArray EXPRESSION_TEMPLATES_DERIVED_double_CLASS_SUPPORT
   {
  // This is the pricipal object within A++/P++.
  // This is the declaration of the array object.
  // It includes a declaration of a descriptor (as member data, which in turn 
  // includes a domain objects (as member data)).  Most of this class is a description
  // of the public interface and is documented in the A++/P++ manual. But (of course) this
  // also includes the private interface (not documented in the manual). Many of the
  // elements of the private interface are declared public to simplify teh design of the
  // object and facilitate its use in contexts that it was not explicitly designed for.
  // Thus we don't make it hard for the uer to get at the raw internal data.  If we made it 
  // difficult we might argue that we preserve incapsulation - but it is not a heard work 
  // around for a user to get at this data and so in the end the internal data is not 
  // really well encapsulated anyway.

  // Note that the BaseArray has a "this" pointer and so there 
  // is an offset for all the data members of this (array) class.

     friend class Where_Statement_Support;
     friend class Array_Statement_Type;
     friend class Expression_Tree_Node_Type;
 
     friend class Operand_Storage;
     friend class doubleArray_Operand_Storage;

#if defined(USE_EXPRESSION_TEMPLATES)
     friend class InArray<double>;
#endif

     friend class Internal_Partitioning_Type;
     friend class Partitioning_Type;

#if defined(SERIAL_APP) && !defined(PPP)
     friend doubleArray & transpose ( const doubleArray & X );
#endif

#ifndef DOUBLEARRAY
     friend class doubleArray;
#if defined(SERIAL_APP)
     friend class doubleSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class doubleArray;
#endif
#if defined(PPP)
     friend class doubleSerialArray;
#endif
#endif

#ifndef FLOATARRAY
     friend class floatArray;
#if defined(SERIAL_APP)
     friend class floatSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class floatArray;
#endif
#if defined(PPP)
     friend class floatSerialArray;
#endif
#endif

#ifndef INTARRAY
     friend class intArray;
#if defined(SERIAL_APP)
     friend class intSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class intArray;
#endif
#if defined(PPP)
     friend class intSerialArray;
#endif
#endif

#if 1
// GNU g++ requires these friend declarations
// Forward class declarations
friend class doubleArray_Function_Steal_Data;
friend class doubleArray_Function_0;
friend class doubleArray_Function_1;
friend class doubleArray_Function_2;
friend class doubleArray_Function_3;
friend class doubleArray_Function_4;
friend class doubleArray_Function_5;
friend class doubleArray_Function_6;
friend class doubleArray_Function_7;
friend class doubleArray_Function_8;
friend class doubleArray_Function_9;
friend class doubleArray_Function_10;
friend class doubleArray_Function_11;
friend class doubleArray_Function_12;
friend class doubleArray_Function_13;
friend class doubleArray_Function_14;
friend class doubleArray_Function_15;
friend class doubleArray_Function_16;

friend class doubleArray_Aggregate_Operator;

// Forward class declarations
friend class floatArray_Function_Steal_Data;
friend class floatArray_Function_0;
friend class floatArray_Function_1;
friend class floatArray_Function_2;
friend class floatArray_Function_3;
friend class floatArray_Function_4;
friend class floatArray_Function_5;
friend class floatArray_Function_6;
friend class floatArray_Function_7;
friend class floatArray_Function_8;
friend class floatArray_Function_9;
friend class floatArray_Function_10;
friend class floatArray_Function_11;
friend class floatArray_Function_12;
friend class floatArray_Function_13;
friend class floatArray_Function_14;
friend class floatArray_Function_15;
friend class floatArray_Function_16;

friend class floatArray_Aggregate_Operator;

// Forward class declarations
friend class intArray_Function_Steal_Data;
friend class intArray_Function_0;
friend class intArray_Function_1;
friend class intArray_Function_2;
friend class intArray_Function_3;
friend class intArray_Function_4;
friend class intArray_Function_5;
friend class intArray_Function_6;
friend class intArray_Function_7;
friend class intArray_Function_8;
friend class intArray_Function_9;
friend class intArray_Function_10;
friend class intArray_Function_11;
friend class intArray_Function_12;
friend class intArray_Function_13;
friend class intArray_Function_14;
friend class intArray_Function_15;
friend class intArray_Function_16;

friend class intArray_Aggregate_Operator;

#endif

     private:
     // Constants for operator identification!
        static const int Plus;
        static const int Minus;
        static const int Times;
        static const int Divided_By;
        static const int Modulo;
        static const int cos_Function;
        static const int sin_Function;
        static const int tan_Function;
        static const int acos_Function;
        static const int asin_Function;
        static const int atan_Function;
        static const int cosh_Function;
        static const int sinh_Function;
        static const int tanh_Function;
        static const int acosh_Function;
        static const int asinh_Function;
        static const int atanh_Function;
        static const int log_Function;
        static const int log10_Function;
        static const int exp_Function;
        static const int sqrt_Function;
        static const int fabs_Function;
        static const int abs_Function;
        static const int ceil_Function;
        static const int floor_Function;
        static const int atan2_Function;
        static const int Unary_Minus;
        static const int fmod_Function;
        static const int mod_Function;
        static const int pow_Function;
        static const int sign_Function;
        static const int sum_Function;
        static const int min_Function;
        static const int max_Function;
        static const int Not;
        static const int LT;
        static const int GT;
        static const int LTEQ;
        static const int GTEQ;
        static const int EQ;
        static const int NOT_EQ;
        static const int AND;
        static const int OR;
        static const int Scalar_Plus;
        static const int Scalar_Minus;
        static const int Scalar_Times;
        static const int Scalar_Divided_By;
        static const int Scalar_Modulo;
        static const int Scalar_cos_Function;
        static const int Scalar_sin_Function;
        static const int Scalar_tan_Function;
        static const int Scalar_acos_Function;
        static const int Scalar_asin_Function;
        static const int Scalar_atan_Function;
        static const int Scalar_cosh_Function;
        static const int Scalar_sinh_Function;
        static const int Scalar_tanh_Function;
        static const int Scalar_acosh_Function;
        static const int Scalar_asinh_Function;
        static const int Scalar_atanh_Function;
        static const int Scalar_log_Function;
        static const int Scalar_log10_Function;
        static const int Scalar_exp_Function;
        static const int Scalar_sqrt_Function;
        static const int Scalar_fabs_Function;
        static const int Scalar_abs_Function;
        static const int Scalar_ceil_Function;
        static const int Scalar_floor_Function;
        static const int Scalar_atan2_Function;
        static const int Scalar_Unary_Minus;
        static const int Scalar_fmod_Function;
        static const int Scalar_mod_Function;
        static const int Scalar_pow_Function;
        static const int Scalar_sign_Function;
        static const int Scalar_sum_Function;
        static const int Scalar_min_Function;
        static const int Scalar_max_Function;
        static const int Scalar_Not;
        static const int Scalar_LT;
        static const int Scalar_GT;
        static const int Scalar_LTEQ;
        static const int Scalar_GTEQ;
        static const int Scalar_EQ;
        static const int Scalar_NOT_EQ;
        static const int Scalar_AND;
        static const int Scalar_OR;

     // These are considered as terminating a statement!
        static const int Assignment;
        static const int replace_Function;
        static const int Plus_Equals;
        static const int Minus_Equals;
        static const int Times_Equals;
        static const int Divided_By_Equals;
        static const int Modulo_Equals;
        static const int Scalar_Assignment;
        static const int Scalar_replace_Function;
        static const int Scalar_Plus_Equals;
        static const int Scalar_Minus_Equals;
        static const int Scalar_Times_Equals;
        static const int Scalar_Divided_By_Equals;
        static const int Scalar_Modulo_Equals;
        static const int indexMap_Function;
        static const int view_Function;
        static const int display_Function;

     // Bitwise operators
        static const int BitwiseComplement;
        static const int BitwiseAND;
        static const int BitwiseOR;
        static const int BitwiseXOR;
        static const int BitwiseLShift;
        static const int BitwiseRShift;
        static const int Scalar_BitwiseAND;
        static const int Scalar_BitwiseOR;
        static const int Scalar_BitwiseXOR;
        static const int Scalar_BitwiseLShift;
        static const int Scalar_BitwiseRShift;
        static const int BitwiseAND_Equals;
        static const int BitwiseOR_Equals;
        static const int BitwiseXOR_Equals;
        static const int Scalar_BitwiseAND_Equals;
        static const int Scalar_BitwiseOR_Equals;
        static const int Scalar_BitwiseXOR_Equals;

     // Conversion operators
        static const int convertTo_intArrayFunction;
        static const int convertTo_floatArrayFunction;
        static const int convertTo_doubleArrayFunction;

     public:
        static bool PREINITIALIZE_OBJECT_IN_CONSTRUCTOR;
        static double      PREINITIALIZE_VALUE;
        static bool USE_DESCRIPTOR_CACHING;

     public:
     // Every array object contains an array descriptor (a special case is the use of
     // expression templates where the array descriptor is stored as a member in a base
     // class of the array object).
#if !defined(USE_EXPRESSION_TEMPLATES)
     // If Expression templates are in use then the array_descriptor is 
     // stored in the Indexable base class!
     // Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> Array_Descriptor;
        doubleArray_Descriptor_Type Array_Descriptor;
#endif

     // #################################################################
     // Access functions for array data (descriptors, domains, and data)
     // #################################################################

     // Access functions to private data to permit use with other languages.
     // Function is inlined for use with the A++ preprocessor
        inline double* APP_RESTRICT_MACRO getDataPointer () const;
#if defined(PPP)
        doubleSerialArray*  getSerialArrayPointer () const;

     // Used for parallel support of "where statements"
        doubleSerialArray** getSerialArrayPointerLoc () const;
#endif

     // Access function for Descriptor
     // Array_Descriptor_Type* getDescriptorPointer () const;
     // Array_Descriptor_Type* getDomainPointer () const;
     // Array_Domain_Type* getDomainPointer () const;

#if 0
     // This function does not appear to be used anywhere!
     // int getDataOffset ( int Axis ) const;
        inline int getDataOffset ( int Axis ) const
             { 
#if defined(PPP)
            // P++ calls the A++ getDataOffset function
               //return getSerialArrayDescriptor()->getDataOffset(Axis);
               return getSerialArrayDescriptor().Array_Domain.Base[Axis];
#else
               return Array_Descriptor.Array_Domain.Base[Axis];
#endif
             };
#endif

     // Access function for Descriptor (if we make this return a const & then we can only call const member functions using it)
        inline const doubleArray_Descriptor_Type & getDescriptor () const
             { return Array_Descriptor; };

     // Access function for Descriptor
        inline const doubleArray_Descriptor_Type* getDescriptorPointer () const
             { return &Array_Descriptor; };

        inline const Array_Domain_Type & getDomain () const
           { return Array_Descriptor.Array_Domain; };

     // inline const Array_Domain_Type* getDomainPointer () const
        inline const Array_Domain_Type* getDomainPointer () const
           { return &Array_Descriptor.Array_Domain; };

#if defined(PPP)
     // Access functions for serial data
        doubleSerialArray_Descriptor_Type &
	   getSerialArrayDescriptor() const
	      { return Array_Descriptor.SerialArray->Array_Descriptor; };

        SerialArray_Domain_Type& getSerialDomain() const 
	   { return Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain; };
#endif

#if !defined(PPP)
#if defined(USE_EXPRESSION_TEMPLATES)
        void setupIndirectAddressingSupport() const
           { Array_Descriptor.setupIndirectAddressingSupport(); }

#if 0
        virtual doubleArray* getdoubleArrayPointer() const
           {
          // printf ("Calling doubleArray* getdoubleArrayPointer() \n");
             return (doubleArray*) this;
           }
#endif

     // Scalar Indexing functions made for expression templates.
        double  scalar_indexing_operator(int i) const 
           { 
          // We needed a scalar indexing operator which could be used within
          // the objects using indirect addressing or not indirect addressing.
          // This function is inlined into the operator()(int) below!
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.offset(i)]; 
           }
        double  operator()(int i) const 
           { 
             APP_ASSERT (Array_Descriptor.usesIndirectAddressing() == FALSE);
          // printf ("CONST Array_Descriptor.offset(%d) = %d \n",i,Array_Descriptor.offset(i));
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return scalar_indexing_operator(i);
           }
        double &operator()(int i)       
           { 
             APP_ASSERT (Array_Descriptor.usesIndirectAddressing() == FALSE);
          // printf ("NON-CONST Array_Descriptor.offset(%d) = %d \n",i,Array_Descriptor.offset(i));
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.offset(i)]; 
           }
        int computeSubscript ( int axis, int i ) const
           { return Array_Descriptor.computeSubscript(axis,i); }

        int indirectOffset_1D ( int i ) const
           { return Array_Descriptor.indirectOffset_1D(i); }

#if (MAX_ARRAY_DIMENSION >= 2)
        double  operator()(int i, int j) const 
           { return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.offset(i, j)]; }
        double &operator()(int i, int j)       
           { return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.offset(i, j)]; }
        int indirectOffset_2D ( int i ) const
           { return Array_Descriptor.indirectOffset_2D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        double  operator()(int i, int j, int k) const 
           { return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.offset(i, j, k)]; }
        double &operator()(int i, int j, int k)       
           { return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.offset(i, j, k)]; }
        int indirectOffset_3D ( int i ) const
           { return Array_Descriptor.indirectOffset_3D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        double  operator()(int i, int j, int k, int l) const 
           { return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.offset(i, j, k, l)]; }
        double &operator()(int i, int j, int k, int l)       
           { return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.offset(i, j, k, l)]; }
        int indirectOffset_4D ( int i ) const
           { return Array_Descriptor.indirectOffset_4D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        double  operator()(int i, int j, int k, int l, int m) const 
           { return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.offset(i, j, k, l, m)]; }
        double &operator()(int i, int j, int k, int l, int m)
           { return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.offset(i, j, k, l, m)]; }
        int indirectOffset_5D ( int i ) const
           { return Array_Descriptor.indirectOffset_5D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        double  operator()(int i, int j, int k, int l, int m, int n) const 
           { 
          // printf ("CONST Array_Descriptor.offset(%d,%d,%d,%d,%d,%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset(i, j, k, l, m, n));
             return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.offset(i, j, k, l, m, n)]; 
           }
        double &operator()(int i, int j, int k, int l, int m, int n)
           { 
          // printf ("NON-CONST Array_Descriptor.offset(%d,%d,%d,%d,%d,%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset(i, j, k, l, m, n));
             return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.offset(i, j, k, l, m, n)]; 
           }
        int indirectOffset_6D ( int i ) const
           { return Array_Descriptor.indirectOffset_6D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        double  operator()(int i, int j, int k, int l, int m, int n, int o) const 
           { return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.offset(i, j, k, l, m, n, o)]; }
        double &operator()(int i, int j, int k, int l, int m, int n, int o)
           { return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.offset(i, j, k, l, m, n, o)]; }
        int indirectOffset_7D ( int i ) const
           { return Array_Descriptor.indirectOffset_7D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        double  operator()(int i, int j, int k, int l, int m, int n, int o, int p) const 
           { return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.offset(i, j, k, l, m, n, o, p)]; }
        double &operator()(int i, int j, int k, int l, int m, int n, int o, int p)       
           { return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.offset(i, j, k, l, m, n, o, p)]; }
        int indirectOffset_8D ( int i ) const
           { return Array_Descriptor.indirectOffset_8D(i); }
#endif

     // Already declared!
     // bool usesIndirectAddressing()const  { return Array_Descriptor.usesIndirectAddressing(); }
     // inline bool usesIndirectAddressing() const { return FALSE; }
        inline bool containsArrayID ( int lhs_id ) const { return FALSE; }
        int internalNumberOfDimensions() const
           { return Array_Descriptor.internalNumberOfDimensions(); }

// end of defined(USE_EXPRESSION_TEMPLATES)
#endif

// end of !defined(PPP)
#endif

#if !defined(USE_DEFERED_EXPRESSION_EVALUATION)
     // This supports the defered evaluation (specifically it supports the means by which
     // the lifetimes of variables are extended by the defered evaluation)!
        Operand_Storage *Array_Storage;
#endif

     // Data required for the "new" and "delete" operators!
        static doubleArray *Current_Link;
        doubleArray *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

     public:
     // Control for display  (used in display member function)
        static const int DECIMAL_DISPLAY_FORMAT;
        static const int EXPONENTIAL_DISPLAY_FORMAT;
        static const int SMART_DISPLAY_FORMAT;
        static int DISPLAY_FORMAT;

     private:
     // Reference Counting for A++ array objects -- allows user to use
     // multiple pointers to the same A++ array object.
     // *************************************************
     // Reference counting data and functions
        int referenceCount;

     public:
       // Use of reference count access is made available for debugging
          inline int getReferenceCount() const
             { return referenceCount; }

       // Support functions for reference counting (inlined for performance)
          inline void incrementReferenceCount() const
             { ((doubleArray*) this)->referenceCount++; }
          inline void decrementReferenceCount() const
             { ((doubleArray*) this)->referenceCount--; }

          inline void resetRawDataReferenceCount() const 
             { 
            // When we steal the data and reuse it from a temporary we have to delete it
            // which decrements the reference count to -1 (typically) and then we copy the
            // data pointers.  But we have to reset the reference cout to ZERO as well.
            // This function allows that operation to be insulated from the details of how the
            // reference count is implemented.  It is used in the A++ operator= ( arrayobject )
#if defined(PPP)
               //APP_ASSERT (getSerialArrayDescriptor() != NULL);
	       // ... (8/7/97,kdb) don't use referenceCount for descriptor 
               //getSerialArrayDescriptor().referenceCount = 0;
            // Array_Descriptor.SerialArray->referenceCount = 0;
               Array_Descriptor.SerialArray->resetReferenceCount();
#else
            // Array_Descriptor_Type::Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // APP_Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID] = 0;
               Array_Descriptor.resetRawDataReferenceCount();
#endif
             }

          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return doubleArray_Descriptor_Type::getReferenceCountBase(); 
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
            // return 0;
               return doubleArray_Descriptor_Type::getRawDataReferenceCountBase(); 
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
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               return Array_Descriptor.SerialArray->getReferenceCount(); 
#else
               return Array_Descriptor.getRawDataReferenceCount();
#endif
             }

          inline void incrementRawDataReferenceCount() const 
             { 
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               Array_Descriptor.SerialArray->incrementReferenceCount(); 
#else
               Array_Descriptor.incrementRawDataReferenceCount();
#endif
             }

          inline void decrementRawDataReferenceCount() const 
             { 
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               Array_Descriptor.SerialArray->decrementReferenceCount(); 
#else
               Array_Descriptor.decrementRawDataReferenceCount();
#endif
             }

       // This function is useful for tracking down memory reference problems
          doubleArray & displayReferenceCounts ( const char* label = "" ) const;

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

#if 0
          inline void ReferenceCountedDelete ()
             {
            // This is a dangerous sort of function since it deletes the class which this
            // function is a member of!.  As long as we don't do anything after calling
            // delete the function just returns and seem to work fine.
               printf ("ERROR: doubleArray::ReferenceCountedDelete called! \n");
               exit(1);
   
            // I'm forced to INCLUDE this code because A++ must have a 
            // consistant interface with other classes in Overture++.
            // The problem that made this sort of statement was another bug
            // that was fixed in the reshape function.
               delete this;

            // if (--referenceCount < 0)
            //      delete this;
             }
#endif
     // *************************************************

     private:
     // Because inlined functions can't contain loops we seperate 
     // out this function which is used in the new operator
        static void New_Function_Loop ();

     public:

     // Used to free memory-in-use internally in doubleArray objects!
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();
        static int numberOfInternalArrays();

#if defined(INLINE_FUNCTIONS)
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
     // DQ (1/9/2007): Commented out the defined "operator delete" since it was a problem for the GNU g++ 3.4.x compiler.
#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )
        void operator delete (void* Pointer, size_t Size);
#endif
     // DQ (1/9/2007): Added more normal delete operator to compile using GNU g++ 3.4.x
     // void operator delete (size_t Size);

     // Default Destructor and Constructor
        virtual ~doubleArray ();
        doubleArray ();

     // Constructors

     // Seperate constructors required to avoid ambiguous call errors in call resolution 
     // between the different types

     // ======================================================
        doubleArray ( int i );
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleArray ( int i , int j );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        doubleArray ( int i , int j , int k );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        doubleArray ( int i , int j , int k , int l );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        doubleArray ( int i , int j , int k , int l , int m );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        doubleArray ( int i , int j , int k , int l , int m, int n );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        doubleArray ( int i , int j , int k , int l , int m, int n, int o );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        doubleArray ( int i , int j , int k , int l , int m, int n, int o, int p);
#endif

     // ======================================================

        doubleArray ( ARGUMENT_LIST_MACRO_INTEGER, bool Force_Memory_Allocation  );

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if defined(APP) || defined(PPP)

     // ======================================================
        doubleArray ( int i , const Partitioning_Type & Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleArray ( int i , int j , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        doubleArray ( int i , int j , int k , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        doubleArray ( int i , int j , int k , int l , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        doubleArray ( int i , int j , int k , int l , int m, const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        doubleArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        doubleArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        doubleArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleArray ( int i , int j , const Partitioning_Type & Partition,
	            bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        doubleArray ( int i , int j , int k , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        doubleArray ( int i , int j , int k , int l , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        doubleArray ( int i , int j , int k , int l , int m, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        doubleArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        doubleArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        doubleArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition , 
		    bool Force_Memory_Allocation );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// defined(APP) || defined(PPP)
#endif

     private:
     // Called inside of each of the Initialize_Array functions (below)
        void preInitializeArray ();

     // Called inside of all array object constructors
        void initializeArray ();

#if defined(APP) || defined(PPP)
     // Called inside of all array object constructors
        void initializeArray ( const Partitioning_Type & Partition );
        void initializeArray ( const Internal_Partitioning_Type & Partition );
#endif

     public:
     // We have to list all the possible variations to avoid a scalar
     // being promoted to a Range object (which has base == bound)
     // The solution is to provide all the permutations of the operator()
     // and promote scalars to a range (defaultBase:defaultBase+Scalar-1)
     // doubleArray ( const Range & I_Base_Bound , 
     //             const Range & J_Base_Bound = APP_Unit_Range , 
     //             const Range & K_Base_Bound = APP_Unit_Range , 
     //             const Range & L_Base_Bound = APP_Unit_Range );
        doubleArray ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // These are for internal use and allow initialization from an array of data
        doubleArray ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

#if defined(PPP) || defined (APP)
     // Added to support resize function used with an existing partitioning object
        doubleArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List,
                    const Internal_Partitioning_Type & partition );
#endif

        doubleArray ( int Number_Of_Valid_Dimensions, const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // This constructor avoids an ambiguous call to the copy constructor 
     // (since a doubleArray could be built from a Range and an int is the same as a bool)
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleArray ( const Range & I_Base_Bound , int j );
#endif

#if defined(PPP)
     // Construct a doubleArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // Additionally for P++ the local partition must be provided.  Due to restrictions on the
     // type of partitioning supported (i.e. FORTRAN D Partitioning: BLOCK PARTI partitioning)
     // a specific partitoning might not conform to what P++ can presently handle.  The result
     // is an error.  So it is not a good idea to use this function in the parallel environment.
     // doubleArray::doubleArray ( double* Data_Pointer , int i , const Range & Range_I , 
     //                                           int j , const Range & Range_J ,
     //                                           int k , const Range & Range_K ,
     //                                           int l , const Range & Range_L );
     // doubleArray::doubleArray ( const double* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        doubleArray ( const double* Data_Pointer , 
                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );

#if 0
     // We can't use this member function and remain dimension independent (so it is removed for now)
        doubleArray ( double* Data_Pointer , const Range & Span_I , const Range & Range_I , 
                                                  const Range & Span_J , const Range & Range_J ,
                                                  const Range & Span_K , const Range & Range_K ,
                                                  const Range & Span_L , const Range & Range_L );
#endif
#else
     // Construct a doubleArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // doubleArray ( double* Data_Pointer , int i , int j = 1 , int k = 1 , int l = 1 );
        doubleArray ( const double* Data_Pointer , ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );

     // doubleArray ( double* Data_Pointer , const Range & Span_I , 
     //             const Range & Span_J = APP_Unit_Range , 
     //             const Range & Span_K = APP_Unit_Range , 
     //             const Range & Span_L = APP_Unit_Range );
        doubleArray ( const double* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
#endif

     // Constructors to support deffered evaluation of array expressions! (these should be private, I think)
     // I'm not certain these are still used!
        doubleArray ( int Operation_Code , doubleArray *Lhs_Pointer );
        doubleArray ( int Operation_Code , doubleArray *Lhs_Pointer , doubleArray *Rhs_Pointer );

     // Copy Constructors
        doubleArray ( const doubleArray & X , int Type_Of_Copy = DEEPCOPY );

#if defined(APP) || defined(SERIAL_APP)
     // Constructor for initialization from a string (We built the AppString type to avoid autopromotion problems)
     // doubleArray ( const char* );
        doubleArray ( const AppString & X );
#endif

     // Evaluation operator! Evaluates an expression and asigns local scope to its result
     // useful when handing expresions to functions that pass by reference.
     // There is no peformance penalty associated with this function since no memory
     // is copied! 
        friend doubleArray evaluate ( const doubleArray & X );

     // reference operator (builds a view) (an M++ function)
        doubleArray & reference ( const doubleArray & X );
        doubleArray & breakReference ();

        bool Is_Built_By_Defered_Evaluation() const;

     // void display ( const char *Label = "" ) const;
     // doubleArray & display ( const char *Label = "" );
        doubleArray & view    ( const char *Label = "" ) const;
        doubleArray & display ( const char *Label = "" ) const;
        doubleArray & globalDisplay ( const char *Label = "" ) const;
#if defined(USE_EXPRESSION_TEMPLATES)
        void MDI_Display() const;
#endif

     // Provided to permit easy access from dbx debugger
#ifdef __clang__
     // Clang requires default arguments of friend functions to appear in the definitions.
        friend void APP_view    ( const doubleArray & X , const char *Label );
        friend void APP_display ( const doubleArray & X , const char *Label );
#else

     // DQ (7/23/2020): This might be dependent on EDG more than GNU version 10.
#if ((__GNUC__ == 10) && (__GNUC_MINOR__ >= 1) && (__GNUC_PATCHLEVEL__ >= 0))
     // GNU 10 requires default arguments of friend functions to appear in the definitions.
        friend void APP_view    ( const doubleArray & X , const char *Label );
        friend void APP_display ( const doubleArray & X , const char *Label );
#else
        friend void APP_view    ( const doubleArray & X , const char *Label = "" );
        friend void APP_display ( const doubleArray & X , const char *Label = "" );
#endif
#endif

     // This is a member function defined in the BaseArray class
        int Array_ID () const;

        inline void setTemporary( bool x ) const { Array_Descriptor.setTemporary(x); }
        inline bool isTemporary() const { return Array_Descriptor.isTemporary(); }
        inline bool isNullArray() const { return Array_Descriptor.isNullArray(); }
        inline bool isView()      const { return Array_Descriptor.isView(); }
        inline bool isContiguousData() const { return Array_Descriptor.isContiguousData(); }

     // Equivalent to operator= (double x)
        doubleArray & fill ( double x );

     // Fills the array with different values given a base and stride
        doubleArray & seqAdd ( double Base = (double) 0 , double Stride = (double) 1 );  // Start at zero and count by 1

     // This function tests for many sorts of errors that could crop up in
     // the internal usage of the doubleArray objects -- it is a debugging tool!
        void Test_Consistency ( const char *Label = "" ) const;
        inline bool Binary_Conformable  ( const doubleArray & X ) const;

     // Part of implementation for diagnostics -- permits more aggressive a 
     // destructive testing than Test_Consistency(). (private interface)
        void testArray();

     // Test for confomability regardless of the input!
     // Templates would simplify this since the template need not be "bound"
     // See Lippman for more details on templates (bound and unbound)!

     // Public interface
        bool isConformable ( const doubleArray & X ) const;
        bool isConformable ( const floatArray & X ) const;
        bool isConformable ( const intArray & X ) const;

     // For internal use (though it is marked public at the moment)
        void Test_Conformability ( const doubleArray & X ) const;
        void Test_Conformability ( const floatArray & X ) const;
        void Test_Conformability ( const intArray & X ) const;

#if defined(INTARRAY)
     // intArray ( const Index & X );
     //  ... (7/20/98,kdb) function below is commented out in src code
     //   so remove here to avoid linking problems ...
     // intArray ( const Internal_Index & X );
        intArray & operator=( const Internal_Index & X );
        intArray & indexMap ();
#endif

     // Range checking function for scalar indexing (this has been moved to thr descriptor class)
     // void Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const;

#if !defined(USE_EXPRESSION_TEMPLATES)
        bool usesIndirectAddressing()const  { return Array_Descriptor.usesIndirectAddressing(); }

     // Indexing operators (scalar)
     // To get constness correct we should move to using this construction
     // The indexing of a const array should return a const ref and 
     // the indexing of a nonconst array should return a nonconst ref.
     // inline const double & operator() ( int i ) const;
     // inline       double & operator() ( int i );
        inline double & operator() ( int i ) const;

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const double & operator() ( int i , int j ) const;
        inline       double & operator() ( int i , int j );
#else
        inline double & operator() ( int i , int j ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const double & operator() ( int i , int j , int k ) const;
        inline       double & operator() ( int i , int j , int k );
#else
        inline double & operator() ( int i , int j , int k ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const double & operator() ( int i , int j , int k , int l ) const;
        inline       double & operator() ( int i , int j , int k , int l );
#else
        inline double & operator() ( int i , int j , int k , int l ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const double & operator() ( int i , int j , int k , int l, int m ) const;
        inline       double & operator() ( int i , int j , int k , int l, int m );
#else
        inline double & operator() ( int i , int j , int k , int l, int m ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const double & operator() ( int i , int j , int k , int l, int m , int n ) const;
        inline       double & operator() ( int i , int j , int k , int l, int m , int n );
#else
        inline double & operator() ( int i , int j , int k , int l, int m , int n ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const double & operator() ( int i , int j , int k , int l, int m , int n , int o ) const;
        inline       double & operator() ( int i , int j , int k , int l, int m , int n , int o );
#else
        inline double & operator() ( int i , int j , int k , int l, int m , int n , int o ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const double & operator() ( int i , int j , int k , int l, int m , int n , int o , int p ) const;
        inline       double & operator() ( int i , int j , int k , int l, int m , int n , int o , int p );
#else
        inline double & operator() ( int i , int j , int k , int l, int m , int n , int o , int p ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// End of if !defined(USE_EXPRESSION_TEMPLATES)
#endif

        void Scalar_Indexing_For_doubleArray_With_Message_Passing (
             int Address_Subscript, bool Off_Processor_With_Ghost_Boundaries,
             bool Off_Processor_Without_Ghost_Boundaries, int & Array_Index_For_double_Variable) const;

     /* Indexing operators (using Index objects)
     // Indexing operators have to have local scope (since we don't want them to have
     // a permanent lifetime).  By having local scope they there lifetime is
     // controled by the compiler (this means they hang around longer than temporaries
     // and have the same scope as local variable).  This is NOT a problem as is would be
     // for temporaries since views returned by the indexing operators have no array data
     // associated with them (directly -- they peek into another array objects data).
     // Thus these indexing operators return by value not by reference.  
     // The importance of this is clear if we imagine a function taking a view by reference
     // and then managing its lifetime -- after its first use in an array expression the 
     // view would have been deleted and then susequent use of it would be an error 
     // (as in segment fault)!
     */

     // Forces one dimensional interpretation of multidimensional doubleArray!
        doubleArray operator() () const;

     // This operator() takes an array of pointers to Internal_Index objects (length MAX_ARRAY_DIMENSION)
        doubleArray operator() ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;
        doubleArray operator() ( int Number_Of_Valid_Dimensions, const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;

     // Indexing operators for use with Index objects
     //   doubleArray operator() 
     //( ARGUMENT_LIST_MACRO_CONST_REF_INTERNAL_INDEX_WITH_PREINITIALIZATION )
     //const ;

#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleArray operator() ( const Internal_Index & I ) const;
              doubleArray operator() ( const Internal_Index & I );
#else
        doubleArray operator() ( const Internal_Index & I ) const;
#endif

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J );
#else
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K );
#else
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L );
#else
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M );
#else
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N );
#else
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O );
#else
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P );
#else
        doubleArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if !defined(PPP)
     // ... use this to make scalar indexing subscripts consistent ...
        void Fixup_User_Base 
	 (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List,
	  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List);
#endif

     // Simplifies getting a pointer to a view which is used internally in A++/P++
     // The Index_Array is length MAX_ARRAY_DIMENSION == 4
     // static doubleArray* Build_Pointer_To_View_Of_Array ( const doubleArray & X , Internal_Index *Index_Array );
        static doubleArray* Build_Pointer_To_View_Of_Array ( const doubleArray & X ,
                                                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Indexing operators for use with Index objects
        doubleArray operator() ( const Internal_Indirect_Addressing_Index & I ) const;
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        doubleArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        doubleArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        doubleArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        doubleArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        doubleArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N,
                               const Internal_Indirect_Addressing_Index & O ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        doubleArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N
                               const Internal_Indirect_Addressing_Index & O, const Internal_Indirect_Addressing_Index & P ) const;
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

     /* All other operators which can return actual array data are implemented as
     // return by reference so that their data can be manipulate (so specifically
     // so that the compiler will not manipulate the lifetime and as result
     // leave too many array object around (ones with real array data attached and thus ones which
     // would take up lots of space)).  This is a priciple part of the problem
     // with poorly designed array classes (temporaries must be managed to avoid 
     // waisting memory (especially in numerical applications)).
     // The restriction is that while views can be passed by reference into functions
     // expressions can NOT be passes by reference into functions.  For the sake of
     // temporary management (and thus efficient control over the memory useage)
     // we give up the ability to pass temporaries into functions.  The compromise is to 
     // use the "evaluate()" function which will have a non temporary object from the 
     // temporary one.  If you have a better idea please let me know: dquinlan@lanl.gov.
     // So expresions can be passed as parameters to functions taking input by reference
     // it just requires the use of the "evaluate()" function.
     */

     // Unary Operators + and -
        doubleArray & operator+ () const;
        doubleArray & operator- () const;

     // Prefix Increment and Decrement Operators ++A and --A
        doubleArray & operator++ ();
        doubleArray & operator-- ();

     // Postfix Increment and Decrement Operators A++ and A--  (argument is ZERO see 594 r.13.4.7 Stroustrup)
        doubleArray & operator++ (int x);
        doubleArray & operator-- (int x);

#if !defined(USE_EXPRESSION_TEMPLATES)
     // operator+ related functions
        friend doubleArray & operator+ ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator+ ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator+ ( const doubleArray & Lhs , double x );
        doubleArray & operator+= ( const doubleArray & Rhs );
        doubleArray & operator+= ( double Rhs );

     // operator- related functions
        friend doubleArray & operator- ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator- ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator- ( const doubleArray & Lhs , double x );
        doubleArray & operator-= ( const doubleArray & Rhs );
        doubleArray & operator-= ( double Rhs );

     // operator* related functions
        friend doubleArray & operator* ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator* ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator* ( const doubleArray & Lhs , double x );
        doubleArray & operator*= ( const doubleArray & Rhs );
        doubleArray & operator*= ( double Rhs );

     // operator/ related functions
        friend doubleArray & operator/ ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator/ ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator/ ( const doubleArray & Lhs , double x );
        doubleArray & operator/= ( const doubleArray & Rhs );
        doubleArray & operator/= ( double Rhs );

     // operator% related functions
        friend doubleArray & operator% ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator% ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator% ( const doubleArray & Lhs , double x );
        doubleArray & operator%= ( const doubleArray & Rhs );
        doubleArray & operator%= ( double Rhs );

#ifdef INTARRAY
     // Unary Operator ~ (bitwise one's complement)
        doubleArray & operator~ () const;

     // operator& related functions
        friend doubleArray & operator& ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator& ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator& ( const doubleArray & Lhs , double x );
        doubleArray & operator&= ( const doubleArray & Rhs );
        doubleArray & operator&= ( double Rhs );

     // operator| related functions
        friend doubleArray & operator| ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator| ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator| ( const doubleArray & Lhs , double x );
        doubleArray & operator|= ( const doubleArray & Rhs );
        doubleArray & operator|= ( double Rhs );

     // operator^ related functions
        friend doubleArray & operator^ ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator^ ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator^ ( const doubleArray & Lhs , double x );
        doubleArray & operator^= ( const doubleArray & Rhs );
        doubleArray & operator^= ( double Rhs );

     // operator<< related functions
        friend doubleArray & operator<< ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator<< ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator<< ( const doubleArray & Lhs , double x );

     // operator>> related functions
        friend doubleArray & operator>> ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend doubleArray & operator>> ( double x    , const doubleArray & Rhs );
        friend doubleArray & operator>> ( const doubleArray & Lhs , double x );
#endif

     // Internal conversion member functions (used in casting operators)
        intArray    & convertTo_intArray    () const;
        floatArray  & convertTo_floatArray  () const;
        doubleArray & convertTo_doubleArray () const;

        doubleArray & operator= ( double x );
        doubleArray & operator= ( const doubleArray & Rhs );
#else
     // Need these to be defined for the expression template version
        doubleArray & operator+= ( const doubleArray & Rhs );
        doubleArray & operator+= ( double Rhs );

        doubleArray & operator-= ( const doubleArray & Rhs );
        doubleArray & operator-= ( double Rhs );

        doubleArray & operator*= ( const doubleArray & Rhs );
        doubleArray & operator*= ( double Rhs );

        doubleArray & operator/= ( const doubleArray & Rhs );
        doubleArray & operator/= ( double Rhs );

        doubleArray & operator%= ( const doubleArray & Rhs );
        doubleArray & operator%= ( double Rhs );
#endif

     // Access function for static Base and Bound variables in Array_Descriptor class!
        friend void setGlobalBase( int Base );
        friend int getGlobalBase();
        friend char* APP_version();

     // abort function for A++ (defined in domain.h)
     // friend void APP_ABORT();
     // friend void APP_Assertion_Support( char* Source_File_With_Error, unsigned Line_Number_In_File );

     // friend void setGlobalBase( int Base , int Axis );
     // friend int getGlobalBase( int Axis );

     // void setBase( int New_Base_For_All_Axes );
        doubleArray & setBase( int New_Base_For_All_Axes );
        int getBase() const;
        int getRawBase() const;

     // Access function for static Base and Bound variables (by Axis)!
     // void setBase( int Base , int Axis );
        doubleArray & setBase( int Base , int Axis );
        doubleArray & setParallelBase( int Base , int Axis );
        int getBase( int Axis ) const;
        int getStride( int Axis ) const;
        int getBound( int Axis ) const;
        int getLength( int Axis ) const;
        int getRawDataSize( int Axis ) const;
        int getRawBase( int Axis ) const;
        int getRawStride( int Axis ) const;
        int getRawBound( int Axis ) const;
        int getDataBase( int Axis ) const;
        int numberOfDimensions () const;
        Range dimension( int Axis ) const;
        Range getFullRange( int Axis ) const;


#if defined(APP) || defined(PPP)

#if defined(PPP)
     // Added (7/26/2000)
        Internal_Index getLocalMaskIndex  (int i) const { return Array_Descriptor.getLocalMaskIndex(i); }
        Internal_Index getGlobalMaskIndex (int i) const { return Array_Descriptor.getGlobalMaskIndex(i); }

     // Added (12/13/2000)
        int getLeftNumberOfPoints  ( int Axis ) const { return Array_Descriptor.getLeftNumberOfPoints(Axis);  }
        int getRightNumberOfPoints ( int Axis ) const { return Array_Descriptor.getRightNumberOfPoints(Axis); }
#endif

     // P++ specific functions report on the local partition
        int getLocalBase( int Axis ) const;
        int getLocalStride( int Axis ) const;
        int getLocalBound( int Axis ) const;
        int getLocalLength( int Axis ) const;
        int getLocalRawBase( int Axis ) const;
        int getLocalRawStride( int Axis ) const;
        int getLocalRawBound( int Axis ) const;
        Range localDimension( int Axis ) const;
#endif
        Range getLocalFullRange( int Axis ) const;

	// Returns reference to SerialArray if P++ or itself if A++
#if defined(PPP)
        doubleSerialArray getLocalArray () const;
#elif defined(APP)
        doubleArray getLocalArray () const;
#endif
	// Returns new SerialArray including ghost cells if P++ array
	// is nonview or view of SerialArray if P++ array is a view 
	// or itself if A++
#if defined(PPP)
        doubleSerialArray getLocalArrayWithGhostBoundaries () const;
        doubleSerialArray* getLocalArrayWithGhostBoundariesPointer () const;
// elif defined(APP)
#else
        doubleArray getLocalArrayWithGhostBoundaries () const;
        doubleArray* getLocalArrayWithGhostBoundariesPointer () const;
#endif

     // Dimension independent query functions
        bool isSameBase   ( const doubleArray & X ) const;
        bool isSameBound  ( const doubleArray & X ) const;
        bool isSameStride ( const doubleArray & X ) const;
        bool isSameLength ( const doubleArray & X ) const;
#if defined(APP) || defined(PPP)
        bool isSameGhostBoundaryWidth ( const doubleArray & X ) const;
        //bool isSameDistribution ( const doubleArray & X ) const;
        bool isSameDistribution ( const intArray & X ) const;
        bool isSameDistribution ( const floatArray & X ) const;
        bool isSameDistribution ( const doubleArray & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const doubleArray & X ) const;

     // Member functions for access to the geometry of the A++ object
        int getGeometryBase ( int Axis ) const;
        doubleArray & setGeometryBase ( int New_Geometry_Base , int Axis );

#if defined(PPP)
     // Is a P++ array partitioned along an axis
     // bool isPartitioned ( int Axis ) const;
#endif

     // total array size! (this function should be depreciated in favor of the getSize() function)
        int elementCount() const;

        int getSize() const;
#if defined(APP) || defined(PPP)
        int getLocalSize() const;
#endif

     // Number of columns and rows usefull for 2D arrays (part of  d e f i n e d  interface)!
        int cols() const;
        int rows() const;

     // Control the ability to resize and array throught assignment
     // This whole process is greatly cleaned up over that of M++!
     // int lock();
     // int unlock();

     // redimensioning member operations
     // doubleArray & redim ( int i , int j = 1 , int k = 1 , int l = 1 );
        doubleArray & redim ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // doubleArray & redim ( const Range & i , 
     //                     const Range & j = APP_Unit_Range , 
     //                     const Range & k = APP_Unit_Range , 
     //                     const Range & l = APP_Unit_Range );
        doubleArray & redim ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // We need this to avoid an ERROR: Overloading ambiguity between "doubleArray::redim(const Array_Domain_Type&)" 
     // and "doubleArray::redim(const Range&, const Range&, const Range&, const Range&, const Range&, const Range&)" 
     // so we need to implement a redim that takes a single Index object.
        doubleArray & redim ( const Index & I );
        doubleArray & redim ( const doubleArray & X );
        doubleArray & redim ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleArray & redim ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // The Sun C++ compiler can't handle template declarations in non templated classes
     // template<class T, int Template_Dimension>
     // doubleArray & redim ( const Array_Descriptor_Type<T,Template_Dimension> & X );
        doubleArray & redim ( const Array_Domain_Type & X );

     // Changes dimensions of array object with no change in number of elements
     // doubleArray & reshape ( int i , int j = 1 , int k = 1 , int l = 1 );
        doubleArray & reshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // doubleArray & reshape ( const Range & i , 
     //                       const Range & j = APP_Unit_Range , 
     //                       const Range & k = APP_Unit_Range , 
     //                       const Range & l = APP_Unit_Range );
        doubleArray & reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        doubleArray & reshape ( const doubleArray & X );
        doubleArray & reshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleArray & reshape ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Used for support of weakened conformability assignment in operator= (maybe this should be private or protected)
     // doubleArray & internalReshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // doubleArray & internalReshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

	void fix_view_bases (int* View_Bases, int* View_Sizes, int numdims, 
			     const int* old_lengths, const int* Integer_List);
	void fix_view_bases_non_short_span 
	   (int* View_Bases, int* View_Sizes, int numdims, const int* Integer_List);

     // resize function redimensions array and copies exiting data into new array
     // doubleArray & resize ( int i , int j = 1 , int k = 1 , int l = 1 );
        doubleArray & resize ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // doubleArray & resize ( const Range & i , 
     //                      const Range & j = APP_Unit_Range , 
     //                      const Range & k = APP_Unit_Range , 
     //                      const Range & l = APP_Unit_Range );
        doubleArray & resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        doubleArray & resize ( const doubleArray & X );
        doubleArray & resize ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleArray & resize ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // adopt existing data as an A++ array
     // versions of adopt which are specific to P++
     // To avoid the 1D P++ adopt function from being confused with the
     // 2D A++ adopt function we must provide different member function names.
        doubleArray & adopt ( const double* Data_Pointer , const Partitioning_Type & Partition ,
                                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        doubleArray & adopt ( const double* Data_Pointer , const Partitioning_Type & Partition ,
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // doubleArray & adopt ( const double* Data_Pointer , const Partitioning_Type & Partition ,
     //                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // A++ specific implementation of adopt member functions
     // These A++ specific function are made available to P++ to maintain
     // the identical interface even though under P++ these functions
     // are not implemented.

     // doubleArray & adopt ( const double* Data_Pointer , 
     //                     int i , int j = 1 , int k = 1 , int l = 1 ); 
     // function with interface for internal use only
        doubleArray & adopt ( const double* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
        doubleArray & adopt ( const double* Data_Pointer , 
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleArray & adopt ( const double* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
     // Use the size of an existing A++/P++ array object
        doubleArray & adopt ( const double* Data_Pointer , const doubleArray & X );

     // These are used only within the internal implementation of A++/P++
        doubleArray & adopt ( const double* Data_Pointer , 
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Force a deep copy since deep copy constructors are not alway called
     // even when explicit attempts are made to call them 
        doubleArray & copy ( const doubleArray & X , int DeepCopy = DEEPCOPY );

     // Convert indexing of view to be non HPF!
        doubleArray & useFortranIndexing ( bool Flag );

     // This would effectively be a reference (so call reference instead)!
     // doubleArray & adopt ( const doubleArray & X  );

     // Reduction operators!
        friend double min ( const doubleArray & X );
        friend double max ( const doubleArray & X );

#if defined(PPP)
     // P++ support for reduction operations
     // By making each of these a friend function we give them 
     // access to the doubleArray::Operation_Type variables
        friend void Reduction_Operation ( int Input_Function, double & x );
        friend void Reduction_Operation ( int Input_Function, float & x );
        friend void Reduction_Operation ( int Input_Function, int & x );
#endif

#if defined(USE_EXPRESSION_TEMPLATES)
     // The non-expression template version returns values by value (not by reference)
        friend doubleArray min ( const doubleArray & X , const doubleArray & Y );
        friend doubleArray min ( const doubleArray & X , double y );
        friend doubleArray min ( double x , const doubleArray & Y );
        friend doubleArray min ( const doubleArray & X , const doubleArray & Y , const doubleArray & Z );
        friend doubleArray min ( double x , const doubleArray & Y , const doubleArray & Z );
        friend doubleArray min ( const doubleArray & X , double y , const doubleArray & Z );
        friend doubleArray min ( const doubleArray & X , const doubleArray & Y , double z );

        friend doubleArray max ( const doubleArray & X , const doubleArray & Y );
        friend doubleArray max ( const doubleArray & X , double y );
        friend doubleArray max ( double x , const doubleArray & Y );
        friend doubleArray max ( const doubleArray & X , const doubleArray & Y , const doubleArray & Z );
        friend doubleArray max ( double x , const doubleArray & Y , const doubleArray & Z );
        friend doubleArray max ( const doubleArray & X , double y , const doubleArray & Z );
        friend doubleArray max ( const doubleArray & X , const doubleArray & Y , double z );

     // Misc functions
        doubleArray & replace ( const intArray & X , const doubleArray & Y );
        doubleArray & replace ( int x , const doubleArray & Y );
        doubleArray & replace ( const intArray & X , double y );

        friend double sum ( const doubleArray & X );

     // Sum along axis 
        friend doubleArray sum ( const doubleArray & X , int Axis );
#else

     // The non-expression template version returns values by reference
        friend doubleArray & min ( const doubleArray & X , const doubleArray & Y );
        friend doubleArray & min ( const doubleArray & X , double y );
        friend doubleArray & min ( double x , const doubleArray & Y );
        friend doubleArray & min ( const doubleArray & X , const doubleArray & Y , const doubleArray & Z );
        friend doubleArray & min ( double x , const doubleArray & Y , const doubleArray & Z );
        friend doubleArray & min ( const doubleArray & X , double y , const doubleArray & Z );
        friend doubleArray & min ( const doubleArray & X , const doubleArray & Y , double z );

        friend doubleArray & max ( const doubleArray & X , const doubleArray & Y );
        friend doubleArray & max ( const doubleArray & X , double y );
        friend doubleArray & max ( double x , const doubleArray & Y );
        friend doubleArray & max ( const doubleArray & X , const doubleArray & Y , const doubleArray & Z );
        friend doubleArray & max ( double x , const doubleArray & Y , const doubleArray & Z );
        friend doubleArray & max ( const doubleArray & X , double y , const doubleArray & Z );
        friend doubleArray & max ( const doubleArray & X , const doubleArray & Y , double z );

     // Misc functions
        doubleArray & replace ( const intArray & X , const doubleArray & Y );
        doubleArray & replace ( int x , const doubleArray & Y );
        doubleArray & replace ( const intArray & X , double y );

        friend double sum ( const doubleArray & X );
        friend doubleArray & sum ( const doubleArray & X , const doubleArray & Y );
        friend doubleArray & sum ( const doubleArray & X , const doubleArray & Y , const doubleArray & Z );

     // Sum along axis
        friend doubleArray & sum ( const doubleArray & X , int Axis );
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)
#ifndef INTARRAY
        friend doubleArray & fmod ( const doubleArray & X , double y );
        friend doubleArray & fmod ( double x , const doubleArray & Y );
        friend doubleArray & fmod ( const doubleArray & X , const doubleArray & Y );
#endif
        friend doubleArray & mod ( const doubleArray & X , double y );
        friend doubleArray & mod ( double x , const doubleArray & Y );
        friend doubleArray & mod ( const doubleArray & X , const doubleArray & Y );

        friend doubleArray & pow ( const doubleArray & X , double y );  // returns X**y
        friend doubleArray & pow ( double x , const doubleArray & Y );
        friend doubleArray & pow ( const doubleArray & X , const doubleArray & Y );

        friend doubleArray & sign ( const doubleArray & Array_Signed_Value , const doubleArray & Input_Array );
        friend doubleArray & sign ( double Scalar_Signed_Value             , const doubleArray & Input_Array );
        friend doubleArray & sign ( const doubleArray & Array_Signed_Value , double Input_Value  );

#ifndef INTARRAY
        friend doubleArray & log   ( const doubleArray & X );
        friend doubleArray & log10 ( const doubleArray & X );
        friend doubleArray & exp   ( const doubleArray & X );
        friend doubleArray & sqrt  ( const doubleArray & X );
        friend doubleArray & fabs  ( const doubleArray & X );
        friend doubleArray & ceil  ( const doubleArray & X );
        friend doubleArray & floor ( const doubleArray & X );
#endif
        friend doubleArray & abs   ( const doubleArray & X );

     // Not fully supported yet (only works for 1D or 2D arrays object)!
        friend doubleArray & transpose ( const doubleArray & X );

     // Trig functions!
#ifndef INTARRAY
        friend doubleArray & cos   ( const doubleArray & X );
        friend doubleArray & sin   ( const doubleArray & X );
        friend doubleArray & tan   ( const doubleArray & X );
        friend doubleArray & acos  ( const doubleArray & X );
        friend doubleArray & asin  ( const doubleArray & X );
        friend doubleArray & atan  ( const doubleArray & X );
        friend doubleArray & cosh  ( const doubleArray & X );
        friend doubleArray & sinh  ( const doubleArray & X );
        friend doubleArray & tanh  ( const doubleArray & X );
        friend doubleArray & acosh ( const doubleArray & X );
        friend doubleArray & asinh ( const doubleArray & X );
        friend doubleArray & atanh ( const doubleArray & X );

     // All the different atan2 functions for arrays (ask for by Bill Henshaw)
        friend doubleArray & atan2 ( const doubleArray & X , double y );
        friend doubleArray & atan2 ( double x , const doubleArray & Y );
        friend doubleArray & atan2 ( const doubleArray & X , const doubleArray & Y );
#endif

     // relational operators 
     // operator< related functions
        friend intArray & operator< ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend intArray & operator< ( double x    , const doubleArray & Rhs );
        friend intArray & operator< ( const doubleArray & Lhs , double x );

     // operator<= related functions
        friend intArray & operator<= ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend intArray & operator<= ( double x    , const doubleArray & Rhs );
        friend intArray & operator<= ( const doubleArray & Lhs , double x );

     // operator> related functions
        friend intArray & operator> ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend intArray & operator> ( double x    , const doubleArray & Rhs );
        friend intArray & operator> ( const doubleArray & Lhs , double x );

     // operator>= related functions
        friend intArray & operator>= ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend intArray & operator>= ( double x    , const doubleArray & Rhs );
        friend intArray & operator>= ( const doubleArray & Lhs , double x );

     // operator== related functions
        friend intArray & operator== ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend intArray & operator== ( double x    , const doubleArray & Rhs );
        friend intArray & operator== ( const doubleArray & Lhs , double x );

     // operator!= related functions
        friend intArray & operator!= ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend intArray & operator!= ( double x    , const doubleArray & Rhs );
        friend intArray & operator!= ( const doubleArray & Lhs , double x );

     // operator&& related functions
        friend intArray & operator&& ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend intArray & operator&& ( double x    , const doubleArray & Rhs );
        friend intArray & operator&& ( const doubleArray & Lhs , double x );

     // operator|| related functions
        friend intArray & operator|| ( const doubleArray & Lhs , const doubleArray & Rhs );
        friend intArray & operator|| ( double x    , const doubleArray & Rhs );
        friend intArray & operator|| ( const doubleArray & Lhs , double x );
#else
     // If we ARE using expression templates then we have to declare some friend functions
     // friend double sum ( const doubleArray & X );
     // friend doubleArray sum ( const doubleArray & X , int axis );

        template<class T, class A>
        friend T sum ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T max ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T min ( const Indexable<T,A> & X );
#endif

     // operator! related functions
        intArray & operator! ();

#if defined(APP) || defined(PPP)
     // Note that P++ assumes that any results returned from these functions are not used
     // effect the parallel control flow (this is due to the SPMD nature of a P++ application).
     // No attempt is made to provide access to parallel info in P++ nor is P++ intended to
     // be a way to write explicitly parallel applications (use explicit message passing for that!).

     // These allow an existing array to be partitioned of the
     // existing array repartitioned acording to another existing array
     // in which case the array is alligned with the input array
        Partitioning_Type getPartition () const;
        Internal_Partitioning_Type* getInternalPartitionPointer () const;
#if defined(USE_PADRE)
        void partition ( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition );
     // void partition ( const BaseArray   & Example_Array );
#endif
        doubleArray & partition ( const Internal_Partitioning_Type & Partition );
        doubleArray & partition ( const Partitioning_Type & Partition );
        doubleArray & partition ( const doubleArray & Example_Array );
        doubleArray & partition ( const floatArray & Example_Array );
        doubleArray & partition ( const intArray & Example_Array );

     // typedef support for member function pointers as parameters for the map diagnostic mechanism
     // We support two different function prototypes
        typedef int (doubleArray::*doubleArrayMemberVoidFunctionPointerType) (void) const;
        typedef int (doubleArray::*doubleArrayMemberIntegerFunctionPointerType) (int) const;

     // Builds array with base/bound info for each axis and processor
     // calls special purpose communication function

#if defined(PPP)
     // This only makes since in P++
        intSerialArray buildProcessorMap ( doubleArrayMemberVoidFunctionPointerType X );
        intSerialArray buildProcessorMap ( doubleArrayMemberIntegerFunctionPointerType X );
        intSerialArray buildProcessorMap ( doubleArrayMemberIntegerFunctionPointerType X, doubleArrayMemberIntegerFunctionPointerType Y );
#else
     // Preserve the interface in A++
        intArray buildProcessorMap ( doubleArrayMemberVoidFunctionPointerType X );
        intArray buildProcessorMap ( doubleArrayMemberIntegerFunctionPointerType X );
        intArray buildProcessorMap ( doubleArrayMemberIntegerFunctionPointerType X, doubleArrayMemberIntegerFunctionPointerType Y );
#endif

        void displayArraySizesPerProcessor (const char* Label = "");
        void displayPartitioning  (const char *Label = "");
        void displayCommunication (const char* Label = "");
        void displayLeftRightNumberOfPoints (const char* Label);

     // functions specific to the manipulation of internal ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
        int getGhostBoundaryWidth ( int Axis ) const;

     // Depreciated function (not documented in manual)
        int getInternalGhostCellWidth ( int Axis ) const;

     // Depreciated function (not documented in manual)
        doubleArray & setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleArray & setInternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        doubleArray & setInternalGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // New versions
        doubleArray & setGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleArray & setGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        doubleArray & setGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // functions specific to the manipulation of external ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
     // these are usually zero unless the boundaries are meant to have large NON-PARTITIONED boundary regions
        int getExternalGhostCellWidth ( int Axis ) const;
     // doubleArray & setExternalGhostCellWidth ( int Number_Of_Ghost_Cells_I, int Number_Of_Ghost_Cells_J, 
     //                                         int Number_Of_Ghost_Cells_K, int Number_Of_Ghost_Cells_L );
        doubleArray & setExternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        void updateGhostBoundaries() const;
#endif

#if defined(PPP)
        doubleArray( const double* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        doubleArray( const double* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
#endif
     public:
     // Support for defered evaluation!  (This function does nothing and will be removed soon)
        void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const;

     // This constructor is used by the indexing operators to build array objects defined in
     // local scope.  It allows the support of deferred evaluation on the resulting views!

#if defined(PPP)
        doubleArray ( doubleSerialArray* SerialArray_Pointer , 
        	    const Array_Domain_Type* Array_Domain_Pointer, 
		    bool AvoidBuildingIndirectAddressingView = FALSE );

#if 0
     // This function is called in lazy_operand.C!
        doubleArray ( const doubleSerialArray* SerialArray_Pointer , 
        	    doubleArray_Descriptor_Type* 
		    Array_Descriptor_Pointer ,
        	    /*Array_Descriptor_Type* Array_Descriptor_Pointer ,*/
                    Operand_Storage* Array_Storage_Pointer );
#endif
        doubleArray( const doubleSerialArray* SerialArray_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        doubleArray( const doubleSerialArray* SerialArray_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
		   Indirect_Index_List );
        doubleArray( const doubleSerialArray* SerialArray_Pointer, 
                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
#else

        doubleArray ( const double* Array_Data_Pointer ,
                    const Array_Domain_Type* Array_Domain_Pointer,
                    bool AvoidBuildingIndirectAddressingView = FALSE );

    public:
        doubleArray( const double* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        doubleArray( const double* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

#if 0
     // This function is called in lazy_operand.C!
        doubleArray ( const double* Array_Data_Pointer , 
	   doubleArray_Descriptor_Type* 
	   Array_Descriptor_Pointer , Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
        static double Abstract_Unary_Operator ( const doubleArray & X ,
                  MDI_double_Prototype_14 , int Operation_Type ); 

        static doubleArray & Abstract_Unary_Operator ( const doubleArray & X ,
                  MDI_double_Prototype_0 , MDI_double_Prototype_1 ,
                  int Operation_Type ); 

     /* We need a special binary operator to use in the case of operator- and
     // operator/ since they don't commute and so if the Rhs of the binary operation
     // is a temporary it is reused in a different way that for a commutative
     // operator.  Basically if the Rhs is a temporary we DO reuse it, but we 
     // can't do the equivalent of accumulation into it (which is optimal in
     // efficiency) as in the operator+ and as we can in the case of the Lhs
     // operand being a temporary.  
     // Notice that the non commuting operator reuses the Rhs differently than 
     // it can reuse the Lhs if the Lhs is a temporary.
     // (Lhs = Lhs + Rhs  --> Lhs += Rhs) == (Rhs = Lhs + Rhs --> Rhs += Lhs)
     // (Lhs = Lhs - Rhs  --> Lhs -= Rhs) == (Rhs = Lhs - Rhs --> Rhs = Lhs - Rhs)
     */
        static doubleArray & Abstract_Binary_Operator ( 
                  const doubleArray & Lhs , const doubleArray & Rhs ,
                  MDI_double_Prototype_2 , MDI_double_Prototype_3 ,
                  int Operation_Type );

        static doubleArray & Abstract_Binary_Operator ( 
                  const doubleArray & Lhs , double x ,
                  MDI_double_Prototype_4 , MDI_double_Prototype_5 ,
                  int Operation_Type );
 
        static doubleArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const doubleArray & Lhs , const doubleArray & Rhs ,
                  MDI_double_Prototype_2 , MDI_double_Prototype_3 ,
                  int Operation_Type );

        static doubleArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const doubleArray & Lhs , double x ,
                  MDI_double_Prototype_4 , MDI_double_Prototype_5 ,
                  int Operation_Type );
 
     // Used for replace function since it is of form "doubleArray replace (intArray,doubleArray)"
        doubleArray & Abstract_Modification_Operator ( 
                  const intArray & Lhs , const doubleArray & Rhs ,
                  MDI_double_Prototype_6 ,
                  int Operation_Type ); 

     // Used for replace function since it is of form "doubleArray replace (intArray,double)"
        doubleArray & Abstract_Modification_Operator ( 
                  const intArray & Lhs , double x ,
                  MDI_double_Prototype_7 ,
                  int Operation_Type ); 

#ifndef INTARRAY
     // Used for replace function since it is of form "doubleArray replace (int,doubleArray)"
        doubleArray & Abstract_Modification_Operator ( 
                  const doubleArray & Lhs , int x ,
                  MDI_double_Prototype_8 ,
                  int Operation_Type ); 
#endif

        static void Abstract_Operator_Operation_Equals ( 
                  const doubleArray & Lhs , const doubleArray & Rhs ,
                  MDI_double_Prototype_3 ,
                  int Operation_Type ); 

        static void Abstract_Operator_Operation_Equals ( 
                  const doubleArray & Lhs , double x ,
                  MDI_double_Prototype_5 ,
                  int Operation_Type ); 

        static intArray & Abstract_Unary_Operator_Returning_IntArray ( const doubleArray & X ,
                  MDI_double_Prototype_9 , MDI_double_Prototype_10 ,
                  int Operation_Type ); 

        static intArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const doubleArray & Lhs , const doubleArray & Rhs ,
                  MDI_double_Prototype_11 , MDI_double_Prototype_9 ,
                  int Operation_Type ); 

        static intArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const doubleArray & Lhs , double x ,
                  MDI_double_Prototype_12 , MDI_double_Prototype_13 ,
                  int Operation_Type );

     // Support for conversion operators
        static intArray & Abstract_int_Conversion_Operator (
                  const doubleArray & X ,
                  MDI_double_Prototype_convertTo_intArray ,
                  int Operation_Type );

        static floatArray & Abstract_float_Conversion_Operator (
                  const doubleArray & X ,
                  MDI_double_Prototype_convertTo_floatArray ,
                  int Operation_Type );

        static doubleArray & Abstract_double_Conversion_Operator (
                  const doubleArray & X ,
                  MDI_double_Prototype_convertTo_doubleArray ,
                  int Operation_Type );

#elif defined(PPP)


     // Inline functions must appear in the header file (not the source file) when using the
     // GNU compiler!  We could alternatively use the INLINE macro (perhaps we will later)
     // the INLINE macro is defined to be "" when the GNU compiler is used (though this 
     // should be verified).
#if !defined(GNU)
// inline
#endif
        static void Modify_Reference_Counts_And_Manage_Temporaries (
             Array_Conformability_Info_Type *Array_Set,
             doubleArray *Temporary,
             doubleSerialArray* X_Serial_PCE_Array,
             doubleSerialArray *Data );

#if !defined(GNU)
// inline
#endif
        static void Modify_Reference_Counts_And_Manage_Temporaries (
             Array_Conformability_Info_Type *Array_Set,
             doubleArray *Temporary,
             doubleSerialArray* Lhs_Serial_PCE_Array,
             doubleSerialArray* Rhs_Serial_PCE_Array,
             doubleSerialArray *Data );

     // reduction operator for sum min max etc.
        static double Abstract_Operator 
	   ( Array_Conformability_Info_Type *Array_Set, 
	     const doubleArray & X, doubleSerialArray* X_Serial_PCE_Array, 
	     double x, int Operation_Type );

     // usual binary operators: operator+ operator- operator* operator/ 
     // operator% for both array to array and array to scalar operations
        static doubleArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
		  const doubleArray & X_ParallelArray, 
		  doubleSerialArray* X_Serial_PCE_Array, 
		  doubleSerialArray & X_SerialArray );

        static doubleArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
     //           doubleArray & Lhs, doubleArray & Rhs, 
                  const doubleArray & Lhs, const doubleArray & Rhs, 
                  doubleSerialArray* Lhs_Serial_PCE_Array, doubleSerialArray* Rhs_Serial_PCE_Array, 
                  doubleSerialArray & X_SerialArray );

     // These operators are special because they don't generate temporaries
     // Used in array to scalar versions of operator+= operator-= operato*= operator/= operator%=
        static doubleArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , const doubleArray & This_ParallelArray, doubleSerialArray* This_Serial_PCE_Array, const doubleSerialArray & This_SerialArray );
     // Used in array to array versions of operator+= operator-= operato*= operator/= operator%=
        static doubleArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleArray & This_ParallelArray, const doubleArray & Lhs, 
                  doubleSerialArray* This_Serial_PCE_Array, doubleSerialArray* Lhs_Serial_PCE_Array, 
                  const doubleSerialArray & X_SerialArray );

#if !defined(INTARRAY)
     // relational operators operator&& operator|| operator<= operator< etc.
        static intArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleArray & X_ParallelArray, 
                  doubleSerialArray* X_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

        static intArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleArray & Lhs, const doubleArray & Rhs, 
                  doubleSerialArray* Lhs_Serial_PCE_Array, 
		  doubleSerialArray* Rhs_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

     // replace operators for array to array and array to scalar
        static doubleArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleArray & This_ParallelArray, const intArray & Lhs, 
                  doubleSerialArray* This_Serial_PCE_Array, intSerialArray* Lhs_Serial_PCE_Array, 
                  const doubleSerialArray & X_SerialArray );
        static doubleArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleArray & This_ParallelArray, const intArray & Lhs, const doubleArray & Rhs, 
                  doubleSerialArray* This_Serial_PCE_Array, 
                  intSerialArray* Lhs_Serial_PCE_Array,
                  doubleSerialArray* Rhs_Serial_PCE_Array, 
                  const doubleSerialArray & X_SerialArray );
#endif

     // Abstract operator support for conversion operators
        static intArray & Abstract_int_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const doubleArray & X_ParallelArray,
                  doubleSerialArray* X_Serial_PCE_Array,
                  intSerialArray & X_SerialArray );

        static floatArray & Abstract_float_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const doubleArray & X_ParallelArray,
                  doubleSerialArray* X_Serial_PCE_Array,
                  floatSerialArray & X_SerialArray );

        static doubleArray & Abstract_double_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set ,
                  const doubleArray & X_ParallelArray,
                  doubleSerialArray* X_Serial_PCE_Array,
                  doubleSerialArray & X_SerialArray );


#else
#error Must DEFINE either APP || SERIAL_APP || PPP
#endif

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif
 
        static doubleArray*  Build_Temporary_By_Example ( const intArray    & Lhs );
        static doubleArray*  Build_Temporary_By_Example ( const floatArray  & Lhs );
        static doubleArray*  Build_Temporary_By_Example ( const doubleArray & Lhs );
        static doubleArray & Build_New_Array_Or_Reuse_Operand ( const doubleArray & Lhs , const doubleArray & Rhs , 
                                                              Memory_Source_Type & Result_Array_Memory );
        static doubleArray & Build_New_Array_Or_Reuse_Operand ( const doubleArray & X , 
                                                              Memory_Source_Type & Result_Array_Memory );

     // GNU g++ compiler likes to see these marked as inlined if they are in the header file as well.
     // (11/6/2000) Moved Delete_If_Temporary to array.C
     // friend inline void Delete_If_Temporary     ( const doubleArray & X );
        friend void Delete_If_Temporary     ( const doubleArray & X );

     // (11/24/2000) Moved Delete_Lhs_If_Temporary to array.C
     // friend inline void Delete_Lhs_If_Temporary ( const doubleArray & Lhs );
        friend void Delete_Lhs_If_Temporary ( const doubleArray & Lhs );

     // Raw memory allocation and dealocation located in these functions
        void Delete_Array_Data ();

     // void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) const;
        void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) ;

#if defined(PPP)
     // void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) const;
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) ;

     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( doubleArray* This_Pointer , doubleArray* Lhs , doubleArray* Rhs );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const doubleArray & X_ParallelArray,    
	     doubleSerialArray* & X_SerialArray );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const doubleArray & Lhs_ParallelArray,  
	     doubleSerialArray* & Lhs_SerialArray,
             const doubleArray & Rhs_ParallelArray,  
	     doubleSerialArray* & Rhs_SerialArray );
     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( const doubleArray & This_ParallelArray, 
     //      doubleSerialArray* & This_SerialArray,
     //      const doubleArray & Lhs_ParallelArray,  
     //      doubleSerialArray* & Lhs_SerialArray,
     //      const doubleArray & Rhs_ParallelArray,  
     //      doubleSerialArray* & Rhs_SerialArray );
#if !defined(INTARRAY)
     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( doubleArray* This_Pointer , intArray* Lhs , doubleArray* Rhs );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const doubleArray  & This_ParallelArray, 
	     doubleSerialArray*  & This_SerialArray,
             const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray );
#if 0
     // I think we can skip this function (I hope)
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const doubleArray  & Rhs_ParallelArray,  
	     doubleSerialArray*  & Rhs_SerialArray );
#endif
#endif
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const doubleArray  & This_ParallelArray, 
	     doubleSerialArray*  & This_SerialArray,
             const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const doubleArray  & Rhs_ParallelArray,  
	     doubleSerialArray*  & Rhs_SerialArray );

        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const doubleArray & Lhs_ParallelArray,  
	     doubleSerialArray* & Lhs_SerialArray,
             const doubleArray & Rhs_ParallelArray,  
	     doubleSerialArray* & Rhs_SerialArray );
        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const doubleArray & Lhs_ParallelArray,    
	     doubleSerialArray* & Lhs_SerialArray );

#if !defined(INTARRAY)
        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const doubleArray & Lhs_ParallelArray,  
	     doubleSerialArray* & Lhs_SerialArray,
             const intArray & Rhs_ParallelArray,  
	     intSerialArray* & Rhs_SerialArray );
#endif
#endif

#if defined(USE_EXPRESSION_TEMPLATES)
     public:

        doubleArray & operator= ( double x );
        doubleArray & operator= ( const doubleArray & Rhs );

     // Access to pointer specific to expression template indexing
        inline double* APP_RESTRICT_MACRO getExpressionTemplateDataPointer () const { return Array_Descriptor.getExpressionTemplateDataPointer(); }

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.h HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
        doubleArray(const Indexable<T1, A> &Rhs);

        template <class T1, class A>
        doubleArray & operator=(const Indexable<T1, A> & Rhs);

     // We comment this out for now while we get the
     // operator=(const Indexable<T1, A> &rhs) working (above)
        template <class T1, class A>
        doubleArray & operator+=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        doubleArray & operator-=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        doubleArray & operator*=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        doubleArray & operator/=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        doubleArray & operator%=(const Indexable<T1, A> &Rhs);
#else
        doubleArray(const InAssign<double> &a);

        doubleArray & operator=(const InAssign<double> &Rhs);
        doubleArray & operator+=(const InAssign<double> &Rhs);
        doubleArray & operator-=(const InAssign<double> &Rhs);
        doubleArray & operator*=(const InAssign<double> &Rhs);
        doubleArray & operator/=(const InAssign<double> &Rhs);
        doubleArray & operator%=(const InAssign<double> &Rhs);

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif
   };


#undef DOUBLEARRAY

#define FLOATARRAY
// Default base and bound object for use in floatArray constructor
extern Range APP_Unit_Range;



// Forward class declarations
class doubleArray_Function_Steal_Data;
class doubleArray_Function_0;
class doubleArray_Function_1;
class doubleArray_Function_2;
class doubleArray_Function_3;
class doubleArray_Function_4;
class doubleArray_Function_5;
class doubleArray_Function_6;
class doubleArray_Function_7;
class doubleArray_Function_8;
class doubleArray_Function_9;
class doubleArray_Function_10;
class doubleArray_Function_11;
class doubleArray_Function_12;
class doubleArray_Function_13;
class doubleArray_Function_14;
class doubleArray_Function_15;
class doubleArray_Function_16;

class doubleArray_Aggregate_Operator;

// Forward class declarations
class floatArray_Function_Steal_Data;
class floatArray_Function_0;
class floatArray_Function_1;
class floatArray_Function_2;
class floatArray_Function_3;
class floatArray_Function_4;
class floatArray_Function_5;
class floatArray_Function_6;
class floatArray_Function_7;
class floatArray_Function_8;
class floatArray_Function_9;
class floatArray_Function_10;
class floatArray_Function_11;
class floatArray_Function_12;
class floatArray_Function_13;
class floatArray_Function_14;
class floatArray_Function_15;
class floatArray_Function_16;

class floatArray_Aggregate_Operator;

// Forward class declarations
class intArray_Function_Steal_Data;
class intArray_Function_0;
class intArray_Function_1;
class intArray_Function_2;
class intArray_Function_3;
class intArray_Function_4;
class intArray_Function_5;
class intArray_Function_6;
class intArray_Function_7;
class intArray_Function_8;
class intArray_Function_9;
class intArray_Function_10;
class intArray_Function_11;
class intArray_Function_12;
class intArray_Function_13;
class intArray_Function_14;
class intArray_Function_15;
class intArray_Function_16;

class intArray_Aggregate_Operator;


#if defined(APP) || defined(PPP)
class Array_Conformability_Info_Type;
class Partitioning_Type;
#endif


#if defined(PPP)
// PVM function prototype
// Bugfix (6/20/95) To remove the dependence on pvm in the P++ implementation
// we pass the Operation_Type (an int) instead of a PVM function pointer.
// So this typedef should not be required anymore
// extern "C" { typedef void (Reduction_Function_Type) ( int*, void*, void*, int*, int*); };
#endif

// we use the case where the DOUBLEARRAY is defined so that
// this will be placed first in the file once it is expanded by m4!
// if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)
// if defined(DOUBLEARRAY) && (defined(APP) || defined(PPP))
#if defined(DOUBLEARRAY)
// error "USE_EXPRESSION_TEMPLATES is defined!"
class BaseArray
   {
  // Base class for all array objects.  This base class may eventualy be filled with
  // most of the member function that I presently in the A++/P++ objects themselves.
  // This would simplify the definition of A++/P++ and in the templated version it might
  // simplify the instantiation of the templated member functions.

       // this might be a useful mechanism to avoid any future requirements of runtime typing
       // but we avoid using it unless there is no other way to handle this.
       // int TypeInfo;  // A mechanism to record the derived type -- a poor design mechanism

     public:
#if 1
       // virtual ~BaseArray();
       // virtual ~BaseArray() = 0;
          BaseArray();
#endif

// We comment out these pure virtual functions to test a bug
#if 0
       // This is a list of most of the functions in the array objects
       // I think that most of these could placed into the base class
       // however for the moment I have only made them pure virtual functions.
       // some of these (like the getDomainPointer - could not be implemented within the
       // base class since they make calls to the typed (double, float, int) ArrayDescriptorType
       // object which is only contained in the derived class).
          virtual int getReferenceCount() const = 0;
          virtual void incrementReferenceCount() = 0;
          virtual void decrementReferenceCount() = 0;
          virtual void resetRawDataReferenceCount() const = 0;
          virtual int getRawDataReferenceCount() const = 0;
          virtual void incrementRawDataReferenceCount() const = 0;
          virtual void decrementRawDataReferenceCount() const = 0;
          virtual void ReferenceCountedDelete () = 0;
       // virtual static void New_Function_Loop () = 0;
       // virtual static void freeMemoryInUse() = 0;
       // virtual void *operator new    (size_t) = 0;
       // virtual void operator  delete (void*) = 0;
          virtual void Initialize_Array ( 
               int Number_Of_Valid_Dimensions ,
               const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
               bool Force_Memory_Allocation ) = 0;
       // virtual void Initialization_From_Range_List ( 
       //      int Number_Of_Valid_Dimensions,
       //      const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Range_List ) = 0;
          virtual int getDataOffset ( int Axis ) const = 0;
          virtual Array_Domain_Type* getDomainPointer () = 0;
       // virtual Array_Descriptor_Type *& getArrayDomainReference () const = 0;
          virtual bool Is_Built_By_Defered_Evaluation() const = 0;
          virtual void setTemporary( bool x ) const = 0;
          virtual bool isTemporary() const = 0;
          virtual bool isNullArray() const = 0;
          virtual bool isView()      const = 0;
          virtual bool isContiguousData() const = 0;

       // I have commented this out to test a bugfix!
       // virtual void Test_Consistency ( const char *Label = "" ) const = 0;

          virtual bool usesIndirectAddressing() const = 0; 
#if !defined(PPP)
          virtual void Fixup_User_Base
              (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List,
               const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List) = 0;
#endif
          virtual int getBase() const = 0;
          virtual int getRawBase() const = 0;
          virtual int getBase( int Axis ) const = 0;
          virtual int getStride( int Axis ) const = 0;
          virtual int getBound( int Axis ) const = 0;
          virtual int getLength( int Axis ) const = 0;
          virtual int getRawDataSize( int Axis ) const = 0;
          virtual int getRawBase( int Axis ) const = 0;
          virtual int getRawStride( int Axis ) const = 0;
          virtual int getRawBound( int Axis ) const = 0;
          virtual int getDataBase( int Axis ) const = 0;
          virtual int numberOfDimensions () const = 0;
          virtual Range dimension( int Axis ) const = 0;
          virtual Range getFullRange( int Axis ) const = 0;
#if defined(APP) || defined(PPP)
       // P++ specific functions report on the local partition
          virtual int getLocalBase( int Axis ) const = 0;
          virtual int getLocalStride( int Axis ) const = 0;
          virtual int getLocalBound( int Axis ) const = 0;
          virtual int getLocalLength( int Axis ) const = 0;
          virtual int getLocalRawBase( int Axis ) const = 0;
          virtual int getLocalRawStride( int Axis ) const = 0;
          virtual int getLocalRawBound( int Axis ) const = 0;
          virtual Range localDimension( int Axis ) const = 0;
#endif
          virtual Range getLocalFullRange( int Axis ) const = 0;
          virtual int elementCount() const = 0;
          virtual int cols() const = 0;
          virtual int rows() const = 0;

       // note that redim reshape resize may be functions that might have
       // to be defined only within the derived class.  Some support might
       // be possible from the base class if so this this would be worthwhile.

          virtual void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const = 0;
          virtual void Delete_Array_Data () = 0;
          virtual void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) = 0;
#endif

#if 1   
       // Bugs in the Sun C++ compiler have routinely caused the base class member functions
       // to be called instead of the derived class.  One solution might be to have the
       // Array_ID variable stored in the base class so that it can always be accessed from 
       // the base class.  Since the other function is only defined for APP and PPP
       // I think the definition of some virtual function here is required.
       // virtual int Array_ID () const = 0;
          virtual int Dummy_Function_Array_ID () const
             { 
               printf ("ERROR: Virtual Base Class called: BaseArray::Array_ID() \n");
               APP_ABORT();
               return -1000;
             }
#endif

#if defined(APP) || defined(PPP)
#if defined(USE_PADRE)
       // virtual void partition( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition ) = 0;
          virtual void partition( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition )
             {
            // Avoid compiler warning about unused variable
               if (&Partition);

               printf ("ERROR: Virtual Base Class called: BaseArray::partition(PADRE_Distribution<*,*,*>) \n");
               APP_ABORT();
             }
#endif
// We comment out these pure virtual functions to test a bug
#if 0
          virtual Partitioning_Type getPartition () const = 0;
          virtual void displayPartitioning (const char *Label = "") = 0;
          virtual int getInternalGhostCellWidth ( int Axis ) const = 0;
          virtual int getExternalGhostCellWidth ( int Axis ) const = 0;
          virtual void updateGhostBoundaries() const = 0;
#endif
#endif

#if 0
       // This function exists in both bases classes of an array object and 
       // creates an ambiguous reference to the buildListOfArrayOperands member 
       // function of a doubleArray or floatArray or intArray.
          void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
             {
               printf ("Inside of BaseArray::buildListOfArrayOperands(list<BaseArray*> & arrayList) \n");
               abort();
            // APP_ABORT();
            // There is a const problem here!
            // arrayList.push_back (this);
             }
#endif
   };
#endif

#if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)
/*
///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    InArray
//
// DESCRIPTION
//    A version of Indexable that container classes, like Arrays, can
//    inherit from in order to become indexable as well.
//
///////////////////////////////////////////////////////////////////////////
*/

template<class T>
class InArray : public Indexable<T, InArray<T> > 
   {
     public:
        typedef T RetT;

     protected:

        InArray ()  
           { 
             printf ("ERROR: inside of InArray::InArray() \n"); 
             APP_ABORT(); 
           }

     // We pass FALSE to the Array_Descriptor constructor because we want 
     // to have AvoidBuildingIndirectAddressingView = FALSE internally
        InArray(const Array_Domain_Type & Domain)
           : Array_Descriptor(Domain,FALSE) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}

     // I don't think we need this now!
        InArray ( const doubleArray* InputArrayPointer ) {}
        InArray ( const floatArray* InputArrayPointer ) {}
        InArray ( const intArray* InputArrayPointer ) {}

     public:

        inline bool usesIndirectAddressing() const APP_RESTRICT_MACRO;
        inline int internalNumberOfDimensions() const APP_RESTRICT_MACRO;
        inline bool containsArrayID ( int lhs_id ) const APP_RESTRICT_MACRO;

        void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<doubleArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<floatArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<intArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<intArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<BaseArray*> & arrayList) \n");
           }

        void setupIndirectAddressingSupport() const APP_RESTRICT_MACRO
           { Array_Descriptor.setupIndirectAddressingSupport(); }

        T expand(int i) const APP_RESTRICT_MACRO;
        T indirect_expand_1D(int i) const APP_RESTRICT_MACRO;
#if (MAX_ARRAY_DIMENSION >= 2)
        T expand(int i, int j) const APP_RESTRICT_MACRO;
        T indirect_expand_2D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        T expand(int i, int j, int k) const APP_RESTRICT_MACRO;
        T indirect_expand_3D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        T expand(int i, int j, int k, int l) const APP_RESTRICT_MACRO;
        T indirect_expand_4D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        T expand(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO;
        T indirect_expand_5D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        T expand(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO;
        T indirect_expand_6D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        T expand(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO;
        T indirect_expand_7D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        T expand(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO;
        T indirect_expand_8D(int i) const APP_RESTRICT_MACRO;
#endif

        const Array_Domain_Type &shape() const APP_RESTRICT_MACRO;
        const Array_Domain_Type *shapePtr() const APP_RESTRICT_MACRO;

public:
     // To avoid access to relavanet data through a pointer which makes the expression 
     // template implementation less efficient we put all the pointers and the traditiona
     // descriptor data into the Array_Descriptor which has been templated to handle
     // the different types of data (pointers of different types).
        TArray_Descriptor_Type Array_Descriptor;
   };

// endif for "if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)" above
#endif


// We use this as a mechanism to define the base class differently when we use expression templates
#if defined(USE_EXPRESSION_TEMPLATES)
#define EXPRESSION_TEMPLATES_DERIVED_float_CLASS_SUPPORT : public BaseArray , public InArray<float>
#else
// We modify A++/P++ to have a common base class for all array objects - this permits
// many additional design features such as a mechanism for forming references to array 
// object independent of the type of the array.
// #define EXPRESSION_TEMPLATES_DERIVED_float_CLASS_SUPPORT
#undef EXPRESSION_TEMPLATES_DERIVED_float_CLASS_SUPPORT
#define EXPRESSION_TEMPLATES_DERIVED_float_CLASS_SUPPORT : public BaseArray
#endif


// #################################################################
// ######  Definition of the principal object within A++/P++  ######
// #################################################################

class floatArray EXPRESSION_TEMPLATES_DERIVED_float_CLASS_SUPPORT
   {
  // This is the pricipal object within A++/P++.
  // This is the declaration of the array object.
  // It includes a declaration of a descriptor (as member data, which in turn 
  // includes a domain objects (as member data)).  Most of this class is a description
  // of the public interface and is documented in the A++/P++ manual. But (of course) this
  // also includes the private interface (not documented in the manual). Many of the
  // elements of the private interface are declared public to simplify teh design of the
  // object and facilitate its use in contexts that it was not explicitly designed for.
  // Thus we don't make it hard for the uer to get at the raw internal data.  If we made it 
  // difficult we might argue that we preserve incapsulation - but it is not a heard work 
  // around for a user to get at this data and so in the end the internal data is not 
  // really well encapsulated anyway.

  // Note that the BaseArray has a "this" pointer and so there 
  // is an offset for all the data members of this (array) class.

     friend class Where_Statement_Support;
     friend class Array_Statement_Type;
     friend class Expression_Tree_Node_Type;
 
     friend class Operand_Storage;
     friend class floatArray_Operand_Storage;

#if defined(USE_EXPRESSION_TEMPLATES)
     friend class InArray<float>;
#endif

     friend class Internal_Partitioning_Type;
     friend class Partitioning_Type;

#if defined(SERIAL_APP) && !defined(PPP)
     friend floatArray & transpose ( const floatArray & X );
#endif

#ifndef DOUBLEARRAY
     friend class doubleArray;
#if defined(SERIAL_APP)
     friend class doubleSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class doubleArray;
#endif
#if defined(PPP)
     friend class doubleSerialArray;
#endif
#endif

#ifndef FLOATARRAY
     friend class floatArray;
#if defined(SERIAL_APP)
     friend class floatSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class floatArray;
#endif
#if defined(PPP)
     friend class floatSerialArray;
#endif
#endif

#ifndef INTARRAY
     friend class intArray;
#if defined(SERIAL_APP)
     friend class intSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class intArray;
#endif
#if defined(PPP)
     friend class intSerialArray;
#endif
#endif

#if 1
// GNU g++ requires these friend declarations
// Forward class declarations
friend class doubleArray_Function_Steal_Data;
friend class doubleArray_Function_0;
friend class doubleArray_Function_1;
friend class doubleArray_Function_2;
friend class doubleArray_Function_3;
friend class doubleArray_Function_4;
friend class doubleArray_Function_5;
friend class doubleArray_Function_6;
friend class doubleArray_Function_7;
friend class doubleArray_Function_8;
friend class doubleArray_Function_9;
friend class doubleArray_Function_10;
friend class doubleArray_Function_11;
friend class doubleArray_Function_12;
friend class doubleArray_Function_13;
friend class doubleArray_Function_14;
friend class doubleArray_Function_15;
friend class doubleArray_Function_16;

friend class doubleArray_Aggregate_Operator;

// Forward class declarations
friend class floatArray_Function_Steal_Data;
friend class floatArray_Function_0;
friend class floatArray_Function_1;
friend class floatArray_Function_2;
friend class floatArray_Function_3;
friend class floatArray_Function_4;
friend class floatArray_Function_5;
friend class floatArray_Function_6;
friend class floatArray_Function_7;
friend class floatArray_Function_8;
friend class floatArray_Function_9;
friend class floatArray_Function_10;
friend class floatArray_Function_11;
friend class floatArray_Function_12;
friend class floatArray_Function_13;
friend class floatArray_Function_14;
friend class floatArray_Function_15;
friend class floatArray_Function_16;

friend class floatArray_Aggregate_Operator;

// Forward class declarations
friend class intArray_Function_Steal_Data;
friend class intArray_Function_0;
friend class intArray_Function_1;
friend class intArray_Function_2;
friend class intArray_Function_3;
friend class intArray_Function_4;
friend class intArray_Function_5;
friend class intArray_Function_6;
friend class intArray_Function_7;
friend class intArray_Function_8;
friend class intArray_Function_9;
friend class intArray_Function_10;
friend class intArray_Function_11;
friend class intArray_Function_12;
friend class intArray_Function_13;
friend class intArray_Function_14;
friend class intArray_Function_15;
friend class intArray_Function_16;

friend class intArray_Aggregate_Operator;

#endif

     private:
     // Constants for operator identification!
        static const int Plus;
        static const int Minus;
        static const int Times;
        static const int Divided_By;
        static const int Modulo;
        static const int cos_Function;
        static const int sin_Function;
        static const int tan_Function;
        static const int acos_Function;
        static const int asin_Function;
        static const int atan_Function;
        static const int cosh_Function;
        static const int sinh_Function;
        static const int tanh_Function;
        static const int acosh_Function;
        static const int asinh_Function;
        static const int atanh_Function;
        static const int log_Function;
        static const int log10_Function;
        static const int exp_Function;
        static const int sqrt_Function;
        static const int fabs_Function;
        static const int abs_Function;
        static const int ceil_Function;
        static const int floor_Function;
        static const int atan2_Function;
        static const int Unary_Minus;
        static const int fmod_Function;
        static const int mod_Function;
        static const int pow_Function;
        static const int sign_Function;
        static const int sum_Function;
        static const int min_Function;
        static const int max_Function;
        static const int Not;
        static const int LT;
        static const int GT;
        static const int LTEQ;
        static const int GTEQ;
        static const int EQ;
        static const int NOT_EQ;
        static const int AND;
        static const int OR;
        static const int Scalar_Plus;
        static const int Scalar_Minus;
        static const int Scalar_Times;
        static const int Scalar_Divided_By;
        static const int Scalar_Modulo;
        static const int Scalar_cos_Function;
        static const int Scalar_sin_Function;
        static const int Scalar_tan_Function;
        static const int Scalar_acos_Function;
        static const int Scalar_asin_Function;
        static const int Scalar_atan_Function;
        static const int Scalar_cosh_Function;
        static const int Scalar_sinh_Function;
        static const int Scalar_tanh_Function;
        static const int Scalar_acosh_Function;
        static const int Scalar_asinh_Function;
        static const int Scalar_atanh_Function;
        static const int Scalar_log_Function;
        static const int Scalar_log10_Function;
        static const int Scalar_exp_Function;
        static const int Scalar_sqrt_Function;
        static const int Scalar_fabs_Function;
        static const int Scalar_abs_Function;
        static const int Scalar_ceil_Function;
        static const int Scalar_floor_Function;
        static const int Scalar_atan2_Function;
        static const int Scalar_Unary_Minus;
        static const int Scalar_fmod_Function;
        static const int Scalar_mod_Function;
        static const int Scalar_pow_Function;
        static const int Scalar_sign_Function;
        static const int Scalar_sum_Function;
        static const int Scalar_min_Function;
        static const int Scalar_max_Function;
        static const int Scalar_Not;
        static const int Scalar_LT;
        static const int Scalar_GT;
        static const int Scalar_LTEQ;
        static const int Scalar_GTEQ;
        static const int Scalar_EQ;
        static const int Scalar_NOT_EQ;
        static const int Scalar_AND;
        static const int Scalar_OR;

     // These are considered as terminating a statement!
        static const int Assignment;
        static const int replace_Function;
        static const int Plus_Equals;
        static const int Minus_Equals;
        static const int Times_Equals;
        static const int Divided_By_Equals;
        static const int Modulo_Equals;
        static const int Scalar_Assignment;
        static const int Scalar_replace_Function;
        static const int Scalar_Plus_Equals;
        static const int Scalar_Minus_Equals;
        static const int Scalar_Times_Equals;
        static const int Scalar_Divided_By_Equals;
        static const int Scalar_Modulo_Equals;
        static const int indexMap_Function;
        static const int view_Function;
        static const int display_Function;

     // Bitwise operators
        static const int BitwiseComplement;
        static const int BitwiseAND;
        static const int BitwiseOR;
        static const int BitwiseXOR;
        static const int BitwiseLShift;
        static const int BitwiseRShift;
        static const int Scalar_BitwiseAND;
        static const int Scalar_BitwiseOR;
        static const int Scalar_BitwiseXOR;
        static const int Scalar_BitwiseLShift;
        static const int Scalar_BitwiseRShift;
        static const int BitwiseAND_Equals;
        static const int BitwiseOR_Equals;
        static const int BitwiseXOR_Equals;
        static const int Scalar_BitwiseAND_Equals;
        static const int Scalar_BitwiseOR_Equals;
        static const int Scalar_BitwiseXOR_Equals;

     // Conversion operators
        static const int convertTo_intArrayFunction;
        static const int convertTo_floatArrayFunction;
        static const int convertTo_doubleArrayFunction;

     public:
        static bool PREINITIALIZE_OBJECT_IN_CONSTRUCTOR;
        static float      PREINITIALIZE_VALUE;
        static bool USE_DESCRIPTOR_CACHING;

     public:
     // Every array object contains an array descriptor (a special case is the use of
     // expression templates where the array descriptor is stored as a member in a base
     // class of the array object).
#if !defined(USE_EXPRESSION_TEMPLATES)
     // If Expression templates are in use then the array_descriptor is 
     // stored in the Indexable base class!
     // Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> Array_Descriptor;
        floatArray_Descriptor_Type Array_Descriptor;
#endif

     // #################################################################
     // Access functions for array data (descriptors, domains, and data)
     // #################################################################

     // Access functions to private data to permit use with other languages.
     // Function is inlined for use with the A++ preprocessor
        inline float* APP_RESTRICT_MACRO getDataPointer () const;
#if defined(PPP)
        floatSerialArray*  getSerialArrayPointer () const;

     // Used for parallel support of "where statements"
        floatSerialArray** getSerialArrayPointerLoc () const;
#endif

     // Access function for Descriptor
     // Array_Descriptor_Type* getDescriptorPointer () const;
     // Array_Descriptor_Type* getDomainPointer () const;
     // Array_Domain_Type* getDomainPointer () const;

#if 0
     // This function does not appear to be used anywhere!
     // int getDataOffset ( int Axis ) const;
        inline int getDataOffset ( int Axis ) const
             { 
#if defined(PPP)
            // P++ calls the A++ getDataOffset function
               //return getSerialArrayDescriptor()->getDataOffset(Axis);
               return getSerialArrayDescriptor().Array_Domain.Base[Axis];
#else
               return Array_Descriptor.Array_Domain.Base[Axis];
#endif
             };
#endif

     // Access function for Descriptor (if we make this return a const & then we can only call const member functions using it)
        inline const floatArray_Descriptor_Type & getDescriptor () const
             { return Array_Descriptor; };

     // Access function for Descriptor
        inline const floatArray_Descriptor_Type* getDescriptorPointer () const
             { return &Array_Descriptor; };

        inline const Array_Domain_Type & getDomain () const
           { return Array_Descriptor.Array_Domain; };

     // inline const Array_Domain_Type* getDomainPointer () const
        inline const Array_Domain_Type* getDomainPointer () const
           { return &Array_Descriptor.Array_Domain; };

#if defined(PPP)
     // Access functions for serial data
        floatSerialArray_Descriptor_Type &
	   getSerialArrayDescriptor() const
	      { return Array_Descriptor.SerialArray->Array_Descriptor; };

        SerialArray_Domain_Type& getSerialDomain() const 
	   { return Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain; };
#endif

#if !defined(PPP)
#if defined(USE_EXPRESSION_TEMPLATES)
        void setupIndirectAddressingSupport() const
           { Array_Descriptor.setupIndirectAddressingSupport(); }

#if 0
        virtual floatArray* getfloatArrayPointer() const
           {
          // printf ("Calling doubleArray* getfloatArrayPointer() \n");
             return (floatArray*) this;
           }
#endif

     // Scalar Indexing functions made for expression templates.
        float  scalar_indexing_operator(int i) const 
           { 
          // We needed a scalar indexing operator which could be used within
          // the objects using indirect addressing or not indirect addressing.
          // This function is inlined into the operator()(int) below!
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.offset(i)]; 
           }
        float  operator()(int i) const 
           { 
             APP_ASSERT (Array_Descriptor.usesIndirectAddressing() == FALSE);
          // printf ("CONST Array_Descriptor.offset(%d) = %d \n",i,Array_Descriptor.offset(i));
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return scalar_indexing_operator(i);
           }
        float &operator()(int i)       
           { 
             APP_ASSERT (Array_Descriptor.usesIndirectAddressing() == FALSE);
          // printf ("NON-CONST Array_Descriptor.offset(%d) = %d \n",i,Array_Descriptor.offset(i));
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.offset(i)]; 
           }
        int computeSubscript ( int axis, int i ) const
           { return Array_Descriptor.computeSubscript(axis,i); }

        int indirectOffset_1D ( int i ) const
           { return Array_Descriptor.indirectOffset_1D(i); }

#if (MAX_ARRAY_DIMENSION >= 2)
        float  operator()(int i, int j) const 
           { return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.offset(i, j)]; }
        float &operator()(int i, int j)       
           { return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.offset(i, j)]; }
        int indirectOffset_2D ( int i ) const
           { return Array_Descriptor.indirectOffset_2D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        float  operator()(int i, int j, int k) const 
           { return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.offset(i, j, k)]; }
        float &operator()(int i, int j, int k)       
           { return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.offset(i, j, k)]; }
        int indirectOffset_3D ( int i ) const
           { return Array_Descriptor.indirectOffset_3D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        float  operator()(int i, int j, int k, int l) const 
           { return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.offset(i, j, k, l)]; }
        float &operator()(int i, int j, int k, int l)       
           { return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.offset(i, j, k, l)]; }
        int indirectOffset_4D ( int i ) const
           { return Array_Descriptor.indirectOffset_4D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        float  operator()(int i, int j, int k, int l, int m) const 
           { return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.offset(i, j, k, l, m)]; }
        float &operator()(int i, int j, int k, int l, int m)
           { return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.offset(i, j, k, l, m)]; }
        int indirectOffset_5D ( int i ) const
           { return Array_Descriptor.indirectOffset_5D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        float  operator()(int i, int j, int k, int l, int m, int n) const 
           { 
          // printf ("CONST Array_Descriptor.offset(%d,%d,%d,%d,%d,%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset(i, j, k, l, m, n));
             return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.offset(i, j, k, l, m, n)]; 
           }
        float &operator()(int i, int j, int k, int l, int m, int n)
           { 
          // printf ("NON-CONST Array_Descriptor.offset(%d,%d,%d,%d,%d,%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset(i, j, k, l, m, n));
             return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.offset(i, j, k, l, m, n)]; 
           }
        int indirectOffset_6D ( int i ) const
           { return Array_Descriptor.indirectOffset_6D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        float  operator()(int i, int j, int k, int l, int m, int n, int o) const 
           { return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.offset(i, j, k, l, m, n, o)]; }
        float &operator()(int i, int j, int k, int l, int m, int n, int o)
           { return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.offset(i, j, k, l, m, n, o)]; }
        int indirectOffset_7D ( int i ) const
           { return Array_Descriptor.indirectOffset_7D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        float  operator()(int i, int j, int k, int l, int m, int n, int o, int p) const 
           { return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.offset(i, j, k, l, m, n, o, p)]; }
        float &operator()(int i, int j, int k, int l, int m, int n, int o, int p)       
           { return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.offset(i, j, k, l, m, n, o, p)]; }
        int indirectOffset_8D ( int i ) const
           { return Array_Descriptor.indirectOffset_8D(i); }
#endif

     // Already declared!
     // bool usesIndirectAddressing()const  { return Array_Descriptor.usesIndirectAddressing(); }
     // inline bool usesIndirectAddressing() const { return FALSE; }
        inline bool containsArrayID ( int lhs_id ) const { return FALSE; }
        int internalNumberOfDimensions() const
           { return Array_Descriptor.internalNumberOfDimensions(); }

// end of defined(USE_EXPRESSION_TEMPLATES)
#endif

// end of !defined(PPP)
#endif

#if !defined(USE_DEFERED_EXPRESSION_EVALUATION)
     // This supports the defered evaluation (specifically it supports the means by which
     // the lifetimes of variables are extended by the defered evaluation)!
        Operand_Storage *Array_Storage;
#endif

     // Data required for the "new" and "delete" operators!
        static floatArray *Current_Link;
        floatArray *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

     public:
     // Control for display  (used in display member function)
        static const int DECIMAL_DISPLAY_FORMAT;
        static const int EXPONENTIAL_DISPLAY_FORMAT;
        static const int SMART_DISPLAY_FORMAT;
        static int DISPLAY_FORMAT;

     private:
     // Reference Counting for A++ array objects -- allows user to use
     // multiple pointers to the same A++ array object.
     // *************************************************
     // Reference counting data and functions
        int referenceCount;

     public:
       // Use of reference count access is made available for debugging
          inline int getReferenceCount() const
             { return referenceCount; }

       // Support functions for reference counting (inlined for performance)
          inline void incrementReferenceCount() const
             { ((floatArray*) this)->referenceCount++; }
          inline void decrementReferenceCount() const
             { ((floatArray*) this)->referenceCount--; }

          inline void resetRawDataReferenceCount() const 
             { 
            // When we steal the data and reuse it from a temporary we have to delete it
            // which decrements the reference count to -1 (typically) and then we copy the
            // data pointers.  But we have to reset the reference cout to ZERO as well.
            // This function allows that operation to be insulated from the details of how the
            // reference count is implemented.  It is used in the A++ operator= ( arrayobject )
#if defined(PPP)
               //APP_ASSERT (getSerialArrayDescriptor() != NULL);
	       // ... (8/7/97,kdb) don't use referenceCount for descriptor 
               //getSerialArrayDescriptor().referenceCount = 0;
            // Array_Descriptor.SerialArray->referenceCount = 0;
               Array_Descriptor.SerialArray->resetReferenceCount();
#else
            // Array_Descriptor_Type::Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // APP_Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID] = 0;
               Array_Descriptor.resetRawDataReferenceCount();
#endif
             }

          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return floatArray_Descriptor_Type::getReferenceCountBase(); 
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
            // return 0;
               return floatArray_Descriptor_Type::getRawDataReferenceCountBase(); 
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
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               return Array_Descriptor.SerialArray->getReferenceCount(); 
#else
               return Array_Descriptor.getRawDataReferenceCount();
#endif
             }

          inline void incrementRawDataReferenceCount() const 
             { 
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               Array_Descriptor.SerialArray->incrementReferenceCount(); 
#else
               Array_Descriptor.incrementRawDataReferenceCount();
#endif
             }

          inline void decrementRawDataReferenceCount() const 
             { 
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               Array_Descriptor.SerialArray->decrementReferenceCount(); 
#else
               Array_Descriptor.decrementRawDataReferenceCount();
#endif
             }

       // This function is useful for tracking down memory reference problems
          floatArray & displayReferenceCounts ( const char* label = "" ) const;

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

#if 0
          inline void ReferenceCountedDelete ()
             {
            // This is a dangerous sort of function since it deletes the class which this
            // function is a member of!.  As long as we don't do anything after calling
            // delete the function just returns and seem to work fine.
               printf ("ERROR: floatArray::ReferenceCountedDelete called! \n");
               exit(1);
   
            // I'm forced to INCLUDE this code because A++ must have a 
            // consistant interface with other classes in Overture++.
            // The problem that made this sort of statement was another bug
            // that was fixed in the reshape function.
               delete this;

            // if (--referenceCount < 0)
            //      delete this;
             }
#endif
     // *************************************************

     private:
     // Because inlined functions can't contain loops we seperate 
     // out this function which is used in the new operator
        static void New_Function_Loop ();

     public:

     // Used to free memory-in-use internally in floatArray objects!
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();
        static int numberOfInternalArrays();

#if defined(INLINE_FUNCTIONS)
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
     // DQ (1/9/2007): Commented out the defined "operator delete" since it was a problem for the GNU g++ 3.4.x compiler.
#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )
        void operator delete (void* Pointer, size_t Size);
#endif

     // Default Destructor and Constructor
        virtual ~floatArray ();
        floatArray ();

     // Constructors

     // Seperate constructors required to avoid ambiguous call errors in call resolution 
     // between the different types

     // ======================================================
        floatArray ( int i );
#if (MAX_ARRAY_DIMENSION >= 2)
        floatArray ( int i , int j );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        floatArray ( int i , int j , int k );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        floatArray ( int i , int j , int k , int l );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        floatArray ( int i , int j , int k , int l , int m );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        floatArray ( int i , int j , int k , int l , int m, int n );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        floatArray ( int i , int j , int k , int l , int m, int n, int o );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        floatArray ( int i , int j , int k , int l , int m, int n, int o, int p);
#endif

     // ======================================================

        floatArray ( ARGUMENT_LIST_MACRO_INTEGER, bool Force_Memory_Allocation  );

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if defined(APP) || defined(PPP)

     // ======================================================
        floatArray ( int i , const Partitioning_Type & Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
        floatArray ( int i , int j , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        floatArray ( int i , int j , int k , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        floatArray ( int i , int j , int k , int l , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        floatArray ( int i , int j , int k , int l , int m, const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        floatArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        floatArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        floatArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION >= 2)
        floatArray ( int i , int j , const Partitioning_Type & Partition,
	            bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        floatArray ( int i , int j , int k , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        floatArray ( int i , int j , int k , int l , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        floatArray ( int i , int j , int k , int l , int m, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        floatArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        floatArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        floatArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition , 
		    bool Force_Memory_Allocation );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// defined(APP) || defined(PPP)
#endif

     private:
     // Called inside of each of the Initialize_Array functions (below)
        void preInitializeArray ();

     // Called inside of all array object constructors
        void initializeArray ();

#if defined(APP) || defined(PPP)
     // Called inside of all array object constructors
        void initializeArray ( const Partitioning_Type & Partition );
        void initializeArray ( const Internal_Partitioning_Type & Partition );
#endif

     public:
     // We have to list all the possible variations to avoid a scalar
     // being promoted to a Range object (which has base == bound)
     // The solution is to provide all the permutations of the operator()
     // and promote scalars to a range (defaultBase:defaultBase+Scalar-1)
     // floatArray ( const Range & I_Base_Bound , 
     //             const Range & J_Base_Bound = APP_Unit_Range , 
     //             const Range & K_Base_Bound = APP_Unit_Range , 
     //             const Range & L_Base_Bound = APP_Unit_Range );
        floatArray ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // These are for internal use and allow initialization from an array of data
        floatArray ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

#if defined(PPP) || defined (APP)
     // Added to support resize function used with an existing partitioning object
        floatArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List,
                    const Internal_Partitioning_Type & partition );
#endif

        floatArray ( int Number_Of_Valid_Dimensions, const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // This constructor avoids an ambiguous call to the copy constructor 
     // (since a floatArray could be built from a Range and an int is the same as a bool)
#if (MAX_ARRAY_DIMENSION >= 2)
        floatArray ( const Range & I_Base_Bound , int j );
#endif

#if defined(PPP)
     // Construct a floatArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // Additionally for P++ the local partition must be provided.  Due to restrictions on the
     // type of partitioning supported (i.e. FORTRAN D Partitioning: BLOCK PARTI partitioning)
     // a specific partitoning might not conform to what P++ can presently handle.  The result
     // is an error.  So it is not a good idea to use this function in the parallel environment.
     // floatArray::floatArray ( float* Data_Pointer , int i , const Range & Range_I , 
     //                                           int j , const Range & Range_J ,
     //                                           int k , const Range & Range_K ,
     //                                           int l , const Range & Range_L );
     // floatArray::floatArray ( const float* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        floatArray ( const float* Data_Pointer , 
                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );

#if 0
     // We can't use this member function and remain dimension independent (so it is removed for now)
        floatArray ( float* Data_Pointer , const Range & Span_I , const Range & Range_I , 
                                                  const Range & Span_J , const Range & Range_J ,
                                                  const Range & Span_K , const Range & Range_K ,
                                                  const Range & Span_L , const Range & Range_L );
#endif
#else
     // Construct a floatArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // floatArray ( float* Data_Pointer , int i , int j = 1 , int k = 1 , int l = 1 );
        floatArray ( const float* Data_Pointer , ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );

     // floatArray ( float* Data_Pointer , const Range & Span_I , 
     //             const Range & Span_J = APP_Unit_Range , 
     //             const Range & Span_K = APP_Unit_Range , 
     //             const Range & Span_L = APP_Unit_Range );
        floatArray ( const float* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
#endif

     // Constructors to support deffered evaluation of array expressions! (these should be private, I think)
     // I'm not certain these are still used!
        floatArray ( int Operation_Code , floatArray *Lhs_Pointer );
        floatArray ( int Operation_Code , floatArray *Lhs_Pointer , floatArray *Rhs_Pointer );

     // Copy Constructors
        floatArray ( const floatArray & X , int Type_Of_Copy = DEEPCOPY );

#if defined(APP) || defined(SERIAL_APP)
     // Constructor for initialization from a string (We built the AppString type to avoid autopromotion problems)
     // floatArray ( const char* );
        floatArray ( const AppString & X );
#endif

     // Evaluation operator! Evaluates an expression and asigns local scope to its result
     // useful when handing expresions to functions that pass by reference.
     // There is no peformance penalty associated with this function since no memory
     // is copied! 
        friend floatArray evaluate ( const floatArray & X );

     // reference operator (builds a view) (an M++ function)
        floatArray & reference ( const floatArray & X );
        floatArray & breakReference ();

        bool Is_Built_By_Defered_Evaluation() const;

     // void display ( const char *Label = "" ) const;
     // floatArray & display ( const char *Label = "" );
        floatArray & view    ( const char *Label = "" ) const;
        floatArray & display ( const char *Label = "" ) const;
        floatArray & globalDisplay ( const char *Label = "" ) const;
#if defined(USE_EXPRESSION_TEMPLATES)
        void MDI_Display() const;
#endif

     // Provided to permit easy access from dbx debugger
#ifdef __clang__
     // Clang requires default arguments of friend functions to appear in the definitions.
        friend void APP_view    ( const floatArray & X , const char *Label );
        friend void APP_display ( const floatArray & X , const char *Label );
#else
     // DQ (7/23/2020): This might be dependent on EDG more than GNU version 10.
#if ((__GNUC__ == 10) && (__GNUC_MINOR__ >= 1) && (__GNUC_PATCHLEVEL__ >= 0))
     // GNU 10 requires default arguments of friend functions to appear in the definitions.
        friend void APP_view    ( const floatArray & X , const char *Label );
        friend void APP_display ( const floatArray & X , const char *Label );
#else
        friend void APP_view    ( const floatArray & X , const char *Label = "" );
        friend void APP_display ( const floatArray & X , const char *Label = "" );
#endif
#endif

     // This is a member function defined in the BaseArray class
        int Array_ID () const;

        inline void setTemporary( bool x ) const { Array_Descriptor.setTemporary(x); }
        inline bool isTemporary() const { return Array_Descriptor.isTemporary(); }
        inline bool isNullArray() const { return Array_Descriptor.isNullArray(); }
        inline bool isView()      const { return Array_Descriptor.isView(); }
        inline bool isContiguousData() const { return Array_Descriptor.isContiguousData(); }

     // Equivalent to operator= (float x)
        floatArray & fill ( float x );

     // Fills the array with different values given a base and stride
        floatArray & seqAdd ( float Base = (float) 0 , float Stride = (float) 1 );  // Start at zero and count by 1

     // This function tests for many sorts of errors that could crop up in
     // the internal usage of the floatArray objects -- it is a debugging tool!
        void Test_Consistency ( const char *Label = "" ) const;
        inline bool Binary_Conformable  ( const floatArray & X ) const;

     // Part of implementation for diagnostics -- permits more aggressive a 
     // destructive testing than Test_Consistency(). (private interface)
        void testArray();

     // Test for confomability regardless of the input!
     // Templates would simplify this since the template need not be "bound"
     // See Lippman for more details on templates (bound and unbound)!

     // Public interface
        bool isConformable ( const doubleArray & X ) const;
        bool isConformable ( const floatArray & X ) const;
        bool isConformable ( const intArray & X ) const;

     // For internal use (though it is marked public at the moment)
        void Test_Conformability ( const doubleArray & X ) const;
        void Test_Conformability ( const floatArray & X ) const;
        void Test_Conformability ( const intArray & X ) const;

#if defined(INTARRAY)
     // intArray ( const Index & X );
     //  ... (7/20/98,kdb) function below is commented out in src code
     //   so remove here to avoid linking problems ...
     // intArray ( const Internal_Index & X );
        intArray & operator=( const Internal_Index & X );
        intArray & indexMap ();
#endif

     // Range checking function for scalar indexing (this has been moved to thr descriptor class)
     // void Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const;

#if !defined(USE_EXPRESSION_TEMPLATES)
        bool usesIndirectAddressing()const  { return Array_Descriptor.usesIndirectAddressing(); }

     // Indexing operators (scalar)
     // To get constness correct we should move to using this construction
     // The indexing of a const array should return a const ref and 
     // the indexing of a nonconst array should return a nonconst ref.
     // inline const float & operator() ( int i ) const;
     // inline       float & operator() ( int i );
        inline float & operator() ( int i ) const;

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const float & operator() ( int i , int j ) const;
        inline       float & operator() ( int i , int j );
#else
        inline float & operator() ( int i , int j ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const float & operator() ( int i , int j , int k ) const;
        inline       float & operator() ( int i , int j , int k );
#else
        inline float & operator() ( int i , int j , int k ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const float & operator() ( int i , int j , int k , int l ) const;
        inline       float & operator() ( int i , int j , int k , int l );
#else
        inline float & operator() ( int i , int j , int k , int l ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const float & operator() ( int i , int j , int k , int l, int m ) const;
        inline       float & operator() ( int i , int j , int k , int l, int m );
#else
        inline float & operator() ( int i , int j , int k , int l, int m ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const float & operator() ( int i , int j , int k , int l, int m , int n ) const;
        inline       float & operator() ( int i , int j , int k , int l, int m , int n );
#else
        inline float & operator() ( int i , int j , int k , int l, int m , int n ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const float & operator() ( int i , int j , int k , int l, int m , int n , int o ) const;
        inline       float & operator() ( int i , int j , int k , int l, int m , int n , int o );
#else
        inline float & operator() ( int i , int j , int k , int l, int m , int n , int o ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const float & operator() ( int i , int j , int k , int l, int m , int n , int o , int p ) const;
        inline       float & operator() ( int i , int j , int k , int l, int m , int n , int o , int p );
#else
        inline float & operator() ( int i , int j , int k , int l, int m , int n , int o , int p ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// End of if !defined(USE_EXPRESSION_TEMPLATES)
#endif

        void Scalar_Indexing_For_floatArray_With_Message_Passing (
             int Address_Subscript, bool Off_Processor_With_Ghost_Boundaries,
             bool Off_Processor_Without_Ghost_Boundaries, int & Array_Index_For_float_Variable) const;

     /* Indexing operators (using Index objects)
     // Indexing operators have to have local scope (since we don't want them to have
     // a permanent lifetime).  By having local scope they there lifetime is
     // controled by the compiler (this means they hang around longer than temporaries
     // and have the same scope as local variable).  This is NOT a problem as is would be
     // for temporaries since views returned by the indexing operators have no array data
     // associated with them (directly -- they peek into another array objects data).
     // Thus these indexing operators return by value not by reference.  
     // The importance of this is clear if we imagine a function taking a view by reference
     // and then managing its lifetime -- after its first use in an array expression the 
     // view would have been deleted and then susequent use of it would be an error 
     // (as in segment fault)!
     */

     // Forces one dimensional interpretation of multidimensional floatArray!
        floatArray operator() () const;

     // This operator() takes an array of pointers to Internal_Index objects (length MAX_ARRAY_DIMENSION)
        floatArray operator() ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;
        floatArray operator() ( int Number_Of_Valid_Dimensions, const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;

     // Indexing operators for use with Index objects
     //   floatArray operator() 
     //( ARGUMENT_LIST_MACRO_CONST_REF_INTERNAL_INDEX_WITH_PREINITIALIZATION )
     //const ;

#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatArray operator() ( const Internal_Index & I ) const;
              floatArray operator() ( const Internal_Index & I );
#else
        floatArray operator() ( const Internal_Index & I ) const;
#endif

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J );
#else
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K );
#else
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L );
#else
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M );
#else
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N );
#else
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O );
#else
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P );
#else
        floatArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if !defined(PPP)
     // ... use this to make scalar indexing subscripts consistent ...
        void Fixup_User_Base 
	 (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List,
	  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List);
#endif

     // Simplifies getting a pointer to a view which is used internally in A++/P++
     // The Index_Array is length MAX_ARRAY_DIMENSION == 4
     // static floatArray* Build_Pointer_To_View_Of_Array ( const floatArray & X , Internal_Index *Index_Array );
        static floatArray* Build_Pointer_To_View_Of_Array ( const floatArray & X ,
                                                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Indexing operators for use with Index objects
        floatArray operator() ( const Internal_Indirect_Addressing_Index & I ) const;
#if (MAX_ARRAY_DIMENSION >= 2)
        floatArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        floatArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        floatArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        floatArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        floatArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        floatArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N,
                               const Internal_Indirect_Addressing_Index & O ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        floatArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N
                               const Internal_Indirect_Addressing_Index & O, const Internal_Indirect_Addressing_Index & P ) const;
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

     /* All other operators which can return actual array data are implemented as
     // return by reference so that their data can be manipulate (so specifically
     // so that the compiler will not manipulate the lifetime and as result
     // leave too many array object around (ones with real array data attached and thus ones which
     // would take up lots of space)).  This is a priciple part of the problem
     // with poorly designed array classes (temporaries must be managed to avoid 
     // waisting memory (especially in numerical applications)).
     // The restriction is that while views can be passed by reference into functions
     // expressions can NOT be passes by reference into functions.  For the sake of
     // temporary management (and thus efficient control over the memory useage)
     // we give up the ability to pass temporaries into functions.  The compromise is to 
     // use the "evaluate()" function which will have a non temporary object from the 
     // temporary one.  If you have a better idea please let me know: dquinlan@lanl.gov.
     // So expresions can be passed as parameters to functions taking input by reference
     // it just requires the use of the "evaluate()" function.
     */

     // Unary Operators + and -
        floatArray & operator+ () const;
        floatArray & operator- () const;

     // Prefix Increment and Decrement Operators ++A and --A
        floatArray & operator++ ();
        floatArray & operator-- ();

     // Postfix Increment and Decrement Operators A++ and A--  (argument is ZERO see 594 r.13.4.7 Stroustrup)
        floatArray & operator++ (int x);
        floatArray & operator-- (int x);

#if !defined(USE_EXPRESSION_TEMPLATES)
     // operator+ related functions
        friend floatArray & operator+ ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator+ ( float x    , const floatArray & Rhs );
        friend floatArray & operator+ ( const floatArray & Lhs , float x );
        floatArray & operator+= ( const floatArray & Rhs );
        floatArray & operator+= ( float Rhs );

     // operator- related functions
        friend floatArray & operator- ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator- ( float x    , const floatArray & Rhs );
        friend floatArray & operator- ( const floatArray & Lhs , float x );
        floatArray & operator-= ( const floatArray & Rhs );
        floatArray & operator-= ( float Rhs );

     // operator* related functions
        friend floatArray & operator* ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator* ( float x    , const floatArray & Rhs );
        friend floatArray & operator* ( const floatArray & Lhs , float x );
        floatArray & operator*= ( const floatArray & Rhs );
        floatArray & operator*= ( float Rhs );

     // operator/ related functions
        friend floatArray & operator/ ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator/ ( float x    , const floatArray & Rhs );
        friend floatArray & operator/ ( const floatArray & Lhs , float x );
        floatArray & operator/= ( const floatArray & Rhs );
        floatArray & operator/= ( float Rhs );

     // operator% related functions
        friend floatArray & operator% ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator% ( float x    , const floatArray & Rhs );
        friend floatArray & operator% ( const floatArray & Lhs , float x );
        floatArray & operator%= ( const floatArray & Rhs );
        floatArray & operator%= ( float Rhs );

#ifdef INTARRAY
     // Unary Operator ~ (bitwise one's complement)
        floatArray & operator~ () const;

     // operator& related functions
        friend floatArray & operator& ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator& ( float x    , const floatArray & Rhs );
        friend floatArray & operator& ( const floatArray & Lhs , float x );
        floatArray & operator&= ( const floatArray & Rhs );
        floatArray & operator&= ( float Rhs );

     // operator| related functions
        friend floatArray & operator| ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator| ( float x    , const floatArray & Rhs );
        friend floatArray & operator| ( const floatArray & Lhs , float x );
        floatArray & operator|= ( const floatArray & Rhs );
        floatArray & operator|= ( float Rhs );

     // operator^ related functions
        friend floatArray & operator^ ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator^ ( float x    , const floatArray & Rhs );
        friend floatArray & operator^ ( const floatArray & Lhs , float x );
        floatArray & operator^= ( const floatArray & Rhs );
        floatArray & operator^= ( float Rhs );

     // operator<< related functions
        friend floatArray & operator<< ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator<< ( float x    , const floatArray & Rhs );
        friend floatArray & operator<< ( const floatArray & Lhs , float x );

     // operator>> related functions
        friend floatArray & operator>> ( const floatArray & Lhs , const floatArray & Rhs );
        friend floatArray & operator>> ( float x    , const floatArray & Rhs );
        friend floatArray & operator>> ( const floatArray & Lhs , float x );
#endif

     // Internal conversion member functions (used in casting operators)
        intArray    & convertTo_intArray    () const;
        floatArray  & convertTo_floatArray  () const;
        doubleArray & convertTo_doubleArray () const;

        floatArray & operator= ( float x );
        floatArray & operator= ( const floatArray & Rhs );
#else
     // Need these to be defined for the expression template version
        floatArray & operator+= ( const floatArray & Rhs );
        floatArray & operator+= ( float Rhs );

        floatArray & operator-= ( const floatArray & Rhs );
        floatArray & operator-= ( float Rhs );

        floatArray & operator*= ( const floatArray & Rhs );
        floatArray & operator*= ( float Rhs );

        floatArray & operator/= ( const floatArray & Rhs );
        floatArray & operator/= ( float Rhs );

        floatArray & operator%= ( const floatArray & Rhs );
        floatArray & operator%= ( float Rhs );
#endif

     // Access function for static Base and Bound variables in Array_Descriptor class!
        friend void setGlobalBase( int Base );
        friend int getGlobalBase();
        friend char* APP_version();

     // abort function for A++ (defined in domain.h)
     // friend void APP_ABORT();
     // friend void APP_Assertion_Support( char* Source_File_With_Error, unsigned Line_Number_In_File );

     // friend void setGlobalBase( int Base , int Axis );
     // friend int getGlobalBase( int Axis );

     // void setBase( int New_Base_For_All_Axes );
        floatArray & setBase( int New_Base_For_All_Axes );
        int getBase() const;
        int getRawBase() const;

     // Access function for static Base and Bound variables (by Axis)!
     // void setBase( int Base , int Axis );
        floatArray & setBase( int Base , int Axis );
        floatArray & setParallelBase( int Base , int Axis );
        int getBase( int Axis ) const;
        int getStride( int Axis ) const;
        int getBound( int Axis ) const;
        int getLength( int Axis ) const;
        int getRawDataSize( int Axis ) const;
        int getRawBase( int Axis ) const;
        int getRawStride( int Axis ) const;
        int getRawBound( int Axis ) const;
        int getDataBase( int Axis ) const;
        int numberOfDimensions () const;
        Range dimension( int Axis ) const;
        Range getFullRange( int Axis ) const;


#if defined(APP) || defined(PPP)

#if defined(PPP)
     // Added (7/26/2000)
        Internal_Index getLocalMaskIndex  (int i) const { return Array_Descriptor.getLocalMaskIndex(i); }
        Internal_Index getGlobalMaskIndex (int i) const { return Array_Descriptor.getGlobalMaskIndex(i); }

     // Added (12/13/2000)
        int getLeftNumberOfPoints  ( int Axis ) const { return Array_Descriptor.getLeftNumberOfPoints(Axis);  }
        int getRightNumberOfPoints ( int Axis ) const { return Array_Descriptor.getRightNumberOfPoints(Axis); }
#endif

     // P++ specific functions report on the local partition
        int getLocalBase( int Axis ) const;
        int getLocalStride( int Axis ) const;
        int getLocalBound( int Axis ) const;
        int getLocalLength( int Axis ) const;
        int getLocalRawBase( int Axis ) const;
        int getLocalRawStride( int Axis ) const;
        int getLocalRawBound( int Axis ) const;
        Range localDimension( int Axis ) const;
#endif
        Range getLocalFullRange( int Axis ) const;

	// Returns reference to SerialArray if P++ or itself if A++
#if defined(PPP)
        floatSerialArray getLocalArray () const;
#elif defined(APP)
        floatArray getLocalArray () const;
#endif
	// Returns new SerialArray including ghost cells if P++ array
	// is nonview or view of SerialArray if P++ array is a view 
	// or itself if A++
#if defined(PPP)
        floatSerialArray getLocalArrayWithGhostBoundaries () const;
        floatSerialArray* getLocalArrayWithGhostBoundariesPointer () const;
// elif defined(APP)
#else
        floatArray getLocalArrayWithGhostBoundaries () const;
        floatArray* getLocalArrayWithGhostBoundariesPointer () const;
#endif

     // Dimension independent query functions
        bool isSameBase   ( const floatArray & X ) const;
        bool isSameBound  ( const floatArray & X ) const;
        bool isSameStride ( const floatArray & X ) const;
        bool isSameLength ( const floatArray & X ) const;
#if defined(APP) || defined(PPP)
        bool isSameGhostBoundaryWidth ( const floatArray & X ) const;
        //bool isSameDistribution ( const floatArray & X ) const;
        bool isSameDistribution ( const intArray & X ) const;
        bool isSameDistribution ( const floatArray & X ) const;
        bool isSameDistribution ( const doubleArray & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const floatArray & X ) const;

     // Member functions for access to the geometry of the A++ object
        int getGeometryBase ( int Axis ) const;
        floatArray & setGeometryBase ( int New_Geometry_Base , int Axis );

#if defined(PPP)
     // Is a P++ array partitioned along an axis
     // bool isPartitioned ( int Axis ) const;
#endif

     // total array size! (this function should be depreciated in favor of the getSize() function)
        int elementCount() const;

        int getSize() const;
#if defined(APP) || defined(PPP)
        int getLocalSize() const;
#endif

     // Number of columns and rows usefull for 2D arrays (part of  d e f i n e d  interface)!
        int cols() const;
        int rows() const;

     // Control the ability to resize and array throught assignment
     // This whole process is greatly cleaned up over that of M++!
     // int lock();
     // int unlock();

     // redimensioning member operations
     // floatArray & redim ( int i , int j = 1 , int k = 1 , int l = 1 );
        floatArray & redim ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // floatArray & redim ( const Range & i , 
     //                     const Range & j = APP_Unit_Range , 
     //                     const Range & k = APP_Unit_Range , 
     //                     const Range & l = APP_Unit_Range );
        floatArray & redim ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // We need this to avoid an ERROR: Overloading ambiguity between "doubleArray::redim(const Array_Domain_Type&)" 
     // and "doubleArray::redim(const Range&, const Range&, const Range&, const Range&, const Range&, const Range&)" 
     // so we need to implement a redim that takes a single Index object.
        floatArray & redim ( const Index & I );
        floatArray & redim ( const floatArray & X );
        floatArray & redim ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatArray & redim ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // The Sun C++ compiler can't handle template declarations in non templated classes
     // template<class T, int Template_Dimension>
     // floatArray & redim ( const Array_Descriptor_Type<T,Template_Dimension> & X );
        floatArray & redim ( const Array_Domain_Type & X );

     // Changes dimensions of array object with no change in number of elements
     // floatArray & reshape ( int i , int j = 1 , int k = 1 , int l = 1 );
        floatArray & reshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // floatArray & reshape ( const Range & i , 
     //                       const Range & j = APP_Unit_Range , 
     //                       const Range & k = APP_Unit_Range , 
     //                       const Range & l = APP_Unit_Range );
        floatArray & reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        floatArray & reshape ( const floatArray & X );
        floatArray & reshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatArray & reshape ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Used for support of weakened conformability assignment in operator= (maybe this should be private or protected)
     // floatArray & internalReshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // floatArray & internalReshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

	void fix_view_bases (int* View_Bases, int* View_Sizes, int numdims, 
			     const int* old_lengths, const int* Integer_List);
	void fix_view_bases_non_short_span 
	   (int* View_Bases, int* View_Sizes, int numdims, const int* Integer_List);

     // resize function redimensions array and copies exiting data into new array
     // floatArray & resize ( int i , int j = 1 , int k = 1 , int l = 1 );
        floatArray & resize ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // floatArray & resize ( const Range & i , 
     //                      const Range & j = APP_Unit_Range , 
     //                      const Range & k = APP_Unit_Range , 
     //                      const Range & l = APP_Unit_Range );
        floatArray & resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        floatArray & resize ( const floatArray & X );
        floatArray & resize ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatArray & resize ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // adopt existing data as an A++ array
     // versions of adopt which are specific to P++
     // To avoid the 1D P++ adopt function from being confused with the
     // 2D A++ adopt function we must provide different member function names.
        floatArray & adopt ( const float* Data_Pointer , const Partitioning_Type & Partition ,
                                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        floatArray & adopt ( const float* Data_Pointer , const Partitioning_Type & Partition ,
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // floatArray & adopt ( const float* Data_Pointer , const Partitioning_Type & Partition ,
     //                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // A++ specific implementation of adopt member functions
     // These A++ specific function are made available to P++ to maintain
     // the identical interface even though under P++ these functions
     // are not implemented.

     // floatArray & adopt ( const float* Data_Pointer , 
     //                     int i , int j = 1 , int k = 1 , int l = 1 ); 
     // function with interface for internal use only
        floatArray & adopt ( const float* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
        floatArray & adopt ( const float* Data_Pointer , 
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatArray & adopt ( const float* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
     // Use the size of an existing A++/P++ array object
        floatArray & adopt ( const float* Data_Pointer , const floatArray & X );

     // These are used only within the internal implementation of A++/P++
        floatArray & adopt ( const float* Data_Pointer , 
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Force a deep copy since deep copy constructors are not alway called
     // even when explicit attempts are made to call them 
        floatArray & copy ( const floatArray & X , int DeepCopy = DEEPCOPY );

     // Convert indexing of view to be non HPF!
        floatArray & useFortranIndexing ( bool Flag );

     // This would effectively be a reference (so call reference instead)!
     // floatArray & adopt ( const floatArray & X  );

     // Reduction operators!
        friend float min ( const floatArray & X );
        friend float max ( const floatArray & X );

#if defined(PPP)
     // P++ support for reduction operations
     // By making each of these a friend function we give them 
     // access to the doubleArray::Operation_Type variables
        friend void Reduction_Operation ( int Input_Function, double & x );
        friend void Reduction_Operation ( int Input_Function, float & x );
        friend void Reduction_Operation ( int Input_Function, int & x );
#endif

#if defined(USE_EXPRESSION_TEMPLATES)
     // The non-expression template version returns values by value (not by reference)
        friend floatArray min ( const floatArray & X , const floatArray & Y );
        friend floatArray min ( const floatArray & X , float y );
        friend floatArray min ( float x , const floatArray & Y );
        friend floatArray min ( const floatArray & X , const floatArray & Y , const floatArray & Z );
        friend floatArray min ( float x , const floatArray & Y , const floatArray & Z );
        friend floatArray min ( const floatArray & X , float y , const floatArray & Z );
        friend floatArray min ( const floatArray & X , const floatArray & Y , float z );

        friend floatArray max ( const floatArray & X , const floatArray & Y );
        friend floatArray max ( const floatArray & X , float y );
        friend floatArray max ( float x , const floatArray & Y );
        friend floatArray max ( const floatArray & X , const floatArray & Y , const floatArray & Z );
        friend floatArray max ( float x , const floatArray & Y , const floatArray & Z );
        friend floatArray max ( const floatArray & X , float y , const floatArray & Z );
        friend floatArray max ( const floatArray & X , const floatArray & Y , float z );

     // Misc functions
        floatArray & replace ( const intArray & X , const floatArray & Y );
        floatArray & replace ( int x , const floatArray & Y );
        floatArray & replace ( const intArray & X , float y );

        friend float sum ( const floatArray & X );

     // Sum along axis 
        friend floatArray sum ( const floatArray & X , int Axis );
#else

     // The non-expression template version returns values by reference
        friend floatArray & min ( const floatArray & X , const floatArray & Y );
        friend floatArray & min ( const floatArray & X , float y );
        friend floatArray & min ( float x , const floatArray & Y );
        friend floatArray & min ( const floatArray & X , const floatArray & Y , const floatArray & Z );
        friend floatArray & min ( float x , const floatArray & Y , const floatArray & Z );
        friend floatArray & min ( const floatArray & X , float y , const floatArray & Z );
        friend floatArray & min ( const floatArray & X , const floatArray & Y , float z );

        friend floatArray & max ( const floatArray & X , const floatArray & Y );
        friend floatArray & max ( const floatArray & X , float y );
        friend floatArray & max ( float x , const floatArray & Y );
        friend floatArray & max ( const floatArray & X , const floatArray & Y , const floatArray & Z );
        friend floatArray & max ( float x , const floatArray & Y , const floatArray & Z );
        friend floatArray & max ( const floatArray & X , float y , const floatArray & Z );
        friend floatArray & max ( const floatArray & X , const floatArray & Y , float z );

     // Misc functions
        floatArray & replace ( const intArray & X , const floatArray & Y );
        floatArray & replace ( int x , const floatArray & Y );
        floatArray & replace ( const intArray & X , float y );

        friend float sum ( const floatArray & X );
        friend floatArray & sum ( const floatArray & X , const floatArray & Y );
        friend floatArray & sum ( const floatArray & X , const floatArray & Y , const floatArray & Z );

     // Sum along axis
        friend floatArray & sum ( const floatArray & X , int Axis );
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)
#ifndef INTARRAY
        friend floatArray & fmod ( const floatArray & X , float y );
        friend floatArray & fmod ( float x , const floatArray & Y );
        friend floatArray & fmod ( const floatArray & X , const floatArray & Y );
#endif
        friend floatArray & mod ( const floatArray & X , float y );
        friend floatArray & mod ( float x , const floatArray & Y );
        friend floatArray & mod ( const floatArray & X , const floatArray & Y );

        friend floatArray & pow ( const floatArray & X , float y );  // returns X**y
        friend floatArray & pow ( float x , const floatArray & Y );
        friend floatArray & pow ( const floatArray & X , const floatArray & Y );

        friend floatArray & sign ( const floatArray & Array_Signed_Value , const floatArray & Input_Array );
        friend floatArray & sign ( float Scalar_Signed_Value             , const floatArray & Input_Array );
        friend floatArray & sign ( const floatArray & Array_Signed_Value , float Input_Value  );

#ifndef INTARRAY
        friend floatArray & log   ( const floatArray & X );
        friend floatArray & log10 ( const floatArray & X );
        friend floatArray & exp   ( const floatArray & X );
        friend floatArray & sqrt  ( const floatArray & X );
        friend floatArray & fabs  ( const floatArray & X );
        friend floatArray & ceil  ( const floatArray & X );
        friend floatArray & floor ( const floatArray & X );
#endif
        friend floatArray & abs   ( const floatArray & X );

     // Not fully supported yet (only works for 1D or 2D arrays object)!
        friend floatArray & transpose ( const floatArray & X );

     // Trig functions!
#ifndef INTARRAY
        friend floatArray & cos   ( const floatArray & X );
        friend floatArray & sin   ( const floatArray & X );
        friend floatArray & tan   ( const floatArray & X );
        friend floatArray & acos  ( const floatArray & X );
        friend floatArray & asin  ( const floatArray & X );
        friend floatArray & atan  ( const floatArray & X );
        friend floatArray & cosh  ( const floatArray & X );
        friend floatArray & sinh  ( const floatArray & X );
        friend floatArray & tanh  ( const floatArray & X );
        friend floatArray & acosh ( const floatArray & X );
        friend floatArray & asinh ( const floatArray & X );
        friend floatArray & atanh ( const floatArray & X );

     // All the different atan2 functions for arrays (ask for by Bill Henshaw)
        friend floatArray & atan2 ( const floatArray & X , float y );
        friend floatArray & atan2 ( float x , const floatArray & Y );
        friend floatArray & atan2 ( const floatArray & X , const floatArray & Y );
#endif

     // relational operators 
     // operator< related functions
        friend intArray & operator< ( const floatArray & Lhs , const floatArray & Rhs );
        friend intArray & operator< ( float x    , const floatArray & Rhs );
        friend intArray & operator< ( const floatArray & Lhs , float x );

     // operator<= related functions
        friend intArray & operator<= ( const floatArray & Lhs , const floatArray & Rhs );
        friend intArray & operator<= ( float x    , const floatArray & Rhs );
        friend intArray & operator<= ( const floatArray & Lhs , float x );

     // operator> related functions
        friend intArray & operator> ( const floatArray & Lhs , const floatArray & Rhs );
        friend intArray & operator> ( float x    , const floatArray & Rhs );
        friend intArray & operator> ( const floatArray & Lhs , float x );

     // operator>= related functions
        friend intArray & operator>= ( const floatArray & Lhs , const floatArray & Rhs );
        friend intArray & operator>= ( float x    , const floatArray & Rhs );
        friend intArray & operator>= ( const floatArray & Lhs , float x );

     // operator== related functions
        friend intArray & operator== ( const floatArray & Lhs , const floatArray & Rhs );
        friend intArray & operator== ( float x    , const floatArray & Rhs );
        friend intArray & operator== ( const floatArray & Lhs , float x );

     // operator!= related functions
        friend intArray & operator!= ( const floatArray & Lhs , const floatArray & Rhs );
        friend intArray & operator!= ( float x    , const floatArray & Rhs );
        friend intArray & operator!= ( const floatArray & Lhs , float x );

     // operator&& related functions
        friend intArray & operator&& ( const floatArray & Lhs , const floatArray & Rhs );
        friend intArray & operator&& ( float x    , const floatArray & Rhs );
        friend intArray & operator&& ( const floatArray & Lhs , float x );

     // operator|| related functions
        friend intArray & operator|| ( const floatArray & Lhs , const floatArray & Rhs );
        friend intArray & operator|| ( float x    , const floatArray & Rhs );
        friend intArray & operator|| ( const floatArray & Lhs , float x );
#else
     // If we ARE using expression templates then we have to declare some friend functions
     // friend float sum ( const floatArray & X );
     // friend floatArray sum ( const floatArray & X , int axis );

        template<class T, class A>
        friend T sum ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T max ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T min ( const Indexable<T,A> & X );
#endif

     // operator! related functions
        intArray & operator! ();

#if defined(APP) || defined(PPP)
     // Note that P++ assumes that any results returned from these functions are not used
     // effect the parallel control flow (this is due to the SPMD nature of a P++ application).
     // No attempt is made to provide access to parallel info in P++ nor is P++ intended to
     // be a way to write explicitly parallel applications (use explicit message passing for that!).

     // These allow an existing array to be partitioned of the
     // existing array repartitioned acording to another existing array
     // in which case the array is alligned with the input array
        Partitioning_Type getPartition () const;
        Internal_Partitioning_Type* getInternalPartitionPointer () const;
#if defined(USE_PADRE)
        void partition ( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition );
     // void partition ( const BaseArray   & Example_Array );
#endif
        floatArray & partition ( const Internal_Partitioning_Type & Partition );
        floatArray & partition ( const Partitioning_Type & Partition );
        floatArray & partition ( const doubleArray & Example_Array );
        floatArray & partition ( const floatArray & Example_Array );
        floatArray & partition ( const intArray & Example_Array );

     // typedef support for member function pointers as parameters for the map diagnostic mechanism
     // We support two different function prototypes
        typedef int (floatArray::*floatArrayMemberVoidFunctionPointerType) (void) const;
        typedef int (floatArray::*floatArrayMemberIntegerFunctionPointerType) (int) const;

     // Builds array with base/bound info for each axis and processor
     // calls special purpose communication function

#if defined(PPP)
     // This only makes since in P++
        intSerialArray buildProcessorMap ( floatArrayMemberVoidFunctionPointerType X );
        intSerialArray buildProcessorMap ( floatArrayMemberIntegerFunctionPointerType X );
        intSerialArray buildProcessorMap ( floatArrayMemberIntegerFunctionPointerType X, floatArrayMemberIntegerFunctionPointerType Y );
#else
     // Preserve the interface in A++
        intArray buildProcessorMap ( floatArrayMemberVoidFunctionPointerType X );
        intArray buildProcessorMap ( floatArrayMemberIntegerFunctionPointerType X );
        intArray buildProcessorMap ( floatArrayMemberIntegerFunctionPointerType X, floatArrayMemberIntegerFunctionPointerType Y );
#endif

        void displayArraySizesPerProcessor (const char* Label = "");
        void displayPartitioning  (const char *Label = "");
        void displayCommunication (const char* Label = "");
        void displayLeftRightNumberOfPoints (const char* Label);

     // functions specific to the manipulation of internal ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
        int getGhostBoundaryWidth ( int Axis ) const;

     // Depreciated function (not documented in manual)
        int getInternalGhostCellWidth ( int Axis ) const;

     // Depreciated function (not documented in manual)
        floatArray & setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatArray & setInternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        floatArray & setInternalGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // New versions
        floatArray & setGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatArray & setGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        floatArray & setGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // functions specific to the manipulation of external ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
     // these are usually zero unless the boundaries are meant to have large NON-PARTITIONED boundary regions
        int getExternalGhostCellWidth ( int Axis ) const;
     // floatArray & setExternalGhostCellWidth ( int Number_Of_Ghost_Cells_I, int Number_Of_Ghost_Cells_J, 
     //                                         int Number_Of_Ghost_Cells_K, int Number_Of_Ghost_Cells_L );
        floatArray & setExternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        void updateGhostBoundaries() const;
#endif

#if defined(PPP)
        floatArray( const float* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        floatArray( const float* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
#endif
     private:
     // Support for defered evaluation!  (This function does nothing and will be removed soon)
        void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const;

     // This constructor is used by the indexing operators to build array objects defined in
     // local scope.  It allows the support of deferred evaluation on the resulting views!

#if defined(PPP)
        floatArray ( floatSerialArray* SerialArray_Pointer , 
        	    const Array_Domain_Type* Array_Domain_Pointer, 
		    bool AvoidBuildingIndirectAddressingView = FALSE );

#if 0
     // This function is called in lazy_operand.C!
        floatArray ( const floatSerialArray* SerialArray_Pointer , 
        	    floatArray_Descriptor_Type* 
		    Array_Descriptor_Pointer ,
        	    /*Array_Descriptor_Type* Array_Descriptor_Pointer ,*/
                    Operand_Storage* Array_Storage_Pointer );
#endif
        floatArray( const floatSerialArray* SerialArray_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        floatArray( const floatSerialArray* SerialArray_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
		   Indirect_Index_List );
        floatArray( const floatSerialArray* SerialArray_Pointer, 
                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
#else

        floatArray ( const float* Array_Data_Pointer ,
                    const Array_Domain_Type* Array_Domain_Pointer,
                    bool AvoidBuildingIndirectAddressingView = FALSE );

    public:
        floatArray( const float* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        floatArray( const float* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

#if 0
     // This function is called in lazy_operand.C!
        floatArray ( const float* Array_Data_Pointer , 
	   floatArray_Descriptor_Type* 
	   Array_Descriptor_Pointer , Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
        static float Abstract_Unary_Operator ( const floatArray & X ,
                  MDI_float_Prototype_14 , int Operation_Type ); 

        static floatArray & Abstract_Unary_Operator ( const floatArray & X ,
                  MDI_float_Prototype_0 , MDI_float_Prototype_1 ,
                  int Operation_Type ); 

     /* We need a special binary operator to use in the case of operator- and
     // operator/ since they don't commute and so if the Rhs of the binary operation
     // is a temporary it is reused in a different way that for a commutative
     // operator.  Basically if the Rhs is a temporary we DO reuse it, but we 
     // can't do the equivalent of accumulation into it (which is optimal in
     // efficiency) as in the operator+ and as we can in the case of the Lhs
     // operand being a temporary.  
     // Notice that the non commuting operator reuses the Rhs differently than 
     // it can reuse the Lhs if the Lhs is a temporary.
     // (Lhs = Lhs + Rhs  --> Lhs += Rhs) == (Rhs = Lhs + Rhs --> Rhs += Lhs)
     // (Lhs = Lhs - Rhs  --> Lhs -= Rhs) == (Rhs = Lhs - Rhs --> Rhs = Lhs - Rhs)
     */
        static floatArray & Abstract_Binary_Operator ( 
                  const floatArray & Lhs , const floatArray & Rhs ,
                  MDI_float_Prototype_2 , MDI_float_Prototype_3 ,
                  int Operation_Type );

        static floatArray & Abstract_Binary_Operator ( 
                  const floatArray & Lhs , float x ,
                  MDI_float_Prototype_4 , MDI_float_Prototype_5 ,
                  int Operation_Type );
 
        static floatArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const floatArray & Lhs , const floatArray & Rhs ,
                  MDI_float_Prototype_2 , MDI_float_Prototype_3 ,
                  int Operation_Type );

        static floatArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const floatArray & Lhs , float x ,
                  MDI_float_Prototype_4 , MDI_float_Prototype_5 ,
                  int Operation_Type );
 
     // Used for replace function since it is of form "floatArray replace (intArray,floatArray)"
        floatArray & Abstract_Modification_Operator ( 
                  const intArray & Lhs , const floatArray & Rhs ,
                  MDI_float_Prototype_6 ,
                  int Operation_Type ); 

     // Used for replace function since it is of form "floatArray replace (intArray,float)"
        floatArray & Abstract_Modification_Operator ( 
                  const intArray & Lhs , float x ,
                  MDI_float_Prototype_7 ,
                  int Operation_Type ); 

#ifndef INTARRAY
     // Used for replace function since it is of form "floatArray replace (int,floatArray)"
        floatArray & Abstract_Modification_Operator ( 
                  const floatArray & Lhs , int x ,
                  MDI_float_Prototype_8 ,
                  int Operation_Type ); 
#endif

        static void Abstract_Operator_Operation_Equals ( 
                  const floatArray & Lhs , const floatArray & Rhs ,
                  MDI_float_Prototype_3 ,
                  int Operation_Type ); 

        static void Abstract_Operator_Operation_Equals ( 
                  const floatArray & Lhs , float x ,
                  MDI_float_Prototype_5 ,
                  int Operation_Type ); 

        static intArray & Abstract_Unary_Operator_Returning_IntArray ( const floatArray & X ,
                  MDI_float_Prototype_9 , MDI_float_Prototype_10 ,
                  int Operation_Type ); 

        static intArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const floatArray & Lhs , const floatArray & Rhs ,
                  MDI_float_Prototype_11 , MDI_float_Prototype_9 ,
                  int Operation_Type ); 

        static intArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const floatArray & Lhs , float x ,
                  MDI_float_Prototype_12 , MDI_float_Prototype_13 ,
                  int Operation_Type );

     // Support for conversion operators
        static intArray & Abstract_int_Conversion_Operator (
                  const floatArray & X ,
                  MDI_float_Prototype_convertTo_intArray ,
                  int Operation_Type );

        static floatArray & Abstract_float_Conversion_Operator (
                  const floatArray & X ,
                  MDI_float_Prototype_convertTo_floatArray ,
                  int Operation_Type );

        static doubleArray & Abstract_double_Conversion_Operator (
                  const floatArray & X ,
                  MDI_float_Prototype_convertTo_doubleArray ,
                  int Operation_Type );

#elif defined(PPP)


     // Inline functions must appear in the header file (not the source file) when using the
     // GNU compiler!  We could alternatively use the INLINE macro (perhaps we will later)
     // the INLINE macro is defined to be "" when the GNU compiler is used (though this 
     // should be verified).
#if !defined(GNU)
// inline
#endif
        static void Modify_Reference_Counts_And_Manage_Temporaries (
             Array_Conformability_Info_Type *Array_Set,
             floatArray *Temporary,
             floatSerialArray* X_Serial_PCE_Array,
             floatSerialArray *Data );

#if !defined(GNU)
// inline
#endif
        static void Modify_Reference_Counts_And_Manage_Temporaries (
             Array_Conformability_Info_Type *Array_Set,
             floatArray *Temporary,
             floatSerialArray* Lhs_Serial_PCE_Array,
             floatSerialArray* Rhs_Serial_PCE_Array,
             floatSerialArray *Data );

     // reduction operator for sum min max etc.
        static float Abstract_Operator 
	   ( Array_Conformability_Info_Type *Array_Set, 
	     const floatArray & X, floatSerialArray* X_Serial_PCE_Array, 
	     float x, int Operation_Type );

     // usual binary operators: operator+ operator- operator* operator/ 
     // operator% for both array to array and array to scalar operations
        static floatArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
		  const floatArray & X_ParallelArray, 
		  floatSerialArray* X_Serial_PCE_Array, 
		  floatSerialArray & X_SerialArray );

        static floatArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
     //           floatArray & Lhs, floatArray & Rhs, 
                  const floatArray & Lhs, const floatArray & Rhs, 
                  floatSerialArray* Lhs_Serial_PCE_Array, floatSerialArray* Rhs_Serial_PCE_Array, 
                  floatSerialArray & X_SerialArray );

     // These operators are special because they don't generate temporaries
     // Used in array to scalar versions of operator+= operator-= operato*= operator/= operator%=
        static floatArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , const floatArray & This_ParallelArray, floatSerialArray* This_Serial_PCE_Array, const floatSerialArray & This_SerialArray );
     // Used in array to array versions of operator+= operator-= operato*= operator/= operator%=
        static floatArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatArray & This_ParallelArray, const floatArray & Lhs, 
                  floatSerialArray* This_Serial_PCE_Array, floatSerialArray* Lhs_Serial_PCE_Array, 
                  const floatSerialArray & X_SerialArray );

#if !defined(INTARRAY)
     // relational operators operator&& operator|| operator<= operator< etc.
        static intArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatArray & X_ParallelArray, 
                  floatSerialArray* X_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

        static intArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatArray & Lhs, const floatArray & Rhs, 
                  floatSerialArray* Lhs_Serial_PCE_Array, 
		  floatSerialArray* Rhs_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

     // replace operators for array to array and array to scalar
        static floatArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatArray & This_ParallelArray, const intArray & Lhs, 
                  floatSerialArray* This_Serial_PCE_Array, intSerialArray* Lhs_Serial_PCE_Array, 
                  const floatSerialArray & X_SerialArray );
        static floatArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatArray & This_ParallelArray, const intArray & Lhs, const floatArray & Rhs, 
                  floatSerialArray* This_Serial_PCE_Array, 
                  intSerialArray* Lhs_Serial_PCE_Array,
                  floatSerialArray* Rhs_Serial_PCE_Array, 
                  const floatSerialArray & X_SerialArray );
#endif

     // Abstract operator support for conversion operators
        static intArray & Abstract_int_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const floatArray & X_ParallelArray,
                  floatSerialArray* X_Serial_PCE_Array,
                  intSerialArray & X_SerialArray );

        static floatArray & Abstract_float_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const floatArray & X_ParallelArray,
                  floatSerialArray* X_Serial_PCE_Array,
                  floatSerialArray & X_SerialArray );

        static doubleArray & Abstract_double_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set ,
                  const floatArray & X_ParallelArray,
                  floatSerialArray* X_Serial_PCE_Array,
                  doubleSerialArray & X_SerialArray );


#else
#error Must DEFINE either APP || SERIAL_APP || PPP
#endif

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif
 
        static floatArray*  Build_Temporary_By_Example ( const intArray    & Lhs );
        static floatArray*  Build_Temporary_By_Example ( const floatArray  & Lhs );
        static floatArray*  Build_Temporary_By_Example ( const doubleArray & Lhs );
        static floatArray & Build_New_Array_Or_Reuse_Operand ( const floatArray & Lhs , const floatArray & Rhs , 
                                                              Memory_Source_Type & Result_Array_Memory );
        static floatArray & Build_New_Array_Or_Reuse_Operand ( const floatArray & X , 
                                                              Memory_Source_Type & Result_Array_Memory );

     // GNU g++ compiler likes to see these marked as inlined if they are in the header file as well.
     // (11/6/2000) Moved Delete_If_Temporary to array.C
     // friend inline void Delete_If_Temporary     ( const floatArray & X );
        friend void Delete_If_Temporary     ( const floatArray & X );

     // (11/24/2000) Moved Delete_Lhs_If_Temporary to array.C
     // friend inline void Delete_Lhs_If_Temporary ( const floatArray & Lhs );
        friend void Delete_Lhs_If_Temporary ( const floatArray & Lhs );

     // Raw memory allocation and dealocation located in these functions
        void Delete_Array_Data ();

     // void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) const;
        void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) ;

#if defined(PPP)
     // void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) const;
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) ;

     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( floatArray* This_Pointer , floatArray* Lhs , floatArray* Rhs );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const floatArray & X_ParallelArray,    
	     floatSerialArray* & X_SerialArray );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const floatArray & Lhs_ParallelArray,  
	     floatSerialArray* & Lhs_SerialArray,
             const floatArray & Rhs_ParallelArray,  
	     floatSerialArray* & Rhs_SerialArray );
     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( const floatArray & This_ParallelArray, 
     //      floatSerialArray* & This_SerialArray,
     //      const floatArray & Lhs_ParallelArray,  
     //      floatSerialArray* & Lhs_SerialArray,
     //      const floatArray & Rhs_ParallelArray,  
     //      floatSerialArray* & Rhs_SerialArray );
#if !defined(INTARRAY)
     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( floatArray* This_Pointer , intArray* Lhs , floatArray* Rhs );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const floatArray  & This_ParallelArray, 
	     floatSerialArray*  & This_SerialArray,
             const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray );
#if 0
     // I think we can skip this function (I hope)
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const floatArray  & Rhs_ParallelArray,  
	     floatSerialArray*  & Rhs_SerialArray );
#endif
#endif
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const floatArray  & This_ParallelArray, 
	     floatSerialArray*  & This_SerialArray,
             const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const floatArray  & Rhs_ParallelArray,  
	     floatSerialArray*  & Rhs_SerialArray );

        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const floatArray & Lhs_ParallelArray,  
	     floatSerialArray* & Lhs_SerialArray,
             const floatArray & Rhs_ParallelArray,  
	     floatSerialArray* & Rhs_SerialArray );
        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const floatArray & Lhs_ParallelArray,    
	     floatSerialArray* & Lhs_SerialArray );

#if !defined(INTARRAY)
        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const floatArray & Lhs_ParallelArray,  
	     floatSerialArray* & Lhs_SerialArray,
             const intArray & Rhs_ParallelArray,  
	     intSerialArray* & Rhs_SerialArray );
#endif
#endif

#if defined(USE_EXPRESSION_TEMPLATES)
     public:

        floatArray & operator= ( float x );
        floatArray & operator= ( const floatArray & Rhs );

     // Access to pointer specific to expression template indexing
        inline float* APP_RESTRICT_MACRO getExpressionTemplateDataPointer () const { return Array_Descriptor.getExpressionTemplateDataPointer(); }

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.h HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
        floatArray(const Indexable<T1, A> &Rhs);

        template <class T1, class A>
        floatArray & operator=(const Indexable<T1, A> & Rhs);

     // We comment this out for now while we get the
     // operator=(const Indexable<T1, A> &rhs) working (above)
        template <class T1, class A>
        floatArray & operator+=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        floatArray & operator-=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        floatArray & operator*=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        floatArray & operator/=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        floatArray & operator%=(const Indexable<T1, A> &Rhs);
#else
        floatArray(const InAssign<float> &a);

        floatArray & operator=(const InAssign<float> &Rhs);
        floatArray & operator+=(const InAssign<float> &Rhs);
        floatArray & operator-=(const InAssign<float> &Rhs);
        floatArray & operator*=(const InAssign<float> &Rhs);
        floatArray & operator/=(const InAssign<float> &Rhs);
        floatArray & operator%=(const InAssign<float> &Rhs);

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif
   };


#undef FLOATARRAY

#define INTARRAY
// Default base and bound object for use in intArray constructor
extern Range APP_Unit_Range;



// Forward class declarations
class doubleArray_Function_Steal_Data;
class doubleArray_Function_0;
class doubleArray_Function_1;
class doubleArray_Function_2;
class doubleArray_Function_3;
class doubleArray_Function_4;
class doubleArray_Function_5;
class doubleArray_Function_6;
class doubleArray_Function_7;
class doubleArray_Function_8;
class doubleArray_Function_9;
class doubleArray_Function_10;
class doubleArray_Function_11;
class doubleArray_Function_12;
class doubleArray_Function_13;
class doubleArray_Function_14;
class doubleArray_Function_15;
class doubleArray_Function_16;

class doubleArray_Aggregate_Operator;

// Forward class declarations
class floatArray_Function_Steal_Data;
class floatArray_Function_0;
class floatArray_Function_1;
class floatArray_Function_2;
class floatArray_Function_3;
class floatArray_Function_4;
class floatArray_Function_5;
class floatArray_Function_6;
class floatArray_Function_7;
class floatArray_Function_8;
class floatArray_Function_9;
class floatArray_Function_10;
class floatArray_Function_11;
class floatArray_Function_12;
class floatArray_Function_13;
class floatArray_Function_14;
class floatArray_Function_15;
class floatArray_Function_16;

class floatArray_Aggregate_Operator;

// Forward class declarations
class intArray_Function_Steal_Data;
class intArray_Function_0;
class intArray_Function_1;
class intArray_Function_2;
class intArray_Function_3;
class intArray_Function_4;
class intArray_Function_5;
class intArray_Function_6;
class intArray_Function_7;
class intArray_Function_8;
class intArray_Function_9;
class intArray_Function_10;
class intArray_Function_11;
class intArray_Function_12;
class intArray_Function_13;
class intArray_Function_14;
class intArray_Function_15;
class intArray_Function_16;

class intArray_Aggregate_Operator;


#if defined(APP) || defined(PPP)
class Array_Conformability_Info_Type;
class Partitioning_Type;
#endif


#if defined(PPP)
// PVM function prototype
// Bugfix (6/20/95) To remove the dependence on pvm in the P++ implementation
// we pass the Operation_Type (an int) instead of a PVM function pointer.
// So this typedef should not be required anymore
// extern "C" { typedef void (Reduction_Function_Type) ( int*, void*, void*, int*, int*); };
#endif

// we use the case where the DOUBLEARRAY is defined so that
// this will be placed first in the file once it is expanded by m4!
// if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)
// if defined(DOUBLEARRAY) && (defined(APP) || defined(PPP))
#if defined(DOUBLEARRAY)
// error "USE_EXPRESSION_TEMPLATES is defined!"
class BaseArray
   {
  // Base class for all array objects.  This base class may eventualy be filled with
  // most of the member function that I presently in the A++/P++ objects themselves.
  // This would simplify the definition of A++/P++ and in the templated version it might
  // simplify the instantiation of the templated member functions.

       // this might be a useful mechanism to avoid any future requirements of runtime typing
       // but we avoid using it unless there is no other way to handle this.
       // int TypeInfo;  // A mechanism to record the derived type -- a poor design mechanism

     public:
#if 1
       // virtual ~BaseArray();
       // virtual ~BaseArray() = 0;
          BaseArray();
#endif

// We comment out these pure virtual functions to test a bug
#if 0
       // This is a list of most of the functions in the array objects
       // I think that most of these could placed into the base class
       // however for the moment I have only made them pure virtual functions.
       // some of these (like the getDomainPointer - could not be implemented within the
       // base class since they make calls to the typed (double, float, int) ArrayDescriptorType
       // object which is only contained in the derived class).
          virtual int getReferenceCount() const = 0;
          virtual void incrementReferenceCount() = 0;
          virtual void decrementReferenceCount() = 0;
          virtual void resetRawDataReferenceCount() const = 0;
          virtual int getRawDataReferenceCount() const = 0;
          virtual void incrementRawDataReferenceCount() const = 0;
          virtual void decrementRawDataReferenceCount() const = 0;
          virtual void ReferenceCountedDelete () = 0;
       // virtual static void New_Function_Loop () = 0;
       // virtual static void freeMemoryInUse() = 0;
       // virtual void *operator new    (size_t) = 0;
       // virtual void operator  delete (void*) = 0;
          virtual void Initialize_Array ( 
               int Number_Of_Valid_Dimensions ,
               const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
               bool Force_Memory_Allocation ) = 0;
       // virtual void Initialization_From_Range_List ( 
       //      int Number_Of_Valid_Dimensions,
       //      const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Range_List ) = 0;
          virtual int getDataOffset ( int Axis ) const = 0;
          virtual Array_Domain_Type* getDomainPointer () = 0;
       // virtual Array_Descriptor_Type *& getArrayDomainReference () const = 0;
          virtual bool Is_Built_By_Defered_Evaluation() const = 0;
          virtual void setTemporary( bool x ) const = 0;
          virtual bool isTemporary() const = 0;
          virtual bool isNullArray() const = 0;
          virtual bool isView()      const = 0;
          virtual bool isContiguousData() const = 0;

       // I have commented this out to test a bugfix!
       // virtual void Test_Consistency ( const char *Label = "" ) const = 0;

          virtual bool usesIndirectAddressing() const = 0; 
#if !defined(PPP)
          virtual void Fixup_User_Base
              (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List,
               const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List) = 0;
#endif
          virtual int getBase() const = 0;
          virtual int getRawBase() const = 0;
          virtual int getBase( int Axis ) const = 0;
          virtual int getStride( int Axis ) const = 0;
          virtual int getBound( int Axis ) const = 0;
          virtual int getLength( int Axis ) const = 0;
          virtual int getRawDataSize( int Axis ) const = 0;
          virtual int getRawBase( int Axis ) const = 0;
          virtual int getRawStride( int Axis ) const = 0;
          virtual int getRawBound( int Axis ) const = 0;
          virtual int getDataBase( int Axis ) const = 0;
          virtual int numberOfDimensions () const = 0;
          virtual Range dimension( int Axis ) const = 0;
          virtual Range getFullRange( int Axis ) const = 0;
#if defined(APP) || defined(PPP)
       // P++ specific functions report on the local partition
          virtual int getLocalBase( int Axis ) const = 0;
          virtual int getLocalStride( int Axis ) const = 0;
          virtual int getLocalBound( int Axis ) const = 0;
          virtual int getLocalLength( int Axis ) const = 0;
          virtual int getLocalRawBase( int Axis ) const = 0;
          virtual int getLocalRawStride( int Axis ) const = 0;
          virtual int getLocalRawBound( int Axis ) const = 0;
          virtual Range localDimension( int Axis ) const = 0;
#endif
          virtual Range getLocalFullRange( int Axis ) const = 0;
          virtual int elementCount() const = 0;
          virtual int cols() const = 0;
          virtual int rows() const = 0;

       // note that redim reshape resize may be functions that might have
       // to be defined only within the derived class.  Some support might
       // be possible from the base class if so this this would be worthwhile.

          virtual void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const = 0;
          virtual void Delete_Array_Data () = 0;
          virtual void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) = 0;
#endif

#if 1   
       // Bugs in the Sun C++ compiler have routinely caused the base class member functions
       // to be called instead of the derived class.  One solution might be to have the
       // Array_ID variable stored in the base class so that it can always be accessed from 
       // the base class.  Since the other function is only defined for APP and PPP
       // I think the definition of some virtual function here is required.
       // virtual int Array_ID () const = 0;
          virtual int Dummy_Function_Array_ID () const
             { 
               printf ("ERROR: Virtual Base Class called: BaseArray::Array_ID() \n");
               APP_ABORT();
               return -1000;
             }
#endif

#if defined(APP) || defined(PPP)
#if defined(USE_PADRE)
       // virtual void partition( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition ) = 0;
          virtual void partition( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition )
             {
            // Avoid compiler warning about unused variable
               if (&Partition);

               printf ("ERROR: Virtual Base Class called: BaseArray::partition(PADRE_Distribution<*,*,*>) \n");
               APP_ABORT();
             }
#endif
// We comment out these pure virtual functions to test a bug
#if 0
          virtual Partitioning_Type getPartition () const = 0;
          virtual void displayPartitioning (const char *Label = "") = 0;
          virtual int getInternalGhostCellWidth ( int Axis ) const = 0;
          virtual int getExternalGhostCellWidth ( int Axis ) const = 0;
          virtual void updateGhostBoundaries() const = 0;
#endif
#endif

#if 0
       // This function exists in both bases classes of an array object and 
       // creates an ambiguous reference to the buildListOfArrayOperands member 
       // function of a doubleArray or floatArray or intArray.
          void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
             {
               printf ("Inside of BaseArray::buildListOfArrayOperands(list<BaseArray*> & arrayList) \n");
               abort();
            // APP_ABORT();
            // There is a const problem here!
            // arrayList.push_back (this);
             }
#endif
   };
#endif

#if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)
/*
///////////////////////////////////////////////////////////////////////////
//
// CLASS NAME
//    InArray
//
// DESCRIPTION
//    A version of Indexable that container classes, like Arrays, can
//    inherit from in order to become indexable as well.
//
///////////////////////////////////////////////////////////////////////////
*/

template<class T>
class InArray : public Indexable<T, InArray<T> > 
   {
     public:
        typedef T RetT;

     protected:

        InArray ()  
           { 
             printf ("ERROR: inside of InArray::InArray() \n"); 
             APP_ABORT(); 
           }

     // We pass FALSE to the Array_Descriptor constructor because we want 
     // to have AvoidBuildingIndirectAddressingView = FALSE internally
        InArray(const Array_Domain_Type & Domain)
           : Array_Descriptor(Domain,FALSE) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const floatArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const intArray* InputArrayPointer, const Array_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}

     // I don't think we need this now!
        InArray ( const doubleArray* InputArrayPointer ) {}
        InArray ( const floatArray* InputArrayPointer ) {}
        InArray ( const intArray* InputArrayPointer ) {}

     public:

        inline bool usesIndirectAddressing() const APP_RESTRICT_MACRO;
        inline int internalNumberOfDimensions() const APP_RESTRICT_MACRO;
        inline bool containsArrayID ( int lhs_id ) const APP_RESTRICT_MACRO;

        void buildListOfArrayOperands( list<doubleArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<doubleArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<floatArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<floatArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<intArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<intArray*> & arrayList) \n");
           }

        void buildListOfArrayOperands( list<BaseArray*> & arrayList ) const
           {
             if (APP_DEBUG > 0)
                  printf ("Inside of InArray::buildListOfArrayOperands(list<BaseArray*> & arrayList) \n");
           }

        void setupIndirectAddressingSupport() const APP_RESTRICT_MACRO
           { Array_Descriptor.setupIndirectAddressingSupport(); }

        T expand(int i) const APP_RESTRICT_MACRO;
        T indirect_expand_1D(int i) const APP_RESTRICT_MACRO;
#if (MAX_ARRAY_DIMENSION >= 2)
        T expand(int i, int j) const APP_RESTRICT_MACRO;
        T indirect_expand_2D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        T expand(int i, int j, int k) const APP_RESTRICT_MACRO;
        T indirect_expand_3D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        T expand(int i, int j, int k, int l) const APP_RESTRICT_MACRO;
        T indirect_expand_4D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        T expand(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO;
        T indirect_expand_5D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        T expand(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO;
        T indirect_expand_6D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        T expand(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO;
        T indirect_expand_7D(int i) const APP_RESTRICT_MACRO;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        T expand(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO;
        T indirect_expand_8D(int i) const APP_RESTRICT_MACRO;
#endif

        const Array_Domain_Type &shape() const APP_RESTRICT_MACRO;
        const Array_Domain_Type *shapePtr() const APP_RESTRICT_MACRO;

public:
     // To avoid access to relavanet data through a pointer which makes the expression 
     // template implementation less efficient we put all the pointers and the traditiona
     // descriptor data into the Array_Descriptor which has been templated to handle
     // the different types of data (pointers of different types).
        TArray_Descriptor_Type Array_Descriptor;
   };

// endif for "if defined(USE_EXPRESSION_TEMPLATES) && defined(DOUBLEARRAY)" above
#endif


// We use this as a mechanism to define the base class differently when we use expression templates
#if defined(USE_EXPRESSION_TEMPLATES)
#define EXPRESSION_TEMPLATES_DERIVED_int_CLASS_SUPPORT : public BaseArray , public InArray<int>
#else
// We modify A++/P++ to have a common base class for all array objects - this permits
// many additional design features such as a mechanism for forming references to array 
// object independent of the type of the array.
// #define EXPRESSION_TEMPLATES_DERIVED_int_CLASS_SUPPORT
#undef EXPRESSION_TEMPLATES_DERIVED_int_CLASS_SUPPORT
#define EXPRESSION_TEMPLATES_DERIVED_int_CLASS_SUPPORT : public BaseArray
#endif


// #################################################################
// ######  Definition of the principal object within A++/P++  ######
// #################################################################

class intArray EXPRESSION_TEMPLATES_DERIVED_int_CLASS_SUPPORT
   {
  // This is the pricipal object within A++/P++.
  // This is the declaration of the array object.
  // It includes a declaration of a descriptor (as member data, which in turn 
  // includes a domain objects (as member data)).  Most of this class is a description
  // of the public interface and is documented in the A++/P++ manual. But (of course) this
  // also includes the private interface (not documented in the manual). Many of the
  // elements of the private interface are declared public to simplify teh design of the
  // object and facilitate its use in contexts that it was not explicitly designed for.
  // Thus we don't make it hard for the uer to get at the raw internal data.  If we made it 
  // difficult we might argue that we preserve incapsulation - but it is not a heard work 
  // around for a user to get at this data and so in the end the internal data is not 
  // really well encapsulated anyway.

  // Note that the BaseArray has a "this" pointer and so there 
  // is an offset for all the data members of this (array) class.

     friend class Where_Statement_Support;
     friend class Array_Statement_Type;
     friend class Expression_Tree_Node_Type;
 
     friend class Operand_Storage;
     friend class intArray_Operand_Storage;

#if defined(USE_EXPRESSION_TEMPLATES)
     friend class InArray<int>;
#endif

     friend class Internal_Partitioning_Type;
     friend class Partitioning_Type;

#if defined(SERIAL_APP) && !defined(PPP)
     friend intArray & transpose ( const intArray & X );
#endif

#ifndef DOUBLEARRAY
     friend class doubleArray;
#if defined(SERIAL_APP)
     friend class doubleSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class doubleArray;
#endif
#if defined(PPP)
     friend class doubleSerialArray;
#endif
#endif

#ifndef FLOATARRAY
     friend class floatArray;
#if defined(SERIAL_APP)
     friend class floatSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class floatArray;
#endif
#if defined(PPP)
     friend class floatSerialArray;
#endif
#endif

#ifndef INTARRAY
     friend class intArray;
#if defined(SERIAL_APP)
     friend class intSerialArray;
#endif
#else
#if defined(SERIAL_APP) && !defined(PPP)
     friend class intArray;
#endif
#if defined(PPP)
     friend class intSerialArray;
#endif
#endif

#if 1
// GNU g++ requires these friend declarations
// Forward class declarations
friend class doubleArray_Function_Steal_Data;
friend class doubleArray_Function_0;
friend class doubleArray_Function_1;
friend class doubleArray_Function_2;
friend class doubleArray_Function_3;
friend class doubleArray_Function_4;
friend class doubleArray_Function_5;
friend class doubleArray_Function_6;
friend class doubleArray_Function_7;
friend class doubleArray_Function_8;
friend class doubleArray_Function_9;
friend class doubleArray_Function_10;
friend class doubleArray_Function_11;
friend class doubleArray_Function_12;
friend class doubleArray_Function_13;
friend class doubleArray_Function_14;
friend class doubleArray_Function_15;
friend class doubleArray_Function_16;

friend class doubleArray_Aggregate_Operator;

// Forward class declarations
friend class floatArray_Function_Steal_Data;
friend class floatArray_Function_0;
friend class floatArray_Function_1;
friend class floatArray_Function_2;
friend class floatArray_Function_3;
friend class floatArray_Function_4;
friend class floatArray_Function_5;
friend class floatArray_Function_6;
friend class floatArray_Function_7;
friend class floatArray_Function_8;
friend class floatArray_Function_9;
friend class floatArray_Function_10;
friend class floatArray_Function_11;
friend class floatArray_Function_12;
friend class floatArray_Function_13;
friend class floatArray_Function_14;
friend class floatArray_Function_15;
friend class floatArray_Function_16;

friend class floatArray_Aggregate_Operator;

// Forward class declarations
friend class intArray_Function_Steal_Data;
friend class intArray_Function_0;
friend class intArray_Function_1;
friend class intArray_Function_2;
friend class intArray_Function_3;
friend class intArray_Function_4;
friend class intArray_Function_5;
friend class intArray_Function_6;
friend class intArray_Function_7;
friend class intArray_Function_8;
friend class intArray_Function_9;
friend class intArray_Function_10;
friend class intArray_Function_11;
friend class intArray_Function_12;
friend class intArray_Function_13;
friend class intArray_Function_14;
friend class intArray_Function_15;
friend class intArray_Function_16;

friend class intArray_Aggregate_Operator;

#endif

     private:
     // Constants for operator identification!
        static const int Plus;
        static const int Minus;
        static const int Times;
        static const int Divided_By;
        static const int Modulo;
        static const int cos_Function;
        static const int sin_Function;
        static const int tan_Function;
        static const int acos_Function;
        static const int asin_Function;
        static const int atan_Function;
        static const int cosh_Function;
        static const int sinh_Function;
        static const int tanh_Function;
        static const int acosh_Function;
        static const int asinh_Function;
        static const int atanh_Function;
        static const int log_Function;
        static const int log10_Function;
        static const int exp_Function;
        static const int sqrt_Function;
        static const int fabs_Function;
        static const int abs_Function;
        static const int ceil_Function;
        static const int floor_Function;
        static const int atan2_Function;
        static const int Unary_Minus;
        static const int fmod_Function;
        static const int mod_Function;
        static const int pow_Function;
        static const int sign_Function;
        static const int sum_Function;
        static const int min_Function;
        static const int max_Function;
        static const int Not;
        static const int LT;
        static const int GT;
        static const int LTEQ;
        static const int GTEQ;
        static const int EQ;
        static const int NOT_EQ;
        static const int AND;
        static const int OR;
        static const int Scalar_Plus;
        static const int Scalar_Minus;
        static const int Scalar_Times;
        static const int Scalar_Divided_By;
        static const int Scalar_Modulo;
        static const int Scalar_cos_Function;
        static const int Scalar_sin_Function;
        static const int Scalar_tan_Function;
        static const int Scalar_acos_Function;
        static const int Scalar_asin_Function;
        static const int Scalar_atan_Function;
        static const int Scalar_cosh_Function;
        static const int Scalar_sinh_Function;
        static const int Scalar_tanh_Function;
        static const int Scalar_acosh_Function;
        static const int Scalar_asinh_Function;
        static const int Scalar_atanh_Function;
        static const int Scalar_log_Function;
        static const int Scalar_log10_Function;
        static const int Scalar_exp_Function;
        static const int Scalar_sqrt_Function;
        static const int Scalar_fabs_Function;
        static const int Scalar_abs_Function;
        static const int Scalar_ceil_Function;
        static const int Scalar_floor_Function;
        static const int Scalar_atan2_Function;
        static const int Scalar_Unary_Minus;
        static const int Scalar_fmod_Function;
        static const int Scalar_mod_Function;
        static const int Scalar_pow_Function;
        static const int Scalar_sign_Function;
        static const int Scalar_sum_Function;
        static const int Scalar_min_Function;
        static const int Scalar_max_Function;
        static const int Scalar_Not;
        static const int Scalar_LT;
        static const int Scalar_GT;
        static const int Scalar_LTEQ;
        static const int Scalar_GTEQ;
        static const int Scalar_EQ;
        static const int Scalar_NOT_EQ;
        static const int Scalar_AND;
        static const int Scalar_OR;

     // These are considered as terminating a statement!
        static const int Assignment;
        static const int replace_Function;
        static const int Plus_Equals;
        static const int Minus_Equals;
        static const int Times_Equals;
        static const int Divided_By_Equals;
        static const int Modulo_Equals;
        static const int Scalar_Assignment;
        static const int Scalar_replace_Function;
        static const int Scalar_Plus_Equals;
        static const int Scalar_Minus_Equals;
        static const int Scalar_Times_Equals;
        static const int Scalar_Divided_By_Equals;
        static const int Scalar_Modulo_Equals;
        static const int indexMap_Function;
        static const int view_Function;
        static const int display_Function;

     // Bitwise operators
        static const int BitwiseComplement;
        static const int BitwiseAND;
        static const int BitwiseOR;
        static const int BitwiseXOR;
        static const int BitwiseLShift;
        static const int BitwiseRShift;
        static const int Scalar_BitwiseAND;
        static const int Scalar_BitwiseOR;
        static const int Scalar_BitwiseXOR;
        static const int Scalar_BitwiseLShift;
        static const int Scalar_BitwiseRShift;
        static const int BitwiseAND_Equals;
        static const int BitwiseOR_Equals;
        static const int BitwiseXOR_Equals;
        static const int Scalar_BitwiseAND_Equals;
        static const int Scalar_BitwiseOR_Equals;
        static const int Scalar_BitwiseXOR_Equals;

     // Conversion operators
        static const int convertTo_intArrayFunction;
        static const int convertTo_floatArrayFunction;
        static const int convertTo_doubleArrayFunction;

     public:
        static bool PREINITIALIZE_OBJECT_IN_CONSTRUCTOR;
        static int      PREINITIALIZE_VALUE;
        static bool USE_DESCRIPTOR_CACHING;

     public:
     // Every array object contains an array descriptor (a special case is the use of
     // expression templates where the array descriptor is stored as a member in a base
     // class of the array object).
#if !defined(USE_EXPRESSION_TEMPLATES)
     // If Expression templates are in use then the array_descriptor is 
     // stored in the Indexable base class!
     // Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> Array_Descriptor;
        intArray_Descriptor_Type Array_Descriptor;
#endif

     // #################################################################
     // Access functions for array data (descriptors, domains, and data)
     // #################################################################

     // Access functions to private data to permit use with other languages.
     // Function is inlined for use with the A++ preprocessor
        inline int* APP_RESTRICT_MACRO getDataPointer () const;
#if defined(PPP)
        intSerialArray*  getSerialArrayPointer () const;

     // Used for parallel support of "where statements"
        intSerialArray** getSerialArrayPointerLoc () const;
#endif

     // Access function for Descriptor
     // Array_Descriptor_Type* getDescriptorPointer () const;
     // Array_Descriptor_Type* getDomainPointer () const;
     // Array_Domain_Type* getDomainPointer () const;

#if 0
     // This function does not appear to be used anywhere!
     // int getDataOffset ( int Axis ) const;
        inline int getDataOffset ( int Axis ) const
             { 
#if defined(PPP)
            // P++ calls the A++ getDataOffset function
               //return getSerialArrayDescriptor()->getDataOffset(Axis);
               return getSerialArrayDescriptor().Array_Domain.Base[Axis];
#else
               return Array_Descriptor.Array_Domain.Base[Axis];
#endif
             };
#endif

     // Access function for Descriptor (if we make this return a const & then we can only call const member functions using it)
        inline const intArray_Descriptor_Type & getDescriptor () const
             { return Array_Descriptor; };

     // Access function for Descriptor
        inline const intArray_Descriptor_Type* getDescriptorPointer () const
             { return &Array_Descriptor; };

        inline const Array_Domain_Type & getDomain () const
           { return Array_Descriptor.Array_Domain; };

     // inline const Array_Domain_Type* getDomainPointer () const
        inline const Array_Domain_Type* getDomainPointer () const
           { return &Array_Descriptor.Array_Domain; };

#if defined(PPP)
     // Access functions for serial data
        intSerialArray_Descriptor_Type &
	   getSerialArrayDescriptor() const
	      { return Array_Descriptor.SerialArray->Array_Descriptor; };

        SerialArray_Domain_Type& getSerialDomain() const 
	   { return Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain; };
#endif

#if !defined(PPP)
#if defined(USE_EXPRESSION_TEMPLATES)
        void setupIndirectAddressingSupport() const
           { Array_Descriptor.setupIndirectAddressingSupport(); }

#if 0
        virtual intArray* getintArrayPointer() const
           {
          // printf ("Calling doubleArray* getintArrayPointer() \n");
             return (intArray*) this;
           }
#endif

     // Scalar Indexing functions made for expression templates.
        int  scalar_indexing_operator(int i) const 
           { 
          // We needed a scalar indexing operator which could be used within
          // the objects using indirect addressing or not indirect addressing.
          // This function is inlined into the operator()(int) below!
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.offset(i)]; 
           }
        int  operator()(int i) const 
           { 
             APP_ASSERT (Array_Descriptor.usesIndirectAddressing() == FALSE);
          // printf ("CONST Array_Descriptor.offset(%d) = %d \n",i,Array_Descriptor.offset(i));
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return scalar_indexing_operator(i);
           }
        int &operator()(int i)       
           { 
             APP_ASSERT (Array_Descriptor.usesIndirectAddressing() == FALSE);
          // printf ("NON-CONST Array_Descriptor.offset(%d) = %d \n",i,Array_Descriptor.offset(i));
          // return Array_View_Pointer0[Array_Descriptor.offset(i)]; 
             return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.offset(i)]; 
           }
        int computeSubscript ( int axis, int i ) const
           { return Array_Descriptor.computeSubscript(axis,i); }

        int indirectOffset_1D ( int i ) const
           { return Array_Descriptor.indirectOffset_1D(i); }

#if (MAX_ARRAY_DIMENSION >= 2)
        int  operator()(int i, int j) const 
           { return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.offset(i, j)]; }
        int &operator()(int i, int j)       
           { return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.offset(i, j)]; }
        int indirectOffset_2D ( int i ) const
           { return Array_Descriptor.indirectOffset_2D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        int  operator()(int i, int j, int k) const 
           { return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.offset(i, j, k)]; }
        int &operator()(int i, int j, int k)       
           { return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.offset(i, j, k)]; }
        int indirectOffset_3D ( int i ) const
           { return Array_Descriptor.indirectOffset_3D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        int  operator()(int i, int j, int k, int l) const 
           { return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.offset(i, j, k, l)]; }
        int &operator()(int i, int j, int k, int l)       
           { return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.offset(i, j, k, l)]; }
        int indirectOffset_4D ( int i ) const
           { return Array_Descriptor.indirectOffset_4D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        int  operator()(int i, int j, int k, int l, int m) const 
           { return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.offset(i, j, k, l, m)]; }
        int &operator()(int i, int j, int k, int l, int m)
           { return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.offset(i, j, k, l, m)]; }
        int indirectOffset_5D ( int i ) const
           { return Array_Descriptor.indirectOffset_5D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        int  operator()(int i, int j, int k, int l, int m, int n) const 
           { 
          // printf ("CONST Array_Descriptor.offset(%d,%d,%d,%d,%d,%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset(i, j, k, l, m, n));
             return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.offset(i, j, k, l, m, n)]; 
           }
        int &operator()(int i, int j, int k, int l, int m, int n)
           { 
          // printf ("NON-CONST Array_Descriptor.offset(%d,%d,%d,%d,%d,%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset(i, j, k, l, m, n));
             return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.offset(i, j, k, l, m, n)]; 
           }
        int indirectOffset_6D ( int i ) const
           { return Array_Descriptor.indirectOffset_6D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        int  operator()(int i, int j, int k, int l, int m, int n, int o) const 
           { return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.offset(i, j, k, l, m, n, o)]; }
        int &operator()(int i, int j, int k, int l, int m, int n, int o)
           { return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.offset(i, j, k, l, m, n, o)]; }
        int indirectOffset_7D ( int i ) const
           { return Array_Descriptor.indirectOffset_7D(i); }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        int  operator()(int i, int j, int k, int l, int m, int n, int o, int p) const 
           { return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.offset(i, j, k, l, m, n, o, p)]; }
        int &operator()(int i, int j, int k, int l, int m, int n, int o, int p)       
           { return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.offset(i, j, k, l, m, n, o, p)]; }
        int indirectOffset_8D ( int i ) const
           { return Array_Descriptor.indirectOffset_8D(i); }
#endif

     // Already declared!
     // bool usesIndirectAddressing()const  { return Array_Descriptor.usesIndirectAddressing(); }
     // inline bool usesIndirectAddressing() const { return FALSE; }
        inline bool containsArrayID ( int lhs_id ) const { return FALSE; }
        int internalNumberOfDimensions() const
           { return Array_Descriptor.internalNumberOfDimensions(); }

// end of defined(USE_EXPRESSION_TEMPLATES)
#endif

// end of !defined(PPP)
#endif

#if !defined(USE_DEFERED_EXPRESSION_EVALUATION)
     // This supports the defered evaluation (specifically it supports the means by which
     // the lifetimes of variables are extended by the defered evaluation)!
        Operand_Storage *Array_Storage;
#endif

     // Data required for the "new" and "delete" operators!
        static intArray *Current_Link;
        intArray *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

     public:
     // Control for display  (used in display member function)
        static const int DECIMAL_DISPLAY_FORMAT;
        static const int EXPONENTIAL_DISPLAY_FORMAT;
        static const int SMART_DISPLAY_FORMAT;
        static int DISPLAY_FORMAT;

     private:
     // Reference Counting for A++ array objects -- allows user to use
     // multiple pointers to the same A++ array object.
     // *************************************************
     // Reference counting data and functions
        int referenceCount;

     public:
       // Use of reference count access is made available for debugging
          inline int getReferenceCount() const
             { return referenceCount; }

       // Support functions for reference counting (inlined for performance)
          inline void incrementReferenceCount() const
             { ((intArray*) this)->referenceCount++; }
          inline void decrementReferenceCount() const
             { ((intArray*) this)->referenceCount--; }

          inline void resetRawDataReferenceCount() const 
             { 
            // When we steal the data and reuse it from a temporary we have to delete it
            // which decrements the reference count to -1 (typically) and then we copy the
            // data pointers.  But we have to reset the reference cout to ZERO as well.
            // This function allows that operation to be insulated from the details of how the
            // reference count is implemented.  It is used in the A++ operator= ( arrayobject )
#if defined(PPP)
               //APP_ASSERT (getSerialArrayDescriptor() != NULL);
	       // ... (8/7/97,kdb) don't use referenceCount for descriptor 
               //getSerialArrayDescriptor().referenceCount = 0;
            // Array_Descriptor.SerialArray->referenceCount = 0;
               Array_Descriptor.SerialArray->resetReferenceCount();
#else
            // Array_Descriptor_Type::Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // APP_Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // Array_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID] = 0;
               Array_Descriptor.resetRawDataReferenceCount();
#endif
             }

          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return intArray_Descriptor_Type::getReferenceCountBase(); 
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
            // return 0;
               return intArray_Descriptor_Type::getRawDataReferenceCountBase(); 
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
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               return Array_Descriptor.SerialArray->getReferenceCount(); 
#else
               return Array_Descriptor.getRawDataReferenceCount();
#endif
             }

          inline void incrementRawDataReferenceCount() const 
             { 
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               Array_Descriptor.SerialArray->incrementReferenceCount(); 
#else
               Array_Descriptor.incrementRawDataReferenceCount();
#endif
             }

          inline void decrementRawDataReferenceCount() const 
             { 
               APP_ASSERT (getRawDataReferenceCount() >= 0);
#if defined(PPP)
               APP_ASSERT (Array_Descriptor.SerialArray != NULL);
               Array_Descriptor.SerialArray->decrementReferenceCount(); 
#else
               Array_Descriptor.decrementRawDataReferenceCount();
#endif
             }

       // This function is useful for tracking down memory reference problems
          intArray & displayReferenceCounts ( const char* label = "" ) const;

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

#if 0
          inline void ReferenceCountedDelete ()
             {
            // This is a dangerous sort of function since it deletes the class which this
            // function is a member of!.  As long as we don't do anything after calling
            // delete the function just returns and seem to work fine.
               printf ("ERROR: intArray::ReferenceCountedDelete called! \n");
               exit(1);
   
            // I'm forced to INCLUDE this code because A++ must have a 
            // consistant interface with other classes in Overture++.
            // The problem that made this sort of statement was another bug
            // that was fixed in the reshape function.
               delete this;

            // if (--referenceCount < 0)
            //      delete this;
             }
#endif
     // *************************************************

     private:
     // Because inlined functions can't contain loops we seperate 
     // out this function which is used in the new operator
        static void New_Function_Loop ();

     public:

     // Used to free memory-in-use internally in intArray objects!
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();
        static int numberOfInternalArrays();

#if defined(INLINE_FUNCTIONS)
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
     // DQ (1/9/2007): Commented out the defined "operator delete" since it was a problem for the GNU g++ 3.4.x compiler.
#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )
        void operator delete (void* Pointer, size_t Size);
#endif

     // Default Destructor and Constructor
        virtual ~intArray ();
        intArray ();

     // Constructors

     // Seperate constructors required to avoid ambiguous call errors in call resolution 
     // between the different types

     // ======================================================
        intArray ( int i );
#if (MAX_ARRAY_DIMENSION >= 2)
        intArray ( int i , int j );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        intArray ( int i , int j , int k );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        intArray ( int i , int j , int k , int l );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        intArray ( int i , int j , int k , int l , int m );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        intArray ( int i , int j , int k , int l , int m, int n );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        intArray ( int i , int j , int k , int l , int m, int n, int o );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        intArray ( int i , int j , int k , int l , int m, int n, int o, int p);
#endif

     // ======================================================

        intArray ( ARGUMENT_LIST_MACRO_INTEGER, bool Force_Memory_Allocation  );

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if defined(APP) || defined(PPP)

     // ======================================================
        intArray ( int i , const Partitioning_Type & Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
        intArray ( int i , int j , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        intArray ( int i , int j , int k , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        intArray ( int i , int j , int k , int l , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        intArray ( int i , int j , int k , int l , int m, const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        intArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        intArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        intArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION >= 2)
        intArray ( int i , int j , const Partitioning_Type & Partition,
	            bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        intArray ( int i , int j , int k , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        intArray ( int i , int j , int k , int l , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        intArray ( int i , int j , int k , int l , int m, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        intArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        intArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        intArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition , 
		    bool Force_Memory_Allocation );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// defined(APP) || defined(PPP)
#endif

     private:
     // Called inside of each of the Initialize_Array functions (below)
        void preInitializeArray ();

     // Called inside of all array object constructors
        void initializeArray ();

#if defined(APP) || defined(PPP)
     // Called inside of all array object constructors
        void initializeArray ( const Partitioning_Type & Partition );
        void initializeArray ( const Internal_Partitioning_Type & Partition );
#endif

     public:
     // We have to list all the possible variations to avoid a scalar
     // being promoted to a Range object (which has base == bound)
     // The solution is to provide all the permutations of the operator()
     // and promote scalars to a range (defaultBase:defaultBase+Scalar-1)
     // intArray ( const Range & I_Base_Bound , 
     //             const Range & J_Base_Bound = APP_Unit_Range , 
     //             const Range & K_Base_Bound = APP_Unit_Range , 
     //             const Range & L_Base_Bound = APP_Unit_Range );
        intArray ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // These are for internal use and allow initialization from an array of data
        intArray ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

#if defined(PPP) || defined (APP)
     // Added to support resize function used with an existing partitioning object
        intArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List,
                    const Internal_Partitioning_Type & partition );
#endif

        intArray ( int Number_Of_Valid_Dimensions, const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // This constructor avoids an ambiguous call to the copy constructor 
     // (since a intArray could be built from a Range and an int is the same as a bool)
#if (MAX_ARRAY_DIMENSION >= 2)
        intArray ( const Range & I_Base_Bound , int j );
#endif

#if defined(PPP)
     // Construct a intArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // Additionally for P++ the local partition must be provided.  Due to restrictions on the
     // type of partitioning supported (i.e. FORTRAN D Partitioning: BLOCK PARTI partitioning)
     // a specific partitoning might not conform to what P++ can presently handle.  The result
     // is an error.  So it is not a good idea to use this function in the parallel environment.
     // intArray::intArray ( int* Data_Pointer , int i , const Range & Range_I , 
     //                                           int j , const Range & Range_J ,
     //                                           int k , const Range & Range_K ,
     //                                           int l , const Range & Range_L );
     // intArray::intArray ( const int* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        intArray ( const int* Data_Pointer , 
                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );

#if 0
     // We can't use this member function and remain dimension independent (so it is removed for now)
        intArray ( int* Data_Pointer , const Range & Span_I , const Range & Range_I , 
                                                  const Range & Span_J , const Range & Range_J ,
                                                  const Range & Span_K , const Range & Range_K ,
                                                  const Range & Span_L , const Range & Range_L );
#endif
#else
     // Construct a intArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // intArray ( int* Data_Pointer , int i , int j = 1 , int k = 1 , int l = 1 );
        intArray ( const int* Data_Pointer , ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );

     // intArray ( int* Data_Pointer , const Range & Span_I , 
     //             const Range & Span_J = APP_Unit_Range , 
     //             const Range & Span_K = APP_Unit_Range , 
     //             const Range & Span_L = APP_Unit_Range );
        intArray ( const int* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
#endif

     // Constructors to support deffered evaluation of array expressions! (these should be private, I think)
     // I'm not certain these are still used!
        intArray ( int Operation_Code , intArray *Lhs_Pointer );
        intArray ( int Operation_Code , intArray *Lhs_Pointer , intArray *Rhs_Pointer );

     // Copy Constructors
        intArray ( const intArray & X , int Type_Of_Copy = DEEPCOPY );

#if defined(APP) || defined(SERIAL_APP)
     // Constructor for initialization from a string (We built the AppString type to avoid autopromotion problems)
     // intArray ( const char* );
        intArray ( const AppString & X );
#endif

     // Evaluation operator! Evaluates an expression and asigns local scope to its result
     // useful when handing expresions to functions that pass by reference.
     // There is no peformance penalty associated with this function since no memory
     // is copied! 
        friend intArray evaluate ( const intArray & X );

     // reference operator (builds a view) (an M++ function)
        intArray & reference ( const intArray & X );
        intArray & breakReference ();

        bool Is_Built_By_Defered_Evaluation() const;

     // void display ( const char *Label = "" ) const;
     // intArray & display ( const char *Label = "" );
        intArray & view    ( const char *Label = "" ) const;
        intArray & display ( const char *Label = "" ) const;
        intArray & globalDisplay ( const char *Label = "" ) const;
#if defined(USE_EXPRESSION_TEMPLATES)
        void MDI_Display() const;
#endif

     // Provided to permit easy access from dbx debugger
#ifdef __clang__
     // Clang requires default arguments of friend functions to appear in the definitions.
        friend void APP_view    ( const intArray & X , const char *Label );
        friend void APP_display ( const intArray & X , const char *Label );
#else
     // DQ (7/23/2020): This might be dependent on EDG more than GNU version 10.
#if ((__GNUC__ == 10) && (__GNUC_MINOR__ >= 1) && (__GNUC_PATCHLEVEL__ >= 0))
     // GNU 10 requires default arguments of friend functions to appear in the definitions.
        friend void APP_view    ( const intArray & X , const char *Label );
        friend void APP_display ( const intArray & X , const char *Label );
#else
        friend void APP_view    ( const intArray & X , const char *Label = "" );
        friend void APP_display ( const intArray & X , const char *Label = "" );
#endif
#endif
     // This is a member function defined in the BaseArray class
        int Array_ID () const;

        inline void setTemporary( bool x ) const { Array_Descriptor.setTemporary(x); }
        inline bool isTemporary() const { return Array_Descriptor.isTemporary(); }
        inline bool isNullArray() const { return Array_Descriptor.isNullArray(); }
        inline bool isView()      const { return Array_Descriptor.isView(); }
        inline bool isContiguousData() const { return Array_Descriptor.isContiguousData(); }

     // Equivalent to operator= (int x)
        intArray & fill ( int x );

     // Fills the array with different values given a base and stride
        intArray & seqAdd ( int Base = (int) 0 , int Stride = (int) 1 );  // Start at zero and count by 1

     // This function tests for many sorts of errors that could crop up in
     // the internal usage of the intArray objects -- it is a debugging tool!
        void Test_Consistency ( const char *Label = "" ) const;
        inline bool Binary_Conformable  ( const intArray & X ) const;

     // Part of implementation for diagnostics -- permits more aggressive a 
     // destructive testing than Test_Consistency(). (private interface)
        void testArray();

     // Test for confomability regardless of the input!
     // Templates would simplify this since the template need not be "bound"
     // See Lippman for more details on templates (bound and unbound)!

     // Public interface
        bool isConformable ( const doubleArray & X ) const;
        bool isConformable ( const floatArray & X ) const;
        bool isConformable ( const intArray & X ) const;

     // For internal use (though it is marked public at the moment)
        void Test_Conformability ( const doubleArray & X ) const;
        void Test_Conformability ( const floatArray & X ) const;
        void Test_Conformability ( const intArray & X ) const;

#if defined(INTARRAY)
     // intArray ( const Index & X );
     //  ... (7/20/98,kdb) function below is commented out in src code
     //   so remove here to avoid linking problems ...
     // intArray ( const Internal_Index & X );
        intArray & operator=( const Internal_Index & X );
        intArray & indexMap ();
#endif

     // Range checking function for scalar indexing (this has been moved to thr descriptor class)
     // void Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const;

#if !defined(USE_EXPRESSION_TEMPLATES)
        bool usesIndirectAddressing()const  { return Array_Descriptor.usesIndirectAddressing(); }

     // Indexing operators (scalar)
     // To get constness correct we should move to using this construction
     // The indexing of a const array should return a const ref and 
     // the indexing of a nonconst array should return a nonconst ref.
     // inline const int & operator() ( int i ) const;
     // inline       int & operator() ( int i );
        inline int & operator() ( int i ) const;

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const int & operator() ( int i , int j ) const;
        inline       int & operator() ( int i , int j );
#else
        inline int & operator() ( int i , int j ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const int & operator() ( int i , int j , int k ) const;
        inline       int & operator() ( int i , int j , int k );
#else
        inline int & operator() ( int i , int j , int k ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const int & operator() ( int i , int j , int k , int l ) const;
        inline       int & operator() ( int i , int j , int k , int l );
#else
        inline int & operator() ( int i , int j , int k , int l ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const int & operator() ( int i , int j , int k , int l, int m ) const;
        inline       int & operator() ( int i , int j , int k , int l, int m );
#else
        inline int & operator() ( int i , int j , int k , int l, int m ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const int & operator() ( int i , int j , int k , int l, int m , int n ) const;
        inline       int & operator() ( int i , int j , int k , int l, int m , int n );
#else
        inline int & operator() ( int i , int j , int k , int l, int m , int n ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const int & operator() ( int i , int j , int k , int l, int m , int n , int o ) const;
        inline       int & operator() ( int i , int j , int k , int l, int m , int n , int o );
#else
        inline int & operator() ( int i , int j , int k , int l, int m , int n , int o ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        inline const int & operator() ( int i , int j , int k , int l, int m , int n , int o , int p ) const;
        inline       int & operator() ( int i , int j , int k , int l, int m , int n , int o , int p );
#else
        inline int & operator() ( int i , int j , int k , int l, int m , int n , int o , int p ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

// End of if !defined(USE_EXPRESSION_TEMPLATES)
#endif

        void Scalar_Indexing_For_intArray_With_Message_Passing (
             int Address_Subscript, bool Off_Processor_With_Ghost_Boundaries,
             bool Off_Processor_Without_Ghost_Boundaries, int & Array_Index_For_int_Variable) const;

     /* Indexing operators (using Index objects)
     // Indexing operators have to have local scope (since we don't want them to have
     // a permanent lifetime).  By having local scope they there lifetime is
     // controled by the compiler (this means they hang around longer than temporaries
     // and have the same scope as local variable).  This is NOT a problem as is would be
     // for temporaries since views returned by the indexing operators have no array data
     // associated with them (directly -- they peek into another array objects data).
     // Thus these indexing operators return by value not by reference.  
     // The importance of this is clear if we imagine a function taking a view by reference
     // and then managing its lifetime -- after its first use in an array expression the 
     // view would have been deleted and then susequent use of it would be an error 
     // (as in segment fault)!
     */

     // Forces one dimensional interpretation of multidimensional intArray!
        intArray operator() () const;

     // This operator() takes an array of pointers to Internal_Index objects (length MAX_ARRAY_DIMENSION)
        intArray operator() ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;
        intArray operator() ( int Number_Of_Valid_Dimensions, const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;

     // Indexing operators for use with Index objects
     //   intArray operator() 
     //( ARGUMENT_LIST_MACRO_CONST_REF_INTERNAL_INDEX_WITH_PREINITIALIZATION )
     //const ;

#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intArray operator() ( const Internal_Index & I ) const;
              intArray operator() ( const Internal_Index & I );
#else
        intArray operator() ( const Internal_Index & I ) const;
#endif

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
        intArray operator() ( const Internal_Index & I, const Internal_Index & J );
#else
        intArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K );
#else
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L );
#else
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M );
#else
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N );
#else
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O );
#else
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P );
#else
        intArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if !defined(PPP)
     // ... use this to make scalar indexing subscripts consistent ...
        void Fixup_User_Base 
	 (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List,
	  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List);
#endif

     // Simplifies getting a pointer to a view which is used internally in A++/P++
     // The Index_Array is length MAX_ARRAY_DIMENSION == 4
     // static intArray* Build_Pointer_To_View_Of_Array ( const intArray & X , Internal_Index *Index_Array );
        static intArray* Build_Pointer_To_View_Of_Array ( const intArray & X ,
                                                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Indexing operators for use with Index objects
        intArray operator() ( const Internal_Indirect_Addressing_Index & I ) const;
#if (MAX_ARRAY_DIMENSION >= 2)
        intArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        intArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        intArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        intArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        intArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        intArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N,
                               const Internal_Indirect_Addressing_Index & O ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        intArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N
                               const Internal_Indirect_Addressing_Index & O, const Internal_Indirect_Addressing_Index & P ) const;
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

     /* All other operators which can return actual array data are implemented as
     // return by reference so that their data can be manipulate (so specifically
     // so that the compiler will not manipulate the lifetime and as result
     // leave too many array object around (ones with real array data attached and thus ones which
     // would take up lots of space)).  This is a priciple part of the problem
     // with poorly designed array classes (temporaries must be managed to avoid 
     // waisting memory (especially in numerical applications)).
     // The restriction is that while views can be passed by reference into functions
     // expressions can NOT be passes by reference into functions.  For the sake of
     // temporary management (and thus efficient control over the memory useage)
     // we give up the ability to pass temporaries into functions.  The compromise is to 
     // use the "evaluate()" function which will have a non temporary object from the 
     // temporary one.  If you have a better idea please let me know: dquinlan@lanl.gov.
     // So expresions can be passed as parameters to functions taking input by reference
     // it just requires the use of the "evaluate()" function.
     */

     // Unary Operators + and -
        intArray & operator+ () const;
        intArray & operator- () const;

     // Prefix Increment and Decrement Operators ++A and --A
        intArray & operator++ ();
        intArray & operator-- ();

     // Postfix Increment and Decrement Operators A++ and A--  (argument is ZERO see 594 r.13.4.7 Stroustrup)
        intArray & operator++ (int x);
        intArray & operator-- (int x);

#if !defined(USE_EXPRESSION_TEMPLATES)
     // operator+ related functions
        friend intArray & operator+ ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator+ ( int x    , const intArray & Rhs );
        friend intArray & operator+ ( const intArray & Lhs , int x );
        intArray & operator+= ( const intArray & Rhs );
        intArray & operator+= ( int Rhs );

     // operator- related functions
        friend intArray & operator- ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator- ( int x    , const intArray & Rhs );
        friend intArray & operator- ( const intArray & Lhs , int x );
        intArray & operator-= ( const intArray & Rhs );
        intArray & operator-= ( int Rhs );

     // operator* related functions
        friend intArray & operator* ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator* ( int x    , const intArray & Rhs );
        friend intArray & operator* ( const intArray & Lhs , int x );
        intArray & operator*= ( const intArray & Rhs );
        intArray & operator*= ( int Rhs );

     // operator/ related functions
        friend intArray & operator/ ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator/ ( int x    , const intArray & Rhs );
        friend intArray & operator/ ( const intArray & Lhs , int x );
        intArray & operator/= ( const intArray & Rhs );
        intArray & operator/= ( int Rhs );

     // operator% related functions
        friend intArray & operator% ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator% ( int x    , const intArray & Rhs );
        friend intArray & operator% ( const intArray & Lhs , int x );
        intArray & operator%= ( const intArray & Rhs );
        intArray & operator%= ( int Rhs );

#ifdef INTARRAY
     // Unary Operator ~ (bitwise one's complement)
        intArray & operator~ () const;

#if 0
     // DQ (4/9/2005): these are conflicting with operators in g++ (turn them off for now!)
     // operator& related functions
        friend intArray & operator& ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator& ( int x    , const intArray & Rhs );
        friend intArray & operator& ( const intArray & Lhs , int x );
        intArray & operator&= ( const intArray & Rhs );
        intArray & operator&= ( int Rhs );

     // operator| related functions
        friend intArray & operator| ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator| ( int x    , const intArray & Rhs );
        friend intArray & operator| ( const intArray & Lhs , int x );
        intArray & operator|= ( const intArray & Rhs );
        intArray & operator|= ( int Rhs );

     // operator^ related functions
        friend intArray & operator^ ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator^ ( int x    , const intArray & Rhs );
        friend intArray & operator^ ( const intArray & Lhs , int x );
        intArray & operator^= ( const intArray & Rhs );
        intArray & operator^= ( int Rhs );
#endif

     // operator<< related functions
        friend intArray & operator<< ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator<< ( int x    , const intArray & Rhs );
        friend intArray & operator<< ( const intArray & Lhs , int x );

     // operator>> related functions
        friend intArray & operator>> ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator>> ( int x    , const intArray & Rhs );
        friend intArray & operator>> ( const intArray & Lhs , int x );
#endif

     // Internal conversion member functions (used in casting operators)
        intArray    & convertTo_intArray    () const;
        floatArray  & convertTo_floatArray  () const;
        doubleArray & convertTo_doubleArray () const;

        intArray & operator= ( int x );
        intArray & operator= ( const intArray & Rhs );
#else
     // Need these to be defined for the expression template version
        intArray & operator+= ( const intArray & Rhs );
        intArray & operator+= ( int Rhs );

        intArray & operator-= ( const intArray & Rhs );
        intArray & operator-= ( int Rhs );

        intArray & operator*= ( const intArray & Rhs );
        intArray & operator*= ( int Rhs );

        intArray & operator/= ( const intArray & Rhs );
        intArray & operator/= ( int Rhs );

        intArray & operator%= ( const intArray & Rhs );
        intArray & operator%= ( int Rhs );
#endif

     // Access function for static Base and Bound variables in Array_Descriptor class!
        friend void setGlobalBase( int Base );
        friend int getGlobalBase();
        friend char* APP_version();

     // abort function for A++ (defined in domain.h)
     // friend void APP_ABORT();
     // friend void APP_Assertion_Support( char* Source_File_With_Error, unsigned Line_Number_In_File );

     // friend void setGlobalBase( int Base , int Axis );
     // friend int getGlobalBase( int Axis );

     // void setBase( int New_Base_For_All_Axes );
        intArray & setBase( int New_Base_For_All_Axes );
        int getBase() const;
        int getRawBase() const;

     // Access function for static Base and Bound variables (by Axis)!
     // void setBase( int Base , int Axis );
        intArray & setBase( int Base , int Axis );
        intArray & setParallelBase( int Base , int Axis );
        int getBase( int Axis ) const;
        int getStride( int Axis ) const;
        int getBound( int Axis ) const;
        int getLength( int Axis ) const;
        int getRawDataSize( int Axis ) const;
        int getRawBase( int Axis ) const;
        int getRawStride( int Axis ) const;
        int getRawBound( int Axis ) const;
        int getDataBase( int Axis ) const;
        int numberOfDimensions () const;
        Range dimension( int Axis ) const;
        Range getFullRange( int Axis ) const;


#if defined(APP) || defined(PPP)

#if defined(PPP)
     // Added (7/26/2000)
        Internal_Index getLocalMaskIndex  (int i) const { return Array_Descriptor.getLocalMaskIndex(i); }
        Internal_Index getGlobalMaskIndex (int i) const { return Array_Descriptor.getGlobalMaskIndex(i); }

     // Added (12/13/2000)
        int getLeftNumberOfPoints  ( int Axis ) const { return Array_Descriptor.getLeftNumberOfPoints(Axis);  }
        int getRightNumberOfPoints ( int Axis ) const { return Array_Descriptor.getRightNumberOfPoints(Axis); }
#endif

     // P++ specific functions report on the local partition
        int getLocalBase( int Axis ) const;
        int getLocalStride( int Axis ) const;
        int getLocalBound( int Axis ) const;
        int getLocalLength( int Axis ) const;
        int getLocalRawBase( int Axis ) const;
        int getLocalRawStride( int Axis ) const;
        int getLocalRawBound( int Axis ) const;
        Range localDimension( int Axis ) const;
#endif
        Range getLocalFullRange( int Axis ) const;

	// Returns reference to SerialArray if P++ or itself if A++
#if defined(PPP)
        intSerialArray getLocalArray () const;
#elif defined(APP)
        intArray getLocalArray () const;
#endif
	// Returns new SerialArray including ghost cells if P++ array
	// is nonview or view of SerialArray if P++ array is a view 
	// or itself if A++
#if defined(PPP)
        intSerialArray getLocalArrayWithGhostBoundaries () const;
        intSerialArray* getLocalArrayWithGhostBoundariesPointer () const;
// elif defined(APP)
#else
        intArray getLocalArrayWithGhostBoundaries () const;
        intArray* getLocalArrayWithGhostBoundariesPointer () const;
#endif

     // Dimension independent query functions
        bool isSameBase   ( const intArray & X ) const;
        bool isSameBound  ( const intArray & X ) const;
        bool isSameStride ( const intArray & X ) const;
        bool isSameLength ( const intArray & X ) const;
#if defined(APP) || defined(PPP)
        bool isSameGhostBoundaryWidth ( const intArray & X ) const;
        //bool isSameDistribution ( const intArray & X ) const;
        bool isSameDistribution ( const intArray & X ) const;
        bool isSameDistribution ( const floatArray & X ) const;
        bool isSameDistribution ( const doubleArray & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const intArray & X ) const;

     // Member functions for access to the geometry of the A++ object
        int getGeometryBase ( int Axis ) const;
        intArray & setGeometryBase ( int New_Geometry_Base , int Axis );

#if defined(PPP)
     // Is a P++ array partitioned along an axis
     // bool isPartitioned ( int Axis ) const;
#endif

     // total array size! (this function should be depreciated in favor of the getSize() function)
        int elementCount() const;

        int getSize() const;
#if defined(APP) || defined(PPP)
        int getLocalSize() const;
#endif

     // Number of columns and rows usefull for 2D arrays (part of  d e f i n e d  interface)!
        int cols() const;
        int rows() const;

     // Control the ability to resize and array throught assignment
     // This whole process is greatly cleaned up over that of M++!
     // int lock();
     // int unlock();

     // redimensioning member operations
     // intArray & redim ( int i , int j = 1 , int k = 1 , int l = 1 );
        intArray & redim ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // intArray & redim ( const Range & i , 
     //                     const Range & j = APP_Unit_Range , 
     //                     const Range & k = APP_Unit_Range , 
     //                     const Range & l = APP_Unit_Range );
        intArray & redim ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // We need this to avoid an ERROR: Overloading ambiguity between "doubleArray::redim(const Array_Domain_Type&)" 
     // and "doubleArray::redim(const Range&, const Range&, const Range&, const Range&, const Range&, const Range&)" 
     // so we need to implement a redim that takes a single Index object.
        intArray & redim ( const Index & I );
        intArray & redim ( const intArray & X );
        intArray & redim ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intArray & redim ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // The Sun C++ compiler can't handle template declarations in non templated classes
     // template<class T, int Template_Dimension>
     // intArray & redim ( const Array_Descriptor_Type<T,Template_Dimension> & X );
        intArray & redim ( const Array_Domain_Type & X );

     // Changes dimensions of array object with no change in number of elements
     // intArray & reshape ( int i , int j = 1 , int k = 1 , int l = 1 );
        intArray & reshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // intArray & reshape ( const Range & i , 
     //                       const Range & j = APP_Unit_Range , 
     //                       const Range & k = APP_Unit_Range , 
     //                       const Range & l = APP_Unit_Range );
        intArray & reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        intArray & reshape ( const intArray & X );
        intArray & reshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intArray & reshape ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Used for support of weakened conformability assignment in operator= (maybe this should be private or protected)
     // intArray & internalReshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // intArray & internalReshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

	void fix_view_bases (int* View_Bases, int* View_Sizes, int numdims, 
			     const int* old_lengths, const int* Integer_List);
	void fix_view_bases_non_short_span 
	   (int* View_Bases, int* View_Sizes, int numdims, const int* Integer_List);

     // resize function redimensions array and copies exiting data into new array
     // intArray & resize ( int i , int j = 1 , int k = 1 , int l = 1 );
        intArray & resize ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // intArray & resize ( const Range & i , 
     //                      const Range & j = APP_Unit_Range , 
     //                      const Range & k = APP_Unit_Range , 
     //                      const Range & l = APP_Unit_Range );
        intArray & resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        intArray & resize ( const intArray & X );
        intArray & resize ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intArray & resize ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // adopt existing data as an A++ array
     // versions of adopt which are specific to P++
     // To avoid the 1D P++ adopt function from being confused with the
     // 2D A++ adopt function we must provide different member function names.
        intArray & adopt ( const int* Data_Pointer , const Partitioning_Type & Partition ,
                                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        intArray & adopt ( const int* Data_Pointer , const Partitioning_Type & Partition ,
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // intArray & adopt ( const int* Data_Pointer , const Partitioning_Type & Partition ,
     //                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // A++ specific implementation of adopt member functions
     // These A++ specific function are made available to P++ to maintain
     // the identical interface even though under P++ these functions
     // are not implemented.

     // intArray & adopt ( const int* Data_Pointer , 
     //                     int i , int j = 1 , int k = 1 , int l = 1 ); 
     // function with interface for internal use only
        intArray & adopt ( const int* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
        intArray & adopt ( const int* Data_Pointer , 
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intArray & adopt ( const int* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
     // Use the size of an existing A++/P++ array object
        intArray & adopt ( const int* Data_Pointer , const intArray & X );

     // These are used only within the internal implementation of A++/P++
        intArray & adopt ( const int* Data_Pointer , 
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Force a deep copy since deep copy constructors are not alway called
     // even when explicit attempts are made to call them 
        intArray & copy ( const intArray & X , int DeepCopy = DEEPCOPY );

     // Convert indexing of view to be non HPF!
        intArray & useFortranIndexing ( bool Flag );

     // This would effectively be a reference (so call reference instead)!
     // intArray & adopt ( const intArray & X  );

     // Reduction operators!
        friend int min ( const intArray & X );
        friend int max ( const intArray & X );

#if defined(PPP)
     // P++ support for reduction operations
     // By making each of these a friend function we give them 
     // access to the doubleArray::Operation_Type variables
        friend void Reduction_Operation ( int Input_Function, double & x );
        friend void Reduction_Operation ( int Input_Function, float & x );
        friend void Reduction_Operation ( int Input_Function, int & x );
#endif

#if defined(USE_EXPRESSION_TEMPLATES)
     // The non-expression template version returns values by value (not by reference)
        friend intArray min ( const intArray & X , const intArray & Y );
        friend intArray min ( const intArray & X , int y );
        friend intArray min ( int x , const intArray & Y );
        friend intArray min ( const intArray & X , const intArray & Y , const intArray & Z );
        friend intArray min ( int x , const intArray & Y , const intArray & Z );
        friend intArray min ( const intArray & X , int y , const intArray & Z );
        friend intArray min ( const intArray & X , const intArray & Y , int z );

        friend intArray max ( const intArray & X , const intArray & Y );
        friend intArray max ( const intArray & X , int y );
        friend intArray max ( int x , const intArray & Y );
        friend intArray max ( const intArray & X , const intArray & Y , const intArray & Z );
        friend intArray max ( int x , const intArray & Y , const intArray & Z );
        friend intArray max ( const intArray & X , int y , const intArray & Z );
        friend intArray max ( const intArray & X , const intArray & Y , int z );

     // Misc functions
        intArray & replace ( const intArray & X , const intArray & Y );
        intArray & replace ( int x , const intArray & Y );
        intArray & replace ( const intArray & X , int y );

        friend int sum ( const intArray & X );

     // Sum along axis 
        friend intArray sum ( const intArray & X , int Axis );
#else

     // The non-expression template version returns values by reference
        friend intArray & min ( const intArray & X , const intArray & Y );
        friend intArray & min ( const intArray & X , int y );
        friend intArray & min ( int x , const intArray & Y );
        friend intArray & min ( const intArray & X , const intArray & Y , const intArray & Z );
        friend intArray & min ( int x , const intArray & Y , const intArray & Z );
        friend intArray & min ( const intArray & X , int y , const intArray & Z );
        friend intArray & min ( const intArray & X , const intArray & Y , int z );

        friend intArray & max ( const intArray & X , const intArray & Y );
        friend intArray & max ( const intArray & X , int y );
        friend intArray & max ( int x , const intArray & Y );
        friend intArray & max ( const intArray & X , const intArray & Y , const intArray & Z );
        friend intArray & max ( int x , const intArray & Y , const intArray & Z );
        friend intArray & max ( const intArray & X , int y , const intArray & Z );
        friend intArray & max ( const intArray & X , const intArray & Y , int z );

     // Misc functions
        intArray & replace ( const intArray & X , const intArray & Y );
        intArray & replace ( int x , const intArray & Y );
        intArray & replace ( const intArray & X , int y );

        friend int sum ( const intArray & X );
        friend intArray & sum ( const intArray & X , const intArray & Y );
        friend intArray & sum ( const intArray & X , const intArray & Y , const intArray & Z );

     // Sum along axis
        friend intArray & sum ( const intArray & X , int Axis );
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)
#ifndef INTARRAY
        friend intArray & fmod ( const intArray & X , int y );
        friend intArray & fmod ( int x , const intArray & Y );
        friend intArray & fmod ( const intArray & X , const intArray & Y );
#endif
        friend intArray & mod ( const intArray & X , int y );
        friend intArray & mod ( int x , const intArray & Y );
        friend intArray & mod ( const intArray & X , const intArray & Y );

        friend intArray & pow ( const intArray & X , int y );  // returns X**y
        friend intArray & pow ( int x , const intArray & Y );
        friend intArray & pow ( const intArray & X , const intArray & Y );

        friend intArray & sign ( const intArray & Array_Signed_Value , const intArray & Input_Array );
        friend intArray & sign ( int Scalar_Signed_Value             , const intArray & Input_Array );
        friend intArray & sign ( const intArray & Array_Signed_Value , int Input_Value  );

#ifndef INTARRAY
        friend intArray & log   ( const intArray & X );
        friend intArray & log10 ( const intArray & X );
        friend intArray & exp   ( const intArray & X );
        friend intArray & sqrt  ( const intArray & X );
        friend intArray & fabs  ( const intArray & X );
        friend intArray & ceil  ( const intArray & X );
        friend intArray & floor ( const intArray & X );
#endif
        friend intArray & abs   ( const intArray & X );

     // Not fully supported yet (only works for 1D or 2D arrays object)!
        friend intArray & transpose ( const intArray & X );

     // Trig functions!
#ifndef INTARRAY
        friend intArray & cos   ( const intArray & X );
        friend intArray & sin   ( const intArray & X );
        friend intArray & tan   ( const intArray & X );
        friend intArray & acos  ( const intArray & X );
        friend intArray & asin  ( const intArray & X );
        friend intArray & atan  ( const intArray & X );
        friend intArray & cosh  ( const intArray & X );
        friend intArray & sinh  ( const intArray & X );
        friend intArray & tanh  ( const intArray & X );
        friend intArray & acosh ( const intArray & X );
        friend intArray & asinh ( const intArray & X );
        friend intArray & atanh ( const intArray & X );

     // All the different atan2 functions for arrays (ask for by Bill Henshaw)
        friend intArray & atan2 ( const intArray & X , int y );
        friend intArray & atan2 ( int x , const intArray & Y );
        friend intArray & atan2 ( const intArray & X , const intArray & Y );
#endif

     // relational operators 
     // operator< related functions
        friend intArray & operator< ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator< ( int x    , const intArray & Rhs );
        friend intArray & operator< ( const intArray & Lhs , int x );

     // operator<= related functions
        friend intArray & operator<= ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator<= ( int x    , const intArray & Rhs );
        friend intArray & operator<= ( const intArray & Lhs , int x );

     // operator> related functions
        friend intArray & operator> ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator> ( int x    , const intArray & Rhs );
        friend intArray & operator> ( const intArray & Lhs , int x );

     // operator>= related functions
        friend intArray & operator>= ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator>= ( int x    , const intArray & Rhs );
        friend intArray & operator>= ( const intArray & Lhs , int x );

     // operator== related functions
        friend intArray & operator== ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator== ( int x    , const intArray & Rhs );
        friend intArray & operator== ( const intArray & Lhs , int x );

     // operator!= related functions
        friend intArray & operator!= ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator!= ( int x    , const intArray & Rhs );
        friend intArray & operator!= ( const intArray & Lhs , int x );

#if 0
     // DQ (4/9/2005): these are conflicting with operators in g++ (turn them off for now!)
     // operator&& related functions
        friend intArray & operator&& ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator&& ( int x    , const intArray & Rhs );
        friend intArray & operator&& ( const intArray & Lhs , int x );

     // operator|| related functions
        friend intArray & operator|| ( const intArray & Lhs , const intArray & Rhs );
        friend intArray & operator|| ( int x    , const intArray & Rhs );
        friend intArray & operator|| ( const intArray & Lhs , int x );
#endif

#else
     // If we ARE using expression templates then we have to declare some friend functions
     // friend int sum ( const intArray & X );
     // friend intArray sum ( const intArray & X , int axis );

        template<class T, class A>
        friend T sum ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T max ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T min ( const Indexable<T,A> & X );
#endif

     // operator! related functions
        intArray & operator! ();

#if defined(APP) || defined(PPP)
     // Note that P++ assumes that any results returned from these functions are not used
     // effect the parallel control flow (this is due to the SPMD nature of a P++ application).
     // No attempt is made to provide access to parallel info in P++ nor is P++ intended to
     // be a way to write explicitly parallel applications (use explicit message passing for that!).

     // These allow an existing array to be partitioned of the
     // existing array repartitioned acording to another existing array
     // in which case the array is alligned with the input array
        Partitioning_Type getPartition () const;
        Internal_Partitioning_Type* getInternalPartitionPointer () const;
#if defined(USE_PADRE)
        void partition ( PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *Partition );
     // void partition ( const BaseArray   & Example_Array );
#endif
        intArray & partition ( const Internal_Partitioning_Type & Partition );
        intArray & partition ( const Partitioning_Type & Partition );
        intArray & partition ( const doubleArray & Example_Array );
        intArray & partition ( const floatArray & Example_Array );
        intArray & partition ( const intArray & Example_Array );

     // typedef support for member function pointers as parameters for the map diagnostic mechanism
     // We support two different function prototypes
        typedef int (intArray::*intArrayMemberVoidFunctionPointerType) (void) const;
        typedef int (intArray::*intArrayMemberIntegerFunctionPointerType) (int) const;

     // Builds array with base/bound info for each axis and processor
     // calls special purpose communication function

#if defined(PPP)
     // This only makes since in P++
        intSerialArray buildProcessorMap ( intArrayMemberVoidFunctionPointerType X );
        intSerialArray buildProcessorMap ( intArrayMemberIntegerFunctionPointerType X );
        intSerialArray buildProcessorMap ( intArrayMemberIntegerFunctionPointerType X, intArrayMemberIntegerFunctionPointerType Y );
#else
     // Preserve the interface in A++
        intArray buildProcessorMap ( intArrayMemberVoidFunctionPointerType X );
        intArray buildProcessorMap ( intArrayMemberIntegerFunctionPointerType X );
        intArray buildProcessorMap ( intArrayMemberIntegerFunctionPointerType X, intArrayMemberIntegerFunctionPointerType Y );
#endif

        void displayArraySizesPerProcessor (const char* Label = "");
        void displayPartitioning  (const char *Label = "");
        void displayCommunication (const char* Label = "");
        void displayLeftRightNumberOfPoints (const char* Label);

     // functions specific to the manipulation of internal ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
        int getGhostBoundaryWidth ( int Axis ) const;

     // Depreciated function (not documented in manual)
        int getInternalGhostCellWidth ( int Axis ) const;

     // Depreciated function (not documented in manual)
        intArray & setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intArray & setInternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        intArray & setInternalGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // New versions
        intArray & setGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intArray & setGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        intArray & setGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // functions specific to the manipulation of external ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
     // these are usually zero unless the boundaries are meant to have large NON-PARTITIONED boundary regions
        int getExternalGhostCellWidth ( int Axis ) const;
     // intArray & setExternalGhostCellWidth ( int Number_Of_Ghost_Cells_I, int Number_Of_Ghost_Cells_J, 
     //                                         int Number_Of_Ghost_Cells_K, int Number_Of_Ghost_Cells_L );
        intArray & setExternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        void updateGhostBoundaries() const;
#endif

#if defined(PPP)
        intArray( const int* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        intArray( const int* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
#endif
     private:
     // Support for defered evaluation!  (This function does nothing and will be removed soon)
        void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const;

     // This constructor is used by the indexing operators to build array objects defined in
     // local scope.  It allows the support of deferred evaluation on the resulting views!

#if defined(PPP)
        intArray ( intSerialArray* SerialArray_Pointer , 
        	    const Array_Domain_Type* Array_Domain_Pointer, 
		    bool AvoidBuildingIndirectAddressingView = FALSE );

#if 0
     // This function is called in lazy_operand.C!
        intArray ( const intSerialArray* SerialArray_Pointer , 
        	    intArray_Descriptor_Type* 
		    Array_Descriptor_Pointer ,
        	    /*Array_Descriptor_Type* Array_Descriptor_Pointer ,*/
                    Operand_Storage* Array_Storage_Pointer );
#endif
        intArray( const intSerialArray* SerialArray_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        intArray( const intSerialArray* SerialArray_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
		   Indirect_Index_List );
        intArray( const intSerialArray* SerialArray_Pointer, 
                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
#else

        intArray ( const int* Array_Data_Pointer ,
                    const Array_Domain_Type* Array_Domain_Pointer,
                    bool AvoidBuildingIndirectAddressingView = FALSE );

    public:
        intArray( const int* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
        intArray( const int* Array_Data_Pointer, 
                   const Array_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

#if 0
     // This function is called in lazy_operand.C!
        intArray ( const int* Array_Data_Pointer , 
	   intArray_Descriptor_Type* 
	   Array_Descriptor_Pointer , Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
        static int Abstract_Unary_Operator ( const intArray & X ,
                  MDI_int_Prototype_14 , int Operation_Type ); 

        static intArray & Abstract_Unary_Operator ( const intArray & X ,
                  MDI_int_Prototype_0 , MDI_int_Prototype_1 ,
                  int Operation_Type ); 

     /* We need a special binary operator to use in the case of operator- and
     // operator/ since they don't commute and so if the Rhs of the binary operation
     // is a temporary it is reused in a different way that for a commutative
     // operator.  Basically if the Rhs is a temporary we DO reuse it, but we 
     // can't do the equivalent of accumulation into it (which is optimal in
     // efficiency) as in the operator+ and as we can in the case of the Lhs
     // operand being a temporary.  
     // Notice that the non commuting operator reuses the Rhs differently than 
     // it can reuse the Lhs if the Lhs is a temporary.
     // (Lhs = Lhs + Rhs  --> Lhs += Rhs) == (Rhs = Lhs + Rhs --> Rhs += Lhs)
     // (Lhs = Lhs - Rhs  --> Lhs -= Rhs) == (Rhs = Lhs - Rhs --> Rhs = Lhs - Rhs)
     */
        static intArray & Abstract_Binary_Operator ( 
                  const intArray & Lhs , const intArray & Rhs ,
                  MDI_int_Prototype_2 , MDI_int_Prototype_3 ,
                  int Operation_Type );

        static intArray & Abstract_Binary_Operator ( 
                  const intArray & Lhs , int x ,
                  MDI_int_Prototype_4 , MDI_int_Prototype_5 ,
                  int Operation_Type );
 
        static intArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const intArray & Lhs , const intArray & Rhs ,
                  MDI_int_Prototype_2 , MDI_int_Prototype_3 ,
                  int Operation_Type );

        static intArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const intArray & Lhs , int x ,
                  MDI_int_Prototype_4 , MDI_int_Prototype_5 ,
                  int Operation_Type );
 
     // Used for replace function since it is of form "intArray replace (intArray,intArray)"
        intArray & Abstract_Modification_Operator ( 
                  const intArray & Lhs , const intArray & Rhs ,
                  MDI_int_Prototype_6 ,
                  int Operation_Type ); 

     // Used for replace function since it is of form "intArray replace (intArray,int)"
        intArray & Abstract_Modification_Operator ( 
                  const intArray & Lhs , int x ,
                  MDI_int_Prototype_7 ,
                  int Operation_Type ); 

#ifndef INTARRAY
     // Used for replace function since it is of form "intArray replace (int,intArray)"
        intArray & Abstract_Modification_Operator ( 
                  const intArray & Lhs , int x ,
                  MDI_int_Prototype_8 ,
                  int Operation_Type ); 
#endif

        static void Abstract_Operator_Operation_Equals ( 
                  const intArray & Lhs , const intArray & Rhs ,
                  MDI_int_Prototype_3 ,
                  int Operation_Type ); 

        static void Abstract_Operator_Operation_Equals ( 
                  const intArray & Lhs , int x ,
                  MDI_int_Prototype_5 ,
                  int Operation_Type ); 

        static intArray & Abstract_Unary_Operator_Returning_IntArray ( const intArray & X ,
                  MDI_int_Prototype_9 , MDI_int_Prototype_10 ,
                  int Operation_Type ); 

        static intArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const intArray & Lhs , const intArray & Rhs ,
                  MDI_int_Prototype_11 , MDI_int_Prototype_9 ,
                  int Operation_Type ); 

        static intArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const intArray & Lhs , int x ,
                  MDI_int_Prototype_12 , MDI_int_Prototype_13 ,
                  int Operation_Type );

     // Support for conversion operators
        static intArray & Abstract_int_Conversion_Operator (
                  const intArray & X ,
                  MDI_int_Prototype_convertTo_intArray ,
                  int Operation_Type );

        static floatArray & Abstract_float_Conversion_Operator (
                  const intArray & X ,
                  MDI_int_Prototype_convertTo_floatArray ,
                  int Operation_Type );

        static doubleArray & Abstract_double_Conversion_Operator (
                  const intArray & X ,
                  MDI_int_Prototype_convertTo_doubleArray ,
                  int Operation_Type );

#elif defined(PPP)


     // Inline functions must appear in the header file (not the source file) when using the
     // GNU compiler!  We could alternatively use the INLINE macro (perhaps we will later)
     // the INLINE macro is defined to be "" when the GNU compiler is used (though this 
     // should be verified).
#if !defined(GNU)
// inline
#endif
        static void Modify_Reference_Counts_And_Manage_Temporaries (
             Array_Conformability_Info_Type *Array_Set,
             intArray *Temporary,
             intSerialArray* X_Serial_PCE_Array,
             intSerialArray *Data );

#if !defined(GNU)
// inline
#endif
        static void Modify_Reference_Counts_And_Manage_Temporaries (
             Array_Conformability_Info_Type *Array_Set,
             intArray *Temporary,
             intSerialArray* Lhs_Serial_PCE_Array,
             intSerialArray* Rhs_Serial_PCE_Array,
             intSerialArray *Data );

     // reduction operator for sum min max etc.
        static int Abstract_Operator 
	   ( Array_Conformability_Info_Type *Array_Set, 
	     const intArray & X, intSerialArray* X_Serial_PCE_Array, 
	     int x, int Operation_Type );

     // usual binary operators: operator+ operator- operator* operator/ 
     // operator% for both array to array and array to scalar operations
        static intArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
		  const intArray & X_ParallelArray, 
		  intSerialArray* X_Serial_PCE_Array, 
		  intSerialArray & X_SerialArray );

        static intArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
     //           intArray & Lhs, intArray & Rhs, 
                  const intArray & Lhs, const intArray & Rhs, 
                  intSerialArray* Lhs_Serial_PCE_Array, intSerialArray* Rhs_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

     // These operators are special because they don't generate temporaries
     // Used in array to scalar versions of operator+= operator-= operato*= operator/= operator%=
        static intArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , const intArray & This_ParallelArray, intSerialArray* This_Serial_PCE_Array, const intSerialArray & This_SerialArray );
     // Used in array to array versions of operator+= operator-= operato*= operator/= operator%=
        static intArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intArray & This_ParallelArray, const intArray & Lhs, 
                  intSerialArray* This_Serial_PCE_Array, intSerialArray* Lhs_Serial_PCE_Array, 
                  const intSerialArray & X_SerialArray );

#if !defined(INTARRAY)
     // relational operators operator&& operator|| operator<= operator< etc.
        static intArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intArray & X_ParallelArray, 
                  intSerialArray* X_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

        static intArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intArray & Lhs, const intArray & Rhs, 
                  intSerialArray* Lhs_Serial_PCE_Array, 
		  intSerialArray* Rhs_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

     // replace operators for array to array and array to scalar
        static intArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intArray & This_ParallelArray, const intArray & Lhs, 
                  intSerialArray* This_Serial_PCE_Array, intSerialArray* Lhs_Serial_PCE_Array, 
                  const intSerialArray & X_SerialArray );
        static intArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intArray & This_ParallelArray, const intArray & Lhs, const intArray & Rhs, 
                  intSerialArray* This_Serial_PCE_Array, 
                  intSerialArray* Lhs_Serial_PCE_Array,
                  intSerialArray* Rhs_Serial_PCE_Array, 
                  const intSerialArray & X_SerialArray );
#endif

     // Abstract operator support for conversion operators
        static intArray & Abstract_int_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const intArray & X_ParallelArray,
                  intSerialArray* X_Serial_PCE_Array,
                  intSerialArray & X_SerialArray );

        static floatArray & Abstract_float_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const intArray & X_ParallelArray,
                  intSerialArray* X_Serial_PCE_Array,
                  floatSerialArray & X_SerialArray );

        static doubleArray & Abstract_double_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set ,
                  const intArray & X_ParallelArray,
                  intSerialArray* X_Serial_PCE_Array,
                  doubleSerialArray & X_SerialArray );


#else
#error Must DEFINE either APP || SERIAL_APP || PPP
#endif

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif
 
        static intArray*  Build_Temporary_By_Example ( const intArray    & Lhs );
        static intArray*  Build_Temporary_By_Example ( const floatArray  & Lhs );
        static intArray*  Build_Temporary_By_Example ( const doubleArray & Lhs );
        static intArray & Build_New_Array_Or_Reuse_Operand ( const intArray & Lhs , const intArray & Rhs , 
                                                              Memory_Source_Type & Result_Array_Memory );
        static intArray & Build_New_Array_Or_Reuse_Operand ( const intArray & X , 
                                                              Memory_Source_Type & Result_Array_Memory );

     // GNU g++ compiler likes to see these marked as inlined if they are in the header file as well.
     // (11/6/2000) Moved Delete_If_Temporary to array.C
     // friend inline void Delete_If_Temporary     ( const intArray & X );
        friend void Delete_If_Temporary     ( const intArray & X );

     // (11/24/2000) Moved Delete_Lhs_If_Temporary to array.C
     // friend inline void Delete_Lhs_If_Temporary ( const intArray & Lhs );
        friend void Delete_Lhs_If_Temporary ( const intArray & Lhs );

     // Raw memory allocation and dealocation located in these functions
        void Delete_Array_Data ();

     // void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) const;
        void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) ;

#if defined(PPP)
     // void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) const;
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) ;

     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( intArray* This_Pointer , intArray* Lhs , intArray* Rhs );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const intArray & X_ParallelArray,    
	     intSerialArray* & X_SerialArray );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const intArray & Rhs_ParallelArray,  
	     intSerialArray* & Rhs_SerialArray );
     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( const intArray & This_ParallelArray, 
     //      intSerialArray* & This_SerialArray,
     //      const intArray & Lhs_ParallelArray,  
     //      intSerialArray* & Lhs_SerialArray,
     //      const intArray & Rhs_ParallelArray,  
     //      intSerialArray* & Rhs_SerialArray );
#if !defined(INTARRAY)
     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( intArray* This_Pointer , intArray* Lhs , intArray* Rhs );
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const intArray  & This_ParallelArray, 
	     intSerialArray*  & This_SerialArray,
             const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray );
#if 0
     // I think we can skip this function (I hope)
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const intArray  & Rhs_ParallelArray,  
	     intSerialArray*  & Rhs_SerialArray );
#endif
#endif
        static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
	   ( const intArray  & This_ParallelArray, 
	     intSerialArray*  & This_SerialArray,
             const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const intArray  & Rhs_ParallelArray,  
	     intSerialArray*  & Rhs_SerialArray );

        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const intArray & Rhs_ParallelArray,  
	     intSerialArray* & Rhs_SerialArray );
        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const intArray & Lhs_ParallelArray,    
	     intSerialArray* & Lhs_SerialArray );

#if !defined(INTARRAY)
        static Array_Conformability_Info_Type* Parallel_Indirect_Conformability_Enforcement 
	   ( const intArray & Lhs_ParallelArray,  
	     intSerialArray* & Lhs_SerialArray,
             const intArray & Rhs_ParallelArray,  
	     intSerialArray* & Rhs_SerialArray );
#endif
#endif

#if defined(USE_EXPRESSION_TEMPLATES)
     public:

        intArray & operator= ( int x );
        intArray & operator= ( const intArray & Rhs );

     // Access to pointer specific to expression template indexing
        inline int* APP_RESTRICT_MACRO getExpressionTemplateDataPointer () const { return Array_Descriptor.getExpressionTemplateDataPointer(); }

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.h HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
        intArray(const Indexable<T1, A> &Rhs);

        template <class T1, class A>
        intArray & operator=(const Indexable<T1, A> & Rhs);

     // We comment this out for now while we get the
     // operator=(const Indexable<T1, A> &rhs) working (above)
        template <class T1, class A>
        intArray & operator+=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        intArray & operator-=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        intArray & operator*=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        intArray & operator/=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        intArray & operator%=(const Indexable<T1, A> &Rhs);
#else
        intArray(const InAssign<int> &a);

        intArray & operator=(const InAssign<int> &Rhs);
        intArray & operator+=(const InAssign<int> &Rhs);
        intArray & operator-=(const InAssign<int> &Rhs);
        intArray & operator*=(const InAssign<int> &Rhs);
        intArray & operator/=(const InAssign<int> &Rhs);
        intArray & operator%=(const InAssign<int> &Rhs);

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif
   };


#undef INTARRAY
/* MACRO EXPANSION ENDS HERE */

// where.h must follow since it contains static array variables and
// the lenghts are unknown until after the classes are defined (above)!
#include <where.h>

// The final inline_func.h file is included last since it 
// contains actual code used for the inlining performance of A++!
// SKIP_INLINING_FUNCTIONS is  d e f i n e  in A++_headers.h
// ifndef SKIP_INLINING_FUNCTIONS
// Now at the end we can declare the inline functions!
#if !USE_SAGE
#include <inline_func.h>
#include <inline_expression_templates.h>
#endif
// endif

// if !defined(COMPILE_APP)
// undef APP
// endif

#if 1
// bugfix (9/19/96) we have to be able to have A++ compile P++ specific code
// i.e. reverse compatability
typedef doubleArray doubleSerialArray;
typedef floatArray  floatSerialArray;
typedef intArray    intSerialArray;
#endif

#endif  /* !defined(_APP_ARRAY_H) */




/*include(../src/descriptor_macro.m4)*/


