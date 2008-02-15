// Allow repeated includes of array.h without error 
#ifndef _SERIAL_APP_ARRAY_H
#define _SERIAL_APP_ARRAY_H


// We have to undefine the COMPILE_PPP macro to avide a warning generated when it is re-defined
#undef COMPILE_PPP

// ifdef HAVE_CONFIG_H
// This is included here as specified in the Autoconf manual (using <> instead of "")
// We can skip the inclusion following the Autoconf manual and just ALWAYS include it.
#include <P++_config.h>
// endif

#if defined(COMPILE_APP) && defined(COMPILE_PPP)
#error "COMPILE_PPP and COMPILE_APP should NOT both be defined"
#endif

// if !defined(COMPILE_APP)
// error "COMPILE_APP not defined in P++/include/A++.h"
// define COMPILE_SERIAL_APP
// endif

// GNU will build instances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#pragma implementation "A++_headers.h"
#endif

// Turn ON the SERIAL_APP macro until the A++ (serial array) 
// header files have been read.  Then we turn on the PPP macro to process
// (in many cases the same header files) the P++ header files.
#define SERIAL_APP

#if 0
// START: Evaluate the macros that drive the compilation
#if defined(COMPILE_APP)
#error "In P++/include/A++.h (TOP): COMPILE_APP is defined"
#else
#error "In P++/include/A++.h (TOP): COMPILE_APP is NOT defined"
#endif

#if defined(COMPILE_SERIAL_APP)
#error "In P++/include/A++.h (TOP): COMPILE_SERIAL_APP is defined"
#else
#error "In P++/include/A++.h (TOP): COMPILE_SERIAL_APP is NOT defined"
#endif

#if defined(COMPILE_PPP)
#error "In P++/include/A++.h (TOP): COMPILE_PPP is defined"
#else
#error "In P++/include/A++.h (TOP): COMPILE_PPP is NOT defined"
#endif

#if defined(SERIAL_APP)
#error "In P++/include/A++.h (TOP): SERIAL_APP is defined"
#else
#error "In P++/include/A++.h (TOP): SERIAL_APP is NOT defined"
#endif

#if defined(PPP)
#error "In P++/include/A++.h (TOP): PPP is defined"
#else
#error "In P++/include/A++.h (TOP): PPP is NOT defined"
#endif

#if COMPILE_APP
#error "In P++/include/A++.h (TOP): COMPILE_APP is TRUE"
#else
#error "In P++/include/A++.h (TOP): COMPILE_APP is FALSE"
#endif

#if COMPILE_PPP
#error "In P++/include/A++.h (TOP): COMPILE_PPP is TRUE"
#else
#error "In P++/include/A++.h (TOP): COMPILE_PPP is FALSE"
#endif

// END: Evaluate the macros that drive the compilation
#endif

// the collection of header files required 
// before the A++ objects can be defined are 
// contained in A++_headers.h.  This simplifies
// the implementation.  Depending on if we 
// are compiling A++ or and A++ application
// the collection of required files are different.
#include <A++_headers.h>
















/* MACRO EXPANSION BEGINS HERE */
#define DOUBLEARRAY
// Default base and bound object for use in doubleSerialArray constructor
extern Range APP_Unit_Range;



// Forward class declarations
class doubleSerialArray_Function_Steal_Data;
class doubleSerialArray_Function_0;
class doubleSerialArray_Function_1;
class doubleSerialArray_Function_2;
class doubleSerialArray_Function_3;
class doubleSerialArray_Function_4;
class doubleSerialArray_Function_5;
class doubleSerialArray_Function_6;
class doubleSerialArray_Function_7;
class doubleSerialArray_Function_8;
class doubleSerialArray_Function_9;
class doubleSerialArray_Function_10;
class doubleSerialArray_Function_11;
class doubleSerialArray_Function_12;
class doubleSerialArray_Function_13;
class doubleSerialArray_Function_14;
class doubleSerialArray_Function_15;
class doubleSerialArray_Function_16;

class doubleSerialArray_Aggregate_Operator;

// Forward class declarations
class floatSerialArray_Function_Steal_Data;
class floatSerialArray_Function_0;
class floatSerialArray_Function_1;
class floatSerialArray_Function_2;
class floatSerialArray_Function_3;
class floatSerialArray_Function_4;
class floatSerialArray_Function_5;
class floatSerialArray_Function_6;
class floatSerialArray_Function_7;
class floatSerialArray_Function_8;
class floatSerialArray_Function_9;
class floatSerialArray_Function_10;
class floatSerialArray_Function_11;
class floatSerialArray_Function_12;
class floatSerialArray_Function_13;
class floatSerialArray_Function_14;
class floatSerialArray_Function_15;
class floatSerialArray_Function_16;

class floatSerialArray_Aggregate_Operator;

// Forward class declarations
class intSerialArray_Function_Steal_Data;
class intSerialArray_Function_0;
class intSerialArray_Function_1;
class intSerialArray_Function_2;
class intSerialArray_Function_3;
class intSerialArray_Function_4;
class intSerialArray_Function_5;
class intSerialArray_Function_6;
class intSerialArray_Function_7;
class intSerialArray_Function_8;
class intSerialArray_Function_9;
class intSerialArray_Function_10;
class intSerialArray_Function_11;
class intSerialArray_Function_12;
class intSerialArray_Function_13;
class intSerialArray_Function_14;
class intSerialArray_Function_15;
class intSerialArray_Function_16;

class intSerialArray_Aggregate_Operator;


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
class BaseSerialArray
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
       // virtual ~BaseSerialArray();
       // virtual ~BaseSerialArray() = 0;
          BaseSerialArray();
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
          virtual SerialArray_Domain_Type* getDomainPointer () = 0;
       // virtual SerialArray_Descriptor_Type *& getArrayDomainReference () const = 0;
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

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
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
        TSerialArray_Descriptor_Type Array_Descriptor;
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
#define EXPRESSION_TEMPLATES_DERIVED_double_CLASS_SUPPORT : public BaseSerialArray
#endif


// #################################################################
// ######  Definition of the principal object within A++/P++  ######
// #################################################################

class doubleSerialArray EXPRESSION_TEMPLATES_DERIVED_double_CLASS_SUPPORT
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
     friend class doubleSerialArray_Operand_Storage;

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
friend class doubleSerialArray_Function_Steal_Data;
friend class doubleSerialArray_Function_0;
friend class doubleSerialArray_Function_1;
friend class doubleSerialArray_Function_2;
friend class doubleSerialArray_Function_3;
friend class doubleSerialArray_Function_4;
friend class doubleSerialArray_Function_5;
friend class doubleSerialArray_Function_6;
friend class doubleSerialArray_Function_7;
friend class doubleSerialArray_Function_8;
friend class doubleSerialArray_Function_9;
friend class doubleSerialArray_Function_10;
friend class doubleSerialArray_Function_11;
friend class doubleSerialArray_Function_12;
friend class doubleSerialArray_Function_13;
friend class doubleSerialArray_Function_14;
friend class doubleSerialArray_Function_15;
friend class doubleSerialArray_Function_16;

friend class doubleSerialArray_Aggregate_Operator;

// Forward class declarations
friend class floatSerialArray_Function_Steal_Data;
friend class floatSerialArray_Function_0;
friend class floatSerialArray_Function_1;
friend class floatSerialArray_Function_2;
friend class floatSerialArray_Function_3;
friend class floatSerialArray_Function_4;
friend class floatSerialArray_Function_5;
friend class floatSerialArray_Function_6;
friend class floatSerialArray_Function_7;
friend class floatSerialArray_Function_8;
friend class floatSerialArray_Function_9;
friend class floatSerialArray_Function_10;
friend class floatSerialArray_Function_11;
friend class floatSerialArray_Function_12;
friend class floatSerialArray_Function_13;
friend class floatSerialArray_Function_14;
friend class floatSerialArray_Function_15;
friend class floatSerialArray_Function_16;

friend class floatSerialArray_Aggregate_Operator;

// Forward class declarations
friend class intSerialArray_Function_Steal_Data;
friend class intSerialArray_Function_0;
friend class intSerialArray_Function_1;
friend class intSerialArray_Function_2;
friend class intSerialArray_Function_3;
friend class intSerialArray_Function_4;
friend class intSerialArray_Function_5;
friend class intSerialArray_Function_6;
friend class intSerialArray_Function_7;
friend class intSerialArray_Function_8;
friend class intSerialArray_Function_9;
friend class intSerialArray_Function_10;
friend class intSerialArray_Function_11;
friend class intSerialArray_Function_12;
friend class intSerialArray_Function_13;
friend class intSerialArray_Function_14;
friend class intSerialArray_Function_15;
friend class intSerialArray_Function_16;

friend class intSerialArray_Aggregate_Operator;

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
     // SerialArray_Descriptor_Type<double,MAX_ARRAY_DIMENSION> Array_Descriptor;
        doubleSerialArray_Descriptor_Type Array_Descriptor;
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
     // SerialArray_Descriptor_Type* getDescriptorPointer () const;
     // SerialArray_Descriptor_Type* getDomainPointer () const;
     // SerialArray_Domain_Type* getDomainPointer () const;

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
        inline const doubleSerialArray_Descriptor_Type & getDescriptor () const
             { return Array_Descriptor; };

     // Access function for Descriptor
        inline const doubleSerialArray_Descriptor_Type* getDescriptorPointer () const
             { return &Array_Descriptor; };

        inline const SerialArray_Domain_Type & getDomain () const
           { return Array_Descriptor.Array_Domain; };

     // inline const SerialArray_Domain_Type* getDomainPointer () const
        inline const SerialArray_Domain_Type* getDomainPointer () const
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
        virtual doubleSerialArray* getdoubleArrayPointer() const
           {
          // printf ("Calling doubleArray* getdoubleArrayPointer() \n");
             return (doubleSerialArray*) this;
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
        static doubleSerialArray *Current_Link;
        doubleSerialArray *freepointer;
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
             { ((doubleSerialArray*) this)->referenceCount++; }
          inline void decrementReferenceCount() const
             { ((doubleSerialArray*) this)->referenceCount--; }

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
            // SerialArray_Descriptor_Type::Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // APP_Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // SerialArray_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID] = 0;
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
               return doubleSerialArray_Descriptor_Type::getReferenceCountBase(); 
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
            // return 0;
               return doubleSerialArray_Descriptor_Type::getRawDataReferenceCountBase(); 
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
          doubleSerialArray & displayReferenceCounts ( const char* label = "" ) const;

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

#if 0
          inline void ReferenceCountedDelete ()
             {
            // This is a dangerous sort of function since it deletes the class which this
            // function is a member of!.  As long as we don't do anything after calling
            // delete the function just returns and seem to work fine.
               printf ("ERROR: doubleSerialArray::ReferenceCountedDelete called! \n");
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

     // Used to free memory-in-use internally in doubleSerialArray objects!
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();
        static int numberOfInternalArrays();

#if defined(INLINE_FUNCTIONS)
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     // Default Destructor and Constructor
        virtual ~doubleSerialArray ();
        doubleSerialArray ();

     // Constructors

     // Seperate constructors required to avoid ambiguous call errors in call resolution 
     // between the different types

     // ======================================================
        doubleSerialArray ( int i );
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleSerialArray ( int i , int j );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        doubleSerialArray ( int i , int j , int k );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        doubleSerialArray ( int i , int j , int k , int l );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        doubleSerialArray ( int i , int j , int k , int l , int m );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n, int o );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n, int o, int p);
#endif

     // ======================================================

        doubleSerialArray ( ARGUMENT_LIST_MACRO_INTEGER, bool Force_Memory_Allocation  );

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if defined(APP) || defined(PPP)

     // ======================================================
        doubleSerialArray ( int i , const Partitioning_Type & Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleSerialArray ( int i , int j , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        doubleSerialArray ( int i , int j , int k , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        doubleSerialArray ( int i , int j , int k , int l , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        doubleSerialArray ( int i , int j , int k , int l , int m, const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleSerialArray ( int i , int j , const Partitioning_Type & Partition,
	            bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        doubleSerialArray ( int i , int j , int k , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        doubleSerialArray ( int i , int j , int k , int l , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        doubleSerialArray ( int i , int j , int k , int l , int m, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        doubleSerialArray ( int i , int j , int k , int l , int m, int n, int o,
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
     // doubleSerialArray ( const Range & I_Base_Bound , 
     //             const Range & J_Base_Bound = APP_Unit_Range , 
     //             const Range & K_Base_Bound = APP_Unit_Range , 
     //             const Range & L_Base_Bound = APP_Unit_Range );
        doubleSerialArray ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // These are for internal use and allow initialization from an array of data
        doubleSerialArray ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleSerialArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

#if defined(PPP) || defined (APP)
     // Added to support resize function used with an existing partitioning object
        doubleSerialArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List,
                    const Internal_Partitioning_Type & partition );
#endif

        doubleSerialArray ( int Number_Of_Valid_Dimensions, const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // This constructor avoids an ambiguous call to the copy constructor 
     // (since a doubleSerialArray could be built from a Range and an int is the same as a bool)
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleSerialArray ( const Range & I_Base_Bound , int j );
#endif

#if defined(PPP)
     // Construct a doubleSerialArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // Additionally for P++ the local partition must be provided.  Due to restrictions on the
     // type of partitioning supported (i.e. FORTRAN D Partitioning: BLOCK PARTI partitioning)
     // a specific partitoning might not conform to what P++ can presently handle.  The result
     // is an error.  So it is not a good idea to use this function in the parallel environment.
     // doubleSerialArray::doubleSerialArray ( double* Data_Pointer , int i , const Range & Range_I , 
     //                                           int j , const Range & Range_J ,
     //                                           int k , const Range & Range_K ,
     //                                           int l , const Range & Range_L );
     // doubleSerialArray::doubleSerialArray ( const double* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        doubleSerialArray ( const double* Data_Pointer , 
                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );

#if 0
     // We can't use this member function and remain dimension independent (so it is removed for now)
        doubleSerialArray ( double* Data_Pointer , const Range & Span_I , const Range & Range_I , 
                                                  const Range & Span_J , const Range & Range_J ,
                                                  const Range & Span_K , const Range & Range_K ,
                                                  const Range & Span_L , const Range & Range_L );
#endif
#else
     // Construct a doubleSerialArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // doubleSerialArray ( double* Data_Pointer , int i , int j = 1 , int k = 1 , int l = 1 );
        doubleSerialArray ( const double* Data_Pointer , ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );

     // doubleSerialArray ( double* Data_Pointer , const Range & Span_I , 
     //             const Range & Span_J = APP_Unit_Range , 
     //             const Range & Span_K = APP_Unit_Range , 
     //             const Range & Span_L = APP_Unit_Range );
        doubleSerialArray ( const double* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
#endif

     // Constructors to support deffered evaluation of array expressions! (these should be private, I think)
     // I'm not certain these are still used!
        doubleSerialArray ( int Operation_Code , doubleSerialArray *Lhs_Pointer );
        doubleSerialArray ( int Operation_Code , doubleSerialArray *Lhs_Pointer , doubleSerialArray *Rhs_Pointer );

     // Copy Constructors
        doubleSerialArray ( const doubleSerialArray & X , int Type_Of_Copy = DEEPCOPY );

#if defined(APP) || defined(SERIAL_APP)
     // Constructor for initialization from a string (We built the AppString type to avoid autopromotion problems)
     // doubleSerialArray ( const char* );
        doubleSerialArray ( const AppString & X );
#endif

     // Evaluation operator! Evaluates an expression and asigns local scope to its result
     // useful when handing expresions to functions that pass by reference.
     // There is no peformance penalty associated with this function since no memory
     // is copied! 
        friend doubleSerialArray evaluate ( const doubleSerialArray & X );

     // reference operator (builds a view) (an M++ function)
        doubleSerialArray & reference ( const doubleSerialArray & X );
        doubleSerialArray & breakReference ();

        bool Is_Built_By_Defered_Evaluation() const;

     // void display ( const char *Label = "" ) const;
     // doubleSerialArray & display ( const char *Label = "" );
        doubleSerialArray & view    ( const char *Label = "" ) const;
        doubleSerialArray & display ( const char *Label = "" ) const;
        doubleSerialArray & globalDisplay ( const char *Label = "" ) const;
#if defined(USE_EXPRESSION_TEMPLATES)
        void MDI_Display() const;
#endif

     // Provided to permit easy access from dbx debugger
        friend void APP_view    ( const doubleSerialArray & X , const char *Label = "" );
        friend void APP_display ( const doubleSerialArray & X , const char *Label = "" );

     // This is a member function defined in the BaseArray class
        int Array_ID () const;

        inline void setTemporary( bool x ) const { Array_Descriptor.setTemporary(x); }
        inline bool isTemporary() const { return Array_Descriptor.isTemporary(); }
        inline bool isNullArray() const { return Array_Descriptor.isNullArray(); }
        inline bool isView()      const { return Array_Descriptor.isView(); }
        inline bool isContiguousData() const { return Array_Descriptor.isContiguousData(); }

     // Equivalent to operator= (double x)
        doubleSerialArray & fill ( double x );

     // Fills the array with different values given a base and stride
        doubleSerialArray & seqAdd ( double Base = (double) 0 , double Stride = (double) 1 );  // Start at zero and count by 1

     // This function tests for many sorts of errors that could crop up in
     // the internal usage of the doubleSerialArray objects -- it is a debugging tool!
        void Test_Consistency ( const char *Label = "" ) const;
        inline bool Binary_Conformable  ( const doubleSerialArray & X ) const;

     // Part of implementation for diagnostics -- permits more aggressive a 
     // destructive testing than Test_Consistency(). (private interface)
        void testArray();

     // Test for confomability regardless of the input!
     // Templates would simplify this since the template need not be "bound"
     // See Lippman for more details on templates (bound and unbound)!

     // Public interface
        bool isConformable ( const doubleSerialArray & X ) const;
        bool isConformable ( const floatSerialArray & X ) const;
        bool isConformable ( const intSerialArray & X ) const;

     // For internal use (though it is marked public at the moment)
        void Test_Conformability ( const doubleSerialArray & X ) const;
        void Test_Conformability ( const floatSerialArray & X ) const;
        void Test_Conformability ( const intSerialArray & X ) const;

#if defined(INTARRAY)
     // intSerialArray ( const Index & X );
     //  ... (7/20/98,kdb) function below is commented out in src code
     //   so remove here to avoid linking problems ...
     // intSerialArray ( const Internal_Index & X );
        intSerialArray & operator=( const Internal_Index & X );
        intSerialArray & indexMap ();
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

     // Forces one dimensional interpretation of multidimensional doubleSerialArray!
        doubleSerialArray operator() () const;

     // This operator() takes an array of pointers to Internal_Index objects (length MAX_ARRAY_DIMENSION)
        doubleSerialArray operator() ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;
        doubleSerialArray operator() ( int Number_Of_Valid_Dimensions, const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;

     // Indexing operators for use with Index objects
     //   doubleSerialArray operator() 
     //( ARGUMENT_LIST_MACRO_CONST_REF_INTERNAL_INDEX_WITH_PREINITIALIZATION )
     //const ;

#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleSerialArray operator() ( const Internal_Index & I ) const;
              doubleSerialArray operator() ( const Internal_Index & I );
#else
        doubleSerialArray operator() ( const Internal_Index & I ) const;
#endif

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J );
#else
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K );
#else
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L );
#else
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M );
#else
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N );
#else
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O );
#else
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P );
#else
        doubleSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
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
     // static doubleSerialArray* Build_Pointer_To_View_Of_Array ( const doubleSerialArray & X , Internal_Index *Index_Array );
        static doubleSerialArray* Build_Pointer_To_View_Of_Array ( const doubleSerialArray & X ,
                                                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Indexing operators for use with Index objects
        doubleSerialArray operator() ( const Internal_Indirect_Addressing_Index & I ) const;
#if (MAX_ARRAY_DIMENSION >= 2)
        doubleSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        doubleSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        doubleSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        doubleSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        doubleSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        doubleSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N,
                               const Internal_Indirect_Addressing_Index & O ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        doubleSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
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
        doubleSerialArray & operator+ () const;
        doubleSerialArray & operator- () const;

     // Prefix Increment and Decrement Operators ++A and --A
        doubleSerialArray & operator++ ();
        doubleSerialArray & operator-- ();

     // Postfix Increment and Decrement Operators A++ and A--  (argument is ZERO see 594 r.13.4.7 Stroustrup)
        doubleSerialArray & operator++ (int x);
        doubleSerialArray & operator-- (int x);

#if !defined(USE_EXPRESSION_TEMPLATES)
     // operator+ related functions
        friend doubleSerialArray & operator+ ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator+ ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator+ ( const doubleSerialArray & Lhs , double x );
        doubleSerialArray & operator+= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator+= ( double Rhs );

     // operator- related functions
        friend doubleSerialArray & operator- ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator- ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator- ( const doubleSerialArray & Lhs , double x );
        doubleSerialArray & operator-= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator-= ( double Rhs );

     // operator* related functions
        friend doubleSerialArray & operator* ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator* ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator* ( const doubleSerialArray & Lhs , double x );
        doubleSerialArray & operator*= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator*= ( double Rhs );

     // operator/ related functions
        friend doubleSerialArray & operator/ ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator/ ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator/ ( const doubleSerialArray & Lhs , double x );
        doubleSerialArray & operator/= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator/= ( double Rhs );

     // operator% related functions
        friend doubleSerialArray & operator% ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator% ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator% ( const doubleSerialArray & Lhs , double x );
        doubleSerialArray & operator%= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator%= ( double Rhs );

#ifdef INTARRAY
     // Unary Operator ~ (bitwise one's complement)
        doubleSerialArray & operator~ () const;

     // operator& related functions
        friend doubleSerialArray & operator& ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator& ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator& ( const doubleSerialArray & Lhs , double x );
        doubleSerialArray & operator&= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator&= ( double Rhs );

     // operator| related functions
        friend doubleSerialArray & operator| ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator| ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator| ( const doubleSerialArray & Lhs , double x );
        doubleSerialArray & operator|= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator|= ( double Rhs );

     // operator^ related functions
        friend doubleSerialArray & operator^ ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator^ ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator^ ( const doubleSerialArray & Lhs , double x );
        doubleSerialArray & operator^= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator^= ( double Rhs );

     // operator<< related functions
        friend doubleSerialArray & operator<< ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator<< ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator<< ( const doubleSerialArray & Lhs , double x );

     // operator>> related functions
        friend doubleSerialArray & operator>> ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator>> ( double x    , const doubleSerialArray & Rhs );
        friend doubleSerialArray & operator>> ( const doubleSerialArray & Lhs , double x );
#endif

     // Internal conversion member functions (used in casting operators)
        intSerialArray    & convertTo_intArray    () const;
        floatSerialArray  & convertTo_floatArray  () const;
        doubleSerialArray & convertTo_doubleArray () const;

        doubleSerialArray & operator= ( double x );
        doubleSerialArray & operator= ( const doubleSerialArray & Rhs );
#else
     // Need these to be defined for the expression template version
        doubleSerialArray & operator+= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator+= ( double Rhs );

        doubleSerialArray & operator-= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator-= ( double Rhs );

        doubleSerialArray & operator*= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator*= ( double Rhs );

        doubleSerialArray & operator/= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator/= ( double Rhs );

        doubleSerialArray & operator%= ( const doubleSerialArray & Rhs );
        doubleSerialArray & operator%= ( double Rhs );
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
        doubleSerialArray & setBase( int New_Base_For_All_Axes );
        int getBase() const;
        int getRawBase() const;

     // Access function for static Base and Bound variables (by Axis)!
     // void setBase( int Base , int Axis );
        doubleSerialArray & setBase( int Base , int Axis );
        doubleSerialArray & setParallelBase( int Base , int Axis );
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
        doubleSerialArray getLocalArrayWithGhostBoundaries () const;
        doubleSerialArray* getLocalArrayWithGhostBoundariesPointer () const;
#endif

     // Dimension independent query functions
        bool isSameBase   ( const doubleSerialArray & X ) const;
        bool isSameBound  ( const doubleSerialArray & X ) const;
        bool isSameStride ( const doubleSerialArray & X ) const;
        bool isSameLength ( const doubleSerialArray & X ) const;
#if defined(APP) || defined(PPP)
        bool isSameGhostBoundaryWidth ( const doubleSerialArray & X ) const;
        //bool isSameDistribution ( const doubleSerialArray & X ) const;
        bool isSameDistribution ( const intSerialArray & X ) const;
        bool isSameDistribution ( const floatSerialArray & X ) const;
        bool isSameDistribution ( const doubleSerialArray & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const doubleSerialArray & X ) const;

     // Member functions for access to the geometry of the A++ object
        int getGeometryBase ( int Axis ) const;
        doubleSerialArray & setGeometryBase ( int New_Geometry_Base , int Axis );

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
     // doubleSerialArray & redim ( int i , int j = 1 , int k = 1 , int l = 1 );
        doubleSerialArray & redim ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // doubleSerialArray & redim ( const Range & i , 
     //                     const Range & j = APP_Unit_Range , 
     //                     const Range & k = APP_Unit_Range , 
     //                     const Range & l = APP_Unit_Range );
        doubleSerialArray & redim ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // We need this to avoid an ERROR: Overloading ambiguity between "doubleArray::redim(const Array_Domain_Type&)" 
     // and "doubleArray::redim(const Range&, const Range&, const Range&, const Range&, const Range&, const Range&)" 
     // so we need to implement a redim that takes a single Index object.
        doubleSerialArray & redim ( const Index & I );
        doubleSerialArray & redim ( const doubleSerialArray & X );
        doubleSerialArray & redim ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleSerialArray & redim ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // The Sun C++ compiler can't handle template declarations in non templated classes
     // template<class T, int Template_Dimension>
     // doubleSerialArray & redim ( const SerialArray_Descriptor_Type<T,Template_Dimension> & X );
        doubleSerialArray & redim ( const SerialArray_Domain_Type & X );

     // Changes dimensions of array object with no change in number of elements
     // doubleSerialArray & reshape ( int i , int j = 1 , int k = 1 , int l = 1 );
        doubleSerialArray & reshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // doubleSerialArray & reshape ( const Range & i , 
     //                       const Range & j = APP_Unit_Range , 
     //                       const Range & k = APP_Unit_Range , 
     //                       const Range & l = APP_Unit_Range );
        doubleSerialArray & reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        doubleSerialArray & reshape ( const doubleSerialArray & X );
        doubleSerialArray & reshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleSerialArray & reshape ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Used for support of weakened conformability assignment in operator= (maybe this should be private or protected)
     // doubleSerialArray & internalReshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // doubleSerialArray & internalReshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

	void fix_view_bases (int* View_Bases, int* View_Sizes, int numdims, 
			     const int* old_lengths, const int* Integer_List);
	void fix_view_bases_non_short_span 
	   (int* View_Bases, int* View_Sizes, int numdims, const int* Integer_List);

     // resize function redimensions array and copies exiting data into new array
     // doubleSerialArray & resize ( int i , int j = 1 , int k = 1 , int l = 1 );
        doubleSerialArray & resize ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // doubleSerialArray & resize ( const Range & i , 
     //                      const Range & j = APP_Unit_Range , 
     //                      const Range & k = APP_Unit_Range , 
     //                      const Range & l = APP_Unit_Range );
        doubleSerialArray & resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        doubleSerialArray & resize ( const doubleSerialArray & X );
        doubleSerialArray & resize ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleSerialArray & resize ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // adopt existing data as an A++ array
     // versions of adopt which are specific to P++
     // To avoid the 1D P++ adopt function from being confused with the
     // 2D A++ adopt function we must provide different member function names.
        doubleSerialArray & adopt ( const double* Data_Pointer , const Partitioning_Type & Partition ,
                                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        doubleSerialArray & adopt ( const double* Data_Pointer , const Partitioning_Type & Partition ,
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // doubleSerialArray & adopt ( const double* Data_Pointer , const Partitioning_Type & Partition ,
     //                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // A++ specific implementation of adopt member functions
     // These A++ specific function are made available to P++ to maintain
     // the identical interface even though under P++ these functions
     // are not implemented.

     // doubleSerialArray & adopt ( const double* Data_Pointer , 
     //                     int i , int j = 1 , int k = 1 , int l = 1 ); 
     // function with interface for internal use only
        doubleSerialArray & adopt ( const double* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
        doubleSerialArray & adopt ( const double* Data_Pointer , 
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleSerialArray & adopt ( const double* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
     // Use the size of an existing A++/P++ array object
        doubleSerialArray & adopt ( const double* Data_Pointer , const doubleSerialArray & X );

     // These are used only within the internal implementation of A++/P++
        doubleSerialArray & adopt ( const double* Data_Pointer , 
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Force a deep copy since deep copy constructors are not alway called
     // even when explicit attempts are made to call them 
        doubleSerialArray & copy ( const doubleSerialArray & X , int DeepCopy = DEEPCOPY );

     // Convert indexing of view to be non HPF!
        doubleSerialArray & useFortranIndexing ( bool Flag );

     // This would effectively be a reference (so call reference instead)!
     // doubleSerialArray & adopt ( const doubleSerialArray & X  );

     // Reduction operators!
        friend double min ( const doubleSerialArray & X );
        friend double max ( const doubleSerialArray & X );

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
        friend doubleSerialArray min ( const doubleSerialArray & X , const doubleSerialArray & Y );
        friend doubleSerialArray min ( const doubleSerialArray & X , double y );
        friend doubleSerialArray min ( double x , const doubleSerialArray & Y );
        friend doubleSerialArray min ( const doubleSerialArray & X , const doubleSerialArray & Y , const doubleSerialArray & Z );
        friend doubleSerialArray min ( double x , const doubleSerialArray & Y , const doubleSerialArray & Z );
        friend doubleSerialArray min ( const doubleSerialArray & X , double y , const doubleSerialArray & Z );
        friend doubleSerialArray min ( const doubleSerialArray & X , const doubleSerialArray & Y , double z );

        friend doubleSerialArray max ( const doubleSerialArray & X , const doubleSerialArray & Y );
        friend doubleSerialArray max ( const doubleSerialArray & X , double y );
        friend doubleSerialArray max ( double x , const doubleSerialArray & Y );
        friend doubleSerialArray max ( const doubleSerialArray & X , const doubleSerialArray & Y , const doubleSerialArray & Z );
        friend doubleSerialArray max ( double x , const doubleSerialArray & Y , const doubleSerialArray & Z );
        friend doubleSerialArray max ( const doubleSerialArray & X , double y , const doubleSerialArray & Z );
        friend doubleSerialArray max ( const doubleSerialArray & X , const doubleSerialArray & Y , double z );

     // Misc functions
        doubleSerialArray & replace ( const intSerialArray & X , const doubleSerialArray & Y );
        doubleSerialArray & replace ( int x , const doubleSerialArray & Y );
        doubleSerialArray & replace ( const intSerialArray & X , double y );

        friend double sum ( const doubleSerialArray & X );

     // Sum along axis 
        friend doubleSerialArray sum ( const doubleSerialArray & X , int Axis );
#else

     // The non-expression template version returns values by reference
        friend doubleSerialArray & min ( const doubleSerialArray & X , const doubleSerialArray & Y );
        friend doubleSerialArray & min ( const doubleSerialArray & X , double y );
        friend doubleSerialArray & min ( double x , const doubleSerialArray & Y );
        friend doubleSerialArray & min ( const doubleSerialArray & X , const doubleSerialArray & Y , const doubleSerialArray & Z );
        friend doubleSerialArray & min ( double x , const doubleSerialArray & Y , const doubleSerialArray & Z );
        friend doubleSerialArray & min ( const doubleSerialArray & X , double y , const doubleSerialArray & Z );
        friend doubleSerialArray & min ( const doubleSerialArray & X , const doubleSerialArray & Y , double z );

        friend doubleSerialArray & max ( const doubleSerialArray & X , const doubleSerialArray & Y );
        friend doubleSerialArray & max ( const doubleSerialArray & X , double y );
        friend doubleSerialArray & max ( double x , const doubleSerialArray & Y );
        friend doubleSerialArray & max ( const doubleSerialArray & X , const doubleSerialArray & Y , const doubleSerialArray & Z );
        friend doubleSerialArray & max ( double x , const doubleSerialArray & Y , const doubleSerialArray & Z );
        friend doubleSerialArray & max ( const doubleSerialArray & X , double y , const doubleSerialArray & Z );
        friend doubleSerialArray & max ( const doubleSerialArray & X , const doubleSerialArray & Y , double z );

     // Misc functions
        doubleSerialArray & replace ( const intSerialArray & X , const doubleSerialArray & Y );
        doubleSerialArray & replace ( int x , const doubleSerialArray & Y );
        doubleSerialArray & replace ( const intSerialArray & X , double y );

        friend double sum ( const doubleSerialArray & X );
        friend doubleSerialArray & sum ( const doubleSerialArray & X , const doubleSerialArray & Y );
        friend doubleSerialArray & sum ( const doubleSerialArray & X , const doubleSerialArray & Y , const doubleSerialArray & Z );

     // Sum along axis
        friend doubleSerialArray & sum ( const doubleSerialArray & X , int Axis );
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)
#ifndef INTARRAY
        friend doubleSerialArray & fmod ( const doubleSerialArray & X , double y );
        friend doubleSerialArray & fmod ( double x , const doubleSerialArray & Y );
        friend doubleSerialArray & fmod ( const doubleSerialArray & X , const doubleSerialArray & Y );
#endif
        friend doubleSerialArray & mod ( const doubleSerialArray & X , double y );
        friend doubleSerialArray & mod ( double x , const doubleSerialArray & Y );
        friend doubleSerialArray & mod ( const doubleSerialArray & X , const doubleSerialArray & Y );

        friend doubleSerialArray & pow ( const doubleSerialArray & X , double y );  // returns X**y
        friend doubleSerialArray & pow ( double x , const doubleSerialArray & Y );
        friend doubleSerialArray & pow ( const doubleSerialArray & X , const doubleSerialArray & Y );

        friend doubleSerialArray & sign ( const doubleSerialArray & Array_Signed_Value , const doubleSerialArray & Input_Array );
        friend doubleSerialArray & sign ( double Scalar_Signed_Value             , const doubleSerialArray & Input_Array );
        friend doubleSerialArray & sign ( const doubleSerialArray & Array_Signed_Value , double Input_Value  );

#ifndef INTARRAY
        friend doubleSerialArray & log   ( const doubleSerialArray & X );
        friend doubleSerialArray & log10 ( const doubleSerialArray & X );
        friend doubleSerialArray & exp   ( const doubleSerialArray & X );
        friend doubleSerialArray & sqrt  ( const doubleSerialArray & X );
        friend doubleSerialArray & fabs  ( const doubleSerialArray & X );
        friend doubleSerialArray & ceil  ( const doubleSerialArray & X );
        friend doubleSerialArray & floor ( const doubleSerialArray & X );
#endif
        friend doubleSerialArray & abs   ( const doubleSerialArray & X );

     // Not fully supported yet (only works for 1D or 2D arrays object)!
        friend doubleSerialArray & transpose ( const doubleSerialArray & X );

     // Trig functions!
#ifndef INTARRAY
        friend doubleSerialArray & cos   ( const doubleSerialArray & X );
        friend doubleSerialArray & sin   ( const doubleSerialArray & X );
        friend doubleSerialArray & tan   ( const doubleSerialArray & X );
        friend doubleSerialArray & acos  ( const doubleSerialArray & X );
        friend doubleSerialArray & asin  ( const doubleSerialArray & X );
        friend doubleSerialArray & atan  ( const doubleSerialArray & X );
        friend doubleSerialArray & cosh  ( const doubleSerialArray & X );
        friend doubleSerialArray & sinh  ( const doubleSerialArray & X );
        friend doubleSerialArray & tanh  ( const doubleSerialArray & X );
        friend doubleSerialArray & acosh ( const doubleSerialArray & X );
        friend doubleSerialArray & asinh ( const doubleSerialArray & X );
        friend doubleSerialArray & atanh ( const doubleSerialArray & X );

     // All the different atan2 functions for arrays (ask for by Bill Henshaw)
        friend doubleSerialArray & atan2 ( const doubleSerialArray & X , double y );
        friend doubleSerialArray & atan2 ( double x , const doubleSerialArray & Y );
        friend doubleSerialArray & atan2 ( const doubleSerialArray & X , const doubleSerialArray & Y );
#endif

     // relational operators 
     // operator< related functions
        friend intSerialArray & operator< ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend intSerialArray & operator< ( double x    , const doubleSerialArray & Rhs );
        friend intSerialArray & operator< ( const doubleSerialArray & Lhs , double x );

     // operator<= related functions
        friend intSerialArray & operator<= ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend intSerialArray & operator<= ( double x    , const doubleSerialArray & Rhs );
        friend intSerialArray & operator<= ( const doubleSerialArray & Lhs , double x );

     // operator> related functions
        friend intSerialArray & operator> ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend intSerialArray & operator> ( double x    , const doubleSerialArray & Rhs );
        friend intSerialArray & operator> ( const doubleSerialArray & Lhs , double x );

     // operator>= related functions
        friend intSerialArray & operator>= ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend intSerialArray & operator>= ( double x    , const doubleSerialArray & Rhs );
        friend intSerialArray & operator>= ( const doubleSerialArray & Lhs , double x );

     // operator== related functions
        friend intSerialArray & operator== ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend intSerialArray & operator== ( double x    , const doubleSerialArray & Rhs );
        friend intSerialArray & operator== ( const doubleSerialArray & Lhs , double x );

     // operator!= related functions
        friend intSerialArray & operator!= ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend intSerialArray & operator!= ( double x    , const doubleSerialArray & Rhs );
        friend intSerialArray & operator!= ( const doubleSerialArray & Lhs , double x );

     // operator&& related functions
        friend intSerialArray & operator&& ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend intSerialArray & operator&& ( double x    , const doubleSerialArray & Rhs );
        friend intSerialArray & operator&& ( const doubleSerialArray & Lhs , double x );

     // operator|| related functions
        friend intSerialArray & operator|| ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        friend intSerialArray & operator|| ( double x    , const doubleSerialArray & Rhs );
        friend intSerialArray & operator|| ( const doubleSerialArray & Lhs , double x );
#else
     // If we ARE using expression templates then we have to declare some friend functions
     // friend double sum ( const doubleSerialArray & X );
     // friend doubleSerialArray sum ( const doubleSerialArray & X , int axis );

        template<class T, class A>
        friend T sum ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T max ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T min ( const Indexable<T,A> & X );
#endif

     // operator! related functions
        intSerialArray & operator! ();

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
        doubleSerialArray & partition ( const Internal_Partitioning_Type & Partition );
        doubleSerialArray & partition ( const Partitioning_Type & Partition );
        doubleSerialArray & partition ( const doubleArray & Example_Array );
        doubleSerialArray & partition ( const floatArray & Example_Array );
        doubleSerialArray & partition ( const intArray & Example_Array );

     // typedef support for member function pointers as parameters for the map diagnostic mechanism
     // We support two different function prototypes
        typedef int (doubleSerialArray::*doubleSerialArrayMemberVoidFunctionPointerType) (void) const;
        typedef int (doubleSerialArray::*doubleSerialArrayMemberIntegerFunctionPointerType) (int) const;

     // Builds array with base/bound info for each axis and processor
     // calls special purpose communication function

#if defined(PPP)
     // This only makes since in P++
        intSerialArray buildProcessorMap ( doubleSerialArrayMemberVoidFunctionPointerType X );
        intSerialArray buildProcessorMap ( doubleSerialArrayMemberIntegerFunctionPointerType X );
        intSerialArray buildProcessorMap ( doubleSerialArrayMemberIntegerFunctionPointerType X, doubleSerialArrayMemberIntegerFunctionPointerType Y );
#else
     // Preserve the interface in A++
        intArray buildProcessorMap ( doubleSerialArrayMemberVoidFunctionPointerType X );
        intArray buildProcessorMap ( doubleSerialArrayMemberIntegerFunctionPointerType X );
        intArray buildProcessorMap ( doubleSerialArrayMemberIntegerFunctionPointerType X, doubleSerialArrayMemberIntegerFunctionPointerType Y );
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
        doubleSerialArray & setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleSerialArray & setInternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        doubleSerialArray & setInternalGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // New versions
        doubleSerialArray & setGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        doubleSerialArray & setGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        doubleSerialArray & setGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // functions specific to the manipulation of external ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
     // these are usually zero unless the boundaries are meant to have large NON-PARTITIONED boundary regions
        int getExternalGhostCellWidth ( int Axis ) const;
     // doubleSerialArray & setExternalGhostCellWidth ( int Number_Of_Ghost_Cells_I, int Number_Of_Ghost_Cells_J, 
     //                                         int Number_Of_Ghost_Cells_K, int Number_Of_Ghost_Cells_L );
        doubleSerialArray & setExternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        void updateGhostBoundaries() const;
#endif

#if defined(PPP)
    //! I don't know where this is used!
        doubleSerialArray( const double* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
    //! I don't know where this is used!
        doubleSerialArray( const double* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
#endif

     private:
     //! Support for defered evaluation!  (This function does nothing and will be removed soon)
        void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const;

#if defined(PPP)
     //! This constructor is used by the indexing operators to build array objects defined in local scope.
     //! It allows the support of deferred evaluation on the resulting views!
        doubleSerialArray ( doubleSerialArray* SerialArray_Pointer , 
        	    const SerialArray_Domain_Type* Array_Domain_Pointer, 
		    bool AvoidBuildingIndirectAddressingView = FALSE );

#if 0
     //! This function is called in lazy_operand.C!
        doubleSerialArray ( const doubleSerialArray* SerialArray_Pointer , 
        	    doubleSerialArray_Descriptor_Type* 
		    Array_Descriptor_Pointer ,
        	    /*SerialArray_Descriptor_Type* Array_Descriptor_Pointer ,*/
                    Operand_Storage* Array_Storage_Pointer );
#endif

    //! Internal support for parallel indexing (operator()) using Internal_Index objects
        doubleSerialArray( const doubleSerialArray* SerialArray_Pointer, 
                   const SerialArray_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

    //! Internal support for parallel indirect indexing (operator()) using Internal_Indirect_Addressing_Index objects
        doubleSerialArray( const doubleSerialArray* SerialArray_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
		   Indirect_Index_List );

    //! I don't know if this function is used!
        doubleSerialArray( const doubleSerialArray* SerialArray_Pointer, 
                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
#else

        doubleSerialArray ( const double* Array_Data_Pointer ,
                    const SerialArray_Domain_Type* Array_Domain_Pointer,
                    bool AvoidBuildingIndirectAddressingView = FALSE );

    public:
    //! Internal support for serial indexing (operator()) using Internal_Index objects
        doubleSerialArray( const double* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

    //! Internal support for serial indirect indexing (operator()) using Internal_Indirect_Addressing_Index objects
        doubleSerialArray( const double* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

#if 0
    //! This function is called in lazy_operand.C!
        doubleSerialArray ( const double* Array_Data_Pointer,
	   doubleSerialArray_Descriptor_Type* Array_Descriptor_Pointer,
           Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
        static double Abstract_Unary_Operator ( const doubleSerialArray & X ,
                  MDI_double_Prototype_14 , int Operation_Type ); 

        static doubleSerialArray & Abstract_Unary_Operator ( const doubleSerialArray & X ,
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
        static doubleSerialArray & Abstract_Binary_Operator ( 
                  const doubleSerialArray & Lhs , const doubleSerialArray & Rhs ,
                  MDI_double_Prototype_2 , MDI_double_Prototype_3 ,
                  int Operation_Type );

        static doubleSerialArray & Abstract_Binary_Operator ( 
                  const doubleSerialArray & Lhs , double x ,
                  MDI_double_Prototype_4 , MDI_double_Prototype_5 ,
                  int Operation_Type );
 
        static doubleSerialArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const doubleSerialArray & Lhs , const doubleSerialArray & Rhs ,
                  MDI_double_Prototype_2 , MDI_double_Prototype_3 ,
                  int Operation_Type );

        static doubleSerialArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const doubleSerialArray & Lhs , double x ,
                  MDI_double_Prototype_4 , MDI_double_Prototype_5 ,
                  int Operation_Type );
 
     // Used for replace function since it is of form "doubleSerialArray replace (intSerialArray,doubleSerialArray)"
        doubleSerialArray & Abstract_Modification_Operator ( 
                  const intSerialArray & Lhs , const doubleSerialArray & Rhs ,
                  MDI_double_Prototype_6 ,
                  int Operation_Type ); 

     // Used for replace function since it is of form "doubleSerialArray replace (intSerialArray,double)"
        doubleSerialArray & Abstract_Modification_Operator ( 
                  const intSerialArray & Lhs , double x ,
                  MDI_double_Prototype_7 ,
                  int Operation_Type ); 

#ifndef INTARRAY
     // Used for replace function since it is of form "doubleSerialArray replace (int,doubleSerialArray)"
        doubleSerialArray & Abstract_Modification_Operator ( 
                  const doubleSerialArray & Lhs , int x ,
                  MDI_double_Prototype_8 ,
                  int Operation_Type ); 
#endif

        static void Abstract_Operator_Operation_Equals ( 
                  const doubleSerialArray & Lhs , const doubleSerialArray & Rhs ,
                  MDI_double_Prototype_3 ,
                  int Operation_Type ); 

        static void Abstract_Operator_Operation_Equals ( 
                  const doubleSerialArray & Lhs , double x ,
                  MDI_double_Prototype_5 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Unary_Operator_Returning_IntArray ( const doubleSerialArray & X ,
                  MDI_double_Prototype_9 , MDI_double_Prototype_10 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const doubleSerialArray & Lhs , const doubleSerialArray & Rhs ,
                  MDI_double_Prototype_11 , MDI_double_Prototype_9 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const doubleSerialArray & Lhs , double x ,
                  MDI_double_Prototype_12 , MDI_double_Prototype_13 ,
                  int Operation_Type );

     // Support for conversion operators
        static intSerialArray & Abstract_int_Conversion_Operator (
                  const doubleSerialArray & X ,
                  MDI_double_Prototype_convertTo_intArray ,
                  int Operation_Type );

        static floatSerialArray & Abstract_float_Conversion_Operator (
                  const doubleSerialArray & X ,
                  MDI_double_Prototype_convertTo_floatArray ,
                  int Operation_Type );

        static doubleSerialArray & Abstract_double_Conversion_Operator (
                  const doubleSerialArray & X ,
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
        static doubleSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
		  const doubleArray & X_ParallelArray, 
		  doubleSerialArray* X_Serial_PCE_Array, 
		  doubleSerialArray & X_SerialArray );

        static doubleSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
     //           doubleArray & Lhs, doubleArray & Rhs, 
                  const doubleArray & Lhs, const doubleArray & Rhs, 
                  doubleSerialArray* Lhs_Serial_PCE_Array, doubleSerialArray* Rhs_Serial_PCE_Array, 
                  doubleSerialArray & X_SerialArray );

     // These operators are special because they don't generate temporaries
     // Used in array to scalar versions of operator+= operator-= operato*= operator/= operator%=
        static doubleSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , const doubleSerialArray & This_ParallelArray, doubleSerialArray* This_Serial_PCE_Array, const doubleSerialArray & This_SerialArray );
     // Used in array to array versions of operator+= operator-= operato*= operator/= operator%=
        static doubleSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleSerialArray & This_ParallelArray, const doubleSerialArray & Lhs, 
                  doubleSerialArray* This_Serial_PCE_Array, doubleSerialArray* Lhs_Serial_PCE_Array, 
                  const doubleSerialArray & X_SerialArray );

#if !defined(INTARRAY)
     // relational operators operator&& operator|| operator<= operator< etc.
        static intSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleSerialArray & X_ParallelArray, 
                  doubleSerialArray* X_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

        static intSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleSerialArray & Lhs, const doubleSerialArray & Rhs, 
                  doubleSerialArray* Lhs_Serial_PCE_Array, 
		  doubleSerialArray* Rhs_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

     // replace operators for array to array and array to scalar
        static doubleSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleSerialArray & This_ParallelArray, const intSerialArray & Lhs, 
                  doubleSerialArray* This_Serial_PCE_Array, intSerialArray* Lhs_Serial_PCE_Array, 
                  const doubleSerialArray & X_SerialArray );
        static doubleSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const doubleSerialArray & This_ParallelArray, const intSerialArray & Lhs, const doubleSerialArray & Rhs, 
                  doubleSerialArray* This_Serial_PCE_Array, 
                  intSerialArray* Lhs_Serial_PCE_Array,
                  doubleSerialArray* Rhs_Serial_PCE_Array, 
                  const doubleSerialArray & X_SerialArray );
#endif

     // Abstract operator support for conversion operators
        static intSerialArray & Abstract_int_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const doubleSerialArray & X_ParallelArray,
                  doubleSerialArray* X_Serial_PCE_Array,
                  intSerialArray & X_SerialArray );

        static floatSerialArray & Abstract_float_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const doubleSerialArray & X_ParallelArray,
                  doubleSerialArray* X_Serial_PCE_Array,
                  floatSerialArray & X_SerialArray );

        static doubleSerialArray & Abstract_double_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set ,
                  const doubleSerialArray & X_ParallelArray,
                  doubleSerialArray* X_Serial_PCE_Array,
                  doubleSerialArray & X_SerialArray );


#else
#error Must DEFINE either APP || SERIAL_APP || PPP
#endif

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif
 
        static doubleSerialArray*  Build_Temporary_By_Example ( const intSerialArray    & Lhs );
        static doubleSerialArray*  Build_Temporary_By_Example ( const floatSerialArray  & Lhs );
        static doubleSerialArray*  Build_Temporary_By_Example ( const doubleSerialArray & Lhs );
        static doubleSerialArray & Build_New_Array_Or_Reuse_Operand ( const doubleSerialArray & Lhs , const doubleSerialArray & Rhs , 
                                                              Memory_Source_Type & Result_Array_Memory );
        static doubleSerialArray & Build_New_Array_Or_Reuse_Operand ( const doubleSerialArray & X , 
                                                              Memory_Source_Type & Result_Array_Memory );

     // GNU g++ compiler likes to see these marked as inlined if they are in the header file as well.
     // (11/6/2000) Moved Delete_If_Temporary to array.C
     // friend inline void Delete_If_Temporary     ( const doubleSerialArray & X );
        friend void Delete_If_Temporary     ( const doubleSerialArray & X );

     // (11/24/2000) Moved Delete_Lhs_If_Temporary to array.C
     // friend inline void Delete_Lhs_If_Temporary ( const doubleSerialArray & Lhs );
        friend void Delete_Lhs_If_Temporary ( const doubleSerialArray & Lhs );

     // Raw memory allocation and dealocation located in these functions
        void Delete_Array_Data ();

     // void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) const;
        void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) ;

#if defined(PPP)
     // void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) const;
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) ;

     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( doubleSerialArray* This_Pointer , doubleSerialArray* Lhs , doubleSerialArray* Rhs );
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
     //    ( doubleSerialArray* This_Pointer , intSerialArray* Lhs , doubleSerialArray* Rhs );
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

        doubleSerialArray & operator= ( double x );
        doubleSerialArray & operator= ( const doubleSerialArray & Rhs );

     // Access to pointer specific to expression template indexing
        inline double* APP_RESTRICT_MACRO getExpressionTemplateDataPointer () const { return Array_Descriptor.getExpressionTemplateDataPointer(); }

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.h HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
        doubleSerialArray(const Indexable<T1, A> &Rhs);

        template <class T1, class A>
        doubleSerialArray & operator=(const Indexable<T1, A> & Rhs);

     // We comment this out for now while we get the
     // operator=(const Indexable<T1, A> &rhs) working (above)
        template <class T1, class A>
        doubleSerialArray & operator+=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        doubleSerialArray & operator-=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        doubleSerialArray & operator*=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        doubleSerialArray & operator/=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        doubleSerialArray & operator%=(const Indexable<T1, A> &Rhs);
#else
        doubleSerialArray(const InAssign<double> &a);

        doubleSerialArray & operator=(const InAssign<double> &Rhs);
        doubleSerialArray & operator+=(const InAssign<double> &Rhs);
        doubleSerialArray & operator-=(const InAssign<double> &Rhs);
        doubleSerialArray & operator*=(const InAssign<double> &Rhs);
        doubleSerialArray & operator/=(const InAssign<double> &Rhs);
        doubleSerialArray & operator%=(const InAssign<double> &Rhs);

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif
   };


#undef DOUBLEARRAY

#define FLOATARRAY
// Default base and bound object for use in floatSerialArray constructor
extern Range APP_Unit_Range;



// Forward class declarations
class doubleSerialArray_Function_Steal_Data;
class doubleSerialArray_Function_0;
class doubleSerialArray_Function_1;
class doubleSerialArray_Function_2;
class doubleSerialArray_Function_3;
class doubleSerialArray_Function_4;
class doubleSerialArray_Function_5;
class doubleSerialArray_Function_6;
class doubleSerialArray_Function_7;
class doubleSerialArray_Function_8;
class doubleSerialArray_Function_9;
class doubleSerialArray_Function_10;
class doubleSerialArray_Function_11;
class doubleSerialArray_Function_12;
class doubleSerialArray_Function_13;
class doubleSerialArray_Function_14;
class doubleSerialArray_Function_15;
class doubleSerialArray_Function_16;

class doubleSerialArray_Aggregate_Operator;

// Forward class declarations
class floatSerialArray_Function_Steal_Data;
class floatSerialArray_Function_0;
class floatSerialArray_Function_1;
class floatSerialArray_Function_2;
class floatSerialArray_Function_3;
class floatSerialArray_Function_4;
class floatSerialArray_Function_5;
class floatSerialArray_Function_6;
class floatSerialArray_Function_7;
class floatSerialArray_Function_8;
class floatSerialArray_Function_9;
class floatSerialArray_Function_10;
class floatSerialArray_Function_11;
class floatSerialArray_Function_12;
class floatSerialArray_Function_13;
class floatSerialArray_Function_14;
class floatSerialArray_Function_15;
class floatSerialArray_Function_16;

class floatSerialArray_Aggregate_Operator;

// Forward class declarations
class intSerialArray_Function_Steal_Data;
class intSerialArray_Function_0;
class intSerialArray_Function_1;
class intSerialArray_Function_2;
class intSerialArray_Function_3;
class intSerialArray_Function_4;
class intSerialArray_Function_5;
class intSerialArray_Function_6;
class intSerialArray_Function_7;
class intSerialArray_Function_8;
class intSerialArray_Function_9;
class intSerialArray_Function_10;
class intSerialArray_Function_11;
class intSerialArray_Function_12;
class intSerialArray_Function_13;
class intSerialArray_Function_14;
class intSerialArray_Function_15;
class intSerialArray_Function_16;

class intSerialArray_Aggregate_Operator;


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
class BaseSerialArray
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
       // virtual ~BaseSerialArray();
       // virtual ~BaseSerialArray() = 0;
          BaseSerialArray();
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
          virtual SerialArray_Domain_Type* getDomainPointer () = 0;
       // virtual SerialArray_Descriptor_Type *& getArrayDomainReference () const = 0;
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

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
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
        TSerialArray_Descriptor_Type Array_Descriptor;
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
#define EXPRESSION_TEMPLATES_DERIVED_float_CLASS_SUPPORT : public BaseSerialArray
#endif


// #################################################################
// ######  Definition of the principal object within A++/P++  ######
// #################################################################

class floatSerialArray EXPRESSION_TEMPLATES_DERIVED_float_CLASS_SUPPORT
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
     friend class floatSerialArray_Operand_Storage;

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
friend class doubleSerialArray_Function_Steal_Data;
friend class doubleSerialArray_Function_0;
friend class doubleSerialArray_Function_1;
friend class doubleSerialArray_Function_2;
friend class doubleSerialArray_Function_3;
friend class doubleSerialArray_Function_4;
friend class doubleSerialArray_Function_5;
friend class doubleSerialArray_Function_6;
friend class doubleSerialArray_Function_7;
friend class doubleSerialArray_Function_8;
friend class doubleSerialArray_Function_9;
friend class doubleSerialArray_Function_10;
friend class doubleSerialArray_Function_11;
friend class doubleSerialArray_Function_12;
friend class doubleSerialArray_Function_13;
friend class doubleSerialArray_Function_14;
friend class doubleSerialArray_Function_15;
friend class doubleSerialArray_Function_16;

friend class doubleSerialArray_Aggregate_Operator;

// Forward class declarations
friend class floatSerialArray_Function_Steal_Data;
friend class floatSerialArray_Function_0;
friend class floatSerialArray_Function_1;
friend class floatSerialArray_Function_2;
friend class floatSerialArray_Function_3;
friend class floatSerialArray_Function_4;
friend class floatSerialArray_Function_5;
friend class floatSerialArray_Function_6;
friend class floatSerialArray_Function_7;
friend class floatSerialArray_Function_8;
friend class floatSerialArray_Function_9;
friend class floatSerialArray_Function_10;
friend class floatSerialArray_Function_11;
friend class floatSerialArray_Function_12;
friend class floatSerialArray_Function_13;
friend class floatSerialArray_Function_14;
friend class floatSerialArray_Function_15;
friend class floatSerialArray_Function_16;

friend class floatSerialArray_Aggregate_Operator;

// Forward class declarations
friend class intSerialArray_Function_Steal_Data;
friend class intSerialArray_Function_0;
friend class intSerialArray_Function_1;
friend class intSerialArray_Function_2;
friend class intSerialArray_Function_3;
friend class intSerialArray_Function_4;
friend class intSerialArray_Function_5;
friend class intSerialArray_Function_6;
friend class intSerialArray_Function_7;
friend class intSerialArray_Function_8;
friend class intSerialArray_Function_9;
friend class intSerialArray_Function_10;
friend class intSerialArray_Function_11;
friend class intSerialArray_Function_12;
friend class intSerialArray_Function_13;
friend class intSerialArray_Function_14;
friend class intSerialArray_Function_15;
friend class intSerialArray_Function_16;

friend class intSerialArray_Aggregate_Operator;

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
     // SerialArray_Descriptor_Type<float,MAX_ARRAY_DIMENSION> Array_Descriptor;
        floatSerialArray_Descriptor_Type Array_Descriptor;
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
     // SerialArray_Descriptor_Type* getDescriptorPointer () const;
     // SerialArray_Descriptor_Type* getDomainPointer () const;
     // SerialArray_Domain_Type* getDomainPointer () const;

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
        inline const floatSerialArray_Descriptor_Type & getDescriptor () const
             { return Array_Descriptor; };

     // Access function for Descriptor
        inline const floatSerialArray_Descriptor_Type* getDescriptorPointer () const
             { return &Array_Descriptor; };

        inline const SerialArray_Domain_Type & getDomain () const
           { return Array_Descriptor.Array_Domain; };

     // inline const SerialArray_Domain_Type* getDomainPointer () const
        inline const SerialArray_Domain_Type* getDomainPointer () const
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
        virtual floatSerialArray* getfloatArrayPointer() const
           {
          // printf ("Calling doubleArray* getfloatArrayPointer() \n");
             return (floatSerialArray*) this;
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
        static floatSerialArray *Current_Link;
        floatSerialArray *freepointer;
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
             { ((floatSerialArray*) this)->referenceCount++; }
          inline void decrementReferenceCount() const
             { ((floatSerialArray*) this)->referenceCount--; }

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
            // SerialArray_Descriptor_Type::Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // APP_Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // SerialArray_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID] = 0;
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
               return floatSerialArray_Descriptor_Type::getReferenceCountBase(); 
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
            // return 0;
               return floatSerialArray_Descriptor_Type::getRawDataReferenceCountBase(); 
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
          floatSerialArray & displayReferenceCounts ( const char* label = "" ) const;

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

#if 0
          inline void ReferenceCountedDelete ()
             {
            // This is a dangerous sort of function since it deletes the class which this
            // function is a member of!.  As long as we don't do anything after calling
            // delete the function just returns and seem to work fine.
               printf ("ERROR: floatSerialArray::ReferenceCountedDelete called! \n");
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

     // Used to free memory-in-use internally in floatSerialArray objects!
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();
        static int numberOfInternalArrays();

#if defined(INLINE_FUNCTIONS)
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     // Default Destructor and Constructor
        virtual ~floatSerialArray ();
        floatSerialArray ();

     // Constructors

     // Seperate constructors required to avoid ambiguous call errors in call resolution 
     // between the different types

     // ======================================================
        floatSerialArray ( int i );
#if (MAX_ARRAY_DIMENSION >= 2)
        floatSerialArray ( int i , int j );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        floatSerialArray ( int i , int j , int k );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        floatSerialArray ( int i , int j , int k , int l );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        floatSerialArray ( int i , int j , int k , int l , int m );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        floatSerialArray ( int i , int j , int k , int l , int m, int n );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        floatSerialArray ( int i , int j , int k , int l , int m, int n, int o );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        floatSerialArray ( int i , int j , int k , int l , int m, int n, int o, int p);
#endif

     // ======================================================

        floatSerialArray ( ARGUMENT_LIST_MACRO_INTEGER, bool Force_Memory_Allocation  );

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if defined(APP) || defined(PPP)

     // ======================================================
        floatSerialArray ( int i , const Partitioning_Type & Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
        floatSerialArray ( int i , int j , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        floatSerialArray ( int i , int j , int k , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        floatSerialArray ( int i , int j , int k , int l , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        floatSerialArray ( int i , int j , int k , int l , int m, const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        floatSerialArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        floatSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        floatSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION >= 2)
        floatSerialArray ( int i , int j , const Partitioning_Type & Partition,
	            bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        floatSerialArray ( int i , int j , int k , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        floatSerialArray ( int i , int j , int k , int l , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        floatSerialArray ( int i , int j , int k , int l , int m, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        floatSerialArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        floatSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        floatSerialArray ( int i , int j , int k , int l , int m, int n, int o,
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
     // floatSerialArray ( const Range & I_Base_Bound , 
     //             const Range & J_Base_Bound = APP_Unit_Range , 
     //             const Range & K_Base_Bound = APP_Unit_Range , 
     //             const Range & L_Base_Bound = APP_Unit_Range );
        floatSerialArray ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // These are for internal use and allow initialization from an array of data
        floatSerialArray ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatSerialArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

#if defined(PPP) || defined (APP)
     // Added to support resize function used with an existing partitioning object
        floatSerialArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List,
                    const Internal_Partitioning_Type & partition );
#endif

        floatSerialArray ( int Number_Of_Valid_Dimensions, const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // This constructor avoids an ambiguous call to the copy constructor 
     // (since a floatSerialArray could be built from a Range and an int is the same as a bool)
#if (MAX_ARRAY_DIMENSION >= 2)
        floatSerialArray ( const Range & I_Base_Bound , int j );
#endif

#if defined(PPP)
     // Construct a floatSerialArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // Additionally for P++ the local partition must be provided.  Due to restrictions on the
     // type of partitioning supported (i.e. FORTRAN D Partitioning: BLOCK PARTI partitioning)
     // a specific partitoning might not conform to what P++ can presently handle.  The result
     // is an error.  So it is not a good idea to use this function in the parallel environment.
     // floatSerialArray::floatSerialArray ( float* Data_Pointer , int i , const Range & Range_I , 
     //                                           int j , const Range & Range_J ,
     //                                           int k , const Range & Range_K ,
     //                                           int l , const Range & Range_L );
     // floatSerialArray::floatSerialArray ( const float* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        floatSerialArray ( const float* Data_Pointer , 
                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );

#if 0
     // We can't use this member function and remain dimension independent (so it is removed for now)
        floatSerialArray ( float* Data_Pointer , const Range & Span_I , const Range & Range_I , 
                                                  const Range & Span_J , const Range & Range_J ,
                                                  const Range & Span_K , const Range & Range_K ,
                                                  const Range & Span_L , const Range & Range_L );
#endif
#else
     // Construct a floatSerialArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // floatSerialArray ( float* Data_Pointer , int i , int j = 1 , int k = 1 , int l = 1 );
        floatSerialArray ( const float* Data_Pointer , ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );

     // floatSerialArray ( float* Data_Pointer , const Range & Span_I , 
     //             const Range & Span_J = APP_Unit_Range , 
     //             const Range & Span_K = APP_Unit_Range , 
     //             const Range & Span_L = APP_Unit_Range );
        floatSerialArray ( const float* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
#endif

     // Constructors to support deffered evaluation of array expressions! (these should be private, I think)
     // I'm not certain these are still used!
        floatSerialArray ( int Operation_Code , floatSerialArray *Lhs_Pointer );
        floatSerialArray ( int Operation_Code , floatSerialArray *Lhs_Pointer , floatSerialArray *Rhs_Pointer );

     // Copy Constructors
        floatSerialArray ( const floatSerialArray & X , int Type_Of_Copy = DEEPCOPY );

#if defined(APP) || defined(SERIAL_APP)
     // Constructor for initialization from a string (We built the AppString type to avoid autopromotion problems)
     // floatSerialArray ( const char* );
        floatSerialArray ( const AppString & X );
#endif

     // Evaluation operator! Evaluates an expression and asigns local scope to its result
     // useful when handing expresions to functions that pass by reference.
     // There is no peformance penalty associated with this function since no memory
     // is copied! 
        friend floatSerialArray evaluate ( const floatSerialArray & X );

     // reference operator (builds a view) (an M++ function)
        floatSerialArray & reference ( const floatSerialArray & X );
        floatSerialArray & breakReference ();

        bool Is_Built_By_Defered_Evaluation() const;

     // void display ( const char *Label = "" ) const;
     // floatSerialArray & display ( const char *Label = "" );
        floatSerialArray & view    ( const char *Label = "" ) const;
        floatSerialArray & display ( const char *Label = "" ) const;
        floatSerialArray & globalDisplay ( const char *Label = "" ) const;
#if defined(USE_EXPRESSION_TEMPLATES)
        void MDI_Display() const;
#endif

     // Provided to permit easy access from dbx debugger
        friend void APP_view    ( const floatSerialArray & X , const char *Label = "" );
        friend void APP_display ( const floatSerialArray & X , const char *Label = "" );

     // This is a member function defined in the BaseArray class
        int Array_ID () const;

        inline void setTemporary( bool x ) const { Array_Descriptor.setTemporary(x); }
        inline bool isTemporary() const { return Array_Descriptor.isTemporary(); }
        inline bool isNullArray() const { return Array_Descriptor.isNullArray(); }
        inline bool isView()      const { return Array_Descriptor.isView(); }
        inline bool isContiguousData() const { return Array_Descriptor.isContiguousData(); }

     // Equivalent to operator= (float x)
        floatSerialArray & fill ( float x );

     // Fills the array with different values given a base and stride
        floatSerialArray & seqAdd ( float Base = (float) 0 , float Stride = (float) 1 );  // Start at zero and count by 1

     // This function tests for many sorts of errors that could crop up in
     // the internal usage of the floatSerialArray objects -- it is a debugging tool!
        void Test_Consistency ( const char *Label = "" ) const;
        inline bool Binary_Conformable  ( const floatSerialArray & X ) const;

     // Part of implementation for diagnostics -- permits more aggressive a 
     // destructive testing than Test_Consistency(). (private interface)
        void testArray();

     // Test for confomability regardless of the input!
     // Templates would simplify this since the template need not be "bound"
     // See Lippman for more details on templates (bound and unbound)!

     // Public interface
        bool isConformable ( const doubleSerialArray & X ) const;
        bool isConformable ( const floatSerialArray & X ) const;
        bool isConformable ( const intSerialArray & X ) const;

     // For internal use (though it is marked public at the moment)
        void Test_Conformability ( const doubleSerialArray & X ) const;
        void Test_Conformability ( const floatSerialArray & X ) const;
        void Test_Conformability ( const intSerialArray & X ) const;

#if defined(INTARRAY)
     // intSerialArray ( const Index & X );
     //  ... (7/20/98,kdb) function below is commented out in src code
     //   so remove here to avoid linking problems ...
     // intSerialArray ( const Internal_Index & X );
        intSerialArray & operator=( const Internal_Index & X );
        intSerialArray & indexMap ();
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

     // Forces one dimensional interpretation of multidimensional floatSerialArray!
        floatSerialArray operator() () const;

     // This operator() takes an array of pointers to Internal_Index objects (length MAX_ARRAY_DIMENSION)
        floatSerialArray operator() ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;
        floatSerialArray operator() ( int Number_Of_Valid_Dimensions, const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;

     // Indexing operators for use with Index objects
     //   floatSerialArray operator() 
     //( ARGUMENT_LIST_MACRO_CONST_REF_INTERNAL_INDEX_WITH_PREINITIALIZATION )
     //const ;

#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatSerialArray operator() ( const Internal_Index & I ) const;
              floatSerialArray operator() ( const Internal_Index & I );
#else
        floatSerialArray operator() ( const Internal_Index & I ) const;
#endif

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J );
#else
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K );
#else
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L );
#else
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M );
#else
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N );
#else
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O );
#else
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P );
#else
        floatSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
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
     // static floatSerialArray* Build_Pointer_To_View_Of_Array ( const floatSerialArray & X , Internal_Index *Index_Array );
        static floatSerialArray* Build_Pointer_To_View_Of_Array ( const floatSerialArray & X ,
                                                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Indexing operators for use with Index objects
        floatSerialArray operator() ( const Internal_Indirect_Addressing_Index & I ) const;
#if (MAX_ARRAY_DIMENSION >= 2)
        floatSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        floatSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        floatSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        floatSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        floatSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        floatSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N,
                               const Internal_Indirect_Addressing_Index & O ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        floatSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
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
        floatSerialArray & operator+ () const;
        floatSerialArray & operator- () const;

     // Prefix Increment and Decrement Operators ++A and --A
        floatSerialArray & operator++ ();
        floatSerialArray & operator-- ();

     // Postfix Increment and Decrement Operators A++ and A--  (argument is ZERO see 594 r.13.4.7 Stroustrup)
        floatSerialArray & operator++ (int x);
        floatSerialArray & operator-- (int x);

#if !defined(USE_EXPRESSION_TEMPLATES)
     // operator+ related functions
        friend floatSerialArray & operator+ ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator+ ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator+ ( const floatSerialArray & Lhs , float x );
        floatSerialArray & operator+= ( const floatSerialArray & Rhs );
        floatSerialArray & operator+= ( float Rhs );

     // operator- related functions
        friend floatSerialArray & operator- ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator- ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator- ( const floatSerialArray & Lhs , float x );
        floatSerialArray & operator-= ( const floatSerialArray & Rhs );
        floatSerialArray & operator-= ( float Rhs );

     // operator* related functions
        friend floatSerialArray & operator* ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator* ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator* ( const floatSerialArray & Lhs , float x );
        floatSerialArray & operator*= ( const floatSerialArray & Rhs );
        floatSerialArray & operator*= ( float Rhs );

     // operator/ related functions
        friend floatSerialArray & operator/ ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator/ ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator/ ( const floatSerialArray & Lhs , float x );
        floatSerialArray & operator/= ( const floatSerialArray & Rhs );
        floatSerialArray & operator/= ( float Rhs );

     // operator% related functions
        friend floatSerialArray & operator% ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator% ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator% ( const floatSerialArray & Lhs , float x );
        floatSerialArray & operator%= ( const floatSerialArray & Rhs );
        floatSerialArray & operator%= ( float Rhs );

#ifdef INTARRAY
     // Unary Operator ~ (bitwise one's complement)
        floatSerialArray & operator~ () const;

     // operator& related functions
        friend floatSerialArray & operator& ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator& ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator& ( const floatSerialArray & Lhs , float x );
        floatSerialArray & operator&= ( const floatSerialArray & Rhs );
        floatSerialArray & operator&= ( float Rhs );

     // operator| related functions
        friend floatSerialArray & operator| ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator| ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator| ( const floatSerialArray & Lhs , float x );
        floatSerialArray & operator|= ( const floatSerialArray & Rhs );
        floatSerialArray & operator|= ( float Rhs );

     // operator^ related functions
        friend floatSerialArray & operator^ ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator^ ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator^ ( const floatSerialArray & Lhs , float x );
        floatSerialArray & operator^= ( const floatSerialArray & Rhs );
        floatSerialArray & operator^= ( float Rhs );

     // operator<< related functions
        friend floatSerialArray & operator<< ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator<< ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator<< ( const floatSerialArray & Lhs , float x );

     // operator>> related functions
        friend floatSerialArray & operator>> ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend floatSerialArray & operator>> ( float x    , const floatSerialArray & Rhs );
        friend floatSerialArray & operator>> ( const floatSerialArray & Lhs , float x );
#endif

     // Internal conversion member functions (used in casting operators)
        intSerialArray    & convertTo_intArray    () const;
        floatSerialArray  & convertTo_floatArray  () const;
        doubleSerialArray & convertTo_doubleArray () const;

        floatSerialArray & operator= ( float x );
        floatSerialArray & operator= ( const floatSerialArray & Rhs );
#else
     // Need these to be defined for the expression template version
        floatSerialArray & operator+= ( const floatSerialArray & Rhs );
        floatSerialArray & operator+= ( float Rhs );

        floatSerialArray & operator-= ( const floatSerialArray & Rhs );
        floatSerialArray & operator-= ( float Rhs );

        floatSerialArray & operator*= ( const floatSerialArray & Rhs );
        floatSerialArray & operator*= ( float Rhs );

        floatSerialArray & operator/= ( const floatSerialArray & Rhs );
        floatSerialArray & operator/= ( float Rhs );

        floatSerialArray & operator%= ( const floatSerialArray & Rhs );
        floatSerialArray & operator%= ( float Rhs );
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
        floatSerialArray & setBase( int New_Base_For_All_Axes );
        int getBase() const;
        int getRawBase() const;

     // Access function for static Base and Bound variables (by Axis)!
     // void setBase( int Base , int Axis );
        floatSerialArray & setBase( int Base , int Axis );
        floatSerialArray & setParallelBase( int Base , int Axis );
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
        floatSerialArray getLocalArrayWithGhostBoundaries () const;
        floatSerialArray* getLocalArrayWithGhostBoundariesPointer () const;
#endif

     // Dimension independent query functions
        bool isSameBase   ( const floatSerialArray & X ) const;
        bool isSameBound  ( const floatSerialArray & X ) const;
        bool isSameStride ( const floatSerialArray & X ) const;
        bool isSameLength ( const floatSerialArray & X ) const;
#if defined(APP) || defined(PPP)
        bool isSameGhostBoundaryWidth ( const floatSerialArray & X ) const;
        //bool isSameDistribution ( const floatSerialArray & X ) const;
        bool isSameDistribution ( const intSerialArray & X ) const;
        bool isSameDistribution ( const floatSerialArray & X ) const;
        bool isSameDistribution ( const doubleSerialArray & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const floatSerialArray & X ) const;

     // Member functions for access to the geometry of the A++ object
        int getGeometryBase ( int Axis ) const;
        floatSerialArray & setGeometryBase ( int New_Geometry_Base , int Axis );

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
     // floatSerialArray & redim ( int i , int j = 1 , int k = 1 , int l = 1 );
        floatSerialArray & redim ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // floatSerialArray & redim ( const Range & i , 
     //                     const Range & j = APP_Unit_Range , 
     //                     const Range & k = APP_Unit_Range , 
     //                     const Range & l = APP_Unit_Range );
        floatSerialArray & redim ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // We need this to avoid an ERROR: Overloading ambiguity between "doubleArray::redim(const Array_Domain_Type&)" 
     // and "doubleArray::redim(const Range&, const Range&, const Range&, const Range&, const Range&, const Range&)" 
     // so we need to implement a redim that takes a single Index object.
        floatSerialArray & redim ( const Index & I );
        floatSerialArray & redim ( const floatSerialArray & X );
        floatSerialArray & redim ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatSerialArray & redim ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // The Sun C++ compiler can't handle template declarations in non templated classes
     // template<class T, int Template_Dimension>
     // floatSerialArray & redim ( const SerialArray_Descriptor_Type<T,Template_Dimension> & X );
        floatSerialArray & redim ( const SerialArray_Domain_Type & X );

     // Changes dimensions of array object with no change in number of elements
     // floatSerialArray & reshape ( int i , int j = 1 , int k = 1 , int l = 1 );
        floatSerialArray & reshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // floatSerialArray & reshape ( const Range & i , 
     //                       const Range & j = APP_Unit_Range , 
     //                       const Range & k = APP_Unit_Range , 
     //                       const Range & l = APP_Unit_Range );
        floatSerialArray & reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        floatSerialArray & reshape ( const floatSerialArray & X );
        floatSerialArray & reshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatSerialArray & reshape ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Used for support of weakened conformability assignment in operator= (maybe this should be private or protected)
     // floatSerialArray & internalReshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // floatSerialArray & internalReshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

	void fix_view_bases (int* View_Bases, int* View_Sizes, int numdims, 
			     const int* old_lengths, const int* Integer_List);
	void fix_view_bases_non_short_span 
	   (int* View_Bases, int* View_Sizes, int numdims, const int* Integer_List);

     // resize function redimensions array and copies exiting data into new array
     // floatSerialArray & resize ( int i , int j = 1 , int k = 1 , int l = 1 );
        floatSerialArray & resize ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // floatSerialArray & resize ( const Range & i , 
     //                      const Range & j = APP_Unit_Range , 
     //                      const Range & k = APP_Unit_Range , 
     //                      const Range & l = APP_Unit_Range );
        floatSerialArray & resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        floatSerialArray & resize ( const floatSerialArray & X );
        floatSerialArray & resize ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatSerialArray & resize ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // adopt existing data as an A++ array
     // versions of adopt which are specific to P++
     // To avoid the 1D P++ adopt function from being confused with the
     // 2D A++ adopt function we must provide different member function names.
        floatSerialArray & adopt ( const float* Data_Pointer , const Partitioning_Type & Partition ,
                                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        floatSerialArray & adopt ( const float* Data_Pointer , const Partitioning_Type & Partition ,
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // floatSerialArray & adopt ( const float* Data_Pointer , const Partitioning_Type & Partition ,
     //                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // A++ specific implementation of adopt member functions
     // These A++ specific function are made available to P++ to maintain
     // the identical interface even though under P++ these functions
     // are not implemented.

     // floatSerialArray & adopt ( const float* Data_Pointer , 
     //                     int i , int j = 1 , int k = 1 , int l = 1 ); 
     // function with interface for internal use only
        floatSerialArray & adopt ( const float* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
        floatSerialArray & adopt ( const float* Data_Pointer , 
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatSerialArray & adopt ( const float* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
     // Use the size of an existing A++/P++ array object
        floatSerialArray & adopt ( const float* Data_Pointer , const floatSerialArray & X );

     // These are used only within the internal implementation of A++/P++
        floatSerialArray & adopt ( const float* Data_Pointer , 
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Force a deep copy since deep copy constructors are not alway called
     // even when explicit attempts are made to call them 
        floatSerialArray & copy ( const floatSerialArray & X , int DeepCopy = DEEPCOPY );

     // Convert indexing of view to be non HPF!
        floatSerialArray & useFortranIndexing ( bool Flag );

     // This would effectively be a reference (so call reference instead)!
     // floatSerialArray & adopt ( const floatSerialArray & X  );

     // Reduction operators!
        friend float min ( const floatSerialArray & X );
        friend float max ( const floatSerialArray & X );

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
        friend floatSerialArray min ( const floatSerialArray & X , const floatSerialArray & Y );
        friend floatSerialArray min ( const floatSerialArray & X , float y );
        friend floatSerialArray min ( float x , const floatSerialArray & Y );
        friend floatSerialArray min ( const floatSerialArray & X , const floatSerialArray & Y , const floatSerialArray & Z );
        friend floatSerialArray min ( float x , const floatSerialArray & Y , const floatSerialArray & Z );
        friend floatSerialArray min ( const floatSerialArray & X , float y , const floatSerialArray & Z );
        friend floatSerialArray min ( const floatSerialArray & X , const floatSerialArray & Y , float z );

        friend floatSerialArray max ( const floatSerialArray & X , const floatSerialArray & Y );
        friend floatSerialArray max ( const floatSerialArray & X , float y );
        friend floatSerialArray max ( float x , const floatSerialArray & Y );
        friend floatSerialArray max ( const floatSerialArray & X , const floatSerialArray & Y , const floatSerialArray & Z );
        friend floatSerialArray max ( float x , const floatSerialArray & Y , const floatSerialArray & Z );
        friend floatSerialArray max ( const floatSerialArray & X , float y , const floatSerialArray & Z );
        friend floatSerialArray max ( const floatSerialArray & X , const floatSerialArray & Y , float z );

     // Misc functions
        floatSerialArray & replace ( const intSerialArray & X , const floatSerialArray & Y );
        floatSerialArray & replace ( int x , const floatSerialArray & Y );
        floatSerialArray & replace ( const intSerialArray & X , float y );

        friend float sum ( const floatSerialArray & X );

     // Sum along axis 
        friend floatSerialArray sum ( const floatSerialArray & X , int Axis );
#else

     // The non-expression template version returns values by reference
        friend floatSerialArray & min ( const floatSerialArray & X , const floatSerialArray & Y );
        friend floatSerialArray & min ( const floatSerialArray & X , float y );
        friend floatSerialArray & min ( float x , const floatSerialArray & Y );
        friend floatSerialArray & min ( const floatSerialArray & X , const floatSerialArray & Y , const floatSerialArray & Z );
        friend floatSerialArray & min ( float x , const floatSerialArray & Y , const floatSerialArray & Z );
        friend floatSerialArray & min ( const floatSerialArray & X , float y , const floatSerialArray & Z );
        friend floatSerialArray & min ( const floatSerialArray & X , const floatSerialArray & Y , float z );

        friend floatSerialArray & max ( const floatSerialArray & X , const floatSerialArray & Y );
        friend floatSerialArray & max ( const floatSerialArray & X , float y );
        friend floatSerialArray & max ( float x , const floatSerialArray & Y );
        friend floatSerialArray & max ( const floatSerialArray & X , const floatSerialArray & Y , const floatSerialArray & Z );
        friend floatSerialArray & max ( float x , const floatSerialArray & Y , const floatSerialArray & Z );
        friend floatSerialArray & max ( const floatSerialArray & X , float y , const floatSerialArray & Z );
        friend floatSerialArray & max ( const floatSerialArray & X , const floatSerialArray & Y , float z );

     // Misc functions
        floatSerialArray & replace ( const intSerialArray & X , const floatSerialArray & Y );
        floatSerialArray & replace ( int x , const floatSerialArray & Y );
        floatSerialArray & replace ( const intSerialArray & X , float y );

        friend float sum ( const floatSerialArray & X );
        friend floatSerialArray & sum ( const floatSerialArray & X , const floatSerialArray & Y );
        friend floatSerialArray & sum ( const floatSerialArray & X , const floatSerialArray & Y , const floatSerialArray & Z );

     // Sum along axis
        friend floatSerialArray & sum ( const floatSerialArray & X , int Axis );
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)
#ifndef INTARRAY
        friend floatSerialArray & fmod ( const floatSerialArray & X , float y );
        friend floatSerialArray & fmod ( float x , const floatSerialArray & Y );
        friend floatSerialArray & fmod ( const floatSerialArray & X , const floatSerialArray & Y );
#endif
        friend floatSerialArray & mod ( const floatSerialArray & X , float y );
        friend floatSerialArray & mod ( float x , const floatSerialArray & Y );
        friend floatSerialArray & mod ( const floatSerialArray & X , const floatSerialArray & Y );

        friend floatSerialArray & pow ( const floatSerialArray & X , float y );  // returns X**y
        friend floatSerialArray & pow ( float x , const floatSerialArray & Y );
        friend floatSerialArray & pow ( const floatSerialArray & X , const floatSerialArray & Y );

        friend floatSerialArray & sign ( const floatSerialArray & Array_Signed_Value , const floatSerialArray & Input_Array );
        friend floatSerialArray & sign ( float Scalar_Signed_Value             , const floatSerialArray & Input_Array );
        friend floatSerialArray & sign ( const floatSerialArray & Array_Signed_Value , float Input_Value  );

#ifndef INTARRAY
        friend floatSerialArray & log   ( const floatSerialArray & X );
        friend floatSerialArray & log10 ( const floatSerialArray & X );
        friend floatSerialArray & exp   ( const floatSerialArray & X );
        friend floatSerialArray & sqrt  ( const floatSerialArray & X );
        friend floatSerialArray & fabs  ( const floatSerialArray & X );
        friend floatSerialArray & ceil  ( const floatSerialArray & X );
        friend floatSerialArray & floor ( const floatSerialArray & X );
#endif
        friend floatSerialArray & abs   ( const floatSerialArray & X );

     // Not fully supported yet (only works for 1D or 2D arrays object)!
        friend floatSerialArray & transpose ( const floatSerialArray & X );

     // Trig functions!
#ifndef INTARRAY
        friend floatSerialArray & cos   ( const floatSerialArray & X );
        friend floatSerialArray & sin   ( const floatSerialArray & X );
        friend floatSerialArray & tan   ( const floatSerialArray & X );
        friend floatSerialArray & acos  ( const floatSerialArray & X );
        friend floatSerialArray & asin  ( const floatSerialArray & X );
        friend floatSerialArray & atan  ( const floatSerialArray & X );
        friend floatSerialArray & cosh  ( const floatSerialArray & X );
        friend floatSerialArray & sinh  ( const floatSerialArray & X );
        friend floatSerialArray & tanh  ( const floatSerialArray & X );
        friend floatSerialArray & acosh ( const floatSerialArray & X );
        friend floatSerialArray & asinh ( const floatSerialArray & X );
        friend floatSerialArray & atanh ( const floatSerialArray & X );

     // All the different atan2 functions for arrays (ask for by Bill Henshaw)
        friend floatSerialArray & atan2 ( const floatSerialArray & X , float y );
        friend floatSerialArray & atan2 ( float x , const floatSerialArray & Y );
        friend floatSerialArray & atan2 ( const floatSerialArray & X , const floatSerialArray & Y );
#endif

     // relational operators 
     // operator< related functions
        friend intSerialArray & operator< ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend intSerialArray & operator< ( float x    , const floatSerialArray & Rhs );
        friend intSerialArray & operator< ( const floatSerialArray & Lhs , float x );

     // operator<= related functions
        friend intSerialArray & operator<= ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend intSerialArray & operator<= ( float x    , const floatSerialArray & Rhs );
        friend intSerialArray & operator<= ( const floatSerialArray & Lhs , float x );

     // operator> related functions
        friend intSerialArray & operator> ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend intSerialArray & operator> ( float x    , const floatSerialArray & Rhs );
        friend intSerialArray & operator> ( const floatSerialArray & Lhs , float x );

     // operator>= related functions
        friend intSerialArray & operator>= ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend intSerialArray & operator>= ( float x    , const floatSerialArray & Rhs );
        friend intSerialArray & operator>= ( const floatSerialArray & Lhs , float x );

     // operator== related functions
        friend intSerialArray & operator== ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend intSerialArray & operator== ( float x    , const floatSerialArray & Rhs );
        friend intSerialArray & operator== ( const floatSerialArray & Lhs , float x );

     // operator!= related functions
        friend intSerialArray & operator!= ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend intSerialArray & operator!= ( float x    , const floatSerialArray & Rhs );
        friend intSerialArray & operator!= ( const floatSerialArray & Lhs , float x );

     // operator&& related functions
        friend intSerialArray & operator&& ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend intSerialArray & operator&& ( float x    , const floatSerialArray & Rhs );
        friend intSerialArray & operator&& ( const floatSerialArray & Lhs , float x );

     // operator|| related functions
        friend intSerialArray & operator|| ( const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        friend intSerialArray & operator|| ( float x    , const floatSerialArray & Rhs );
        friend intSerialArray & operator|| ( const floatSerialArray & Lhs , float x );
#else
     // If we ARE using expression templates then we have to declare some friend functions
     // friend float sum ( const floatSerialArray & X );
     // friend floatSerialArray sum ( const floatSerialArray & X , int axis );

        template<class T, class A>
        friend T sum ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T max ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T min ( const Indexable<T,A> & X );
#endif

     // operator! related functions
        intSerialArray & operator! ();

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
        floatSerialArray & partition ( const Internal_Partitioning_Type & Partition );
        floatSerialArray & partition ( const Partitioning_Type & Partition );
        floatSerialArray & partition ( const doubleArray & Example_Array );
        floatSerialArray & partition ( const floatArray & Example_Array );
        floatSerialArray & partition ( const intArray & Example_Array );

     // typedef support for member function pointers as parameters for the map diagnostic mechanism
     // We support two different function prototypes
        typedef int (floatSerialArray::*floatSerialArrayMemberVoidFunctionPointerType) (void) const;
        typedef int (floatSerialArray::*floatSerialArrayMemberIntegerFunctionPointerType) (int) const;

     // Builds array with base/bound info for each axis and processor
     // calls special purpose communication function

#if defined(PPP)
     // This only makes since in P++
        intSerialArray buildProcessorMap ( floatSerialArrayMemberVoidFunctionPointerType X );
        intSerialArray buildProcessorMap ( floatSerialArrayMemberIntegerFunctionPointerType X );
        intSerialArray buildProcessorMap ( floatSerialArrayMemberIntegerFunctionPointerType X, floatSerialArrayMemberIntegerFunctionPointerType Y );
#else
     // Preserve the interface in A++
        intArray buildProcessorMap ( floatSerialArrayMemberVoidFunctionPointerType X );
        intArray buildProcessorMap ( floatSerialArrayMemberIntegerFunctionPointerType X );
        intArray buildProcessorMap ( floatSerialArrayMemberIntegerFunctionPointerType X, floatSerialArrayMemberIntegerFunctionPointerType Y );
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
        floatSerialArray & setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatSerialArray & setInternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        floatSerialArray & setInternalGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // New versions
        floatSerialArray & setGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        floatSerialArray & setGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        floatSerialArray & setGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // functions specific to the manipulation of external ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
     // these are usually zero unless the boundaries are meant to have large NON-PARTITIONED boundary regions
        int getExternalGhostCellWidth ( int Axis ) const;
     // floatSerialArray & setExternalGhostCellWidth ( int Number_Of_Ghost_Cells_I, int Number_Of_Ghost_Cells_J, 
     //                                         int Number_Of_Ghost_Cells_K, int Number_Of_Ghost_Cells_L );
        floatSerialArray & setExternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        void updateGhostBoundaries() const;
#endif

#if defined(PPP)
    //! I don't know where this is used!
        floatSerialArray( const float* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
    //! I don't know where this is used!
        floatSerialArray( const float* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
#endif

     private:
     //! Support for defered evaluation!  (This function does nothing and will be removed soon)
        void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const;

#if defined(PPP)
     //! This constructor is used by the indexing operators to build array objects defined in local scope.
     //! It allows the support of deferred evaluation on the resulting views!
        floatSerialArray ( floatSerialArray* SerialArray_Pointer , 
        	    const SerialArray_Domain_Type* Array_Domain_Pointer, 
		    bool AvoidBuildingIndirectAddressingView = FALSE );

#if 0
     //! This function is called in lazy_operand.C!
        floatSerialArray ( const floatSerialArray* SerialArray_Pointer , 
        	    floatSerialArray_Descriptor_Type* 
		    Array_Descriptor_Pointer ,
        	    /*SerialArray_Descriptor_Type* Array_Descriptor_Pointer ,*/
                    Operand_Storage* Array_Storage_Pointer );
#endif

    //! Internal support for parallel indexing (operator()) using Internal_Index objects
        floatSerialArray( const floatSerialArray* SerialArray_Pointer, 
                   const SerialArray_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

    //! Internal support for parallel indirect indexing (operator()) using Internal_Indirect_Addressing_Index objects
        floatSerialArray( const floatSerialArray* SerialArray_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
		   Indirect_Index_List );

    //! I don't know if this function is used!
        floatSerialArray( const floatSerialArray* SerialArray_Pointer, 
                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
#else

        floatSerialArray ( const float* Array_Data_Pointer ,
                    const SerialArray_Domain_Type* Array_Domain_Pointer,
                    bool AvoidBuildingIndirectAddressingView = FALSE );

    public:
    //! Internal support for serial indexing (operator()) using Internal_Index objects
        floatSerialArray( const float* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

    //! Internal support for serial indirect indexing (operator()) using Internal_Indirect_Addressing_Index objects
        floatSerialArray( const float* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

#if 0
    //! This function is called in lazy_operand.C!
        floatSerialArray ( const float* Array_Data_Pointer,
	   floatSerialArray_Descriptor_Type* Array_Descriptor_Pointer,
           Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
        static float Abstract_Unary_Operator ( const floatSerialArray & X ,
                  MDI_float_Prototype_14 , int Operation_Type ); 

        static floatSerialArray & Abstract_Unary_Operator ( const floatSerialArray & X ,
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
        static floatSerialArray & Abstract_Binary_Operator ( 
                  const floatSerialArray & Lhs , const floatSerialArray & Rhs ,
                  MDI_float_Prototype_2 , MDI_float_Prototype_3 ,
                  int Operation_Type );

        static floatSerialArray & Abstract_Binary_Operator ( 
                  const floatSerialArray & Lhs , float x ,
                  MDI_float_Prototype_4 , MDI_float_Prototype_5 ,
                  int Operation_Type );
 
        static floatSerialArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const floatSerialArray & Lhs , const floatSerialArray & Rhs ,
                  MDI_float_Prototype_2 , MDI_float_Prototype_3 ,
                  int Operation_Type );

        static floatSerialArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const floatSerialArray & Lhs , float x ,
                  MDI_float_Prototype_4 , MDI_float_Prototype_5 ,
                  int Operation_Type );
 
     // Used for replace function since it is of form "floatSerialArray replace (intSerialArray,floatSerialArray)"
        floatSerialArray & Abstract_Modification_Operator ( 
                  const intSerialArray & Lhs , const floatSerialArray & Rhs ,
                  MDI_float_Prototype_6 ,
                  int Operation_Type ); 

     // Used for replace function since it is of form "floatSerialArray replace (intSerialArray,float)"
        floatSerialArray & Abstract_Modification_Operator ( 
                  const intSerialArray & Lhs , float x ,
                  MDI_float_Prototype_7 ,
                  int Operation_Type ); 

#ifndef INTARRAY
     // Used for replace function since it is of form "floatSerialArray replace (int,floatSerialArray)"
        floatSerialArray & Abstract_Modification_Operator ( 
                  const floatSerialArray & Lhs , int x ,
                  MDI_float_Prototype_8 ,
                  int Operation_Type ); 
#endif

        static void Abstract_Operator_Operation_Equals ( 
                  const floatSerialArray & Lhs , const floatSerialArray & Rhs ,
                  MDI_float_Prototype_3 ,
                  int Operation_Type ); 

        static void Abstract_Operator_Operation_Equals ( 
                  const floatSerialArray & Lhs , float x ,
                  MDI_float_Prototype_5 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Unary_Operator_Returning_IntArray ( const floatSerialArray & X ,
                  MDI_float_Prototype_9 , MDI_float_Prototype_10 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const floatSerialArray & Lhs , const floatSerialArray & Rhs ,
                  MDI_float_Prototype_11 , MDI_float_Prototype_9 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const floatSerialArray & Lhs , float x ,
                  MDI_float_Prototype_12 , MDI_float_Prototype_13 ,
                  int Operation_Type );

     // Support for conversion operators
        static intSerialArray & Abstract_int_Conversion_Operator (
                  const floatSerialArray & X ,
                  MDI_float_Prototype_convertTo_intArray ,
                  int Operation_Type );

        static floatSerialArray & Abstract_float_Conversion_Operator (
                  const floatSerialArray & X ,
                  MDI_float_Prototype_convertTo_floatArray ,
                  int Operation_Type );

        static doubleSerialArray & Abstract_double_Conversion_Operator (
                  const floatSerialArray & X ,
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
        static floatSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
		  const floatArray & X_ParallelArray, 
		  floatSerialArray* X_Serial_PCE_Array, 
		  floatSerialArray & X_SerialArray );

        static floatSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
     //           floatArray & Lhs, floatArray & Rhs, 
                  const floatArray & Lhs, const floatArray & Rhs, 
                  floatSerialArray* Lhs_Serial_PCE_Array, floatSerialArray* Rhs_Serial_PCE_Array, 
                  floatSerialArray & X_SerialArray );

     // These operators are special because they don't generate temporaries
     // Used in array to scalar versions of operator+= operator-= operato*= operator/= operator%=
        static floatSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , const floatSerialArray & This_ParallelArray, floatSerialArray* This_Serial_PCE_Array, const floatSerialArray & This_SerialArray );
     // Used in array to array versions of operator+= operator-= operato*= operator/= operator%=
        static floatSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatSerialArray & This_ParallelArray, const floatSerialArray & Lhs, 
                  floatSerialArray* This_Serial_PCE_Array, floatSerialArray* Lhs_Serial_PCE_Array, 
                  const floatSerialArray & X_SerialArray );

#if !defined(INTARRAY)
     // relational operators operator&& operator|| operator<= operator< etc.
        static intSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatSerialArray & X_ParallelArray, 
                  floatSerialArray* X_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

        static intSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatSerialArray & Lhs, const floatSerialArray & Rhs, 
                  floatSerialArray* Lhs_Serial_PCE_Array, 
		  floatSerialArray* Rhs_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

     // replace operators for array to array and array to scalar
        static floatSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatSerialArray & This_ParallelArray, const intSerialArray & Lhs, 
                  floatSerialArray* This_Serial_PCE_Array, intSerialArray* Lhs_Serial_PCE_Array, 
                  const floatSerialArray & X_SerialArray );
        static floatSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const floatSerialArray & This_ParallelArray, const intSerialArray & Lhs, const floatSerialArray & Rhs, 
                  floatSerialArray* This_Serial_PCE_Array, 
                  intSerialArray* Lhs_Serial_PCE_Array,
                  floatSerialArray* Rhs_Serial_PCE_Array, 
                  const floatSerialArray & X_SerialArray );
#endif

     // Abstract operator support for conversion operators
        static intSerialArray & Abstract_int_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const floatSerialArray & X_ParallelArray,
                  floatSerialArray* X_Serial_PCE_Array,
                  intSerialArray & X_SerialArray );

        static floatSerialArray & Abstract_float_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const floatSerialArray & X_ParallelArray,
                  floatSerialArray* X_Serial_PCE_Array,
                  floatSerialArray & X_SerialArray );

        static doubleSerialArray & Abstract_double_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set ,
                  const floatSerialArray & X_ParallelArray,
                  floatSerialArray* X_Serial_PCE_Array,
                  doubleSerialArray & X_SerialArray );


#else
#error Must DEFINE either APP || SERIAL_APP || PPP
#endif

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif
 
        static floatSerialArray*  Build_Temporary_By_Example ( const intSerialArray    & Lhs );
        static floatSerialArray*  Build_Temporary_By_Example ( const floatSerialArray  & Lhs );
        static floatSerialArray*  Build_Temporary_By_Example ( const doubleSerialArray & Lhs );
        static floatSerialArray & Build_New_Array_Or_Reuse_Operand ( const floatSerialArray & Lhs , const floatSerialArray & Rhs , 
                                                              Memory_Source_Type & Result_Array_Memory );
        static floatSerialArray & Build_New_Array_Or_Reuse_Operand ( const floatSerialArray & X , 
                                                              Memory_Source_Type & Result_Array_Memory );

     // GNU g++ compiler likes to see these marked as inlined if they are in the header file as well.
     // (11/6/2000) Moved Delete_If_Temporary to array.C
     // friend inline void Delete_If_Temporary     ( const floatSerialArray & X );
        friend void Delete_If_Temporary     ( const floatSerialArray & X );

     // (11/24/2000) Moved Delete_Lhs_If_Temporary to array.C
     // friend inline void Delete_Lhs_If_Temporary ( const floatSerialArray & Lhs );
        friend void Delete_Lhs_If_Temporary ( const floatSerialArray & Lhs );

     // Raw memory allocation and dealocation located in these functions
        void Delete_Array_Data ();

     // void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) const;
        void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) ;

#if defined(PPP)
     // void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) const;
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) ;

     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( floatSerialArray* This_Pointer , floatSerialArray* Lhs , floatSerialArray* Rhs );
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
     //    ( floatSerialArray* This_Pointer , intSerialArray* Lhs , floatSerialArray* Rhs );
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

        floatSerialArray & operator= ( float x );
        floatSerialArray & operator= ( const floatSerialArray & Rhs );

     // Access to pointer specific to expression template indexing
        inline float* APP_RESTRICT_MACRO getExpressionTemplateDataPointer () const { return Array_Descriptor.getExpressionTemplateDataPointer(); }

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.h HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
        floatSerialArray(const Indexable<T1, A> &Rhs);

        template <class T1, class A>
        floatSerialArray & operator=(const Indexable<T1, A> & Rhs);

     // We comment this out for now while we get the
     // operator=(const Indexable<T1, A> &rhs) working (above)
        template <class T1, class A>
        floatSerialArray & operator+=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        floatSerialArray & operator-=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        floatSerialArray & operator*=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        floatSerialArray & operator/=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        floatSerialArray & operator%=(const Indexable<T1, A> &Rhs);
#else
        floatSerialArray(const InAssign<float> &a);

        floatSerialArray & operator=(const InAssign<float> &Rhs);
        floatSerialArray & operator+=(const InAssign<float> &Rhs);
        floatSerialArray & operator-=(const InAssign<float> &Rhs);
        floatSerialArray & operator*=(const InAssign<float> &Rhs);
        floatSerialArray & operator/=(const InAssign<float> &Rhs);
        floatSerialArray & operator%=(const InAssign<float> &Rhs);

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif
   };


#undef FLOATARRAY

#define INTARRAY
// Default base and bound object for use in intSerialArray constructor
extern Range APP_Unit_Range;



// Forward class declarations
class doubleSerialArray_Function_Steal_Data;
class doubleSerialArray_Function_0;
class doubleSerialArray_Function_1;
class doubleSerialArray_Function_2;
class doubleSerialArray_Function_3;
class doubleSerialArray_Function_4;
class doubleSerialArray_Function_5;
class doubleSerialArray_Function_6;
class doubleSerialArray_Function_7;
class doubleSerialArray_Function_8;
class doubleSerialArray_Function_9;
class doubleSerialArray_Function_10;
class doubleSerialArray_Function_11;
class doubleSerialArray_Function_12;
class doubleSerialArray_Function_13;
class doubleSerialArray_Function_14;
class doubleSerialArray_Function_15;
class doubleSerialArray_Function_16;

class doubleSerialArray_Aggregate_Operator;

// Forward class declarations
class floatSerialArray_Function_Steal_Data;
class floatSerialArray_Function_0;
class floatSerialArray_Function_1;
class floatSerialArray_Function_2;
class floatSerialArray_Function_3;
class floatSerialArray_Function_4;
class floatSerialArray_Function_5;
class floatSerialArray_Function_6;
class floatSerialArray_Function_7;
class floatSerialArray_Function_8;
class floatSerialArray_Function_9;
class floatSerialArray_Function_10;
class floatSerialArray_Function_11;
class floatSerialArray_Function_12;
class floatSerialArray_Function_13;
class floatSerialArray_Function_14;
class floatSerialArray_Function_15;
class floatSerialArray_Function_16;

class floatSerialArray_Aggregate_Operator;

// Forward class declarations
class intSerialArray_Function_Steal_Data;
class intSerialArray_Function_0;
class intSerialArray_Function_1;
class intSerialArray_Function_2;
class intSerialArray_Function_3;
class intSerialArray_Function_4;
class intSerialArray_Function_5;
class intSerialArray_Function_6;
class intSerialArray_Function_7;
class intSerialArray_Function_8;
class intSerialArray_Function_9;
class intSerialArray_Function_10;
class intSerialArray_Function_11;
class intSerialArray_Function_12;
class intSerialArray_Function_13;
class intSerialArray_Function_14;
class intSerialArray_Function_15;
class intSerialArray_Function_16;

class intSerialArray_Aggregate_Operator;


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
class BaseSerialArray
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
       // virtual ~BaseSerialArray();
       // virtual ~BaseSerialArray() = 0;
          BaseSerialArray();
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
          virtual SerialArray_Domain_Type* getDomainPointer () = 0;
       // virtual SerialArray_Descriptor_Type *& getArrayDomainReference () const = 0;
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

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List )
           : Array_Descriptor(X,Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List )
           : Array_Descriptor(X,Indirect_Index_List) {}

        InArray ( const doubleArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const floatArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
                  bool AvoidBuildingIndirectAddressingView )
           : Array_Descriptor(X,AvoidBuildingIndirectAddressingView) {}
        InArray ( const intArray* InputArrayPointer, const SerialArray_Domain_Type & X ,
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
        TSerialArray_Descriptor_Type Array_Descriptor;
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
#define EXPRESSION_TEMPLATES_DERIVED_int_CLASS_SUPPORT : public BaseSerialArray
#endif


// #################################################################
// ######  Definition of the principal object within A++/P++  ######
// #################################################################

class intSerialArray EXPRESSION_TEMPLATES_DERIVED_int_CLASS_SUPPORT
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
     friend class intSerialArray_Operand_Storage;

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
friend class doubleSerialArray_Function_Steal_Data;
friend class doubleSerialArray_Function_0;
friend class doubleSerialArray_Function_1;
friend class doubleSerialArray_Function_2;
friend class doubleSerialArray_Function_3;
friend class doubleSerialArray_Function_4;
friend class doubleSerialArray_Function_5;
friend class doubleSerialArray_Function_6;
friend class doubleSerialArray_Function_7;
friend class doubleSerialArray_Function_8;
friend class doubleSerialArray_Function_9;
friend class doubleSerialArray_Function_10;
friend class doubleSerialArray_Function_11;
friend class doubleSerialArray_Function_12;
friend class doubleSerialArray_Function_13;
friend class doubleSerialArray_Function_14;
friend class doubleSerialArray_Function_15;
friend class doubleSerialArray_Function_16;

friend class doubleSerialArray_Aggregate_Operator;

// Forward class declarations
friend class floatSerialArray_Function_Steal_Data;
friend class floatSerialArray_Function_0;
friend class floatSerialArray_Function_1;
friend class floatSerialArray_Function_2;
friend class floatSerialArray_Function_3;
friend class floatSerialArray_Function_4;
friend class floatSerialArray_Function_5;
friend class floatSerialArray_Function_6;
friend class floatSerialArray_Function_7;
friend class floatSerialArray_Function_8;
friend class floatSerialArray_Function_9;
friend class floatSerialArray_Function_10;
friend class floatSerialArray_Function_11;
friend class floatSerialArray_Function_12;
friend class floatSerialArray_Function_13;
friend class floatSerialArray_Function_14;
friend class floatSerialArray_Function_15;
friend class floatSerialArray_Function_16;

friend class floatSerialArray_Aggregate_Operator;

// Forward class declarations
friend class intSerialArray_Function_Steal_Data;
friend class intSerialArray_Function_0;
friend class intSerialArray_Function_1;
friend class intSerialArray_Function_2;
friend class intSerialArray_Function_3;
friend class intSerialArray_Function_4;
friend class intSerialArray_Function_5;
friend class intSerialArray_Function_6;
friend class intSerialArray_Function_7;
friend class intSerialArray_Function_8;
friend class intSerialArray_Function_9;
friend class intSerialArray_Function_10;
friend class intSerialArray_Function_11;
friend class intSerialArray_Function_12;
friend class intSerialArray_Function_13;
friend class intSerialArray_Function_14;
friend class intSerialArray_Function_15;
friend class intSerialArray_Function_16;

friend class intSerialArray_Aggregate_Operator;

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
     // SerialArray_Descriptor_Type<int,MAX_ARRAY_DIMENSION> Array_Descriptor;
        intSerialArray_Descriptor_Type Array_Descriptor;
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
     // SerialArray_Descriptor_Type* getDescriptorPointer () const;
     // SerialArray_Descriptor_Type* getDomainPointer () const;
     // SerialArray_Domain_Type* getDomainPointer () const;

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
        inline const intSerialArray_Descriptor_Type & getDescriptor () const
             { return Array_Descriptor; };

     // Access function for Descriptor
        inline const intSerialArray_Descriptor_Type* getDescriptorPointer () const
             { return &Array_Descriptor; };

        inline const SerialArray_Domain_Type & getDomain () const
           { return Array_Descriptor.Array_Domain; };

     // inline const SerialArray_Domain_Type* getDomainPointer () const
        inline const SerialArray_Domain_Type* getDomainPointer () const
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
        virtual intSerialArray* getintArrayPointer() const
           {
          // printf ("Calling doubleArray* getintArrayPointer() \n");
             return (intSerialArray*) this;
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
        static intSerialArray *Current_Link;
        intSerialArray *freepointer;
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
             { ((intSerialArray*) this)->referenceCount++; }
          inline void decrementReferenceCount() const
             { ((intSerialArray*) this)->referenceCount--; }

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
            // SerialArray_Descriptor_Type::Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // APP_Array_Reference_Count_Array [Array_Descriptor.Array_ID] = 0;
            // SerialArray_Domain_Type::Array_Reference_Count_Array [Array_Descriptor.Array_Domain.Array_ID] = 0;
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
               return intSerialArray_Descriptor_Type::getReferenceCountBase(); 
             }

          inline static int getRawDataReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
            // return 0;
               return intSerialArray_Descriptor_Type::getRawDataReferenceCountBase(); 
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
          intSerialArray & displayReferenceCounts ( const char* label = "" ) const;

#if defined(PPP) && defined(USE_PADRE)
          void setLocalDomainInPADRE_Descriptor ( SerialArray_Domain_Type *inputDomain ) const;
#endif

#if 0
          inline void ReferenceCountedDelete ()
             {
            // This is a dangerous sort of function since it deletes the class which this
            // function is a member of!.  As long as we don't do anything after calling
            // delete the function just returns and seem to work fine.
               printf ("ERROR: intSerialArray::ReferenceCountedDelete called! \n");
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

     // Used to free memory-in-use internally in intSerialArray objects!
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();
        static int numberOfInternalArrays();

#if defined(INLINE_FUNCTIONS)
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     // Default Destructor and Constructor
        virtual ~intSerialArray ();
        intSerialArray ();

     // Constructors

     // Seperate constructors required to avoid ambiguous call errors in call resolution 
     // between the different types

     // ======================================================
        intSerialArray ( int i );
#if (MAX_ARRAY_DIMENSION >= 2)
        intSerialArray ( int i , int j );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        intSerialArray ( int i , int j , int k );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        intSerialArray ( int i , int j , int k , int l );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        intSerialArray ( int i , int j , int k , int l , int m );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        intSerialArray ( int i , int j , int k , int l , int m, int n );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        intSerialArray ( int i , int j , int k , int l , int m, int n, int o );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        intSerialArray ( int i , int j , int k , int l , int m, int n, int o, int p);
#endif

     // ======================================================

        intSerialArray ( ARGUMENT_LIST_MACRO_INTEGER, bool Force_Memory_Allocation  );

     // ======================================================
#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif

#if defined(APP) || defined(PPP)

     // ======================================================
        intSerialArray ( int i , const Partitioning_Type & Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
        intSerialArray ( int i , int j , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        intSerialArray ( int i , int j , int k , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        intSerialArray ( int i , int j , int k , int l , const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        intSerialArray ( int i , int j , int k , int l , int m, const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        intSerialArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        intSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        intSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    int p, const Partitioning_Type & Partition );
#endif

     // ======================================================
#if (MAX_ARRAY_DIMENSION >= 2)
        intSerialArray ( int i , int j , const Partitioning_Type & Partition,
	            bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        intSerialArray ( int i , int j , int k , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        intSerialArray ( int i , int j , int k , int l , 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        intSerialArray ( int i , int j , int k , int l , int m, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation  );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        intSerialArray ( int i , int j , int k , int l , int m, int n, 
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        intSerialArray ( int i , int j , int k , int l , int m, int n, int o,
		    const Partitioning_Type & Partition, 
		    bool Force_Memory_Allocation );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        intSerialArray ( int i , int j , int k , int l , int m, int n, int o,
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
     // intSerialArray ( const Range & I_Base_Bound , 
     //             const Range & J_Base_Bound = APP_Unit_Range , 
     //             const Range & K_Base_Bound = APP_Unit_Range , 
     //             const Range & L_Base_Bound = APP_Unit_Range );
        intSerialArray ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // These are for internal use and allow initialization from an array of data
        intSerialArray ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intSerialArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

#if defined(PPP) || defined (APP)
     // Added to support resize function used with an existing partitioning object
        intSerialArray ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List,
                    const Internal_Partitioning_Type & partition );
#endif

        intSerialArray ( int Number_Of_Valid_Dimensions, const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // This constructor avoids an ambiguous call to the copy constructor 
     // (since a intSerialArray could be built from a Range and an int is the same as a bool)
#if (MAX_ARRAY_DIMENSION >= 2)
        intSerialArray ( const Range & I_Base_Bound , int j );
#endif

#if defined(PPP)
     // Construct a intSerialArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // Additionally for P++ the local partition must be provided.  Due to restrictions on the
     // type of partitioning supported (i.e. FORTRAN D Partitioning: BLOCK PARTI partitioning)
     // a specific partitoning might not conform to what P++ can presently handle.  The result
     // is an error.  So it is not a good idea to use this function in the parallel environment.
     // intSerialArray::intSerialArray ( int* Data_Pointer , int i , const Range & Range_I , 
     //                                           int j , const Range & Range_J ,
     //                                           int k , const Range & Range_K ,
     //                                           int l , const Range & Range_L );
     // intSerialArray::intSerialArray ( const int* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        intSerialArray ( const int* Data_Pointer , 
                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );

#if 0
     // We can't use this member function and remain dimension independent (so it is removed for now)
        intSerialArray ( int* Data_Pointer , const Range & Span_I , const Range & Range_I , 
                                                  const Range & Span_J , const Range & Range_J ,
                                                  const Range & Span_K , const Range & Range_K ,
                                                  const Range & Span_L , const Range & Range_L );
#endif
#else
     // Construct a intSerialArray from a raw FORTRAN array (but the length along each axis must be provided)!
     // intSerialArray ( int* Data_Pointer , int i , int j = 1 , int k = 1 , int l = 1 );
        intSerialArray ( const int* Data_Pointer , ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );

     // intSerialArray ( int* Data_Pointer , const Range & Span_I , 
     //             const Range & Span_J = APP_Unit_Range , 
     //             const Range & Span_K = APP_Unit_Range , 
     //             const Range & Span_L = APP_Unit_Range );
        intSerialArray ( const int* Data_Pointer , ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
#endif

     // Constructors to support deffered evaluation of array expressions! (these should be private, I think)
     // I'm not certain these are still used!
        intSerialArray ( int Operation_Code , intSerialArray *Lhs_Pointer );
        intSerialArray ( int Operation_Code , intSerialArray *Lhs_Pointer , intSerialArray *Rhs_Pointer );

     // Copy Constructors
        intSerialArray ( const intSerialArray & X , int Type_Of_Copy = DEEPCOPY );

#if defined(APP) || defined(SERIAL_APP)
     // Constructor for initialization from a string (We built the AppString type to avoid autopromotion problems)
     // intSerialArray ( const char* );
        intSerialArray ( const AppString & X );
#endif

     // Evaluation operator! Evaluates an expression and asigns local scope to its result
     // useful when handing expresions to functions that pass by reference.
     // There is no peformance penalty associated with this function since no memory
     // is copied! 
        friend intSerialArray evaluate ( const intSerialArray & X );

     // reference operator (builds a view) (an M++ function)
        intSerialArray & reference ( const intSerialArray & X );
        intSerialArray & breakReference ();

        bool Is_Built_By_Defered_Evaluation() const;

     // void display ( const char *Label = "" ) const;
     // intSerialArray & display ( const char *Label = "" );
        intSerialArray & view    ( const char *Label = "" ) const;
        intSerialArray & display ( const char *Label = "" ) const;
        intSerialArray & globalDisplay ( const char *Label = "" ) const;
#if defined(USE_EXPRESSION_TEMPLATES)
        void MDI_Display() const;
#endif

     // Provided to permit easy access from dbx debugger
        friend void APP_view    ( const intSerialArray & X , const char *Label = "" );
        friend void APP_display ( const intSerialArray & X , const char *Label = "" );

     // This is a member function defined in the BaseArray class
        int Array_ID () const;

        inline void setTemporary( bool x ) const { Array_Descriptor.setTemporary(x); }
        inline bool isTemporary() const { return Array_Descriptor.isTemporary(); }
        inline bool isNullArray() const { return Array_Descriptor.isNullArray(); }
        inline bool isView()      const { return Array_Descriptor.isView(); }
        inline bool isContiguousData() const { return Array_Descriptor.isContiguousData(); }

     // Equivalent to operator= (int x)
        intSerialArray & fill ( int x );

     // Fills the array with different values given a base and stride
        intSerialArray & seqAdd ( int Base = (int) 0 , int Stride = (int) 1 );  // Start at zero and count by 1

     // This function tests for many sorts of errors that could crop up in
     // the internal usage of the intSerialArray objects -- it is a debugging tool!
        void Test_Consistency ( const char *Label = "" ) const;
        inline bool Binary_Conformable  ( const intSerialArray & X ) const;

     // Part of implementation for diagnostics -- permits more aggressive a 
     // destructive testing than Test_Consistency(). (private interface)
        void testArray();

     // Test for confomability regardless of the input!
     // Templates would simplify this since the template need not be "bound"
     // See Lippman for more details on templates (bound and unbound)!

     // Public interface
        bool isConformable ( const doubleSerialArray & X ) const;
        bool isConformable ( const floatSerialArray & X ) const;
        bool isConformable ( const intSerialArray & X ) const;

     // For internal use (though it is marked public at the moment)
        void Test_Conformability ( const doubleSerialArray & X ) const;
        void Test_Conformability ( const floatSerialArray & X ) const;
        void Test_Conformability ( const intSerialArray & X ) const;

#if defined(INTARRAY)
     // intSerialArray ( const Index & X );
     //  ... (7/20/98,kdb) function below is commented out in src code
     //   so remove here to avoid linking problems ...
     // intSerialArray ( const Internal_Index & X );
        intSerialArray & operator=( const Internal_Index & X );
        intSerialArray & indexMap ();
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

     // Forces one dimensional interpretation of multidimensional intSerialArray!
        intSerialArray operator() () const;

     // This operator() takes an array of pointers to Internal_Index objects (length MAX_ARRAY_DIMENSION)
        intSerialArray operator() ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;
        intSerialArray operator() ( int Number_Of_Valid_Dimensions, const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const;

     // Indexing operators for use with Index objects
     //   intSerialArray operator() 
     //( ARGUMENT_LIST_MACRO_CONST_REF_INTERNAL_INDEX_WITH_PREINITIALIZATION )
     //const ;

#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intSerialArray operator() ( const Internal_Index & I ) const;
              intSerialArray operator() ( const Internal_Index & I );
#else
        intSerialArray operator() ( const Internal_Index & I ) const;
#endif

#if (MAX_ARRAY_DIMENSION >= 2)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J );
#else
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 3)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K );
#else
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L );
#else
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 5)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M );
#else
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M ) const;
#endif
#endif

#if (MAX_ARRAY_DIMENSION >= 6)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N );
#else
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O );
#else
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N,
                               const Internal_Index & O ) const;
#endif
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
#if defined(CORRECT_CONST_IMPLEMENTATION)
        const intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P ) const;
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
                               const Internal_Index & K, const Internal_Index & L,
                               const Internal_Index & M, const Internal_Index & N
                               const Internal_Index & O, const Internal_Index & P );
#else
        intSerialArray operator() ( const Internal_Index & I, const Internal_Index & J, 
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
     // static intSerialArray* Build_Pointer_To_View_Of_Array ( const intSerialArray & X , Internal_Index *Index_Array );
        static intSerialArray* Build_Pointer_To_View_Of_Array ( const intSerialArray & X ,
                                                           const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Indexing operators for use with Index objects
        intSerialArray operator() ( const Internal_Indirect_Addressing_Index & I ) const;
#if (MAX_ARRAY_DIMENSION >= 2)
        intSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        intSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        intSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        intSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        intSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        intSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
                               const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
                               const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N,
                               const Internal_Indirect_Addressing_Index & O ) const;
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        intSerialArray operator() ( const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
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
        intSerialArray & operator+ () const;
        intSerialArray & operator- () const;

     // Prefix Increment and Decrement Operators ++A and --A
        intSerialArray & operator++ ();
        intSerialArray & operator-- ();

     // Postfix Increment and Decrement Operators A++ and A--  (argument is ZERO see 594 r.13.4.7 Stroustrup)
        intSerialArray & operator++ (int x);
        intSerialArray & operator-- (int x);

#if !defined(USE_EXPRESSION_TEMPLATES)
     // operator+ related functions
        friend intSerialArray & operator+ ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator+ ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator+ ( const intSerialArray & Lhs , int x );
        intSerialArray & operator+= ( const intSerialArray & Rhs );
        intSerialArray & operator+= ( int Rhs );

     // operator- related functions
        friend intSerialArray & operator- ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator- ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator- ( const intSerialArray & Lhs , int x );
        intSerialArray & operator-= ( const intSerialArray & Rhs );
        intSerialArray & operator-= ( int Rhs );

     // operator* related functions
        friend intSerialArray & operator* ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator* ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator* ( const intSerialArray & Lhs , int x );
        intSerialArray & operator*= ( const intSerialArray & Rhs );
        intSerialArray & operator*= ( int Rhs );

     // operator/ related functions
        friend intSerialArray & operator/ ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator/ ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator/ ( const intSerialArray & Lhs , int x );
        intSerialArray & operator/= ( const intSerialArray & Rhs );
        intSerialArray & operator/= ( int Rhs );

     // operator% related functions
        friend intSerialArray & operator% ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator% ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator% ( const intSerialArray & Lhs , int x );
        intSerialArray & operator%= ( const intSerialArray & Rhs );
        intSerialArray & operator%= ( int Rhs );

#ifdef INTARRAY
     // Unary Operator ~ (bitwise one's complement)
        intSerialArray & operator~ () const;

     // operator& related functions
        friend intSerialArray & operator& ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator& ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator& ( const intSerialArray & Lhs , int x );
        intSerialArray & operator&= ( const intSerialArray & Rhs );
        intSerialArray & operator&= ( int Rhs );

     // operator| related functions
        friend intSerialArray & operator| ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator| ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator| ( const intSerialArray & Lhs , int x );
        intSerialArray & operator|= ( const intSerialArray & Rhs );
        intSerialArray & operator|= ( int Rhs );

     // operator^ related functions
        friend intSerialArray & operator^ ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator^ ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator^ ( const intSerialArray & Lhs , int x );
        intSerialArray & operator^= ( const intSerialArray & Rhs );
        intSerialArray & operator^= ( int Rhs );

     // operator<< related functions
        friend intSerialArray & operator<< ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator<< ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator<< ( const intSerialArray & Lhs , int x );

     // operator>> related functions
        friend intSerialArray & operator>> ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator>> ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator>> ( const intSerialArray & Lhs , int x );
#endif

     // Internal conversion member functions (used in casting operators)
        intSerialArray    & convertTo_intArray    () const;
        floatSerialArray  & convertTo_floatArray  () const;
        doubleSerialArray & convertTo_doubleArray () const;

        intSerialArray & operator= ( int x );
        intSerialArray & operator= ( const intSerialArray & Rhs );
#else
     // Need these to be defined for the expression template version
        intSerialArray & operator+= ( const intSerialArray & Rhs );
        intSerialArray & operator+= ( int Rhs );

        intSerialArray & operator-= ( const intSerialArray & Rhs );
        intSerialArray & operator-= ( int Rhs );

        intSerialArray & operator*= ( const intSerialArray & Rhs );
        intSerialArray & operator*= ( int Rhs );

        intSerialArray & operator/= ( const intSerialArray & Rhs );
        intSerialArray & operator/= ( int Rhs );

        intSerialArray & operator%= ( const intSerialArray & Rhs );
        intSerialArray & operator%= ( int Rhs );
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
        intSerialArray & setBase( int New_Base_For_All_Axes );
        int getBase() const;
        int getRawBase() const;

     // Access function for static Base and Bound variables (by Axis)!
     // void setBase( int Base , int Axis );
        intSerialArray & setBase( int Base , int Axis );
        intSerialArray & setParallelBase( int Base , int Axis );
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
        intSerialArray getLocalArrayWithGhostBoundaries () const;
        intSerialArray* getLocalArrayWithGhostBoundariesPointer () const;
#endif

     // Dimension independent query functions
        bool isSameBase   ( const intSerialArray & X ) const;
        bool isSameBound  ( const intSerialArray & X ) const;
        bool isSameStride ( const intSerialArray & X ) const;
        bool isSameLength ( const intSerialArray & X ) const;
#if defined(APP) || defined(PPP)
        bool isSameGhostBoundaryWidth ( const intSerialArray & X ) const;
        //bool isSameDistribution ( const intSerialArray & X ) const;
        bool isSameDistribution ( const intSerialArray & X ) const;
        bool isSameDistribution ( const floatSerialArray & X ) const;
        bool isSameDistribution ( const doubleSerialArray & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const intSerialArray & X ) const;

     // Member functions for access to the geometry of the A++ object
        int getGeometryBase ( int Axis ) const;
        intSerialArray & setGeometryBase ( int New_Geometry_Base , int Axis );

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
     // intSerialArray & redim ( int i , int j = 1 , int k = 1 , int l = 1 );
        intSerialArray & redim ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // intSerialArray & redim ( const Range & i , 
     //                     const Range & j = APP_Unit_Range , 
     //                     const Range & k = APP_Unit_Range , 
     //                     const Range & l = APP_Unit_Range );
        intSerialArray & redim ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );

     // We need this to avoid an ERROR: Overloading ambiguity between "doubleArray::redim(const Array_Domain_Type&)" 
     // and "doubleArray::redim(const Range&, const Range&, const Range&, const Range&, const Range&, const Range&)" 
     // so we need to implement a redim that takes a single Index object.
        intSerialArray & redim ( const Index & I );
        intSerialArray & redim ( const intSerialArray & X );
        intSerialArray & redim ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intSerialArray & redim ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // The Sun C++ compiler can't handle template declarations in non templated classes
     // template<class T, int Template_Dimension>
     // intSerialArray & redim ( const SerialArray_Descriptor_Type<T,Template_Dimension> & X );
        intSerialArray & redim ( const SerialArray_Domain_Type & X );

     // Changes dimensions of array object with no change in number of elements
     // intSerialArray & reshape ( int i , int j = 1 , int k = 1 , int l = 1 );
        intSerialArray & reshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // intSerialArray & reshape ( const Range & i , 
     //                       const Range & j = APP_Unit_Range , 
     //                       const Range & k = APP_Unit_Range , 
     //                       const Range & l = APP_Unit_Range );
        intSerialArray & reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        intSerialArray & reshape ( const intSerialArray & X );
        intSerialArray & reshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intSerialArray & reshape ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Used for support of weakened conformability assignment in operator= (maybe this should be private or protected)
     // intSerialArray & internalReshape ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // intSerialArray & internalReshape ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

	void fix_view_bases (int* View_Bases, int* View_Sizes, int numdims, 
			     const int* old_lengths, const int* Integer_List);
	void fix_view_bases_non_short_span 
	   (int* View_Bases, int* View_Sizes, int numdims, const int* Integer_List);

     // resize function redimensions array and copies exiting data into new array
     // intSerialArray & resize ( int i , int j = 1 , int k = 1 , int l = 1 );
        intSerialArray & resize ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
     // intSerialArray & resize ( const Range & i , 
     //                      const Range & j = APP_Unit_Range , 
     //                      const Range & k = APP_Unit_Range , 
     //                      const Range & l = APP_Unit_Range );
        intSerialArray & resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        intSerialArray & resize ( const intSerialArray & X );
        intSerialArray & resize ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intSerialArray & resize ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // adopt existing data as an A++ array
     // versions of adopt which are specific to P++
     // To avoid the 1D P++ adopt function from being confused with the
     // 2D A++ adopt function we must provide different member function names.
        intSerialArray & adopt ( const int* Data_Pointer , const Partitioning_Type & Partition ,
                                    ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION );
        intSerialArray & adopt ( const int* Data_Pointer , const Partitioning_Type & Partition ,
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );
     // intSerialArray & adopt ( const int* Data_Pointer , const Partitioning_Type & Partition ,
     //                     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // A++ specific implementation of adopt member functions
     // These A++ specific function are made available to P++ to maintain
     // the identical interface even though under P++ these functions
     // are not implemented.

     // intSerialArray & adopt ( const int* Data_Pointer , 
     //                     int i , int j = 1 , int k = 1 , int l = 1 ); 
     // function with interface for internal use only
        intSerialArray & adopt ( const int* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION );
        intSerialArray & adopt ( const int* Data_Pointer , 
                            const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intSerialArray & adopt ( const int* Data_Pointer , 
                            ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION );
     // Use the size of an existing A++/P++ array object
        intSerialArray & adopt ( const int* Data_Pointer , const intSerialArray & X );

     // These are used only within the internal implementation of A++/P++
        intSerialArray & adopt ( const int* Data_Pointer , 
                            const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // Force a deep copy since deep copy constructors are not alway called
     // even when explicit attempts are made to call them 
        intSerialArray & copy ( const intSerialArray & X , int DeepCopy = DEEPCOPY );

     // Convert indexing of view to be non HPF!
        intSerialArray & useFortranIndexing ( bool Flag );

     // This would effectively be a reference (so call reference instead)!
     // intSerialArray & adopt ( const intSerialArray & X  );

     // Reduction operators!
        friend int min ( const intSerialArray & X );
        friend int max ( const intSerialArray & X );

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
        friend intSerialArray min ( const intSerialArray & X , const intSerialArray & Y );
        friend intSerialArray min ( const intSerialArray & X , int y );
        friend intSerialArray min ( int x , const intSerialArray & Y );
        friend intSerialArray min ( const intSerialArray & X , const intSerialArray & Y , const intSerialArray & Z );
        friend intSerialArray min ( int x , const intSerialArray & Y , const intSerialArray & Z );
        friend intSerialArray min ( const intSerialArray & X , int y , const intSerialArray & Z );
        friend intSerialArray min ( const intSerialArray & X , const intSerialArray & Y , int z );

        friend intSerialArray max ( const intSerialArray & X , const intSerialArray & Y );
        friend intSerialArray max ( const intSerialArray & X , int y );
        friend intSerialArray max ( int x , const intSerialArray & Y );
        friend intSerialArray max ( const intSerialArray & X , const intSerialArray & Y , const intSerialArray & Z );
        friend intSerialArray max ( int x , const intSerialArray & Y , const intSerialArray & Z );
        friend intSerialArray max ( const intSerialArray & X , int y , const intSerialArray & Z );
        friend intSerialArray max ( const intSerialArray & X , const intSerialArray & Y , int z );

     // Misc functions
        intSerialArray & replace ( const intSerialArray & X , const intSerialArray & Y );
        intSerialArray & replace ( int x , const intSerialArray & Y );
        intSerialArray & replace ( const intSerialArray & X , int y );

        friend int sum ( const intSerialArray & X );

     // Sum along axis 
        friend intSerialArray sum ( const intSerialArray & X , int Axis );
#else

     // The non-expression template version returns values by reference
        friend intSerialArray & min ( const intSerialArray & X , const intSerialArray & Y );
        friend intSerialArray & min ( const intSerialArray & X , int y );
        friend intSerialArray & min ( int x , const intSerialArray & Y );
        friend intSerialArray & min ( const intSerialArray & X , const intSerialArray & Y , const intSerialArray & Z );
        friend intSerialArray & min ( int x , const intSerialArray & Y , const intSerialArray & Z );
        friend intSerialArray & min ( const intSerialArray & X , int y , const intSerialArray & Z );
        friend intSerialArray & min ( const intSerialArray & X , const intSerialArray & Y , int z );

        friend intSerialArray & max ( const intSerialArray & X , const intSerialArray & Y );
        friend intSerialArray & max ( const intSerialArray & X , int y );
        friend intSerialArray & max ( int x , const intSerialArray & Y );
        friend intSerialArray & max ( const intSerialArray & X , const intSerialArray & Y , const intSerialArray & Z );
        friend intSerialArray & max ( int x , const intSerialArray & Y , const intSerialArray & Z );
        friend intSerialArray & max ( const intSerialArray & X , int y , const intSerialArray & Z );
        friend intSerialArray & max ( const intSerialArray & X , const intSerialArray & Y , int z );

     // Misc functions
        intSerialArray & replace ( const intSerialArray & X , const intSerialArray & Y );
        intSerialArray & replace ( int x , const intSerialArray & Y );
        intSerialArray & replace ( const intSerialArray & X , int y );

        friend int sum ( const intSerialArray & X );
        friend intSerialArray & sum ( const intSerialArray & X , const intSerialArray & Y );
        friend intSerialArray & sum ( const intSerialArray & X , const intSerialArray & Y , const intSerialArray & Z );

     // Sum along axis
        friend intSerialArray & sum ( const intSerialArray & X , int Axis );
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)
#ifndef INTARRAY
        friend intSerialArray & fmod ( const intSerialArray & X , int y );
        friend intSerialArray & fmod ( int x , const intSerialArray & Y );
        friend intSerialArray & fmod ( const intSerialArray & X , const intSerialArray & Y );
#endif
        friend intSerialArray & mod ( const intSerialArray & X , int y );
        friend intSerialArray & mod ( int x , const intSerialArray & Y );
        friend intSerialArray & mod ( const intSerialArray & X , const intSerialArray & Y );

        friend intSerialArray & pow ( const intSerialArray & X , int y );  // returns X**y
        friend intSerialArray & pow ( int x , const intSerialArray & Y );
        friend intSerialArray & pow ( const intSerialArray & X , const intSerialArray & Y );

        friend intSerialArray & sign ( const intSerialArray & Array_Signed_Value , const intSerialArray & Input_Array );
        friend intSerialArray & sign ( int Scalar_Signed_Value             , const intSerialArray & Input_Array );
        friend intSerialArray & sign ( const intSerialArray & Array_Signed_Value , int Input_Value  );

#ifndef INTARRAY
        friend intSerialArray & log   ( const intSerialArray & X );
        friend intSerialArray & log10 ( const intSerialArray & X );
        friend intSerialArray & exp   ( const intSerialArray & X );
        friend intSerialArray & sqrt  ( const intSerialArray & X );
        friend intSerialArray & fabs  ( const intSerialArray & X );
        friend intSerialArray & ceil  ( const intSerialArray & X );
        friend intSerialArray & floor ( const intSerialArray & X );
#endif
        friend intSerialArray & abs   ( const intSerialArray & X );

     // Not fully supported yet (only works for 1D or 2D arrays object)!
        friend intSerialArray & transpose ( const intSerialArray & X );

     // Trig functions!
#ifndef INTARRAY
        friend intSerialArray & cos   ( const intSerialArray & X );
        friend intSerialArray & sin   ( const intSerialArray & X );
        friend intSerialArray & tan   ( const intSerialArray & X );
        friend intSerialArray & acos  ( const intSerialArray & X );
        friend intSerialArray & asin  ( const intSerialArray & X );
        friend intSerialArray & atan  ( const intSerialArray & X );
        friend intSerialArray & cosh  ( const intSerialArray & X );
        friend intSerialArray & sinh  ( const intSerialArray & X );
        friend intSerialArray & tanh  ( const intSerialArray & X );
        friend intSerialArray & acosh ( const intSerialArray & X );
        friend intSerialArray & asinh ( const intSerialArray & X );
        friend intSerialArray & atanh ( const intSerialArray & X );

     // All the different atan2 functions for arrays (ask for by Bill Henshaw)
        friend intSerialArray & atan2 ( const intSerialArray & X , int y );
        friend intSerialArray & atan2 ( int x , const intSerialArray & Y );
        friend intSerialArray & atan2 ( const intSerialArray & X , const intSerialArray & Y );
#endif

     // relational operators 
     // operator< related functions
        friend intSerialArray & operator< ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator< ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator< ( const intSerialArray & Lhs , int x );

     // operator<= related functions
        friend intSerialArray & operator<= ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator<= ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator<= ( const intSerialArray & Lhs , int x );

     // operator> related functions
        friend intSerialArray & operator> ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator> ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator> ( const intSerialArray & Lhs , int x );

     // operator>= related functions
        friend intSerialArray & operator>= ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator>= ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator>= ( const intSerialArray & Lhs , int x );

     // operator== related functions
        friend intSerialArray & operator== ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator== ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator== ( const intSerialArray & Lhs , int x );

     // operator!= related functions
        friend intSerialArray & operator!= ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator!= ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator!= ( const intSerialArray & Lhs , int x );

     // operator&& related functions
        friend intSerialArray & operator&& ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator&& ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator&& ( const intSerialArray & Lhs , int x );

     // operator|| related functions
        friend intSerialArray & operator|| ( const intSerialArray & Lhs , const intSerialArray & Rhs );
        friend intSerialArray & operator|| ( int x    , const intSerialArray & Rhs );
        friend intSerialArray & operator|| ( const intSerialArray & Lhs , int x );
#else
     // If we ARE using expression templates then we have to declare some friend functions
     // friend int sum ( const intSerialArray & X );
     // friend intSerialArray sum ( const intSerialArray & X , int axis );

        template<class T, class A>
        friend T sum ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T max ( const Indexable<T,A> & X );
        template<class T, class A>
        friend T min ( const Indexable<T,A> & X );
#endif

     // operator! related functions
        intSerialArray & operator! ();

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
        intSerialArray & partition ( const Internal_Partitioning_Type & Partition );
        intSerialArray & partition ( const Partitioning_Type & Partition );
        intSerialArray & partition ( const doubleArray & Example_Array );
        intSerialArray & partition ( const floatArray & Example_Array );
        intSerialArray & partition ( const intArray & Example_Array );

     // typedef support for member function pointers as parameters for the map diagnostic mechanism
     // We support two different function prototypes
        typedef int (intSerialArray::*intSerialArrayMemberVoidFunctionPointerType) (void) const;
        typedef int (intSerialArray::*intSerialArrayMemberIntegerFunctionPointerType) (int) const;

     // Builds array with base/bound info for each axis and processor
     // calls special purpose communication function

#if defined(PPP)
     // This only makes since in P++
        intSerialArray buildProcessorMap ( intSerialArrayMemberVoidFunctionPointerType X );
        intSerialArray buildProcessorMap ( intSerialArrayMemberIntegerFunctionPointerType X );
        intSerialArray buildProcessorMap ( intSerialArrayMemberIntegerFunctionPointerType X, intSerialArrayMemberIntegerFunctionPointerType Y );
#else
     // Preserve the interface in A++
        intArray buildProcessorMap ( intSerialArrayMemberVoidFunctionPointerType X );
        intArray buildProcessorMap ( intSerialArrayMemberIntegerFunctionPointerType X );
        intArray buildProcessorMap ( intSerialArrayMemberIntegerFunctionPointerType X, intSerialArrayMemberIntegerFunctionPointerType Y );
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
        intSerialArray & setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intSerialArray & setInternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        intSerialArray & setInternalGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // New versions
        intSerialArray & setGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        intSerialArray & setGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        intSerialArray & setGhostCellWidthSaveData ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );

     // functions specific to the manipulation of external ghost cells in the distributed arrays
     // partition edges (even on a single processor ghost cells exist around the edge of the array)
     // these are usually zero unless the boundaries are meant to have large NON-PARTITIONED boundary regions
        int getExternalGhostCellWidth ( int Axis ) const;
     // intSerialArray & setExternalGhostCellWidth ( int Number_Of_Ghost_Cells_I, int Number_Of_Ghost_Cells_J, 
     //                                         int Number_Of_Ghost_Cells_K, int Number_Of_Ghost_Cells_L );
        intSerialArray & setExternalGhostCellWidth ( ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
        void updateGhostBoundaries() const;
#endif

#if defined(PPP)
    //! I don't know where this is used!
        intSerialArray( const int* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );
    //! I don't know where this is used!
        intSerialArray( const int* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
#endif

     private:
     //! Support for defered evaluation!  (This function does nothing and will be removed soon)
        void Add_Defered_Expression ( Expression_Tree_Node_Type *Expression_Tree_Node ) const;

#if defined(PPP)
     //! This constructor is used by the indexing operators to build array objects defined in local scope.
     //! It allows the support of deferred evaluation on the resulting views!
        intSerialArray ( intSerialArray* SerialArray_Pointer , 
        	    const SerialArray_Domain_Type* Array_Domain_Pointer, 
		    bool AvoidBuildingIndirectAddressingView = FALSE );

#if 0
     //! This function is called in lazy_operand.C!
        intSerialArray ( const intSerialArray* SerialArray_Pointer , 
        	    intSerialArray_Descriptor_Type* 
		    Array_Descriptor_Pointer ,
        	    /*SerialArray_Descriptor_Type* Array_Descriptor_Pointer ,*/
                    Operand_Storage* Array_Storage_Pointer );
#endif

    //! Internal support for parallel indexing (operator()) using Internal_Index objects
        intSerialArray( const intSerialArray* SerialArray_Pointer, 
                   const SerialArray_Domain_Type & Array_Domain_Pointer,  
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

    //! Internal support for parallel indirect indexing (operator()) using Internal_Indirect_Addressing_Index objects
        intSerialArray( const intSerialArray* SerialArray_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,  
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
		   Indirect_Index_List );

    //! I don't know if this function is used!
        intSerialArray( const intSerialArray* SerialArray_Pointer, 
                   const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
#else

        intSerialArray ( const int* Array_Data_Pointer ,
                    const SerialArray_Domain_Type* Array_Domain_Pointer,
                    bool AvoidBuildingIndirectAddressingView = FALSE );

    public:
    //! Internal support for serial indexing (operator()) using Internal_Index objects
        intSerialArray( const int* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

    //! Internal support for serial indirect indexing (operator()) using Internal_Indirect_Addressing_Index objects
        intSerialArray( const int* Array_Data_Pointer,
                   const SerialArray_Domain_Type & Array_Domain_Pointer,
                   const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

#if 0
    //! This function is called in lazy_operand.C!
        intSerialArray ( const int* Array_Data_Pointer,
	   intSerialArray_Descriptor_Type* Array_Descriptor_Pointer,
           Operand_Storage* Array_Storage_Pointer );
#endif
#endif

#if !defined(USE_EXPRESSION_TEMPLATES)

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
        static int Abstract_Unary_Operator ( const intSerialArray & X ,
                  MDI_int_Prototype_14 , int Operation_Type ); 

        static intSerialArray & Abstract_Unary_Operator ( const intSerialArray & X ,
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
        static intSerialArray & Abstract_Binary_Operator ( 
                  const intSerialArray & Lhs , const intSerialArray & Rhs ,
                  MDI_int_Prototype_2 , MDI_int_Prototype_3 ,
                  int Operation_Type );

        static intSerialArray & Abstract_Binary_Operator ( 
                  const intSerialArray & Lhs , int x ,
                  MDI_int_Prototype_4 , MDI_int_Prototype_5 ,
                  int Operation_Type );
 
        static intSerialArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const intSerialArray & Lhs , const intSerialArray & Rhs ,
                  MDI_int_Prototype_2 , MDI_int_Prototype_3 ,
                  int Operation_Type );

        static intSerialArray & Abstract_Binary_Operator_Non_Commutative ( 
                  const intSerialArray & Lhs , int x ,
                  MDI_int_Prototype_4 , MDI_int_Prototype_5 ,
                  int Operation_Type );
 
     // Used for replace function since it is of form "intSerialArray replace (intSerialArray,intSerialArray)"
        intSerialArray & Abstract_Modification_Operator ( 
                  const intSerialArray & Lhs , const intSerialArray & Rhs ,
                  MDI_int_Prototype_6 ,
                  int Operation_Type ); 

     // Used for replace function since it is of form "intSerialArray replace (intSerialArray,int)"
        intSerialArray & Abstract_Modification_Operator ( 
                  const intSerialArray & Lhs , int x ,
                  MDI_int_Prototype_7 ,
                  int Operation_Type ); 

#ifndef INTARRAY
     // Used for replace function since it is of form "intSerialArray replace (int,intSerialArray)"
        intSerialArray & Abstract_Modification_Operator ( 
                  const intSerialArray & Lhs , int x ,
                  MDI_int_Prototype_8 ,
                  int Operation_Type ); 
#endif

        static void Abstract_Operator_Operation_Equals ( 
                  const intSerialArray & Lhs , const intSerialArray & Rhs ,
                  MDI_int_Prototype_3 ,
                  int Operation_Type ); 

        static void Abstract_Operator_Operation_Equals ( 
                  const intSerialArray & Lhs , int x ,
                  MDI_int_Prototype_5 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Unary_Operator_Returning_IntArray ( const intSerialArray & X ,
                  MDI_int_Prototype_9 , MDI_int_Prototype_10 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const intSerialArray & Lhs , const intSerialArray & Rhs ,
                  MDI_int_Prototype_11 , MDI_int_Prototype_9 ,
                  int Operation_Type ); 

        static intSerialArray & Abstract_Binary_Operator_Returning_IntArray ( 
                  const intSerialArray & Lhs , int x ,
                  MDI_int_Prototype_12 , MDI_int_Prototype_13 ,
                  int Operation_Type );

     // Support for conversion operators
        static intSerialArray & Abstract_int_Conversion_Operator (
                  const intSerialArray & X ,
                  MDI_int_Prototype_convertTo_intArray ,
                  int Operation_Type );

        static floatSerialArray & Abstract_float_Conversion_Operator (
                  const intSerialArray & X ,
                  MDI_int_Prototype_convertTo_floatArray ,
                  int Operation_Type );

        static doubleSerialArray & Abstract_double_Conversion_Operator (
                  const intSerialArray & X ,
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
        static intSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
		  const intArray & X_ParallelArray, 
		  intSerialArray* X_Serial_PCE_Array, 
		  intSerialArray & X_SerialArray );

        static intSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
     //           intArray & Lhs, intArray & Rhs, 
                  const intArray & Lhs, const intArray & Rhs, 
                  intSerialArray* Lhs_Serial_PCE_Array, intSerialArray* Rhs_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

     // These operators are special because they don't generate temporaries
     // Used in array to scalar versions of operator+= operator-= operato*= operator/= operator%=
        static intSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , const intSerialArray & This_ParallelArray, intSerialArray* This_Serial_PCE_Array, const intSerialArray & This_SerialArray );
     // Used in array to array versions of operator+= operator-= operato*= operator/= operator%=
        static intSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intSerialArray & This_ParallelArray, const intSerialArray & Lhs, 
                  intSerialArray* This_Serial_PCE_Array, intSerialArray* Lhs_Serial_PCE_Array, 
                  const intSerialArray & X_SerialArray );

#if !defined(INTARRAY)
     // relational operators operator&& operator|| operator<= operator< etc.
        static intSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intSerialArray & X_ParallelArray, 
                  intSerialArray* X_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

        static intSerialArray & Abstract_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intSerialArray & Lhs, const intSerialArray & Rhs, 
                  intSerialArray* Lhs_Serial_PCE_Array, 
		  intSerialArray* Rhs_Serial_PCE_Array, 
                  intSerialArray & X_SerialArray );

     // replace operators for array to array and array to scalar
        static intSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intSerialArray & This_ParallelArray, const intSerialArray & Lhs, 
                  intSerialArray* This_Serial_PCE_Array, intSerialArray* Lhs_Serial_PCE_Array, 
                  const intSerialArray & X_SerialArray );
        static intSerialArray & Abstract_Modification_Operator ( 
                  Array_Conformability_Info_Type *Array_Set , 
                  const intSerialArray & This_ParallelArray, const intSerialArray & Lhs, const intSerialArray & Rhs, 
                  intSerialArray* This_Serial_PCE_Array, 
                  intSerialArray* Lhs_Serial_PCE_Array,
                  intSerialArray* Rhs_Serial_PCE_Array, 
                  const intSerialArray & X_SerialArray );
#endif

     // Abstract operator support for conversion operators
        static intSerialArray & Abstract_int_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const intSerialArray & X_ParallelArray,
                  intSerialArray* X_Serial_PCE_Array,
                  intSerialArray & X_SerialArray );

        static floatSerialArray & Abstract_float_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set,
                  const intSerialArray & X_ParallelArray,
                  intSerialArray* X_Serial_PCE_Array,
                  floatSerialArray & X_SerialArray );

        static doubleSerialArray & Abstract_double_Conversion_Operator (
                  Array_Conformability_Info_Type *Array_Set ,
                  const intSerialArray & X_ParallelArray,
                  intSerialArray* X_Serial_PCE_Array,
                  doubleSerialArray & X_SerialArray );


#else
#error Must DEFINE either APP || SERIAL_APP || PPP
#endif

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif
 
        static intSerialArray*  Build_Temporary_By_Example ( const intSerialArray    & Lhs );
        static intSerialArray*  Build_Temporary_By_Example ( const floatSerialArray  & Lhs );
        static intSerialArray*  Build_Temporary_By_Example ( const doubleSerialArray & Lhs );
        static intSerialArray & Build_New_Array_Or_Reuse_Operand ( const intSerialArray & Lhs , const intSerialArray & Rhs , 
                                                              Memory_Source_Type & Result_Array_Memory );
        static intSerialArray & Build_New_Array_Or_Reuse_Operand ( const intSerialArray & X , 
                                                              Memory_Source_Type & Result_Array_Memory );

     // GNU g++ compiler likes to see these marked as inlined if they are in the header file as well.
     // (11/6/2000) Moved Delete_If_Temporary to array.C
     // friend inline void Delete_If_Temporary     ( const intSerialArray & X );
        friend void Delete_If_Temporary     ( const intSerialArray & X );

     // (11/24/2000) Moved Delete_Lhs_If_Temporary to array.C
     // friend inline void Delete_Lhs_If_Temporary ( const intSerialArray & Lhs );
        friend void Delete_Lhs_If_Temporary ( const intSerialArray & Lhs );

     // Raw memory allocation and dealocation located in these functions
        void Delete_Array_Data ();

     // void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) const;
        void Allocate_Array_Data ( bool Force_Memory_Allocation = TRUE ) ;

#if defined(PPP)
     // void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) const;
        void Allocate_Parallel_Array ( bool Force_Memory_Allocation ) ;

     // static Array_Conformability_Info_Type *Parallel_Conformability_Enforcement 
     //    ( intSerialArray* This_Pointer , intSerialArray* Lhs , intSerialArray* Rhs );
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
     //    ( intSerialArray* This_Pointer , intSerialArray* Lhs , intSerialArray* Rhs );
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

        intSerialArray & operator= ( int x );
        intSerialArray & operator= ( const intSerialArray & Rhs );

     // Access to pointer specific to expression template indexing
        inline int* APP_RESTRICT_MACRO getExpressionTemplateDataPointer () const { return Array_Descriptor.getExpressionTemplateDataPointer(); }

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.h HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
        intSerialArray(const Indexable<T1, A> &Rhs);

        template <class T1, class A>
        intSerialArray & operator=(const Indexable<T1, A> & Rhs);

     // We comment this out for now while we get the
     // operator=(const Indexable<T1, A> &rhs) working (above)
        template <class T1, class A>
        intSerialArray & operator+=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        intSerialArray & operator-=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        intSerialArray & operator*=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        intSerialArray & operator/=(const Indexable<T1, A> &Rhs);
        template <class T1, class A>
        intSerialArray & operator%=(const Indexable<T1, A> &Rhs);
#else
        intSerialArray(const InAssign<int> &a);

        intSerialArray & operator=(const InAssign<int> &Rhs);
        intSerialArray & operator+=(const InAssign<int> &Rhs);
        intSerialArray & operator-=(const InAssign<int> &Rhs);
        intSerialArray & operator*=(const InAssign<int> &Rhs);
        intSerialArray & operator/=(const InAssign<int> &Rhs);
        intSerialArray & operator%=(const InAssign<int> &Rhs);

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif
   };


#undef INTARRAY
/* MACRO EXPANSION ENDS HERE */

// If we are compiling P++ then check to see if COMPILE_SERIAL_APP is defined 
// indicating that we are compiling for the serial array part of P++ else
// we are compiling for the parallel (non-serial) part of P++
#if defined(COMPILE_PPP)
// error "COMPILE_PPP in P++/include/A++.h checking COMPILE_SERIAL_APP to activate SERIAL_APP or PPP"
// define COMPILE_SERIAL_APP
#if defined(COMPILE_SERIAL_APP)
// error "defining SERIAL_APP!"
#define SERIAL_APP
#undef PPP
#else
// error "defining PPP!"
#define PPP
#undef SERIAL_APP
#endif
#endif

// Before we INCLUDE the where header DEFINE the P++ array objects and operations
#include <P++.h>

#if 0
// START: Evaluate the macros that drive the compilation
#if defined(COMPILE_APP)
#error "In P++/include/A++.h (Before where.h): COMPILE_APP is defined"
#else
#error "In P++/include/A++.h (Before where.h): COMPILE_APP is NOT defined"
#endif

#if defined(COMPILE_SERIAL_APP)
#error "In P++/include/A++.h (Before where.h): COMPILE_SERIAL_APP is defined"
#else
#error "In P++/include/A++.h (Before where.h): COMPILE_SERIAL_APP is NOT defined"
#endif

#if defined(COMPILE_PPP)
#error "In P++/include/A++.h (Before where.h): COMPILE_PPP is defined"
#else
#error "In P++/include/A++.h (Before where.h): COMPILE_PPP is NOT defined"
#endif

#if defined(SERIAL_APP)
#error "In P++/include/A++.h (Before where.h): SERIAL_APP is defined"
#else
#error "In P++/include/A++.h (Before where.h): SERIAL_APP is NOT defined"
#endif

#if defined(PPP)
#error "In P++/include/A++.h (Before where.h): PPP is defined"
#else
#error "In P++/include/A++.h (Before where.h): PPP is NOT defined"
#endif

#if COMPILE_APP
#error "In P++/include/A++.h (Before where.h): COMPILE_APP is TRUE"
#else
#error "In P++/include/A++.h (Before where.h): COMPILE_APP is FALSE"
#endif

#if COMPILE_PPP
#error "In P++/include/A++.h (Before where.h): COMPILE_PPP is TRUE"
#else
#error "In P++/include/A++.h (Before where.h): COMPILE_PPP is FALSE"
#endif

// END: Evaluate the macros that drive the compilation
#endif


// Define macros for parallel I/O
#ifndef GNU
// define scanf Parallel_Scanf
#endif

#if defined(USE_PARALLEL_PRINTF)
// int printf ( const char *__builtin_va_alist , ... );
#define printf parallelPrintf
#endif


// where.h must follow since it contains static array variables and
// the lenghts are unknown until after the classes are defined (above)!
#include <where.h>

// The final inline_func.h file is included last since it 
// contains actual code used for the inlining performance of A++!
// SKIP_INLINING_FUNCTIONS is  d e f i n e  in A++_headers.h
// Now at the end we can declare the inline functions!
#if !USE_SAGE
// After the header files from P++ and A++ are included we INCLUDE the ../P++/inline_func.h
#undef PPP
#define SERIAL_APP
#include <inline_func.h>
#undef SERIAL_APP
#define PPP
#include <p_inline_func.h>
#undef PPP
#endif

#if defined(COMPILE_SERIAL_APP)
// if COMPILE_SERIAL_APP
// error "Define SERIAL_APP because COMPILE_SERIAL_APP is defined"
#define SERIAL_APP
// elif defined(COMPILE_PPP)
#elif COMPILE_PPP
// error "Define PPP because COMPILE_SERIAL_APP is NOT defined"
#define PPP
#endif

#if 0
// if !defined(COMPILE_SERIAL_APP) && !defined(COMPILE_APP)
#if defined(COMPILE_PPP)
// error checking
#if defined(COMPILE_APP)
// error "ERROR: COMPILE_APP defined while COMPILE_PPP was defined!"
#endif
#define PPP
#endif
#endif

#if 0
// START: Evaluate the macros that drive the compilation
#if defined(COMPILE_APP)
#error "In P++/include/A++.h (BOTTOM): COMPILE_APP is defined"
#else
#error "In P++/include/A++.h (BOTTOM): COMPILE_APP is NOT defined"
#endif

#if defined(COMPILE_SERIAL_APP)
#error "In P++/include/A++.h (BOTTOM): COMPILE_SERIAL_APP is defined"
#else
#error "In P++/include/A++.h (BOTTOM): COMPILE_SERIAL_APP is NOT defined"
#endif

#if defined(COMPILE_PPP)
#error "In P++/include/A++.h (BOTTOM): COMPILE_PPP is defined"
#else
#error "In P++/include/A++.h (BOTTOM): COMPILE_PPP is NOT defined"
#endif

#if defined(SERIAL_APP)
#error "In P++/include/A++.h (BOTTOM): SERIAL_APP is defined"
#else
#error "In P++/include/A++.h (BOTTOM): SERIAL_APP is NOT defined"
#endif

#if defined(PPP)
#error "In P++/include/A++.h (BOTTOM): PPP is defined"
#else
#error "In P++/include/A++.h (BOTTOM): PPP is NOT defined"
#endif

#if COMPILE_APP
#error "In P++/include/A++.h (BOTTOM): COMPILE_APP is TRUE"
#else
#error "In P++/include/A++.h (BOTTOM): COMPILE_APP is FALSE"
#endif

#if COMPILE_PPP
#error "In P++/include/A++.h (BOTTOM): COMPILE_PPP is TRUE"
#else
#error "In P++/include/A++.h (BOTTOM): COMPILE_PPP is FALSE"
#endif

// END: Evaluate the macros that drive the compilation
#endif

#endif  /* !defined(_SERIAL_APP_ARRAY_H) */
















/*include(../src/descriptor_macro.m4)*/


