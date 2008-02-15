// This file contains the subscript computations required for A++/P++ array transformations

#ifdef USE_ROSE
// error "USE_ROSE is defined!"

// Define the Macros as functions so that they will not be expanded in the resulting transformations
// (this makes the final transformations smaller and earier to read).  By overloading the SC
// function, our transformation can just insert SC(offset1,offset2) into the indexing
// (e.g. A_pointer[SC(0,0)]). This make the design of transformations as simple as possible.

// SC stands for Subscript Computation (longer names would have make the inner loop transformation ugly)
// We have to support a different macro for each operand (potentially) and so we need a lot of macros

int SC ( void );
int SC ( doubleArray & X );
int SC ( doubleArray & X, InternalIndex & I, int x1, int x2 = 0, int x3 = 0, int x4 = 0, int x5 = 0, int x6 = 0 );
int SC ( InternalIndex & I, int x1, int x2 = 0, int x3 = 0, int x4 = 0, int x5 = 0, int x6 = 0 );
int SC ( int x1, int x2 = 0, int x3 = 0, int x4 = 0, int x5 = 0, int x6 = 0 );

#define SUBSCRIPT_FUNCTIONS(OPERAND_INDEX) \
int SC_## OPERAND_INDEX ( void ); \
int SC_## OPERAND_INDEX ( doubleArray & X ); \
int SC_## OPERAND_INDEX ( doubleArray & X, InternalIndex & I, int x1, int x2 = 0, int x3 = 0, int x4 = 0, int x5 = 0, int x6 = 0 ); \
int SC_## OPERAND_INDEX ( InternalIndex & I, int x1, int x2 = 0, int x3 = 0, int x4 = 0, int x5 = 0, int x6 = 0 ); \
int SC_## OPERAND_INDEX ( int x1, int x2 = 0, int x3 = 0, int x4 = 0, int x5 = 0, int x6 = 0 );

#define SUBSCRIPT_FUNCTION_GROUP(TENS) \
SUBSCRIPT_FUNCTIONS(TENS ##0) \
SUBSCRIPT_FUNCTIONS(TENS ##1) \
SUBSCRIPT_FUNCTIONS(TENS ##2) \
SUBSCRIPT_FUNCTIONS(TENS ##3) \
SUBSCRIPT_FUNCTIONS(TENS ##4) \
SUBSCRIPT_FUNCTIONS(TENS ##5) \
SUBSCRIPT_FUNCTIONS(TENS ##6) \
SUBSCRIPT_FUNCTIONS(TENS ##7) \
SUBSCRIPT_FUNCTIONS(TENS ##8) \
SUBSCRIPT_FUNCTIONS(TENS ##9)

// List the first 10 SC_0 through SC_9 separately
SUBSCRIPT_FUNCTIONS(0)
SUBSCRIPT_FUNCTIONS(1)
SUBSCRIPT_FUNCTIONS(2)
SUBSCRIPT_FUNCTIONS(3)
SUBSCRIPT_FUNCTIONS(4)
SUBSCRIPT_FUNCTIONS(5)
SUBSCRIPT_FUNCTIONS(6)
SUBSCRIPT_FUNCTIONS(7)
SUBSCRIPT_FUNCTIONS(8)
SUBSCRIPT_FUNCTIONS(9)

// List the remaining SC function declarations separately
SUBSCRIPT_FUNCTION_GROUP(1)

SUBSCRIPT_FUNCTION_GROUP(2)

SUBSCRIPT_FUNCTION_GROUP(3)

SUBSCRIPT_FUNCTION_GROUP(4)

#else

// error "USE_ROSE is NOT defined!"

// These macros are so numerous because there are so many different ways to optimize the
// subscript computation (assumptions about stride, uniformity of size and stride, etc).

// For indirect addressing, the subscript computation is even more complex!  The number of possible
// variations of subscript computation support for indirect addressing may require that we
// explicitly build the macros at compile-time (as part of the transformations).

// Define these to be true macros seen only by final backend compilation
// Note: use of cpp macro concatination using the "##" operator
// macro used to simplify representation of subscript computation in transformations
#define NONINDEX_VARIABLE_SIZE_VARIABLE_STRIDE_SUBSCRIPT_MACRO(VARIABLE_NAME) \
index1 * VARIABLE_NAME ##_stride1 + \
index2 * VARIABLE_NAME ##_stride2 * VARIABLE_NAME ##_size1 + \
index3 * VARIABLE_NAME ##_stride3 * VARIABLE_NAME ##_size2 + \
index4 * VARIABLE_NAME ##_stride4 * VARIABLE_NAME ##_size3 + \
index5 * VARIABLE_NAME ##_stride5 * VARIABLE_NAME ##_size4 + \
index6 * VARIABLE_NAME ##_stride6 * VARIABLE_NAME ##_size5 

#define NONINDEX_UNIFORM_SIZE_VARIABLE_STRIDE_SUBSCRIPT_MACRO(VARIABLE_NAME) \
index1 * VARIABLE_NAME ##_stride1 + \
index2 * VARIABLE_NAME ##_stride2 * _size1 + \
index3 * VARIABLE_NAME ##_stride3 * _size2 + \
index4 * VARIABLE_NAME ##_stride4 * _size3 + \
index5 * VARIABLE_NAME ##_stride5 * _size4 + \
index6 * VARIABLE_NAME ##_stride6 * _size5 

#define NONINDEX_VARIABLE_SIZE_UNIFORM_STRIDE_SUBSCRIPT_MACRO(VARIABLE_NAME) \
index1 * _stride1 + \
index2 * _stride2 * VARIABLE_NAME ##_size1 + \
index3 * _stride3 * VARIABLE_NAME ##_size2 + \
index4 * _stride4 * VARIABLE_NAME ##_size3 + \
index5 * _stride5 * VARIABLE_NAME ##_size4 + \
index6 * _stride6 * VARIABLE_NAME ##_size5 

#define NONINDEX_UNIFORM_SIZE_UNIT_STRIDE_SUBSCRIPT_MACRO() \
index1 * _stride1 + \
index2 * _stride2 * _size1 + \
index3 * _stride3 * _size2 + \
index4 * _stride4 * _size3 + \
index5 * _stride5 * _size4 + \
index6 * _stride6 * _size5

#define NONINDEX_UNIFORM_SIZE_STRIDE_ONE_SUBSCRIPT_MACRO() \
index1 + \
index2 * _size1 + \
index3 * _size2 + \
index4 * _size3 + \
index5 * _size4 + \
index6 * _size5 

// Once we represent macros for index array operands the complexity increases because we have to have a way to
// represent the offsets for each dimension.  Additionally to keep the transformations as simple as possible
// we want to make them dependent upon the array dimensions.  This multiplies the number of macros by the
// maximum number of array dimensions (typically about 6 for A++/P++).  This may force us to explicitly build the
// macros for subscript computation at compile time (as part of the transformations).

#define INDEX_VARIABLE_SIZE_VARIABLE_STRIDE_SUBSCRIPT_MACRO(ARRAY_VARIABLE_NAME,INDEX_VARIABLE_NAME,OFFSET) \
(INDEX_VARIABLE_NAME ##_index + OFFSET) * ARRAY_VARIABLE_NAME ##_stride1 

#define INDEX_VARIABLE_SIZE_UNIFORM_STRIDE_SUBSCRIPT_MACRO(ARRAY_VARIABLE_NAME,INDEX_VARIABLE_NAME,OFFSET) \
(INDEX_VARIABLE_NAME ##_index + OFFSET) * ARRAY_VARIABLE_NAME ##_stride1 

#define INDEX_UNIFORM_SIZE_VARIABLE_STRIDE_SUBSCRIPT_MACRO(ARRAY_VARIABLE_NAME,INDEX_VARIABLE_NAME,OFFSET) \
(INDEX_VARIABLE_NAME ##_index + OFFSET) * ARRAY_VARIABLE_NAME ##_stride1 

#define INDEX_UNIFORM_SIZE_UNIT_STRIDE_SUBSCRIPT_MACRO(INDEX_VARIABLE_NAME,OFFSET) \
(INDEX_VARIABLE_NAME ##_index + OFFSET) stride1

#define INDEX_UNIFORM_SIZE_STRIDE_ONE_SUBSCRIPT_MACRO(INDEX_VARIABLE_NAME,OFFSET) \
(INDEX_VARIABLE_NAME ##_index + OFFSET)

#define INDEX_SINGLE_INDEX_VARIABLE_UNIFORM_SIZE_STRIDE_ONE_SUBSCRIPT_MACRO(OFFSET1,OFFSET2,OFFSET3,OFFSET4,OFFSET5,OFFSET6) \
(index1 + OFFSET1) + \
(index2 + OFFSET2) * _size1 + \
(index3 + OFFSET3) * _size2 + \
(index4 + OFFSET4) * _size3 + \
(index5 + OFFSET5) * _size4 + \
(index6 + OFFSET6) * _size5

#endif











