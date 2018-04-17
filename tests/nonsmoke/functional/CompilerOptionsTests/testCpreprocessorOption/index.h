// Allow repeated includes of index.h without error
#ifndef _APP_INDEX_H
#define _APP_INDEX_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif

enum Index_Mode_Type { Index_Triplet = 100, Null_Index = 101, All_Index = 102 };
// enum Index_Bounds_Checking_Options { ON , On , OFF , Off };
// enum On_Off_Type { ON , On , OFF , Off };
enum On_Off_Type { On , ON , on , Off , OFF , off };

// required to be present ahead of Range header file
extern int APP_Global_Array_Base;

// The Internal_Index is used as a base class for the Range and Index objects.
// The use of this class allows the operator() operators in the array classes
// to be written with fewer instances (i.e. equal to the number of dimensions
// of the A++/P++ arrays).

// class Range;
// class Index;
// class Internal_Index;

// *************************************************************
// This class is a base class for the Range and Index classes
// *************************************************************
class Internal_Index
   {
     friend class Internal_Indirect_Addressing_Index;
     friend class Array_Domain_Type;
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

#if 0
     template<class T, int Templated_Dimension>
     friend class Array_Descriptor_Type;
#endif

// ifdef SERIAL_APP
#if defined(PPP) || defined(SERIAL_APP)
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;

     friend class SerialArray_Domain_Type;
#endif

     protected:
        int Base;
        int Count;
        int Stride;
        int Bound;
        Index_Mode_Type Index_Mode;

     // functions
        void Consistency_Check( const char* Label = "" ) const;

     public:
     // (bugfix 8/3/200) Must make this public so that it is accessible to
     // all friend functions of the array class interface
        static bool Index_Bounds_Checking;

     public:
        inline ~Internal_Index () {};

     protected:
        inline Internal_Index () : Base (0), Count (0), Stride (1), Bound (-1), Index_Mode (All_Index) 
           {
#if COMPILE_DEBUG_STATEMENTS
             Consistency_Check("destructor");
#endif
           };

      public:

        inline Internal_Index ( int i ) : Base (i), Count (1), Stride (1), Bound (i), Index_Mode (Index_Triplet)
           {
#if defined(COMPILE_DEBUG_STATEMENTS) || defined(BOUNDS_CHECK)
             Consistency_Check("constructor(int)");
#endif
           };

#if 0
        inline Internal_Index ( const Range & X , int Input_Stride = 1 )
           : Base (X.Base), Count ( ((X.Bound - X.Base) / Input_Stride)+1 ),
             Stride (Input_Stride), Bound (X.Base + (Count-1) * Input_Stride),
             Index_Mode (Index_Triplet)
           {
             if (Base > Bound)
                  Index_Mode = Null_Index;
 
             if ( Count == 0 )
                  Index_Mode = Null_Index;
 
#if defined(COMPILE_DEBUG_STATEMENTS) || defined(BOUNDS_CHECK)
             Consistency_Check("constructor(Range,int=1)");
#endif
           }
#endif
           
        inline Internal_Index ( int Input_Base, int Input_Count, int Input_Stride = 1 )
           : Base (Input_Base), Count (Input_Count), Stride (Input_Stride), 
             Bound (Input_Base+(Input_Count-1)*Input_Stride),
             Index_Mode (Index_Triplet)
           {
#if COMPILE_DEBUG_STATEMENTS
          // printf ("Inside of Internal_Index constructor (int,int,int) \n");
#endif
             if ( Count == 0 )
                  Index_Mode = Null_Index;
 
#if defined(COMPILE_DEBUG_STATEMENTS) || defined(BOUNDS_CHECK)
             Consistency_Check("constructor(int,int,int=1)");
#endif
           }

        inline Internal_Index ( int Input_Base, int Input_Count, int Input_Stride, Index_Mode_Type Input_Index_Mode )
           : Base (Input_Base), Count (Input_Count), Stride (Input_Stride), 
             Bound (Input_Base+(Input_Count-1)*Input_Stride),
             Index_Mode (Input_Index_Mode)
           {
          // I think this should only be set to a NULL_Index if the count is ZERO AND the input mode is not All_Index
             if ( Count == 0 )
                  Index_Mode = Null_Index;
 
#if defined(COMPILE_DEBUG_STATEMENTS) || defined(BOUNDS_CHECK)
             Consistency_Check("constructor(int,int,int,Index_Mode_Type)");
#endif
           }

        inline Internal_Index ( const Internal_Index & X )
           : Base (X.Base), Count (X.Count), Stride (X.Stride), 
             Bound (X.Bound), Index_Mode (X.Index_Mode)
           {
             if ( (Index_Mode == Index_Triplet) && (Count == 0) )
                  Index_Mode = Null_Index;
 
#if defined(COMPILE_DEBUG_STATEMENTS) || defined(BOUNDS_CHECK)
             Consistency_Check("copy constructor");
#endif
           }
 
        inline Internal_Index & operator= ( const Internal_Index & X )
           {
             Base       = X.Base;
             Count      = X.Count;
             Stride     = X.Stride;
             Bound      = X.Bound;
             Index_Mode = X.Index_Mode;
 
             if ( (Index_Mode == Index_Triplet) && (Count == 0) )
                  Index_Mode = Null_Index;
 
#if defined(COMPILE_DEBUG_STATEMENTS) || defined(BOUNDS_CHECK)
             Consistency_Check("Internal_Index::operator=");
#endif
             return *this;
           }

     // These old member functions didn't handle 1+I (only I+1)
     // Index operator+ ( int i ) const;
     // Index operator- ( int i ) const;

        Internal_Index & operator+= ( int i );
        Internal_Index & operator-= ( int i );
        Internal_Index & operator*= ( int i );
        Internal_Index & operator/= ( int i );

     // For Index I(a,b,s) and scalars p and q  ----  p*I+q == Index (p*a+q, b, p*s)
     // So n-I should be (n-a,b,-s).  This is constant with the rule above with n-I being (-1)*I + n
     // Within the definition for operator/ integer division is assumed
        inline friend Internal_Index operator+ ( const Internal_Index & Lhs , int i )
           { return Internal_Index ( Lhs.Base + i, Lhs.Count, Lhs.Stride, Lhs.Index_Mode ); }
        inline friend Internal_Index operator+ ( int i , const Internal_Index & Rhs )
           { return Internal_Index ( Rhs.Base + i, Rhs.Count, Rhs.Stride, Rhs.Index_Mode ); }

        inline friend Internal_Index operator- ( const Internal_Index & Lhs , int i )
           { return Internal_Index ( Lhs.Base - i, Lhs.Count, Lhs.Stride, Lhs.Index_Mode ); }
        inline friend Internal_Index operator- ( int i , const Internal_Index & Rhs )
           { return Internal_Index ( i - Rhs.Base, Rhs.Count, -Rhs.Stride, Rhs.Index_Mode ); }

        inline friend Internal_Index operator* ( const Internal_Index & Lhs , int i )
           { return Internal_Index ( Lhs.Base * i, Lhs.Count, Lhs.Stride * i, Lhs.Index_Mode ); }
        inline friend Internal_Index operator* ( int i , const Internal_Index & Rhs )
           { return Internal_Index ( i * Rhs.Base, Rhs.Count, i * Rhs.Stride, Rhs.Index_Mode ); }

        inline friend Internal_Index operator/ ( const Internal_Index & Lhs , int i )
           { return Internal_Index ( Lhs.Base / i, Lhs.Count, Lhs.Stride / i, Lhs.Index_Mode ); }
        inline friend Internal_Index operator/ ( int i , const Internal_Index & Rhs )
           { return Internal_Index ( i / Rhs.Base, Rhs.Count, i / Rhs.Stride, Rhs.Index_Mode ); }

     // Access functions
        inline int getBase() const
           { return Base; }
        inline int getBound() const
           { return Bound; }
        inline int getCount() const
           { return Count; }
        inline int getStride() const
           { return Stride; }
        inline Index_Mode_Type getMode() const
           { return Index_Mode; }

        char* getModeString() const;

        inline int length () const
           {
             int Temp = (Index_Mode == Index_Triplet) ? (Bound - Base) + 1 : 0;
             return (Temp % Stride == 0) ? (Temp/Stride) : (Temp/Stride) + 1;
           }

     // This function added (11/9/97)
        inline int getLength () const
           {
             return length();
           }

     // Unary minus allows the stride to be reversed!
        Internal_Index operator- () const;

        void list( const char* Label = "default display string" ) const;
        void display( const char* Label = "default display string" ) const;

        static void setBoundsCheck ( On_Off_Type On_Off );

     // made this function inline Jan 15th 1997.
        inline bool operator== ( const Internal_Index & X ) const
           { 
             bool Return_Value = FALSE;
             if (Index_Mode == X.Index_Mode)
                {
                  if ( (Index_Mode == Null_Index) || (Index_Mode == All_Index) )
                       Return_Value = TRUE;
                    else
                     {
                       Return_Value = ( (Base == X.Base) && (Bound == X.Bound) && (Stride == X.Stride) );
                     }
                }

             return Return_Value;
           }

     // Added operator!= Jan 15th 1997.
        inline bool operator!= ( const Internal_Index & X ) const
           {
             return !(operator==(X));
           }

        void Test_Consistency ( const char* Label = "" ) const;

     public:
#if defined(PPP) || defined(SERIAL_APP)
        Internal_Index getLocalPart 
	   ( const SerialArray_Domain_Type & X , int Axis ) const;

     // Internally arrays of pointers to Index objects are initialized and we have to
     // avoid taking the address of the member function's returned value.
        Internal_Index* getPointerToLocalPart 
	   ( const SerialArray_Domain_Type & X , int Axis ) const;

     private:
     // used internally in the getLocalPart and getPointerToLocalPart member functions
        void getLocalPartData 
	   ( const SerialArray_Domain_Type & X , int Axis, 
             int & Output_Base, int & Output_Count, int & Output_Stride ) const;

     public:
        void adjustBase  ( int x );
        void adjustBound ( int x );

     // These functions allow the indexing of the Index 
     // objects using either scalar or Index objects.
     // Make this an inlined function later
     // Internal_Index operator() ( int x ) const;
        Internal_Index operator() ( const Internal_Index & X ) const;
#endif

#if 0
     // This function can't return a reference (must return by value)
        int operator() ( int x ) const
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of int Internal_Index::operator(int x) \n");
#endif

          //  ... (1/29/97,kdb) this doesn't compile so comment out for now
          //   APP_ASSERT(Index_Mode != Null_Index);
          //   APP_ASSERT(Index_Mode != All_Index);

          // I need to think again about if this is the correct definition of scalar indexing 
          // also we need to account for stride (not done here).
	     // ... (3/23/98,kdb) this should just be x ...
             //return x-Base;
             return x;
           }
#else
        int operator() ( int x ) const;
#endif

     // (10/11/2000) Special request from Anders
	inline Internal_Index grow ( int i )
           {
          // Call the shrink function
             return shrink (-i);
           }

     // (10/11/2000) Special request from Anders
	inline Internal_Index shrink ( int i )
           {
             int newBase             = Base;
          // int newBound            = Bound;
             int newCount            = Count;
             int newStride           = Stride;
             Index_Mode_Type newMode = Index_Mode;

          // We can only build an All_Index using the default constructor
             if (Index_Mode == All_Index)
                  return Internal_Index();

	     if ( i < 0)
                {
               // Case of growing the index length
                  if ( (Index_Mode == Index_Triplet) && ( Count >= 1 ) )
                     {
                       newBase   = Base  + (i * Stride);
                       newCount  = Count - (2 * i);
                     }
                }
              else
                {
               // Case of shrinking the index length
                  if ( (Index_Mode == Index_Triplet) && ( Count > 2*i ) )
                     {
                       newBase   = Base  + (i * Stride);
                       newCount  = Count - (2 * i);
                     }
                }

             return Internal_Index (newBase,newCount,newStride,newMode);
           }
	 

   };

// *******************************************************************
//     Range class is derived from the Internal_Index class
// *******************************************************************
class Index;
class Range : public Internal_Index
   {
     friend class Index;
     friend class Array_Domain_Type;
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

#if 0
     template<class T, int Templated_Dimension>
     friend class Array_Descriptor_Type;
#endif

#ifdef SERIAL_APP
  // Within P++ we require that the SerialArray (A++ objects) have access to internal data
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;

     friend class SerialArray_Domain_Type;
#endif

     public:
        inline ~Range () {};
        inline Range () : Internal_Index() {}

     // This allows the Internal_Index operators to get back to a Range object
     // but a more efficient solution would be to implement the let the interface
     // of the Internal_Index be represented at the Range and Index derived class level
     // or provide casting operators which had no body (this seems like the best solution)
        inline Range ( const Internal_Index & I ) : Internal_Index(I) {};

        inline Range ( int i ) : Internal_Index(APP_Global_Array_Base,i,1) {};
        inline Range ( int Input_Base, int Input_Bound, int Input_Stride = 1 ) 
             : Internal_Index(Input_Base,((Input_Bound-Input_Base)/Input_Stride)+1,Input_Stride) {};
        inline Range ( int Input_Base, int Input_Bound, int Input_Stride, Index_Mode_Type Input_Index_Mode )
             : Internal_Index(Input_Base,((Input_Bound-Input_Base)/Input_Stride)+1,Input_Stride,Input_Index_Mode) {};
        inline Range ( const Range & X ) : Internal_Index(X) {};
        inline Range & operator= ( const Range & X ) 
           { 
             Internal_Index::operator= (X);
             return *this;
           };
#if 0
     // Index class not defined yet (so do this outside of class later)!
        inline Range ( const Internal_Index & X ) : Internal_Index (X) {};
#endif
   };

// *******************************************************************
//     Index class is derived from the Internal_Index class
// *******************************************************************
class Index : public Internal_Index
   {
     friend class Range;
     friend class Array_Domain_Type;
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

#if 0
     template<class T, int Templated_Dimension>
     friend class Array_Descriptor_Type;
#endif

#ifdef SERIAL_APP
  // Within P++ we require that the SerialArray (A++ objects) have access to internal data
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;

     friend class SerialArray_Domain_Type;
#endif

     public:
        inline ~Index () {};
        inline Index () : Internal_Index() {};

     // This allows the Internal_Index operators to get back to a Range object
     // but a more efficient solution would be to implement the let the interface
     // of the Internal_Index be represented at the Range and Index derived class level
     // or provide casting operators which had no body (this seems like the best solution)
        inline Index ( const Internal_Index & I ) : Internal_Index(I) {};

        inline Index ( const Index & X ) : Internal_Index(X) {};
        inline Index ( int x ) : Internal_Index(x) {};
        inline Index ( int Input_Base, int Input_Count, int Input_Stride = 1 ) 
             : Internal_Index(Input_Base,Input_Count,Input_Stride) {};
        inline Index ( int Input_Base, int Input_Count, int Input_Stride, Index_Mode_Type Input_Index_Mode )
             : Internal_Index(Input_Base,Input_Count,Input_Stride,Input_Index_Mode) {};
        inline Index & operator= ( const Index & X )
           { 
             Internal_Index::operator= (X);
             return *this;
           };
     // Allow conversion from Range to Index but not the other way around!
        inline Index ( const Range & X ) : Internal_Index (X) {}
   };

// Constants used in switch statements (indicates what kind of indirect addressing
// object is used internally in the Internal_Indirect_Addressing_Index object).
#define INDIRECT_ADDRESSING_INTARRAY       6000
#define INDIRECT_ADDRESSING_INTSERIALARRAY 6001
#define INDIRECT_ADDRESSING_INDEX          6002
#define INDIRECT_ADDRESSING_INT            6003

// We might want to use the simular getMode mechanism as in the Internal_Index objects
// if so then here is the enum type (but I don't want to change the code just yet)
enum Internal_Indirect_Addressing_Index_Mode_Type
   {
     IndexInStorage                 = 200, 
     intArrayInStorage              = 201,
     intSerialArrayInStorage        = 202,
     ScalarInStorage                = 203,
     Null_Indirect_Addressing_Index = 204
   };

class Internal_Indirect_Addressing_Index
   {
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class Array_Domain_Type;
#if 0
     template<class T, int Templated_Dimension>
     friend class Array_Descriptor_Type;
#endif

#ifdef SERIAL_APP
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;
     friend class SerialArray_Domain_Type;
#endif

     private:

       // Note that we want to avoid using a union here because it would make it ifficult to do the error checking
          int            Scalar;
          Internal_Index Index_Data;
          intArray*      IndirectionArrayPointer;
#if 0
          int            typeInStorage;
#else
          bool intArrayInStorage;
          bool IndexInStorage;
          bool intSerialArrayInStorage;
#endif

#if (defined(SERIAL_APP) || defined(PPP))
          intSerialArray* IndirectionSerialArrayPointer;
#else
          intArray* IndirectionSerialArrayPointer;
#endif

     public:
       // Access functions
          bool isScalar() const
             { return !(IndexInStorage || IndexInStorage || intSerialArrayInStorage); }
          bool isIntArray() const
             { return intArrayInStorage; }
          bool isIntSerialArray() const
             { return intSerialArrayInStorage; }
          bool isIndex() const
             { return IndexInStorage; }

       // Access function to read internal data
          int getScalar() const
             { return Scalar; }
          Internal_Index getIndex() const
             { return Index_Data; }
          Internal_Index getRange() const
             { return getIndex(); }
          intArray* getIntArrayPointer() const
             {
               APP_ASSERT(isIntArray() == TRUE);
               return IndirectionArrayPointer;
             }
#if (defined(SERIAL_APP) || defined(PPP))
          intSerialArray* getIntSerialArrayPointer() const
#else
          intArray* getIntSerialArrayPointer() const
#endif
             {
               APP_ASSERT(isIntSerialArray() == TRUE); 
               return IndirectionSerialArrayPointer;
             }

       // Access function to set internal data
          void setScalar( int n )
             { Scalar = n; }
          void setIndex( Internal_Index & X );
          void setRange( Internal_Index & X )
             {
               setIndex(X);
             }
          void setIntArray( intArray* Xptr );
#if (defined(SERIAL_APP) || defined(PPP))
          void setIntSerialArray( intSerialArray* Xptr );
#else
          void setIntSerialArray( intArray* Xptr );
#endif

          Internal_Indirect_Addressing_Index ()
             : Scalar(0), Index_Data(0,0,1,Null_Index), IndirectionArrayPointer(NULL),
               intArrayInStorage(FALSE), IndexInStorage(FALSE),intSerialArrayInStorage(FALSE)
              ,IndirectionSerialArrayPointer(NULL)
           {};

         ~Internal_Indirect_Addressing_Index ();
          Internal_Indirect_Addressing_Index (int x)
             : Scalar(x), Index_Data(0,0,1,Null_Index), IndirectionArrayPointer(NULL),
               intArrayInStorage(FALSE), IndexInStorage(FALSE),intSerialArrayInStorage(FALSE),
               IndirectionSerialArrayPointer(NULL)
             {};

       // We might want to provide this constructor later -- for now it remains unsupported
       // It is now supported (added 3/15/96).
          Internal_Indirect_Addressing_Index (const Internal_Index & X) 
             : Scalar(0), Index_Data(X), IndirectionArrayPointer(NULL), 
               intArrayInStorage(FALSE), IndexInStorage(TRUE),intSerialArrayInStorage(FALSE),
               IndirectionSerialArrayPointer(NULL)
             {};

          Internal_Indirect_Addressing_Index (int Base, int Count, int Stride)
             : Scalar(0), Index_Data(Base,Count,Stride), IndirectionArrayPointer(NULL),
               intArrayInStorage(FALSE), IndexInStorage(TRUE),intSerialArrayInStorage(FALSE),
               IndirectionSerialArrayPointer(NULL)
           {};

          Internal_Indirect_Addressing_Index (const intArray & X);

#if (defined(SERIAL_APP) || defined(PPP))
       // Only build this constructor if using P++
          Internal_Indirect_Addressing_Index (const intSerialArray & X);
#endif

       // Copy constructor
          Internal_Indirect_Addressing_Index (const Internal_Indirect_Addressing_Index & X);

       // operator=
          Internal_Indirect_Addressing_Index & operator= (const Internal_Indirect_Addressing_Index & X);

          void display( const char* label = "" ) const;

          void Test_Consistency ( const char* Label = "" ) const;

#if (defined(SERIAL_APP) || defined(PPP))
       // This is part of the P++ interface for Index objects
	  Internal_Indirect_Addressing_Index* getPointerToLocalPart ( const SerialArray_Domain_Type & X, int Axis ) const;
#endif

       // Scalar indexing operator (used in indirect addressing support to fill in PGSLib data structures)
       // we can't define this to be inlined here since the intArray operators are seen only after this
       // header file is processed.
          int operator() ( int i ) const;

       // give this object common interface with the rest of the index objects
          int getLength() const;
          int getBase() const;
          int getBound() const;
          int getStride() const;
   };

#endif  /* !defined(_APP_INDEX_H) */

// Include multidimensional index objects later
// include "region.h"



