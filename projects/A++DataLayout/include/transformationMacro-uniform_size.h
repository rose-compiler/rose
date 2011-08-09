// ###################################################################
//                          UNIFORM SIZE
// ###################################################################
// This macro is used when the sizes of all the operands are the same.
// I have split up the declaration from the initialization because
// the declaration should happen while the initialization needs to
// happen before each nested loop in the transformation is executed.
// I also had to split it up because we can't traverse SgInitializer
// objects in the current version of the global tree traversal (being 
// fixed).
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

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D1(VARIABLE_NAME) \
   length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0);

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D2(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1);

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D3(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1); \
   length3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(2);

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D4(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1); \
   length3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(2); \
   length4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(3);

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D5(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3]; \
   length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1); \
   length3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(2); \
   length4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(3); \
   length5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(4);

#define ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(VARIABLE_NAME) \
   size1   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[0]; \
   size2   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[1]; \
   size3   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[2]; \
   size4   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[3]; \
   size5   = VARIABLE_NAME.Array_Descriptor.Array_Domain.Size[4]; \
   length1 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(0); \
   length2 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(1); \
   length3 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(2); \
   length4 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(3); \
   length5 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(4); \
   length6 = VARIABLE_NAME.Array_Descriptor.Array_Domain.getLength(5);


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

