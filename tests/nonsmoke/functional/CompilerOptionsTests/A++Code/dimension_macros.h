// These macros allow A++/P++ to be dimension independent (allowing upto 8D arrays and
// more if you write the dimension dependent constructors yourself).  Actually
// if anyone has a good reason for wanting more I can easily provide them for you
// I just stopped at 8D arrays.  Fewer dimensions also works fine the number of 
// dimensions provided in the A++/P++ library is specified at compile time (of the 
// A++/P++ library).  A++/P++ must be compiled with at least 1D arrays and upto 8D is
// provided internally.  In priciple you could have 100 or greater dimensional arrays
// but there seems little reason for much beyond 6-8 dimensions.

// These macros contain the strings used in the function definitions for functions
// which are not dimension dependent.  parameter initialization then allows for the
// lower dimension versions to appear equally available.  To change the number of 
// dimensions you only have to fill in the additional number of parameters in the 
// macros below and set the macro called MAX_ARRAY_DIMENSION to the correct value 
// (the max number of dimensions you have provided in the macros below).  This 
// example is for 8D arrays.

// An alternate version of the macros are provided for the SUN4 machines since their 
// compilers // are superior

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif

#include <max_array_dim.h>

#if defined(CRAY) || defined(SGI)
// These machines' compilers do not understand the initialization list used in the 
// macros below so we have provide alternative difinitions of these macros for these 
// machines.
#define USE_SLOWER_DIMENSION_MACRO
#endif

#define IO_CONTROL_STRING_MACRO_INTEGER "%d, %d, %d, %d, %d, %d"
#define ARRAY_TO_LIST_MACRO(A) A[0],A[1],A[2],A[3],A[4],A[5]
#define REPEATING_MACRO(A) A, A, A, A, A, A
#define IOTA_MACRO 0,1,2,3,4,5

#define VARIABLE_LIST_MACRO_INTEGER  i, j, k, l, m, n 
#define ARGUMENT_LIST_MACRO_INTEGER \
     int i, int j, int k, int l, int m, int n 
#define ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION \
     int i, int j=1, int k=1, int l=1, int m=1, int n=1
#define ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO \
     int i, int j=0, int k=0, int l=0, int m=0, int n=0
#define ARGUMENT_LIST_MACRO_CONST_REF_RANGE_WITH_PREINITIALIZATION \
     const Range & Range_I, \
     const Range & Range_J = APP_Unit_Range, \
     const Range & Range_K = APP_Unit_Range, \
     const Range & Range_L = APP_Unit_Range, \
     const Range & Range_M = APP_Unit_Range, \
     const Range & Range_N = APP_Unit_Range
#define VARIABLE_LIST_MACRO_CONST_REF_INDEX Index_I,Index_J,Index_K,Index_L,Index_M,Index_N
#define ARGUMENT_LIST_MACRO_CONST_REF_INDEX \
     const Index & Index_I, \
     const Index & Index_J, \
     const Index & Index_K, \
     const Index & Index_L, \
     const Index & Index_M, \
     const Index & Index_N
#define VARIABLE_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE i,Range_I,j,Range_J,k,Range_K,l,Range_L,m,Range_M,n,Range_N
#define ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE \
     int i , const Range & Range_I , int j, const Range & Range_J, \
     int k , const Range & Range_K , int l, const Range & Range_L, \
     int m , const Range & Range_M , int n, const Range & Range_N

#if defined(USE_SLOWER_DIMENSION_MACRO)
// Some computers use the Cfront C++ compiler which has many unimplemented C++ features
#define INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO \
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List; \
     Integer_List [0] = i; \
     Integer_List [1] = j; \
     Integer_List [2] = k; \
     Integer_List [3] = l; \
     Integer_List [4] = m; \
     Integer_List [5] = n; 
#else
// We previously used "const" for this array but several compilers (SOLARIS) complained
#define INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO \
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List = \
     { i, j, k, l, m, n};
#endif

// Use the previous macro to define this macro  ---  This can't be done
#if defined(USE_SLOWER_DIMENSION_MACRO)
// Some computers use the Cfront C++ compiler which has many unimplemented C++ features
#define USER_DEFINED_INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO(i,j,k,l,m,n,o,p) \
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List; \
     Integer_List [0] = i; \
     Integer_List [1] = j; \
     Integer_List [2] = k; \
     Integer_List [3] = l; \
     Integer_List [4] = m; \
     Integer_List [5] = n; 
#else
// We previously used "const" for this array but several compilers (SOLARIS) complained
#define USER_DEFINED_INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO(i,j,k,l,m,n,o,p) \
     Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List = \
     { i, j, k, l, m, n};
#endif

#define VARIABLE_LIST_MACRO_CONST_REF_RANGE Range_I,Range_J,Range_K,Range_L,Range_M,Range_N
#define ARGUMENT_LIST_MACRO_CONST_REF_RANGE \
     const Range & Range_I , const Range & Range_J , \
     const Range & Range_K , const Range & Range_L , \
     const Range & Range_M , const Range & Range_N 

#if EXTRA_ERROR_CHECKING
#define RANGE_ARGUMENTS_TO_RANGE_LIST_MACRO \
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; \
     Range_I.Test_Consistency("Range_I"); \
     Range_J.Test_Consistency("Range_J"); \
     Range_K.Test_Consistency("Range_K"); \
     Range_L.Test_Consistency("Range_L"); \
     Range_M.Test_Consistency("Range_M"); \
     Range_N.Test_Consistency("Range_N"); \
     Internal_Index_List[0] = &((Range &) Range_I); \
     Internal_Index_List[1] = &((Range &) Range_J); \
     Internal_Index_List[2] = &((Range &) Range_K); \
     Internal_Index_List[3] = &((Range &) Range_L); \
     Internal_Index_List[4] = &((Range &) Range_M); \
     Internal_Index_List[5] = &((Range &) Range_N); 
#else
#define RANGE_ARGUMENTS_TO_RANGE_LIST_MACRO \
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; \
     Internal_Index_List[0] = &((Range &) Range_I); \
     Internal_Index_List[1] = &((Range &) Range_J); \
     Internal_Index_List[2] = &((Range &) Range_K); \
     Internal_Index_List[3] = &((Range &) Range_L); \
     Internal_Index_List[4] = &((Range &) Range_M); \
     Internal_Index_List[5] = &((Range &) Range_N); 
#endif

#define COMPUTE_RANGE_ARGUMENTS_MACRO \
     Range(APP_Global_Array_Base,i+APP_Global_Array_Base-1), \
     Range(APP_Global_Array_Base,j+APP_Global_Array_Base-1), \
     Range(APP_Global_Array_Base,k+APP_Global_Array_Base-1), \
     Range(APP_Global_Array_Base,l+APP_Global_Array_Base-1), \
     Range(APP_Global_Array_Base,m+APP_Global_Array_Base-1), \
     Range(APP_Global_Array_Base,n+APP_Global_Array_Base-1)

#define ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE_WITH_PREINITIALIZATION \
     int i , const Range & Range_I , \
     int j = 1, const Range & Range_J = APP_Unit_Range, \
     int k = 1, const Range & Range_K = APP_Unit_Range, \
     int l = 1, const Range & Range_L = APP_Unit_Range, \
     int m = 1, const Range & Range_M = APP_Unit_Range, \
     int n = 1, const Range & Range_N = APP_Unit_Range

#define INTEGER_AND_RANGE_ARGUMENTS_TO_INTEGER_AND_CONST_REF_RANGE_LIST_MACRO \
                            INTEGER_ARGUMENTS_TO_INTEGER_LIST_MACRO \
                            RANGE_ARGUMENTS_TO_RANGE_LIST_MACRO

#if EXTRA_ERROR_CHECKING
#define INDEX_ARGUMENTS_TO_INDEX_LIST_MACRO \
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; \
     Index_I.Test_Consistency("Index_I"); \
     Index_J.Test_Consistency("Index_J"); \
     Index_K.Test_Consistency("Index_K"); \
     Index_L.Test_Consistency("Index_L"); \
     Index_M.Test_Consistency("Index_M"); \
     Index_N.Test_Consistency("Index_N"); \
     Internal_Index_List[0] = &((Index &) Index_I); \
     Internal_Index_List[1] = &((Index &) Index_J); \
     Internal_Index_List[2] = &((Index &) Index_K); \
     Internal_Index_List[3] = &((Index &) Index_L); \
     Internal_Index_List[4] = &((Index &) Index_M); \
     Internal_Index_List[5] = &((Index &) Index_N); 
#else
#define INDEX_ARGUMENTS_TO_INDEX_LIST_MACRO \
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List; \
     Internal_Index_List[0] = &((Index &) Index_I); \
     Internal_Index_List[1] = &((Index &) Index_J); \
     Internal_Index_List[2] = &((Index &) Index_K); \
     Internal_Index_List[3] = &((Index &) Index_L); \
     Internal_Index_List[4] = &((Index &) Index_M); \
     Internal_Index_List[5] = &((Index &) Index_N); 
#endif

#define POINTER_LIST_INITIALIZATION_MACRO \
     Array_Descriptor.ExpressionTemplateDataPointer = Array_Descriptor.Array_Data+Array_Descriptor.Array_Domain.ExpressionTemplateOffset;\
     Array_Descriptor.Array_View_Pointer0 = Array_Descriptor.Array_Data+Array_Descriptor.Array_Domain.Scalar_Offset[0];\
     Array_Descriptor.Array_View_Pointer1 = Array_Descriptor.Array_Data+Array_Descriptor.Array_Domain.Scalar_Offset[1];\
     Array_Descriptor.Array_View_Pointer2 = Array_Descriptor.Array_Data+Array_Descriptor.Array_Domain.Scalar_Offset[2];\
     Array_Descriptor.Array_View_Pointer3 = Array_Descriptor.Array_Data+Array_Descriptor.Array_Domain.Scalar_Offset[3];\
     Array_Descriptor.Array_View_Pointer4 = Array_Descriptor.Array_Data+Array_Descriptor.Array_Domain.Scalar_Offset[4];\
     Array_Descriptor.Array_View_Pointer5 = Array_Descriptor.Array_Data+Array_Descriptor.Array_Domain.Scalar_Offset[5];

#define DESCRIPTOR_POINTER_LIST_INITIALIZATION_MACRO \
     ExpressionTemplateDataPointer = Array_Data+Array_Domain.ExpressionTemplateOffset;\
     Array_View_Pointer0 = Array_Data+Array_Domain.Scalar_Offset[0];\
     Array_View_Pointer1 = Array_Data+Array_Domain.Scalar_Offset[1];\
     Array_View_Pointer2 = Array_Data+Array_Domain.Scalar_Offset[2];\
     Array_View_Pointer3 = Array_Data+Array_Domain.Scalar_Offset[3];\
     Array_View_Pointer4 = Array_Data+Array_Domain.Scalar_Offset[4];\
     Array_View_Pointer5 = Array_Data+Array_Domain.Scalar_Offset[5];

#define DESCRIPTOR_POINTER_LIST_NULL_INITIALIZATION_MACRO \
     ExpressionTemplateDataPointer = NULL; \
     Array_View_Pointer0 = NULL; \
     Array_View_Pointer1 = NULL; \
     Array_View_Pointer2 = NULL; \
     Array_View_Pointer3 = NULL; \
     Array_View_Pointer4 = NULL; \
     Array_View_Pointer5 = NULL;

#define POINTER_LIST_NULL_INITIALIZATION_MACRO \
     Array_Descriptor.ExpressionTemplateDataPointer = NULL; \
     Array_Descriptor.Array_View_Pointer0 = NULL; \
     Array_Descriptor.Array_View_Pointer1 = NULL; \
     Array_Descriptor.Array_View_Pointer2 = NULL; \
     Array_Descriptor.Array_View_Pointer3 = NULL; \
     Array_Descriptor.Array_View_Pointer4 = NULL; \
     Array_Descriptor.Array_View_Pointer5 = NULL;

#define TEMPORARY_POINTER_LIST_INITIALIZATION_MACRO \
     Temporary_Array->Array_Descriptor.ExpressionTemplateDataPointer = Temporary_Array->Array_Descriptor.Array_Data+ \
        Temporary_Array->Array_Descriptor.Array_Domain.ExpressionTemplateOffset;          \
     Temporary_Array->Array_Descriptor.Array_View_Pointer0 = Temporary_Array->Array_Descriptor.Array_Data+ \
        Temporary_Array->Array_Descriptor.Array_Domain.Scalar_Offset[0];          \
     Temporary_Array->Array_Descriptor.Array_View_Pointer1 = Temporary_Array->Array_Descriptor.Array_Data+ \
        Temporary_Array->Array_Descriptor.Array_Domain.Scalar_Offset[1];          \
     Temporary_Array->Array_Descriptor.Array_View_Pointer2 = Temporary_Array->Array_Descriptor.Array_Data+ \
        Temporary_Array->Array_Descriptor.Array_Domain.Scalar_Offset[2];          \
     Temporary_Array->Array_Descriptor.Array_View_Pointer3 = Temporary_Array->Array_Descriptor.Array_Data+ \
        Temporary_Array->Array_Descriptor.Array_Domain.Scalar_Offset[3];          \
     Temporary_Array->Array_Descriptor.Array_View_Pointer4 = Temporary_Array->Array_Descriptor.Array_Data+ \
        Temporary_Array->Array_Descriptor.Array_Domain.Scalar_Offset[4];          \
     Temporary_Array->Array_Descriptor.Array_View_Pointer5 = Temporary_Array->Array_Descriptor.Array_Data+ \
        Temporary_Array->Array_Descriptor.Array_Domain.Scalar_Offset[5];

#define SERIAL_POINTER_LIST_INITIALIZATION_MACRO        \
     Array_Descriptor.SerialArray->Array_Descriptor.ExpressionTemplateDataPointer = \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Data+        \
      Array_Descriptor.SerialArray->Array_Descriptor.  \
      Array_Domain.ExpressionTemplateOffset;           \
     Array_Descriptor.SerialArray->Array_Descriptor.Array_View_Pointer0 = \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Data+         \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[0];\
     Array_Descriptor.SerialArray->Array_Descriptor.Array_View_Pointer1 = \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Data+        \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[1];\
     Array_Descriptor.SerialArray->Array_Descriptor.Array_View_Pointer2 = \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Data+        \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[2];\
     Array_Descriptor.SerialArray->Array_Descriptor.Array_View_Pointer3 = \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Data+        \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[3];\
     Array_Descriptor.SerialArray->Array_Descriptor.Array_View_Pointer4 = \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Data+        \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[4];\
     Array_Descriptor.SerialArray->Array_Descriptor.Array_View_Pointer5 = \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Data+        \
      Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[5];

#define DESCRIPTOR_SERIAL_POINTER_LIST_INITIALIZATION_MACRO \
     SerialArray->Array_Descriptor.ExpressionTemplateDataPointer = \
        SerialArray->Array_Descriptor.Array_Data+SerialArray->Array_Descriptor.Array_Domain.ExpressionTemplateOffset;\
     SerialArray->Array_Descriptor.Array_View_Pointer0 = \
        SerialArray->Array_Descriptor.Array_Data+ \
	SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[0];\
     SerialArray->Array_Descriptor.Array_View_Pointer1 = \
        SerialArray->Array_Descriptor.Array_Data+ \
	SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[1];\
     SerialArray->Array_Descriptor.Array_View_Pointer2 = \
        SerialArray->Array_Descriptor.Array_Data+ \
	SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[2];\
     SerialArray->Array_Descriptor.Array_View_Pointer3 = \
        SerialArray->Array_Descriptor.Array_Data+ \
	SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[3];\
     SerialArray->Array_Descriptor.Array_View_Pointer4 = \
        SerialArray->Array_Descriptor.Array_Data+ \
	SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[4];\
     SerialArray->Array_Descriptor.Array_View_Pointer5 = \
        SerialArray->Array_Descriptor.Array_Data+ \
	SerialArray->Array_Descriptor.Array_Domain.Scalar_Offset[5];



