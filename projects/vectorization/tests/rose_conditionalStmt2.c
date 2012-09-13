/*
  Test vectorization for simple arithemetic statements.
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_8;
  int i_nom_1;
  float a[16UL];
  __SIMD *a_SIMD;
  float tmp;
  __SIMD tmp_SIMD;
  int n = 16;
  void *cmpReturn_10;
  a_SIMD = ((__SIMD *)a);
  for (i_nom_1 = 0, i_nom_1_strip_8 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_8 += 1)) {
/* if statement is converted into vectorizaed conditional statement */
    _SIMD_cmpeq_ps(tmp_SIMD,_SIMD_splats_ps(0.f),&cmpReturn_10);
    a_SIMD[i_nom_1_strip_8] = _SIMD_sel_ps(_SIMD_splats_ps(1.f),_SIMD_splats_ps(0.f),&cmpReturn_10);
  }
  return 0;
}
