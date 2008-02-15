// UNIFORM SIZE
#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D1(VARIABLE_NAME)

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D2(VARIABLE_NAME) \
   int size1;

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D3(VARIABLE_NAME) \
   int size1; \
   int size2;

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D4(VARIABLE_NAME) \
   int size1; \
   int size2; \
   int size3;

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D5(VARIABLE_NAME) \
   int size1; \
   int size2; \
   int size3; \
   int size4;

#define ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D6(VARIABLE_NAME) \
   int size1; \
   int size2; \
   int size3; \
   int size4; \
   int size5;

// This macro is used when the strides are the same across a number of operands, but not not equal to 1
// thus the subscript computations share variables within their subscript computations
#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D1(VARIABLE_NAME) \
   int stride1;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D2(VARIABLE_NAME) \
   int stride1; \
   int stride2;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D3(VARIABLE_NAME) \
   int stride1; \
   int stride2; \
   int stride3;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D4(VARIABLE_NAME) \
   int stride1; \
   int stride2; \
   int stride3; \
   int stride4;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D5(VARIABLE_NAME) \
   int stride1; \
   int stride2; \
   int stride3; \
   int stride4; \
   int stride5;

#define ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D6(VARIABLE_NAME) \
   int stride1; \
   int stride2; \
   int stride3; \
   int stride4; \
   int stride5; \
   int stride6;

// VARIABLE SIZE
// This macro is used when the sizes are different across a number of operands
#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D1(VARIABLE_NAME)

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D2(VARIABLE_NAME) \
   int VARIABLE_NAME ##_size1;

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D3(VARIABLE_NAME) \
   int VARIABLE_NAME ##_size1; \
   int VARIABLE_NAME ##_size2;

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D4(VARIABLE_NAME) \
   int VARIABLE_NAME ##_size1; \
   int VARIABLE_NAME ##_size2; \
   int VARIABLE_NAME ##_size3;

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D5(VARIABLE_NAME) \
   int VARIABLE_NAME ##_size1; \
   int VARIABLE_NAME ##_size2; \
   int VARIABLE_NAME ##_size3; \
   int VARIABLE_NAME ##_size4;

#define ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D6(VARIABLE_NAME) \
   int VARIABLE_NAME ##_size1; \
   int VARIABLE_NAME ##_size2; \
   int VARIABLE_NAME ##_size3; \
   int VARIABLE_NAME ##_size4; \
   int VARIABLE_NAME ##_size5;

// This macro is used when all the operands can share variables used within there subscript
// computations. Within most compilers this explicit initialization of the stride to 1 allows
// constant folding will allow the elimination of the stride from the subscript computations.
#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D1() \
   stride1 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D2() \
   stride1 = 1; \
   stride2 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D3() \
   stride1 = 1; \
   stride2 = 1; \
   stride3 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D4() \
   stride1 = 1; \
   stride2 = 1; \
   stride3 = 1; \
   stride4 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D5() \
   stride1 = 1; \
   stride2 = 1; \
   stride3 = 1; \
   stride4 = 1; \
   stride5 = 1;

#define ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D6() \
   stride1 = 1; \
   stride2 = 1; \
   stride3 = 1; \
   stride4 = 1; \
   stride5 = 1; \
   stride6 = 1;

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D1(VARIABLE_NAME) \

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D2(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0];

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D3(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1];

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D4(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2];

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D5(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3];

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3]; \
   size5   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[4];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D1(VARIABLE_NAME)

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D2(VARIABLE_NAME) \
   VARIABLE_NAME ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D3(VARIABLE_NAME) \
   VARIABLE_NAME ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   VARIABLE_NAME ##_size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D4(VARIABLE_NAME) \
   VARIABLE_NAME ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   VARIABLE_NAME ##_size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   VARIABLE_NAME ##_size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D5(VARIABLE_NAME) \
   VARIABLE_NAME ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   VARIABLE_NAME ##_size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   VARIABLE_NAME ##_size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   VARIABLE_NAME ##_size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3];

#define ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D6(VARIABLE_NAME) \
   VARIABLE_NAME ##_size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   VARIABLE_NAME ##_size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   VARIABLE_NAME ##_size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   VARIABLE_NAME ##_size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3]; \
   VARIABLE_NAME ##_size5   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[4];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D1(VARIABLE_NAME) \
   stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D2(VARIABLE_NAME) \
   stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D3(VARIABLE_NAME) \
   stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D4(VARIABLE_NAME) \
   stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D5(VARIABLE_NAME) \
   stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3]; \
   stride5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[4];

#define ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D6(VARIABLE_NAME) \
   stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3]; \
   stride5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[4]; \
   stride6 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[5];

// This macro is used when the strides are different across a number of operands and thus each
// requires it's own stride variable.
#define ARRAY_OPERAND_VARIABLE_STRIDE_MACRO_D1(VARIABLE_NAME) \
   int VARIABLE_NAME ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0];

#define ARRAY_OPERAND_VARIABLE_STRIDE_MACRO_D2(VARIABLE_NAME) \
   int VARIABLE_NAME ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   int VARIABLE_NAME ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1];

#define ARRAY_OPERAND_VARIABLE_STRIDE_MACRO_D3(VARIABLE_NAME) \
   int VARIABLE_NAME ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   int VARIABLE_NAME ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   int VARIABLE_NAME ##_stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2];

#define ARRAY_OPERAND_VARIABLE_STRIDE_MACRO_D4(VARIABLE_NAME) \
   int VARIABLE_NAME ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   int VARIABLE_NAME ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   int VARIABLE_NAME ##_stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   int VARIABLE_NAME ##_stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3];

#define ARRAY_OPERAND_VARIABLE_STRIDE_MACRO_D5(VARIABLE_NAME) \
   int VARIABLE_NAME ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   int VARIABLE_NAME ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   int VARIABLE_NAME ##_stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   int VARIABLE_NAME ##_stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3]; \
   int VARIABLE_NAME ##_stride5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[4];

#define ARRAY_OPERAND_VARIABLE_STRIDE_MACRO_D6(VARIABLE_NAME) \
   int VARIABLE_NAME ##_stride1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[0]; \
   int VARIABLE_NAME ##_stride2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[1]; \
   int VARIABLE_NAME ##_stride3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[2]; \
   int VARIABLE_NAME ##_stride4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[3]; \
   int VARIABLE_NAME ##_stride5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[4]; \
   int VARIABLE_NAME ##_stride6 = VARIABLE_NAME.Array_Descriptor.Array_Domain.Stride[5];





















