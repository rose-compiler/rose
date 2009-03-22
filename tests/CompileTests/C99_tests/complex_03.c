// DQ (3/21/2009): The header file "complex.h" is not available in Cygwin.
#if !defined(__CYGWIN__)

#include <complex.h>

int main (void)
   {
  // Older GNU systax for declaration of complex variables (specification of complex types)
     __complex__ float       x_old_syntax = 1.0;
     __complex__ double      y_old_syntax = 2.0;
     __complex__ long double z_old_syntax = 3.0;

  // __complex__ float an_i_old_syntax = __I__;
     _Complex float an_i_new_syntax = __I__;

  // constant value
     __I__;

     _Complex float another_i_new_syntax = 0.0 + __I__;

  // ROSE does not yet support the imaginary add operator
     _Complex float a_complex_value = 0.0;

#ifndef __INTEL_COMPILER
  // This is now to specify imaginary numbers in EDG, not supported in GNU, so
  // the code generation using GNU aas a backend drops the "_Imaginary" prefix.
     _Imaginary float       x_edg_imaginary = 0.0;
     _Imaginary double      y_edg_imaginary = 0.0;
     _Imaginary long double z_edg_imaginary = 0.0;

     a_complex_value = 4.0 + x_edg_imaginary;
#endif
   }

#endif
