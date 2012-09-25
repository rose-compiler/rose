/*
  Test vectorization for simple arithemetic statements.
*/
#include <math.h>
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_9;
  int i_nom_1;
  float a[16UL];
  __SIMD *a_SIMD;
  float b[16UL];
  __SIMD *b_SIMD;
  int n = 16;
  __SIMDi n_SIMD;
  b_SIMD = ((__SIMD *)b);
  a_SIMD = ((__SIMD *)a);
  n_SIMD = _SIMD_splats_epi32(n);
  for (i_nom_1 = 0, i_nom_1_strip_9 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_9 += 1)) {
    b_SIMD[i_nom_1_strip_9] = (_SIMD_sin_ps(a_SIMD[i_nom_1_strip_9]));
    b_SIMD[i_nom_1_strip_9] = (_SIMD_cos_ps(a_SIMD[i_nom_1_strip_9]));
    b_SIMD[i_nom_1_strip_9] = (_SIMD_log_ps(a_SIMD[i_nom_1_strip_9]));
    b_SIMD[i_nom_1_strip_9] = (_SIMD_exp_ps(a_SIMD[i_nom_1_strip_9]));
    b_SIMD[i_nom_1_strip_9] = (_SIMD_sqrt_ps(a_SIMD[i_nom_1_strip_9]));
    b_SIMD[i_nom_1_strip_9] = (_SIMD_max_ps(a_SIMD[i_nom_1_strip_9],b_SIMD[i_nom_1_strip_9]));
    b_SIMD[i_nom_1_strip_9] = (_SIMD_min_ps(a_SIMD[i_nom_1_strip_9],b_SIMD[i_nom_1_strip_9]));
  }
  return 0;
}
