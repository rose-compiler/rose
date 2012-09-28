#include "rose_simd.h" 

int main()
{
  int i_nom_2_strip_11;
  int i_nom_2;
  int i_nom_1_strip_6;
  int i_nom_1;
  float d = 5.f;
  __SIMD d_SIMD;
  float dim[16UL];
  __SIMD *dim_SIMD = (__SIMD *)dim;
  float dim2[16UL];
  __SIMD *dim2_SIMD = (__SIMD *)dim2;
  
#pragma SIMD
  d_SIMD = _SIMD_splats_ps(d);
  for (i_nom_1 = 0, i_nom_1_strip_6 = i_nom_1; i_nom_1 <= 15; (i_nom_1 += 4 , i_nom_1_strip_6 += 1)) {
    dim_SIMD[i_nom_1_strip_6] = d_SIMD;
  }
  d = _SIMD_extract_ps(d_SIMD,3);
  
#pragma SIMD
  for (i_nom_2 = 0, i_nom_2_strip_11 = i_nom_2; i_nom_2 <= 15; (i_nom_2 += 4 , i_nom_2_strip_11 += 1)) {
    d_SIMD = dim_SIMD[i_nom_2_strip_11];
    dim2_SIMD[i_nom_2_strip_11] = d_SIMD;
  }
  d = _SIMD_extract_ps(d_SIMD,3);
  d = (d + 1);
  return 0;
}
