/*
  Test vectorization for simple arithemetic statements.
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_10;
  int i_nom_1;
  float a[16UL];
  __SIMD *a_SIMD;
  float b[16UL];
  __SIMD *b_SIMD;
  float c[16UL];
  __SIMD *c_SIMD;
  int n = 16;
  __SIMDi n_SIMD;
  
#pragma SIMD
  c_SIMD = ((__SIMD *)c);
  b_SIMD = ((__SIMD *)b);
  a_SIMD = ((__SIMD *)a);
  n_SIMD = _SIMD_splats_epi32(n);
  for (i_nom_1 = 0, i_nom_1_strip_10 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_10 += 1)) {
    c_SIMD[i_nom_1_strip_10] = _SIMD_add_ps(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
    c_SIMD[i_nom_1_strip_10] = _SIMD_sub_ps(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
    c_SIMD[i_nom_1_strip_10] = _SIMD_mul_ps(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
    c_SIMD[i_nom_1_strip_10] = _SIMD_div_ps(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
  }
  return 0;
}
