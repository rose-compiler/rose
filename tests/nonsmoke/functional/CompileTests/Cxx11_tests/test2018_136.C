// ROSE-1206 / RZ ROSE-31
// This version works for g++, but fails for ROSE.

#include <xmmintrin.h>
void
func1 () 
   {
     _mm_set_ps(0.0, 0.0, 0.0, 0.0);
     _mm_set_ps1(0.0);

  // Add functions that appear to be missing so that we can fix ROSE.
     __m128i X_m128i;

     _mm_add_epi32 (X_m128i, X_m128i);

     __m128d X_m128d;
     _mm_add_pd (X_m128d, X_m128d);

     __m128 X_m128;
     _mm_add_ps (X_m128, X_m128);

     _mm_and_pd (X_m128d, X_m128d);

     _mm_and_ps (X_m128, X_m128);

     _mm_and_si128 (X_m128i, X_m128i);

     _mm_andnot_pd (X_m128d, X_m128d);

     _mm_andnot_ps (X_m128, X_m128);

     _mm_andnot_si128 (X_m128i, X_m128i);

     _mm_castpd_ps(X_m128d);

     _mm_castpd_si128(X_m128d);

     _mm_castps_si128(X_m128);

     _mm_castsi128_pd(X_m128i);

     _mm_castsi128_ps(X_m128i);

     _mm_cmpgt_epi32 (X_m128i, X_m128i);

     _mm_cmplt_epi32 (X_m128i, X_m128i);

     _mm_cvtsd_f64 (X_m128d);

     _mm_cvtsi128_si32 (X_m128i);

     _mm_cvtss_f32 (X_m128);

     _mm_div_pd (X_m128d, X_m128d);

     _mm_div_ps (X_m128, X_m128);

     double const *P = NULL;
     _mm_load_pd (P);

     float const *F = NULL;
     _mm_load_ps (F);

     _mm_load_sd (P);

     __m128i const *P_m128i;
     _mm_load_si128 (P_m128i);

     _mm_load_ss (F);

     __m128i const *P_const_m128i = NULL;
     _mm_loadl_epi64 (P_const_m128i);

     _mm_loadu_pd (P);

     _mm_loadu_ps (F);

     _mm_loadu_si128 (P_m128i);

     _mm_max_pd (X_m128d, X_m128d);

     _mm_max_ps (X_m128, X_m128);

     _mm_min_pd (X_m128d, X_m128d);

     _mm_min_ps (X_m128, X_m128);

     _mm_mul_epu32 (X_m128i, X_m128i);

     _mm_mul_pd (X_m128d, X_m128d);

     _mm_mul_ps (X_m128, X_m128);

     _mm_or_pd (X_m128d, X_m128d);

     _mm_or_ps (X_m128, X_m128);

     _mm_or_si128 (X_m128i, X_m128i);

  // #define _mm_prefetch(P, I)
     void *P_void = NULL;
     const _mm_hint enum_hint = _MM_HINT_NTA;
     _mm_prefetch(P_void, enum_hint);

     _mm_set1_epi32 (42);

     double F_value;
     _mm_set1_pd (F_value);

     int __q3, __q2, __q1, __q0;
     _mm_set_epi32 (__q3, __q2, __q1, __q0);

     _mm_set_pd1 (F_value);

     const float __Z = 0.0, __Y = 0.0, __X = 0.0, __W = 0.0;
     _mm_set_ps (__Z, __Y, __X, __W);

     _mm_set_ps1 (F_value);

     _mm_set_sd (F_value);

     float F_float_value;
     _mm_set_ss (F_float_value);

     _mm_setr_epi32 (__q0, __q1, __q2, __q3);

  // #define _mm_shuffle_epi32(__A, __B) ((__m128i)__builtin_ia32_pshufd ((__v4si)__A, __B))
     _mm_shuffle_epi32(X_m128i, 4);

  // #define _mm_shuffle_pd(__A, __B, __C) ((__m128d)__builtin_ia32_shufpd ((__v2df)__A, (__v2df)__B, (__C)))
     __v2df X_v2df;
     _mm_shuffle_pd(X_v2df, X_v2df, 4);

  // #define _mm_shuffle_ps(A, B, MASK) ((__m128) __builtin_ia32_shufps ((__v4sf)(A), (__v4sf)(B), (MASK)))
     __v4sf X_v4sf;
     _mm_shuffle_ps(X_v4sf,X_v4sf,4);

     double *F_ptr;
     _mm_store_pd (F_ptr, X_m128d);

     float *F_float_ptr;
     _mm_store_ps (F_float_ptr, X_m128);

     __m128i *X_ptr_m128i;
     _mm_store_si128 (X_ptr_m128i, X_m128i);

     _mm_storeu_pd (F_ptr, X_m128d);

     _mm_storeu_ps (F_float_ptr, X_m128);

     _mm_storeu_si128 (X_ptr_m128i, X_m128i);

     _mm_sub_epi32 (X_m128i, X_m128i);

     _mm_sub_pd (X_m128d, X_m128d);

     _mm_sub_ps (X_m128, X_m128);

     _mm_xor_pd (X_m128d, X_m128d);

     _mm_xor_ps (X_m128, X_m128);

     _mm_xor_si128 (X_m128i, X_m128i);

   }

