// Allow repeated includes of inline_func.h without error
#ifndef _APP_INLINE_SUPPORT_H
#define _APP_INLINE_SUPPORT_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif





/* Set_Of_New_Operators(double) */
/* Set_Of_New_Operators(float)  */

#define INTARRAY
/* Set_Of_New_Operators(int) */
#undef INTARRAY

/* Macro Expansion below */
/* New_Operators_For_Array_Statement */

/* MACRO EXPANSION BEGINS HERE */
#define DOUBLEARRAY
// ***********************************************************************************
// This function is used in the functions below which control the reuse of temporaries
// for the cases when a temporary must be built and also for the abstract binary
// operators which return intArrays (since the Lhs and Rhs are often not intArrays
// and so they can not be reused as intArrays).
// ***********************************************************************************
#if defined(DOUBLEARRAY)
// Declare this only once!
#if 0
inline SerialArray_Descriptor_Type* SerialArray_Descriptor_Type::Build_Temporary_By_Example ( const SerialArray_Descriptor_Type & X )
   {
   }
#endif
#endif



// Expantion of previous macro to handle cross product of types
#if 0
   Function moved to array.C while debugging!
#endif

#if 0
   Function moved to array.C while debugging!
#endif

#if 0
   Function moved to array.C while debugging!
#endif


// ***********************************************************************************
// This function is used in the abstract binary operators to control the reuse of
// temporaries.  If the Lhs or Rhs is a temporary then it can be reused -- else
// a real temporary must be allocated for use within the expression evaluation.
// Temporaries are marked in the descriptor as being temporaries so that they can
// be readily identified as such.  It is possible to provide such functionality
// as a seperate class but I have never seen it done in such a way as to be practical
// for a large and meaningful array class.
// ***********************************************************************************

// ***********************************************************************************
// This function is used in the abstract unary operators to control the reuse of
// temporaries.  If the Lhs or Rhs is a temporary then it can be reused -- else
// a real temporary must be allocated for use within the expression evaluation.
// Temporaries are marked in the descriptor as being temporaries so that they can
// be readily identified as such.  It is possible to provide such functionality
// as a seperate class but I have never seen it done in such a way as to be practical
// for a large and meaningful array class.
// ***********************************************************************************

#undef DOUBLEARRAY

#define FLOATARRAY
// ***********************************************************************************
// This function is used in the functions below which control the reuse of temporaries
// for the cases when a temporary must be built and also for the abstract binary
// operators which return intArrays (since the Lhs and Rhs are often not intArrays
// and so they can not be reused as intArrays).
// ***********************************************************************************
#if defined(DOUBLEARRAY)
// Declare this only once!
#if 0
inline SerialArray_Descriptor_Type* SerialArray_Descriptor_Type::Build_Temporary_By_Example ( const SerialArray_Descriptor_Type & X )
   {
   }
#endif
#endif



// Expantion of previous macro to handle cross product of types
#if 0
   Function moved to array.C while debugging!
#endif

#if 0
   Function moved to array.C while debugging!
#endif

#if 0
   Function moved to array.C while debugging!
#endif


// ***********************************************************************************
// This function is used in the abstract binary operators to control the reuse of
// temporaries.  If the Lhs or Rhs is a temporary then it can be reused -- else
// a real temporary must be allocated for use within the expression evaluation.
// Temporaries are marked in the descriptor as being temporaries so that they can
// be readily identified as such.  It is possible to provide such functionality
// as a seperate class but I have never seen it done in such a way as to be practical
// for a large and meaningful array class.
// ***********************************************************************************

// ***********************************************************************************
// This function is used in the abstract unary operators to control the reuse of
// temporaries.  If the Lhs or Rhs is a temporary then it can be reused -- else
// a real temporary must be allocated for use within the expression evaluation.
// Temporaries are marked in the descriptor as being temporaries so that they can
// be readily identified as such.  It is possible to provide such functionality
// as a seperate class but I have never seen it done in such a way as to be practical
// for a large and meaningful array class.
// ***********************************************************************************

#undef FLOATARRAY

#define INTARRAY
// ***********************************************************************************
// This function is used in the functions below which control the reuse of temporaries
// for the cases when a temporary must be built and also for the abstract binary
// operators which return intArrays (since the Lhs and Rhs are often not intArrays
// and so they can not be reused as intArrays).
// ***********************************************************************************
#if defined(DOUBLEARRAY)
// Declare this only once!
#if 0
inline SerialArray_Descriptor_Type* SerialArray_Descriptor_Type::Build_Temporary_By_Example ( const SerialArray_Descriptor_Type & X )
   {
   }
#endif
#endif



// Expantion of previous macro to handle cross product of types
#if 0
   Function moved to array.C while debugging!
#endif

#if 0
   Function moved to array.C while debugging!
#endif

#if 0
   Function moved to array.C while debugging!
#endif


// ***********************************************************************************
// This function is used in the abstract binary operators to control the reuse of
// temporaries.  If the Lhs or Rhs is a temporary then it can be reused -- else
// a real temporary must be allocated for use within the expression evaluation.
// Temporaries are marked in the descriptor as being temporaries so that they can
// be readily identified as such.  It is possible to provide such functionality
// as a seperate class but I have never seen it done in such a way as to be practical
// for a large and meaningful array class.
// ***********************************************************************************

// ***********************************************************************************
// This function is used in the abstract unary operators to control the reuse of
// temporaries.  If the Lhs or Rhs is a temporary then it can be reused -- else
// a real temporary must be allocated for use within the expression evaluation.
// Temporaries are marked in the descriptor as being temporaries so that they can
// be readily identified as such.  It is possible to provide such functionality
// as a seperate class but I have never seen it done in such a way as to be practical
// for a large and meaningful array class.
// ***********************************************************************************

#undef INTARRAY

#endif  /* !defined(_APP_INLINE_SUPPORT_H) */






