
#ifdef ROSE_MACROS_APPEAR_AS_FUNCTIONS

// Macros defined here appear as functions to the intermediate AST generation mechanisms within
// ROSE. The map directly to macros in the final stages of the xpression of the transformations
// within the source-to-source processing of application codes processed with the A++/P++
// preprocessor.

#if 0
// These are not neded since they would be the same as 
// ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_DX();
void ARRAY_OPERAND_STRIDE_ONE_DECLARATION_MACRO_D1(); \
void ARRAY_OPERAND_STRIDE_ONE_DECLARATION_MACRO_D2(); \
void ARRAY_OPERAND_STRIDE_ONE_DECLARATION_MACRO_D3(); \
void ARRAY_OPERAND_STRIDE_ONE_DECLARATION_MACRO_D4(); \
void ARRAY_OPERAND_STRIDE_ONE_DECLARATION_MACRO_D5(); \
void ARRAY_OPERAND_STRIDE_ONE_DECLARATION_MACRO_D6();
#endif

#define ARRAY_OPERAND_TRANSFORMATION_STRIDE_DATA(T) \
void ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D1(); \
void ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D2(); \
void ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D3(); \
void ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D4(); \
void ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D5(); \
void ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D6(); \
void ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D1(); \
void ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D2(); \
void ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D3(); \
void ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D4(); \
void ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D5(); \
void ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D6(); \
void ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D1(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D2(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D3(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D4(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D5(T ##Array & X); \
void ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D6(T ##Array & X); \
void ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D1(int X); \
void ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D2(int X); \
void ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D3(int X); \
void ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D4(int X); \
void ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D5(int X); \
void ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D6(int X); \
void ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D1(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D2(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D3(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D4(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D5(T ##Array & X, int Y); \
void ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D6(T ##Array & X, int Y);

#if 1
// Declare functions for all different A++/P++ types
ARRAY_OPERAND_TRANSFORMATION_STRIDE_DATA(int)
ARRAY_OPERAND_TRANSFORMATION_STRIDE_DATA(float)
ARRAY_OPERAND_TRANSFORMATION_STRIDE_DATA(double)
#endif

#else

// These macros declare and initialize variables that support the A++/P++ array transformations.
// The different macros handles the different levels of optimization possible, from most general
// transformations to transformation which know either the dimension, size, stride, etc.

// This macro is used when the strides are the same across a number of operands, but not not equal to 1
// thus the subscript computations share variables within their subscript computations
#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D1() \
   int _stride1;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D2() \
   int _stride1; \
   int _stride2;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D3() \
   int _stride1; \
   int _stride2; \
   int _stride3;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D4() \
   int _stride1; \
   int _stride2; \
   int _stride3; \
   int _stride4;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D5() \
   int _stride1; \
   int _stride2; \
   int _stride3; \
   int _stride4; \
   int _stride5;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D6() \
   int _stride1; \
   int _stride2; \
   int _stride3; \
   int _stride4; \
   int _stride5; \
   int _stride6;

// This macro is used when the strides are different across a number of operands and thus each
// requires it's own stride variable.
#define ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D1(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_stride1;

#define ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D2(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_stride1; \
   int VARIABLE_IDENTIFIER ##_stride2;

#define ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D3(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_stride1; \
   int VARIABLE_IDENTIFIER ##_stride2; \
   int VARIABLE_IDENTIFIER ##_stride3;

#define ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D4(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_stride1; \
   int VARIABLE_IDENTIFIER ##_stride2; \
   int VARIABLE_IDENTIFIER ##_stride3; \
   int VARIABLE_IDENTIFIER ##_stride4;

#define ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D5(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_stride1; \
   int VARIABLE_IDENTIFIER ##_stride2; \
   int VARIABLE_IDENTIFIER ##_stride3; \
   int VARIABLE_IDENTIFIER ##_stride4; \
   int VARIABLE_IDENTIFIER ##_stride5;

#define ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D6(VARIABLE_IDENTIFIER) \
   int VARIABLE_IDENTIFIER ##_stride1; \
   int VARIABLE_IDENTIFIER ##_stride2; \
   int VARIABLE_IDENTIFIER ##_stride3; \
   int VARIABLE_IDENTIFIER ##_stride4; \
   int VARIABLE_IDENTIFIER ##_stride5; \
   int VARIABLE_IDENTIFIER ##_stride6;

// This macro is used when all the operands can share variables used within there subscript
// computations. Within most compilers this explicit initialization of the stride to 1 allows
// constant folding will allow the elimination of the stride from the subscript computations.
#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D1() \
   _stride1 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D2() \
   _stride1 = 1; \
   _stride2 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D3() \
   _stride1 = 1; \
   _stride2 = 1; \
   _stride3 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D4() \
   _stride1 = 1; \
   _stride2 = 1; \
   _stride3 = 1; \
   _stride4 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D5() \
   _stride1 = 1; \
   _stride2 = 1; \
   _stride3 = 1; \
   _stride4 = 1; \
   _stride5 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D6() \
   _stride1 = 1; \
   _stride2 = 1; \
   _stride3 = 1; \
   _stride4 = 1; \
   _stride5 = 1; \
   _stride6 = 1;

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D1(VARIABLE_NAME) \
   _stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D2(VARIABLE_NAME) \
   _stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   _stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D3(VARIABLE_NAME) \
   _stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   _stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   _stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D4(VARIABLE_NAME) \
   _stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   _stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   _stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   _stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D5(VARIABLE_NAME) \
   _stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   _stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   _stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   _stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3]; \
   _stride5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[4];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D6(VARIABLE_NAME) \
   _stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   _stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   _stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   _stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3]; \
   _stride5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[4]; \
   _stride6 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[5];

// This macro is used when the strides are different across a number of operands and thus each
// requires it's own stride variable.
#define ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D1(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0];

#define ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D2(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   VARIABLE_IDENTIFIER ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1];

#define ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D3(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   VARIABLE_IDENTIFIER ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   VARIABLE_IDENTIFIER ##_stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2];

#define ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D4(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   VARIABLE_IDENTIFIER ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   VARIABLE_IDENTIFIER ##_stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   VARIABLE_IDENTIFIER ##_stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3];

#define ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D5(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   VARIABLE_IDENTIFIER ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   VARIABLE_IDENTIFIER ##_stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   VARIABLE_IDENTIFIER ##_stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3]; \
   VARIABLE_IDENTIFIER ##_stride5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[4];

#define ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D6(VARIABLE_NAME,VARIABLE_IDENTIFIER) \
   VARIABLE_IDENTIFIER ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   VARIABLE_IDENTIFIER ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   VARIABLE_IDENTIFIER ##_stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   VARIABLE_IDENTIFIER ##_stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3]; \
   VARIABLE_IDENTIFIER ##_stride5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[4]; \
   VARIABLE_IDENTIFIER ##_stride6 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[5];

#endif








