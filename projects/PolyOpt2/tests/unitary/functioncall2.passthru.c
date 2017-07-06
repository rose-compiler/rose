#include <stdio.h>

int main()
{
  int i;
  int n;
  
#pragma scop
{
    int c1;
    if (n >= 1) {
      for (c1 = 0; c1 <= n + -1; c1++) {{
          ;
        }
      }
    }
  }
//  ntaps_filter_ffd (&lp_21_conf, 1, &fm_qd_buffer[i], &band_21[i]);
  
#pragma endscop
}
