#include <stdio.h>

int main()
{
  int i, n;
#pragma scop
  for (i = 0; i < n; ++i)
    ;
  //  ntaps_filter_ffd (&lp_21_conf, 1, &fm_qd_buffer[i], &band_21[i]);
#pragma endscop
}
