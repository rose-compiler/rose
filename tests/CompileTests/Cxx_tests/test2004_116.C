// #include "optionDeclaration.h"
class OptionDeclaration
   {
     public:
       // Constructors designed to trigger auto-promotion of constructor arguments
          OptionDeclaration( char* s, int value );
          OptionDeclaration( char* s, double value );
          OptionDeclaration( char* s, char* value );
   };

OptionDeclaration option1 ("VERSION",1.0);
OptionDeclaration option2 ("PACKAGE","A++P++IndexObject");

// Allow repeated includes of index.h without error
#ifndef _APP_INDEX_H
#define _APP_INDEX_H

enum Index_Mode_Type { Index_Triplet = 100, Null_Index = 101, All_Index = 102 };
enum On_Off_Type { On , ON , on , Off , OFF , off };

// required to be present ahead of Range header file
extern int APP_Global_Array_Base;

#define SERIAL_APP

// DQ (2/20/2010): GNU appears to require this.
class SerialArray_Domain_Type{};

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
             bool Return_Value = false;
             if (Index_Mode == X.Index_Mode)
                {
                  if ( (Index_Mode == Null_Index) || (Index_Mode == All_Index) )
                       Return_Value = true;
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
        Internal_Index getLocalPart ( const SerialArray_Domain_Type & X , int Axis ) const;

     // Internally arrays of pointers to Index objects are initialized and we have to
     // avoid taking the address of the member function's returned value.
        Internal_Index* getPointerToLocalPart ( const SerialArray_Domain_Type & X , int Axis ) const;

     private:
     // used internally in the getLocalPart and getPointerToLocalPart member functions
        void getLocalPartData ( const SerialArray_Domain_Type & X , int Axis, int & Output_Base, int & Output_Count, int & Output_Stride ) const;

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

#endif  /* !defined(_APP_INDEX_H) */

// Include multidimensional index objects later
// include "region.h"


