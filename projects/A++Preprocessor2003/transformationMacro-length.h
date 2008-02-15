#ifdef ROSE_MACROS_APPEAR_AS_FUNCTIONS

// Macros defined here appear as functions to the intermediate AST generation mechanisms within
// ROSE. The map directly to macros in the final stages of the xpression of the transformations
// within the source-to-source processing of application codes processed with the A++/P++
// preprocessor.

#define ARRAY_TRANSFORMATION_LENGTH_DATA(T) \
void ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D1(T ##Array & X); \
void ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D2(T ##Array & X); \
void ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D3(T ##Array & X); \
void ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D4(T ##Array & X); \
void ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D5(T ##Array & X); \
void ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D6(T ##Array & X);

// Declare functions for all different A++/P++ types
ARRAY_TRANSFORMATION_LENGTH_DATA(int)
ARRAY_TRANSFORMATION_LENGTH_DATA(float)
ARRAY_TRANSFORMATION_LENGTH_DATA(double)

#else

// These macros declare and initialize variables that support the A++/P++ array transformations.
// The different macros handles the different levels of optimization possible, from most general
// transformations to transformation which know either the dimension, size, stride, etc.



#define ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D1(VARIABLE_NAME) \
   _length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0);

#define ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D2(VARIABLE_NAME) \
   _length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   _length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1);

#define ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D3(VARIABLE_NAME) \
   _length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   _length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1); \
   _length3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(2);

#define ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D4(VARIABLE_NAME) \
   _length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   _length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1); \
   _length3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(2); \
   _length4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(3);

#define ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D5(VARIABLE_NAME) \
   _length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   _length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1); \
   _length3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(2); \
   _length4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(3); \
   _length5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(4);

#define ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D6(VARIABLE_NAME) \
   _length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   _length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1); \
   _length3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(2); \
   _length4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(3); \
   _length5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(4); \
   _length6 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(5);

#endif
