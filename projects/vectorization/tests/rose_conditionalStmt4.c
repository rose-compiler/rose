/*
  Test vectorization for simple arithemetic statements.
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_10;
  int i_nom_1;
  float a[16UL];
  __SIMD *a_SIMD = (__SIMD *)a;
  float b[16UL];
  __SIMD *b_SIMD = (__SIMD *)b;
  float tmp;
  __SIMD tmp_SIMD;
  int n = 16;
  __SIMDi n_SIMD;
  
#pragma SIMD
  void *cmpReturn_12;
  tmp_SIMD = _SIMD_splats_ps(tmp);
  n_SIMD = _SIMD_splats_epi32(n);
  for (i_nom_1 = 0, i_nom_1_strip_10 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_10 += 1)) {
/* if statement is converted into vectorizaed conditional statement */
    _SIMD_cmpeq_ps(tmp_SIMD,_SIMD_splats_ps(0.f),&cmpReturn_12);
    a_SIMD[i_nom_1_strip_10] = _SIMD_sel_ps(_SIMD_splats_ps(1.f),_SIMD_splats_ps(0.f),&cmpReturn_12);
    b_SIMD[i_nom_1_strip_10] = _SIMD_sel_ps(_SIMD_splats_ps(1.f),_SIMD_splats_ps(0.f),&cmpReturn_12);
  }
  return 0;
}
