/* The Intel API is flexible enough that we must allow aliasing with other
   vector types, and their scalar components.  */
typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));

static __inline __m128 __attribute__((__always_inline__)) _mm_setzero_ps (void)
{
// DQ (8/22/2011): Modified header for ROSE
// return __extension__ (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };
   return __extension__ (__m128){ 0.0f };
}
