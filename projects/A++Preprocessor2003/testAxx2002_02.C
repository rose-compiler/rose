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
     int m,n; \
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

#define TEST_3D_STENCIL_STATEMENTS \
     int n; \
     Range I,J,K; \
     doubleArray Residual(n,n,n); \
     doubleArray Right_Hand_Side(n,n,n); \
     doubleArray Solution(n,n,n); \
     double Scaling_Factor; \
     Residual (I,J,K) = Right_Hand_Side (I,J,K) + Scaling_Factor * ( \
                        Solution (I+1,J,K) + Solution (I-1,J,K) + \
                        Solution (I,J-1,K) + Solution (I,J+1,K) + Solution (I,J,K-1) + \
                        Solution (I,J,K+1) - 6.0 * Solution (I,J,K) );

#define TEST_2D_STENCIL_STATEMENTS \
     int n; \
     Range I,J,K; \
     doubleArray Right_Hand_Side(n,n); \
     doubleArray Solution(n,n); \
     double Mesh_Size; \
     Solution (I,J) = ( (Mesh_Size * Mesh_Size) * Right_Hand_Side (I,J) + \
                        Solution (I+1,J) + Solution (I-1,J) + Solution (I,J+1) + Solution (I,J-1) + \
                        Solution (I+1,J+1) + Solution (I+1,J-1) + Solution (I-1,J+1) + Solution (I-1,J-1)) / 8.0;

// Use the following within the functions below (each function tests the statement with different
// transformation assertion options)
// #define TEST_STATEMENTS TEST_3D_STENCIL_STATEMENTS
// #define TEST_STATEMENTS TEST_2D_STENCIL_STATEMENTS
#define TEST_STATEMENTS TEST_2D_STATEMENTS


// Mechanism to specify options to the preprocessor (global scope)
// Currently we don't handle where statements or indirect addressing (so assert that there are none)
TransformationAssertion globalScope0 (TransformationAssertion::NoWhereStatementTransformation,
                                      TransformationAssertion::NoIndirectAddressing);


int MAX_ITERATIONS = 0;

long
clock()
   {
     return 0L;
   }


int
functionNoTransformation ()
   {
  // TransformationAssertion assertion1 (TransformationAssertion::SkipTransformation);

     doubleArray A(10);

     long startClock = clock();
     for (int i = 0; i < MAX_ITERATIONS; i++)
        {
          TEST_STATEMENTS
        }
     long endClock = clock();
     long time = (endClock - startClock);

     return time;
   }

int
functionA ()
   {

     TEST_STATEMENTS

     return 0;
   }

int
functionB ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
     TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

     TEST_STATEMENTS


#if 0
     InternalIndex I,J,K,L;
     doubleArray A(10);
     doubleArray B(10);
     int m,n;

     A(I) = 0;
     A(I) = B(I-1);
     A(I+(m*2)) = B(I-(n+4)) + B(I+n);
     A(I) = A(I-1);
     A(I) = A(I-1) + A(I+1);
     A(I) = A(I-1) + A(I+1) + n;
#endif

     return 0;
   }


int
functionC ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
     TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

  // Specification of uniform stride permits all the <name>_stride# variables to be the same across
  // all operands (further reducing the register pressure).
     TransformationAssertion assertion2 (TransformationAssertion::ConstantStrideAccess);

     TEST_STATEMENTS

     return 0;
   }


int
functionD ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
     TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

  // Specification of uniform stride permits all the <name>_stride# variables to be the same across
  // all operands (further reducing the register pressure).
     TransformationAssertion assertion2 (TransformationAssertion::ConstantStrideAccess);

  // In addition to uniform stride we specify here that the stride is lenght 1 which permits
  // additional optimizations to the subscript computation (or the loop nest) to remove the stride
  // variable from the computations altogether.
     TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);

     TEST_STATEMENTS

     return 0;
   }


int
functionE ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
  // TransformationAssertion assertion1 (TransformationAssertion::SameSizeArrays);

  // Specification of uniform stride permits all the <name>_stride# variables to be the same across
  // all operands (further reducing the register pressure).
  // TransformationAssertion assertion2 (TransformationAssertion::ConstantStrideAccess);

  // In addition to uniform stride we specify here that the stride is lenght 1 which permits
  // additional optimizations to the subscript computation (or the loop nest) to remove the stride
  // variable from the computations altogether.
     TransformationAssertion assertion3 (TransformationAssertion::StrideOneAccess);

     TEST_STATEMENTS

     return 0;
   }


int
main ()
   {
     int timeWithoutTransformation = functionNoTransformation ();
     printf ("timeWithoutTransformation = %d \n",timeWithoutTransformation);

     return 0;
   }














