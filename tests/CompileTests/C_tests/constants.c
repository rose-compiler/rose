#ifdef __GNUC__
#define _GNU_SOURCE
#endif

#if !defined(__CYGWIN__)
#include <complex.h>
#endif

#include <math.h>

// Test use of C99 and common C constants
void foo()
   {
#if !defined(__CYGWIN__) && !defined(__INTEL_COMPILER)
     float _Complex an_i = _Complex_I;

     _Complex_I;

     _Complex float a_complex_value = 1.0;

     a_complex_value = 1.0 * an_i;
     a_complex_value = 0.0 + 1.0 * _Complex_I;

  // EDG does not recognize the "i" suffix.
  // const double _Complex a_complex_value = 3.0f + 4.0fi;

  // Note that FP_NAN and FP_INFINITE are C99 specific

#ifdef __GNUC__
// TOO (2/14/11): thrifty compiler (gcc 3.4.4) does not
// support NAN macro
#if GNU_CC >= 4 
     float  a_nan_float  = NAN;
     double a_nan_double = NAN;
#endif
#else
     float  a_nan_float  = __NAN__;
     double a_nan_double = __NAN__;
#endif

#ifndef __INTEL_COMPILER
#ifdef __GNUC__
     float  an_infinity_float  = FP_INFINITE;
     double an_infinity_double = FP_INFINITE;
#else
     float  an_infinity_float  = __INFINITY__;
     double an_infinity_double = __INFINITY__;
#endif
#endif

// if !defined(__CYGWIN__)
#endif
   }
