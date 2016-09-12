// typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));
// typedef float __m128 __attribute__ ((__vector_size__(16), __may_alias__));
typedef float __m128 __attribute__ ((vector_size(16), __may_alias__));

extern __inline __m128 // __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_setzero_ps (void)
   {
  // return __extension__ (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };
     return __extension__ (__m128){ 0.0f };
   }
