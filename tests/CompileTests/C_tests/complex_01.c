/* This is a C and C99 test code of the use of complex types.
   It works because the rose_edg_required_macros_and_functions.h
   file redefines __complex__ using:
   #define __complex__ _Complex
 */

/* Relavant C99 types:
   "_Bool", "_Complex", "_Imaginary", "__I__", "__NAN__", "__INFINITY__",
*/

/* JJW: The new frontend handles all of this just fine, without any redefinition */

#ifdef ROSE_USE_NEW_EDG_INTERFACE

#include <complex.h>

int main (void)
   {
  // Older GNU systax for declaration of complex variables (specification of complex types)
     __complex__ float       x_old_syntax = 1.0;
     __complex__ double      y_old_syntax = 2.0;
     __complex__ long double z_old_syntax = 3.0;

  // __complex__ float an_i_old_syntax = __I__;
     _Complex float an_i_new_syntax = __I__;

     __I__;

     _Complex float a_complex_value = 0.0;

     a_complex_value = 4.0;

     a_complex_value = 3.0f + (4.0f * __I__);
     a_complex_value = 3.0f - 4.0f * __I__;
     a_complex_value = 3.0f * (4.0f * __I__);
     a_complex_value = 3.0f / (4.0f * __I__);

  // Newer syntax for specification of complex types
     _Complex float       x = 1.0;
     _Complex double      y = 2.0;
     _Complex long double z = 3.0;

#if 0
  // I think this is less a way to declare real and imaginary types than 
  // a way to extract the real and imaginary parts of a complex number!
  // So this is a bad example of how to use the imaginary type.

  // Note that currently within EDG, it appears the imaginary 
  // numbers are just regular floating point numbers.
     __imag__ float  x_imag = 0.0;
     __imag__ double y_imag = 0.0;
     __real__ float  x_real = 0.0;
     __real__ double y_real = 0.0;
#endif

  // Extract the real and imaginary parts of complex type values into float and double types.
     float  x_imag_extract_part = __imag__ x;
     float  x_real_extract_part = __real__ x;
     double y_imag_extract_part = __imag__ y;
     double y_real_extract_part = __real__ y;

     (__imag__ x)++;

  // This does not exist as a type in C99
  // _Real float  x_edg_real = 0.0;
  // _Real double y_edg_real = 0.0;

#if 1
     _Complex float       x_edg_imaginary = 0.0if;
     _Complex double      y_edg_imaginary = 0.0i;
     _Complex long double z_edg_imaginary = 0.0il;

     a_complex_value = 4.0 + x_edg_imaginary;
#endif

     float a = 1.0;
     a = -a; // this is the floating point negate operator

     int b = 1;
     b = -b; // this is the integer negate operator

  // This does not appear to work with EDG, but works with gcc!
     x = ~x; // this is the complex conjugation operator

     x = -y; // this is the complex negate operator
     x = +y; // this is the complex unary plus operator

     x = y;  // complex assignment operator

     x = x + y; // complex add operator
     x = x - y; // complex subtract operator
     x = x * y; // complex multiply operator
     x = x / y; // complex divide operator

     x += y; // complex add assignment operator
     x -= y; // complex subtract assignment operator
     x *= y; // complex multiply assignment operator
     x /= y; // complex divide assignment operator

     if ( x == y ) // complex equalify operator
          x = -x; 
     if ( x != y ) // complex inequality operator
          x = +x;

     return 0;
   }



#endif /* ROSE_USE_NEW_EDG_INTERFACE */
