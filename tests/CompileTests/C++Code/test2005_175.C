// Note that this function will not unparse the "#warning" macro.

#include <math.h>
// #include <limits>

#ifndef INFINITY
// If this is not defined then define INFINITY as a constant.
const double INFINITY = 1.0e+30;
#else
   #warning "INFINITY is defined"
#endif

int main()
   {
     double infiniteNegativeValue = -INFINITY;

  // Note that we can't generate code that uses the C++ limits data structures because 
  // the user's application might not have included the "limits" header file.
  // bool isInfinity = (INFINITY < std::numeric_limits<float>::infinity());

     return 0;
   }

