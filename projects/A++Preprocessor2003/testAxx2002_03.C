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

// Use the following within the functions below (each function tests the statement with different
// transformation assertion options)
#define TEST_STATEMENTS TEST_3D_STENCIL_STATEMENTS

// Include source code into which the TEST_STATEMENTS macro is expanded into several functions which
// test the transformation mechaism using different options.

#include "testCodeTemplate.inc"













