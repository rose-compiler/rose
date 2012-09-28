/*
  Test vectorization for simple arithemetic statements.
*/
#include <math.h>
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_10;
  int i_nom_1;
  float a[16UL];
  __SIMD *a_SIMD = (__SIMD *)a;
  float b[16UL];
  __SIMD *b_SIMD = (__SIMD *)b;
  int n = 16;
  __SIMDi n_SIMD;
  
#pragma SIMD
  n_SIMD = _SIMD_splats_epi32(n);
  for (i_nom_1 = 0, i_nom_1_strip_10 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_10 += 1)) {
    b_SIMD[i_nom_1_strip_10] = _SIMD_sin_ps(a_SIMD[i_nom_1_strip_10]);
    b_SIMD[i_nom_1_strip_10] = _SIMD_cos_ps(a_SIMD[i_nom_1_strip_10]);
    b_SIMD[i_nom_1_strip_10] = _SIMD_log_ps(a_SIMD[i_nom_1_strip_10]);
    b_SIMD[i_nom_1_strip_10] = _SIMD_exp_ps(a_SIMD[i_nom_1_strip_10]);
    b_SIMD[i_nom_1_strip_10] = _SIMD_sqrt_ps(a_SIMD[i_nom_1_strip_10]);
    b_SIMD[i_nom_1_strip_10] = _SIMD_max_ps(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
    b_SIMD[i_nom_1_strip_10] = _SIMD_min_ps(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
  }
  return 0;
}
