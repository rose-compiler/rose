#if 0

// Use this to access the real A++ library

// (ultimately we have to include transformationMacros.h into A++.h)

#include "A++.h"

#include "transformationOptions.h"

#include "transformationMacros.h"

#else

// This contains a simpler A++ definition useful for fast compilation

#include "simpleA++.h"

#include "transformationOptions.h"

#include "transformationMacros.h"

#endif


#include "stdio.h"

#include "stdlib.h"






#define TEST_1D_STATEMENTS \
     InternalIndex I,J,K,L; \
     doubleArray A(10); \
     doubleArray B(10); \
     A    = 0; \
     A(I) = 0; \
     A(I) = B(I-1); \
     A(I+(m*2)) = B(1+(I-(n+4))) + B(I+n); \
     A(I) = A(I-1); \
     A(I) = A(I-1) + A(I+1); \
     A(I) = A(I-1) + A(I+1) + n; \

#define TEST_2D_STATEMENTS \
     InternalIndex I,J,K,L; \
     doubleArray A(10,10); \
     doubleArray B(10,10); \
     A      = 0; \
     A(I,J) = 0; \
     A(I,J) = B(I-1,J+1); \
     A(I+(m*2),J+(m-1)) = B(1+(I-(n+4)),J) + B(I+n,J+m); \
     A(I,J) = A(I-1,J); \
     A(I,J) = A(I-1,J) + A(I+1,J); \
     A(I,J) = A(I-1,J) + A(I+1,J) + n; \













// Mechanism to specify options to the preprocessor (global scope)

// Currently we don't handle where statements or indirect addressing (so assert that there are none)


class TransformationAssertion globalScope0(TransformationAssertion::NoWhereStatementTransformation,TransformationAssertion::NoIndirectAddressing); 


int MAX_ITERATIONS = (0); 


long clock()
   { 
     int VARIABLE_DECLARATIONS_MARKER_END; return 0;
} 



int functionNoTransformation()
   { 
     int VARIABLE_DECLARATIONS_MARKER_END; 
// TransformationAssertion assertion1 (TransformationAssertion::SkipTransformation);



     class doubleArray A(10); 

     long startClock = clock(); 
     for (int i = 0; i < MAX_ITERATIONS; i++)
          { 
            ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D6(_A); 
            double * _A_pointer = ((double * )0); 
    ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D6(_A); 
    int _1; 
int _2; 
int _3; 
int _4; 
int _5; 
int _6; 
int _length1 = 0; 
int _length2 = 0; 
int _length3 = 0; 
int _length4 = 0; 
int _length5 = 0; 
int _length6 = 0; ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D6(A,_A); 
          ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D6(A,_A); 
          ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D6(A); 
             _A_pointer = (A.getAdjustedDataPointer)(); 
             #define SC_A(x1,x2,x3,x4,x5,x6) /* case VariableSizeVariableStride */ (x1)*_A_stride1+((x2)*_A_stride2*_A_size1)+((x3)*_A_stride3*_A_size2)+((x4)*_A_stride4*_A_size3)+((x5)*_A_stride5*_A_size4)+((x6)*_A_stride6*_A_size5)
; 
             for (_6 = 0; _6 < _length6; _6++)
             { 
               for (_5 = 0; _5 < _length5; _5++)
               { 
                 for (_4 = 0; _4 < _length4; _4++)
                 { 
                   for (_3 = 0; _3 < _length3; _3++)
                   { 
                     for (_2 = 0; _2 < _length2; _2++)
                     { 
                       for (_1 = 0; _1 < _length1; _1++)
                       { 
                         _A_pointer[SC_A(_1,_2,_3,_4,_5,_6)] = 0.0; 
                       } 
                     } 
                   } 
                 } 
               } 
             } 
     } 




     long endClock = clock(); 
     long time = endClock - startClock; 

     return time;
} 


int functionA()
   { 
     ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D6(_A); 
     double * _A_pointer = ((double * )0); 
ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D6(_A); 
int _1; 
int _2; 
int _3; 
int _4; 
int _5; 
int _6; 
int _length1 = 0; 
int _length2 = 0; 
int _length3 = 0; 
int _length4 = 0; 
int _length5 = 0; 
int _length6 = 0;      class doubleArray A(10); ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D6(A,
     _A); 
     ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D6(A,_A); 
     ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D6(A); 
        _A_pointer = (A.getAdjustedDataPointer)(); 
        #define SC_A(x1,x2,x3,x4,x5,x6) /* case VariableSizeVariableStride */ (x1)*_A_stride1+((x2)*_A_stride2*_A_size1)+((x3)*_A_stride3*_A_size2)+((x4)*_A_stride4*_A_size3)+((x5)*_A_stride5*_A_size4)+((x6)*_A_stride6*_A_size5)
; 
        for (_6 = 0; _6 < _length6; _6++)
        { 
          for (_5 = 0; _5 < _length5; _5++)
          { 
            for (_4 = 0; _4 < _length4; _4++)
            { 
              for (_3 = 0; _3 < _length3; _3++)
              { 
                for (_2 = 0; _2 < _length2; _2++)
                { 
                  for (_1 = 0; _1 < _length1; _1++)
                  { 
                    _A_pointer[SC_A(_1,_2,_3,_4,_5,_6)] = 0.0; 
                  } 
                } 
              } 
            } 
          } 
        } 

















     return 0;
} 


int functionB()
   { 
     ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D6(); 
     double * _A_pointer = ((double * )0); 
ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D6(_A); 
int _1; 
int _2; 
int _3; 
int _4; 
int _5; 
int _6; 
int _length1 = 0; 
int _length2 = 0; 
int _length3 = 0; 
int _length4 = 0; 
int _length5 = 0; 
int _length6 = 0; 
// First optimization permits all the size# variables to be the same across all operands (this

// should significantly reduce the registar pressure).

     class TransformationAssertion assertion1(TransformationAssertion::SameSizeArrays); 

     class InternalIndex I; class InternalIndex J; class InternalIndex K; class InternalIndex L; 
     class doubleArray A(10); 
     class doubleArray B(10); 
     int m; int n; ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(A); 
     ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D6(A,_A); 
     ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D6(A); 
        _A_pointer = (A.getAdjustedDataPointer)(); 
        #define SC_A(x1,x2,x3,x4,x5,x6) /* case UniformSizeVariableStride */ (x1)*_A_stride1+((x2)*_A_stride2*_size1)+((x3)*_A_stride3*_size2)+((x4)*_A_stride4*_size3)+((x5)*_A_stride5*_size4)+((x6)*_A_stride6*_size5)
; 
        for (_6 = 0; _6 < _length6; _6++)
        { 
          for (_5 = 0; _5 < _length5; _5++)
          { 
            for (_4 = 0; _4 < _length4; _4++)
            { 
              for (_3 = 0; _3 < _length3; _3++)
              { 
                for (_2 = 0; _2 < _length2; _2++)
                { 
                  for (_1 = 0; _1 < _length1; _1++)
                  { 
                    _A_pointer[SC_A(_1,_2,_3,_4,_5,_6)] = 0.0; 
                  } 
                } 
              } 
            } 
          } 
        } 






























































#if 0












#endif


     return 0;
} 

int functionC()
   { 
     ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D6(); 
     double * _A_pointer = ((double * )0); 
ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D6(); 
int _1; 
int _2; 
int _3; 
int _4; 
int _5; 
int _6; 
int _length1 = 0; 
int _length2 = 0; 
int _length3 = 0; 
int _length4 = 0; 
int _length5 = 0; 
int _length6 = 0; 
extern int _stride1; 
extern int _stride2; 
extern int _stride3; 
extern int _stride4; 
extern int _stride5; 
extern int _stride6; 
// First optimization permits all the size# variables to be the same across all operands (this

// should significantly reduce the registar pressure).

     class TransformationAssertion assertion1(TransformationAssertion::SameSizeArrays); 

// Specification of uniform stride permits all the <name>_stride# variables to be the same across

// all operands (further reducing the register pressure).


     class TransformationAssertion assertion2(TransformationAssertion::ConstantStrideAccess); 

     class doubleArray A(10); ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(A); 
     ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D6(A); 
     ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D6(A); 
        _A_pointer = (A.getAdjustedDataPointer)(); 
        #define SC(x1,x2,x3,x4,x5,x6) /* case UniformSizeUniformStride */ (x1)+((x2)*_size1)+((x3)*_size2)+((x4)*_size3)+((x5)*_size4)+((x6)*_size5)
; 
        for (_6 = 0; _6 < _length6; _6 += _stride6)
        { 
          for (_5 = 0; _5 < _length5; _5 += _stride5)
          { 
            for (_4 = 0; _4 < _length4; _4 += _stride4)
            { 
              for (_3 = 0; _3 < _length3; _3 += _stride3)
              { 
                for (_2 = 0; _2 < _length2; _2 += _stride2)
                { 
                  for (_1 = 0; _1 < _length1; _1 += _stride1)
                  { 
                    _A_pointer[SC(_1,_2,_3,_4,_5,_6)] = 0.0; 
                  } 
                } 
              } 
            } 
          } 
        } 






     return 0;
} 



int functionD()
   { 
     ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D6(); 
     double * _A_pointer = ((double * )0); 
int _1; 
int _2; 
int _3; 
int _4; 
int _5; 
int _6; 
int _length1 = 0; 
int _length2 = 0; 
int _length3 = 0; 
int _length4 = 0; 
int _length5 = 0; 
int _length6 = 0; // First optimization permits all the size# variables to be the same across all operands (this

// should significantly reduce the registar pressure).


     class TransformationAssertion assertion1(TransformationAssertion::SameSizeArrays); 
// Specification of uniform stride permits all the <name>_stride# variables to be the same across

// all operands (further reducing the register pressure).



     class TransformationAssertion assertion2(TransformationAssertion::ConstantStrideAccess); 
// In addition to uniform stride we specify here that the stride is lenght 1 which permits

// additional optimizations to the subscript computation (or the loop nest) to remove the stride

// variable from the computations altogether.


     class TransformationAssertion assertion3(TransformationAssertion::StrideOneAccess); 

     class doubleArray A(10); ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(A); 
     ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D6(A); 
        _A_pointer = (A.getAdjustedDataPointer)(); 
        #define SC(x1,x2,x3,x4,x5,x6) /* case UniformSizeUnitStride */ (x1)+(x2)*_size1+(x3)*_size2+(x4)*_size3+(x5)*_size4+(x6)*_size5
; 
        for (_6 = 0; _6 < _length6; _6++)
        { 
          for (_5 = 0; _5 < _length5; _5++)
          { 
            for (_4 = 0; _4 < _length4; _4++)
            { 
              for (_3 = 0; _3 < _length3; _3++)
              { 
                for (_2 = 0; _2 < _length2; _2++)
                { 
                  for (_1 = 0; _1 < _length1; _1++)
                  { 
                    _A_pointer[SC(_1,_2,_3,_4,_5,_6)] = 0.0; 
                  } 
                } 
              } 
            } 
          } 
        } 






     return 0;
} 



int main()
   { 
     int VARIABLE_DECLARATIONS_MARKER_END; int timeWithoutTransformation = functionNoTransformation(); 
     printf(("timeWithoutTransformation = %d \n"),timeWithoutTransformation); 

     return 0;
} 
