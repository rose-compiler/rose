// DQ (3/21/2009): The header file "complex.h" is not available in Cygwin.
#if !defined(__CYGWIN__)

#include <complex.h>

// Test use of C99 and common C constants
void foo()
   {
     __complex__ float an_i = __I__;

     __I__;

     _Complex float a_complex_value = 1.0;

     a_complex_value = 1.0 * an_i;
     a_complex_value = 0.0 + 1.0 * __I__;

  // EDG does not recognize the "i" suffix.
  // const double _Complex a_complex_value = 3.0f + 4.0fi;

     float  a_nan_float  = __NAN__;
     double a_nan_double = __NAN__;

#ifndef __INTEL_COMPILER
     float  an_infinity_float  = __INFINITY__;
     double an_infinity_double = __INFINITY__;
#endif
   }

#endif
