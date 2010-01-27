// Note that this function will not unparse the "#warning" macro.

#include <math.h>
// #include <limits>

#ifndef INFINITY
// If this is not defined then define INFINITY as a constant.
const double INFINITY = 1.0e+30;
#else
   #warning "INFINITY is defined"

// DQ (1/9/2010): The Intel compiler defines INFINITY as __infinity, but does not define __infinity (so define it explicitly for this test).
// #if CXX_IS_INTEL_COMPILER
// #define __infinity 1.0e+30
#ifdef __INTEL_COMPILER
#undef INFINITY
#define INFINITY 1.0e+30
#warning "Using Intel specific INFINITY"
#endif
#endif

int main()
   {
     double infiniteNegativeValue = -INFINITY;

  // Note that we can't generate code that uses the C++ limits data structures because 
  // the user's application might not have included the "limits" header file.
  // bool isInfinity = (INFINITY < std::numeric_limits<float>::infinity());

     return 0;
   }

