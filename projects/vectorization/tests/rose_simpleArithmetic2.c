#include "rose_simd.h" 

int s272(float t)
{
  int i_nom_1_strip_5;
  int i_nom_1;
  float a[16UL];
  __SIMD *a_SIMD;
  
#pragma SIMD
  __SIMD t_SIMD;
  a_SIMD = ((__SIMD *)a);
  t_SIMD = _SIMD_splats_ps(t);
  for (i_nom_1 = 0, i_nom_1_strip_5 = i_nom_1; i_nom_1 <= 15; (i_nom_1 += 4 , i_nom_1_strip_5 += 1)) {
    a_SIMD[i_nom_1_strip_5] = t_SIMD;
  }
  return 0;
}
