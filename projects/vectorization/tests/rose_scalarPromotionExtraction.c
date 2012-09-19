#include "rose_simd.h" 

int main()
{
  int i_nom_2_strip_9;
  int i_nom_2;
  int i_nom_1_strip_5;
  int i_nom_1;
  float d = 5.f;
  __SIMD d_SIMD;
  float dim[16UL];
  __SIMD *dim_SIMD;
  float dim2[16UL];
  __SIMD *dim2_SIMD;
  dim2_SIMD = ((__SIMD *)dim2);
  dim_SIMD = ((__SIMD *)dim);
  d_SIMD = _SIMD_splats_ps(d);
  for (i_nom_1 = 0, i_nom_1_strip_5 = i_nom_1; i_nom_1 <= 15; (i_nom_1 += 4 , i_nom_1_strip_5 += 1)) {
    dim_SIMD[i_nom_1_strip_5] = d_SIMD;
  }
  d = _SIMD_extract_ps(d_SIMD,3);
  for (i_nom_2 = 0, i_nom_2_strip_9 = i_nom_2; i_nom_2 <= 15; (i_nom_2 += 4 , i_nom_2_strip_9 += 1)) {
    d_SIMD = dim_SIMD[i_nom_2_strip_9];
    dim2_SIMD[i_nom_2_strip_9] = d_SIMD;
  }
  d = _SIMD_extract_ps(d_SIMD,3);
  d = (d + 1);
  return 0;
}
