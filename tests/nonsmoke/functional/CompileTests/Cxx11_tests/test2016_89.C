// This likely can't be fixed, and makes it clear that we must provide an edited version of the avxintrin.h header file.

typedef double my__m256d __attribute__ ((__vector_size__ (32), __may_alias__));

extern __inline my__m256d __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm256_setzero_pd (void)
   {
  // EDG can't handle this code with 4 initializers.  Only one initializer is accepted.
  // return __extension__ (my__m256d){ 0.0, 0.0, 0.0, 0.0 };
     return __extension__ (my__m256d){ 0.0 };
   }
