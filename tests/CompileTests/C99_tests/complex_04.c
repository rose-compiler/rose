
/* 
   This test code fails for a mysterious reason, if any line is removed 
   a file_info object is not generated correctly and the translation fails :-).
   This may be an EDG problem (this codes was simplified from complex_01.c).

   This is a C and C99 test code of the use of complex types.
   It works because the rose_edg_required_macros_and_functions.h
   file redefines __complex__ using:
   #define __complex__ _Complex
 */

/* Relavant C99 types:
   "_Bool", "_Complex", "_Imaginary", "__I__", "__NAN__", "__INFINITY__",
*/

// DQ (3/21/2009): The header file "complex.h" is not available in Cygwin.
#if !defined(__CYGWIN__)

#include <complex.h>

int main (void)
   {
#if 1
  // Older GNU systax for declaration of complex variables (specification of complex types)
     __complex__ long double z_old_syntax = 3.0;

  // __complex__ float an_i_old_syntax = __I__;
     _Complex float an_i_new_syntax = __I__;

  // __I__;

  // ROSE does not yet support the imaginary add operator
     _Complex float a_complex_value = 0.0;

  // Newer syntax for specification of complex types
     _Complex float       x = 1.0;
     _Complex double      y = 2.0;
     _Complex long double z = 3.0;

  // Specification of complex literals is a bit more complicated 
  // (not clear if this is might just be the use of the commar operator).
  // note that the parenthesis are required.
     _Complex float x_with_real_and_imaginary_parts = (1.0,-1.0);
#endif

#if 1
     _Complex float an_i_new_syntax2 = __I__;

     __I__;

  // This is now to specify imaginary numbers in EDG, not supported in GNU, so
  // the code generation using GNU aas a backend drops the "_Imaginary" prefix.
     _Imaginary float       x_edg_imaginary = 0.0;
     _Imaginary double      y_edg_imaginary = 0.0;
     _Imaginary long double z_edg_imaginary = 0.0;

     _Complex float       x1 = 1.0;
     _Complex double      y1 = 2.0;

  // This fails, but adding the body to an explicitly defined block works just fine?????
     if ( x1 != y1 ) // complex inequality operator
          x1 = x1;
#endif

     return 0;
   }

#endif



