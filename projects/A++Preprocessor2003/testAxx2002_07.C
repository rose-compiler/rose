#include <simpleA++.h>

// First optimization permits all the size# variables to be the same across all operands (this
// should significantly reduce the registar pressure).
TransformationAssertion functionScope1 (TransformationAssertion::SameSizeArrays);

// Specification of uniform stride permits all the <name>_stride# variables to be the same across
// all operands (further reducing the register pressure).
TransformationAssertion functionScope2 (TransformationAssertion::ConstantStrideAccess);

// In addition to uniform stride we specify here that the stride is lenght 1 which permits
// additional optimizations to the subscript computation (or the loop nest) to remove the stride
// variable from the computations altogether.
// TransformationAssertion functionScope3 (TransformationAssertion::StrideOneAccess);

int
main ()
   {
     doubleArray A(10);
     doubleArray B(10);

  // A = 0;
#if 0
     A(0) = B(1) + 0;

#else

     for (int i=0; i < 10; i++)
          A(0) = B(1) + 0;
#endif

   }
