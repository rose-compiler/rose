// ROSE-1206 / RZ ROSE-31
// This version works for g++, but fails for ROSE.

#include <xmmintrin.h>
void
func1 () {
  _mm_set_ps(0.0, 0.0, 0.0, 0.0);
  _mm_set_ps1(0.0);
}

/*
Additional functions from MMX that require support in ROSE:
_mm_add_epi32
_mm_add_pd
_mm_add_ps
_mm_and_pd
_mm_and_ps
_mm_and_si128
_mm_andnot_pd
_mm_andnot_ps
_mm_andnot_si128
_mm_castpd_ps
_mm_castpd_si128
_mm_castps_si128
_mm_castsi128_pd
_mm_castsi128_ps
_mm_cmpgt_epi32
_mm_cmplt_epi32
_mm_cvtsd_f64
_mm_cvtsi128_si32
_mm_cvtss_f32
_mm_div_pd
_mm_div_ps
_mm_load_pd
_mm_load_ps
_mm_load_sd
_mm_load_si128
_mm_load_ss
_mm_loadl_epi64
_mm_loadu_pd
_mm_loadu_ps
_mm_loadu_si128
_mm_max_pd
_mm_max_ps
_mm_min_pd
_mm_min_ps
_mm_mul_epu32
_mm_mul_pd
_mm_mul_ps
_mm_or_pd
_mm_or_ps
_mm_or_si128
_mm_prefetch
_mm_set1_epi32
_mm_set1_pd
_mm_set_epi32
_mm_set_pd
_mm_set_ps
_mm_set_ps1
_mm_set_sd
_mm_set_ss
_mm_setr_epi32
_mm_shuffle_epi32
_mm_shuffle_pd
_mm_shuffle_ps
_mm_store_pd
_mm_store_ps
_mm_store_si128
_mm_storeu_pd
_mm_storeu_ps
_mm_storeu_si128
_mm_sub_epi32
_mm_sub_pd
_mm_sub_ps
_mm_xor_pd
_mm_xor_ps
_mm_xor_si128
 */
