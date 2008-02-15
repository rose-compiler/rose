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

// Mechanism to specify options to the preprocessor (global scope)
TransformationAssertion globalScope0 (TransformationAssertion::NoWhereStatementTransformation,
                                      TransformationAssertion::NoIndirectAddressing);

int
main ()
   {
  // First optimization permits all the size# variables to be the same across all operands (this
  // should significantly reduce the registar pressure).
     TransformationAssertion functionScope1 (TransformationAssertion::SameSizeArrays);

  // Specification of uniform stride permits all the <name>_stride# variables to be the same across
  // all operands (further reducing the register pressure).
     TransformationAssertion functionScope2 (TransformationAssertion::ConstantStrideAccess);

  // In addition to uniform stride we specify here that the stride is lenght 1 which permits
  // additional optimizations to the subscript computation (or the loop nest) to remove the stride
  // variable from the computations altogether.
     TransformationAssertion functionScope3 (TransformationAssertion::StrideOneAccess);

     InternalIndex I,J,K,L;
     doubleArray A(10);
     doubleArray B(10);
     doubleArray C(10);
     doubleArray D(10);
     doubleArray E(10);
     int i,j,k;
     int n = 1,m = 2;


  // Call the friend function operator+(doubleArray,doubleArray) 
  // and then the member function doubleArray::operator=()
  // A = B + C;

#if 0
     for (i=0; i < n; i++)
          A(i) = 1;
#else
     A = 0;
#endif


#if 0
     A = B + (C + D);

     A = A;
     A = A;

     A(I) = A(I);
     A = A;
     A = A(I);
     A(I) = A;
     A(I) = A(I);

     A(I) = 2;
     B(I) = 3;
     C(I) = 4;

     A(I) = B(I) + C(I);

     A = A;
     A = B;
     A(I,J) = C;
     A(I+n) = 0;
  // A(I,2) = 0;
     A(I,J) = 0;
     A = B + C;

     A(I+1,J-2) = B(I,J) + C(J-n,I+m);

     A(I) = B(I-1) + B(I+1);

     A(I) = A(I-1) + A(I+1);

     A = B + C;
     A(I) = B(I) + C(I);

  // tempLocal(ILocInterior,JLocInterior) = oldALocal(ILocInterior,JLocInterior) - 
  //   2.*dt*( ( ALocal(ILocInterior+1,JLocInterior) - ALocal(ILocInterior-1,JLocInterior) ) / (2.0*dx) + 
  //           ( ALocal(ILocInterior,JLocInterior+1) - ALocal(ILocInterior,JLocInterior-1) ) / (2.0*dy) -
  //           (4.0 + 2.0*theTime + xlocal(ILocInterior,JLocInterior) + ylocal(ILocInterior,JLocInterior))  );

     double theTime = 0.1;
     double dt      = 0.1;
     double dx      = 0.1;
     double dy      = 0.1;

  // change of variables on original statement
     A(I,J) = B(I,J) - 2.0 * dt * ( ( C(I+1,J) - C(I-1,J) ) / (2.0*dx) + 
                                    ( C(I,J+1) - C(I,J-1) ) / (2.0*dy) -
                                    (4.0 + 2.0*theTime + D(I,J) + E(I,J)) );
#endif

     return 0;
   }

