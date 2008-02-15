// Allow repeated includes of descriptor.h without error
#ifndef _APP_MDI_TYPEDEF_H
#define _APP_MDI_TYPEDEF_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif














/* MACRO EXPANSION BEGINS HERE */
#define DOUBLEARRAY
class doubleSerialArray;
typedef struct Array_Domain_c array_domain;

// Procedure 0
typedef void (*MDI_double_Prototype_0) ( double* , double* , int* , array_domain* , array_domain* , array_domain* );
// Procedure 1
typedef void (*MDI_double_Prototype_1) ( double* , int* , array_domain* , array_domain* );
// Procedure 2
typedef void (*MDI_double_Prototype_2) ( double* , double* , double* , int* , array_domain* , array_domain* , array_domain* , array_domain* );
// Procedure 3
typedef void (*MDI_double_Prototype_3) ( double* , double* , int* , array_domain* , array_domain* , array_domain* );

// Procedure 4
typedef void (*MDI_double_Prototype_4) ( double* , double* , double , int* , array_domain* , array_domain* , array_domain* );
// Procedure 5
typedef void (*MDI_double_Prototype_5) ( double* , double , int* , array_domain* , array_domain* );
 
// Used for replace function since it is of form "doubleSerialArray replace (intSerialArray,doubleSerialArray)"
// Procedure 6
typedef void (*MDI_double_Prototype_6) ( double* , int* , double* , int* , array_domain* , array_domain* , array_domain* , array_domain* );

// Used for replace function since it is of form "doubleSerialArray replace (intSerialArray,double)"
// Procedure 7
typedef void (*MDI_double_Prototype_7) ( double* , int* , double , int* , array_domain* , array_domain* , array_domain* );

#ifndef INTARRAY
// Used for replace function since it is of form "doubleSerialArray replace (int,doubleSerialArray)"
// Procedure 8
typedef void (*MDI_double_Prototype_8) ( double* , double* , int , int* , array_domain* , array_domain* , array_domain* );
#endif

// Procedure 9
typedef void (*MDI_double_Prototype_9) ( int* , double* , int* , array_domain* , array_domain* , array_domain* );
// Procedure 10
typedef void (*MDI_double_Prototype_10) ( int* , int* , array_domain* , array_domain* );
// Procedure 11
typedef void (*MDI_double_Prototype_11) ( int* , double* , double* , int* , array_domain* , array_domain* , array_domain* , array_domain* );
// Procedure 12
typedef void (*MDI_double_Prototype_12) ( int* , double* , double , int* , array_domain* , array_domain* , array_domain* );
// Procedure 13
typedef void (*MDI_double_Prototype_13) ( int* , double , int* , array_domain* , array_domain* );

// Procedure 14
typedef double (*MDI_double_Prototype_14) ( double* , int* , array_domain* , array_domain* );
 
// Procedure 15 (used by Index_Map function)
// typedef double (*MDI_double_Prototype_15) ( double* , int* , int* , int* );
// typedef void (doubleSerialArray::*MDI_double_Prototype_15) ();
typedef doubleSerialArray & (doubleSerialArray::*MDI_double_Prototype_15) ();

#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
// Procedure 16 (used for display and view member functions)
// typedef void (*MDI_double_Prototype_16) (doubleSerialArray::*function)(char *);
typedef doubleSerialArray & (doubleSerialArray::*MDI_double_Prototype_16) (const char *);
#endif

// typedefs to support the use of large aggregate operators!
// Aggregate Operator type (used for aggregate operator functions)
typedef void (*MDI_double_Prototype_Aggregate) ( int Num_ops , doubleSerialArray & Result, doubleSerialArray** Op_List);

// Procedure convertTo_intArray
typedef void (*MDI_double_Prototype_convertTo_intArray) ( int* , double* , int* , array_domain* , array_domain* , array_domain* );

// Procedure convertTo_floatArray
typedef void (*MDI_double_Prototype_convertTo_floatArray) ( float* , double* , int* , array_domain* , array_domain* , array_domain* );

// Procedure convertTo_doubleArray
typedef void (*MDI_double_Prototype_convertTo_doubleArray) ( double* , double* , int* , array_domain* , array_domain* , array_domain* );


#undef DOUBLEARRAY

#define FLOATARRAY
class floatSerialArray;
typedef struct Array_Domain_c array_domain;

// Procedure 0
typedef void (*MDI_float_Prototype_0) ( float* , float* , int* , array_domain* , array_domain* , array_domain* );
// Procedure 1
typedef void (*MDI_float_Prototype_1) ( float* , int* , array_domain* , array_domain* );
// Procedure 2
typedef void (*MDI_float_Prototype_2) ( float* , float* , float* , int* , array_domain* , array_domain* , array_domain* , array_domain* );
// Procedure 3
typedef void (*MDI_float_Prototype_3) ( float* , float* , int* , array_domain* , array_domain* , array_domain* );

// Procedure 4
typedef void (*MDI_float_Prototype_4) ( float* , float* , float , int* , array_domain* , array_domain* , array_domain* );
// Procedure 5
typedef void (*MDI_float_Prototype_5) ( float* , float , int* , array_domain* , array_domain* );
 
// Used for replace function since it is of form "floatSerialArray replace (intSerialArray,floatSerialArray)"
// Procedure 6
typedef void (*MDI_float_Prototype_6) ( float* , int* , float* , int* , array_domain* , array_domain* , array_domain* , array_domain* );

// Used for replace function since it is of form "floatSerialArray replace (intSerialArray,float)"
// Procedure 7
typedef void (*MDI_float_Prototype_7) ( float* , int* , float , int* , array_domain* , array_domain* , array_domain* );

#ifndef INTARRAY
// Used for replace function since it is of form "floatSerialArray replace (int,floatSerialArray)"
// Procedure 8
typedef void (*MDI_float_Prototype_8) ( float* , float* , int , int* , array_domain* , array_domain* , array_domain* );
#endif

// Procedure 9
typedef void (*MDI_float_Prototype_9) ( int* , float* , int* , array_domain* , array_domain* , array_domain* );
// Procedure 10
typedef void (*MDI_float_Prototype_10) ( int* , int* , array_domain* , array_domain* );
// Procedure 11
typedef void (*MDI_float_Prototype_11) ( int* , float* , float* , int* , array_domain* , array_domain* , array_domain* , array_domain* );
// Procedure 12
typedef void (*MDI_float_Prototype_12) ( int* , float* , float , int* , array_domain* , array_domain* , array_domain* );
// Procedure 13
typedef void (*MDI_float_Prototype_13) ( int* , float , int* , array_domain* , array_domain* );

// Procedure 14
typedef float (*MDI_float_Prototype_14) ( float* , int* , array_domain* , array_domain* );
 
// Procedure 15 (used by Index_Map function)
// typedef float (*MDI_float_Prototype_15) ( float* , int* , int* , int* );
// typedef void (floatSerialArray::*MDI_float_Prototype_15) ();
typedef floatSerialArray & (floatSerialArray::*MDI_float_Prototype_15) ();

#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
// Procedure 16 (used for display and view member functions)
// typedef void (*MDI_float_Prototype_16) (floatSerialArray::*function)(char *);
typedef floatSerialArray & (floatSerialArray::*MDI_float_Prototype_16) (const char *);
#endif

// typedefs to support the use of large aggregate operators!
// Aggregate Operator type (used for aggregate operator functions)
typedef void (*MDI_float_Prototype_Aggregate) ( int Num_ops , floatSerialArray & Result, floatSerialArray** Op_List);

// Procedure convertTo_intArray
typedef void (*MDI_float_Prototype_convertTo_intArray) ( int* , float* , int* , array_domain* , array_domain* , array_domain* );

// Procedure convertTo_floatArray
typedef void (*MDI_float_Prototype_convertTo_floatArray) ( float* , float* , int* , array_domain* , array_domain* , array_domain* );

// Procedure convertTo_doubleArray
typedef void (*MDI_float_Prototype_convertTo_doubleArray) ( double* , float* , int* , array_domain* , array_domain* , array_domain* );


#undef FLOATARRAY

#define INTARRAY
class intSerialArray;
typedef struct Array_Domain_c array_domain;

// Procedure 0
typedef void (*MDI_int_Prototype_0) ( int* , int* , int* , array_domain* , array_domain* , array_domain* );
// Procedure 1
typedef void (*MDI_int_Prototype_1) ( int* , int* , array_domain* , array_domain* );
// Procedure 2
typedef void (*MDI_int_Prototype_2) ( int* , int* , int* , int* , array_domain* , array_domain* , array_domain* , array_domain* );
// Procedure 3
typedef void (*MDI_int_Prototype_3) ( int* , int* , int* , array_domain* , array_domain* , array_domain* );

// Procedure 4
typedef void (*MDI_int_Prototype_4) ( int* , int* , int , int* , array_domain* , array_domain* , array_domain* );
// Procedure 5
typedef void (*MDI_int_Prototype_5) ( int* , int , int* , array_domain* , array_domain* );
 
// Used for replace function since it is of form "intSerialArray replace (intSerialArray,intSerialArray)"
// Procedure 6
typedef void (*MDI_int_Prototype_6) ( int* , int* , int* , int* , array_domain* , array_domain* , array_domain* , array_domain* );

// Used for replace function since it is of form "intSerialArray replace (intSerialArray,int)"
// Procedure 7
typedef void (*MDI_int_Prototype_7) ( int* , int* , int , int* , array_domain* , array_domain* , array_domain* );

#ifndef INTARRAY
// Used for replace function since it is of form "intSerialArray replace (int,intSerialArray)"
// Procedure 8
typedef void (*MDI_int_Prototype_8) ( int* , int* , int , int* , array_domain* , array_domain* , array_domain* );
#endif

// Procedure 9
typedef void (*MDI_int_Prototype_9) ( int* , int* , int* , array_domain* , array_domain* , array_domain* );
// Procedure 10
typedef void (*MDI_int_Prototype_10) ( int* , int* , array_domain* , array_domain* );
// Procedure 11
typedef void (*MDI_int_Prototype_11) ( int* , int* , int* , int* , array_domain* , array_domain* , array_domain* , array_domain* );
// Procedure 12
typedef void (*MDI_int_Prototype_12) ( int* , int* , int , int* , array_domain* , array_domain* , array_domain* );
// Procedure 13
typedef void (*MDI_int_Prototype_13) ( int* , int , int* , array_domain* , array_domain* );

// Procedure 14
typedef int (*MDI_int_Prototype_14) ( int* , int* , array_domain* , array_domain* );
 
// Procedure 15 (used by Index_Map function)
// typedef int (*MDI_int_Prototype_15) ( int* , int* , int* , int* );
// typedef void (intSerialArray::*MDI_int_Prototype_15) ();
typedef intSerialArray & (intSerialArray::*MDI_int_Prototype_15) ();

#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
// Procedure 16 (used for display and view member functions)
// typedef void (*MDI_int_Prototype_16) (intSerialArray::*function)(char *);
typedef intSerialArray & (intSerialArray::*MDI_int_Prototype_16) (const char *);
#endif

// typedefs to support the use of large aggregate operators!
// Aggregate Operator type (used for aggregate operator functions)
typedef void (*MDI_int_Prototype_Aggregate) ( int Num_ops , intSerialArray & Result, intSerialArray** Op_List);

// Procedure convertTo_intArray
typedef void (*MDI_int_Prototype_convertTo_intArray) ( int* , int* , int* , array_domain* , array_domain* , array_domain* );

// Procedure convertTo_floatArray
typedef void (*MDI_int_Prototype_convertTo_floatArray) ( float* , int* , int* , array_domain* , array_domain* , array_domain* );

// Procedure convertTo_doubleArray
typedef void (*MDI_int_Prototype_convertTo_doubleArray) ( double* , int* , int* , array_domain* , array_domain* , array_domain* );


#undef INTARRAY

#endif  /* !defined(_APP_MDI_TYPEDEF_H) */




