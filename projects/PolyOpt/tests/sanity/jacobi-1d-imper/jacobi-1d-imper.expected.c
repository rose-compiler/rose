#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "decls.h"
#include "util.h"
double t_start;
double t_end;

int main()
{
  int t;
  int i;
  int j;
  init_array();
  
#pragma scop
{
    int ub1;
    int lb1;
    int c3;
    int c1;
    if (N >= 4 && T >= 1) {
      for (c1 = 0; c1 <= T + -1; ++c1) {
        for (c3 = 2; c3 <= N + -2; ++c3) {
          b[c3] = (0.33333 * (((a[c3 - 1]) + (a[c3])) + (a[c3 + 1])));
        }
        for (c3 = 2; c3 <= N + -2; ++c3) {
          a[c3] = (b[c3]);
        }
      }
    }
  }
  
#pragma endscop
#ifdef TEST
#endif
  return 0;
}

