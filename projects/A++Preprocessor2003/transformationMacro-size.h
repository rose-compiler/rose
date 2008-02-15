#ifdef ROSE_MACROS_APPEAR_AS_FUNCTIONS

// Macros defined here appear as functions to the intermediate AST generation mechanisms within
// ROSE. The map directly to macros in the final stages of the xpression of the transformations
// within the source-to-source processing of application codes processed with the A++/P++
// preprocessor.

#define ARRAY_OPERAND_TRANSFORMATION_SIZE_DATA(T) \
void ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D1(); \
void ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D2(); \
void ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D3(); \
void ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D4(); \
void ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D5(); \
void ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D6(); \
void ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D1(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D2(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D3(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D4(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D5(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(T ##Array & X); \
void ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D1(int X); \
void ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D2(int X); \
void ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D3(int X); \
void ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D4(int X); \
void ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D5(int X); \
void ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D6(int X); \
void ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D1(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D2(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D3(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D4(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D5(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D6(T ##Array & X, int Y);

#if 1
// Declare functions for all different A++/P++ types
ARRAY_OPERAND_TRANSFORMATION_SIZE_DATA(int)
ARRAY_OPERAND_TRANSFORMATION_SIZE_DATA(float)
ARRAY_OPERAND_TRANSFORMATION_SIZE_DATA(double)
#endif

#else

// These macros declare and initialize variables that support the A++/P++ array transformations.
// The different macros handles the different levels of optimization possible, from most general
// transformations to transformation which know either the dimension, size, stride, etc.

// ###########################
//  UNIFORM SIZE DECLARATION
// ###########################
#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D1()

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D2() \
   int _size1;

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D3() \
   int _size1; \
   int _size2;

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D4() \
   int _size1; \
   int _size2; \
   int _size3;

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D5() \
   int _size1; \
   int _size2; \
   int _size3; \
   int _size4;

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D6() \
   int _size1; \
   int _size2; \
   int _size3; \
   int _size4; \
   int _size5;

// ###########################
// UNIFORM SIZE INITIALIZATION
// ###########################
#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D1(VARIABLE_NAME) \

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D2(VARIABLE_NAME) \
   _size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0];

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D3(VARIABLE_NAME) \
   _size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   _size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1];

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D4(VARIABLE_NAME) \
   _size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   _size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   _size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2];

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D5(VARIABLE_NAME) \
   _size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   _size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   _size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   _size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3];

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(VARIABLE_NAME) \
   _size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   _size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   _size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   _size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3]; \
   _size5   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[4];


// ###########################
//  VARIABLE SIZE DECLARATION
// ###########################
// This macro is used when the sizes are different across a number of operands
#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D1(VARIABLE_IDENTIFIER)

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D2(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_size1;

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D3(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_size1; \
   int VARIABLE_IDENTIFIER ##_size2;

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D4(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_size1; \
   int VARIABLE_IDENTIFIER ##_size2; \
   int VARIABLE_IDENTIFIER ##_size3;

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D5(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_size1; \
   int VARIABLE_IDENTIFIER ##_size2; \
   int VARIABLE_IDENTIFIER ##_size3; \
   int VARIABLE_IDENTIFIER ##_size4;

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D6(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_size1; \
   int VARIABLE_IDENTIFIER ##_size2; \
   int VARIABLE_IDENTIFIER ##_size3; \
   int VARIABLE_IDENTIFIER ##_size4; \
   int VARIABLE_IDENTIFIER ##_size5;

// ############################
// VARIABLE SIZE INITIALIZATION
// ############################
#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D1(VARIABLE_NAME,VARIABLE_IDENTIFIER)

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D2(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D3(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   VARIABLE_IDENTIFIER ##_size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D4(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   VARIABLE_IDENTIFIER ##_size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   VARIABLE_IDENTIFIER ##_size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D5(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   VARIABLE_IDENTIFIER ##_size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   VARIABLE_IDENTIFIER ##_size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   VARIABLE_IDENTIFIER ##_size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D6(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   VARIABLE_IDENTIFIER ##_size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   VARIABLE_IDENTIFIER ##_size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   VARIABLE_IDENTIFIER ##_size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3]; \
   VARIABLE_IDENTIFIER ##_size5   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[4];

#endif







