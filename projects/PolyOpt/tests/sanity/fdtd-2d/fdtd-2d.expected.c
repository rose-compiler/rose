#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#ifdef PERFCTR
#include <papi.h>
#include "papi_defs.h"
#endif
#include "decls.h"
#include "util.h"
double t_start;
double t_end;

int main()
{
  int t;
  int i;
  int j;
  int k;
  int l;
  int m;
  int n;
  init_array();
#ifdef PERFCTR
#endif
  
#pragma scop
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (ny >= 1 && tmax >= 1) {
      if (nx >= 2 && ny >= 2) {
        for (c1 = 0; c1 <= tmax + -1; ++c1) {
          for (c3 = 0; c3 <= ny + -1; ++c3) {
            (ey[0])[c3] = (c1);
          }
          for (c3 = 1; c3 <= nx + -1; ++c3) {
            for (c5 = 0; c5 <= ny + -1; ++c5) {
              (ey[c3])[c5] = (((ey[c3])[c5]) - (0.5 * (((hz[c3])[c5]) - ((hz[c3 - 1])[c5]))));
            }
          }
          for (c3 = 0; c3 <= nx + -1; ++c3) {
            for (c5 = 1; c5 <= ny + -1; ++c5) {
              (ex[c3])[c5] = (((ex[c3])[c5]) - (0.5 * (((hz[c3])[c5]) - ((hz[c3])[c5 - 1]))));
            }
          }
          for (c3 = 0; c3 <= nx + -1; ++c3) {
            for (c5 = 0; c5 <= ny + -1; ++c5) {
              (hz[c3])[c5] = (((hz[c3])[c5]) - (0.7 * (((((ex[c3])[c5 + 1]) - ((ex[c3])[c5])) + ((ey[c3 + 1])[c5])) - ((ey[c3])[c5]))));
            }
          }
        }
      }
      if (nx >= 2 && ny == 1) {
        for (c1 = 0; c1 <= tmax + -1; ++c1) {
          (ey[0])[0] = (c1);
          for (c3 = 1; c3 <= nx + -1; ++c3) {
            (ey[c3])[0] = (((ey[c3])[0]) - (0.5 * (((hz[c3])[0]) - ((hz[c3 - 1])[0]))));
          }
          for (c3 = 0; c3 <= nx + -1; ++c3) {
            (hz[c3])[0] = (((hz[c3])[0]) - (0.7 * (((((ex[c3])[0 + 1]) - ((ex[c3])[0])) + ((ey[c3 + 1])[0])) - ((ey[c3])[0]))));
          }
        }
      }
      if (nx == 1 && ny >= 2) {
        for (c1 = 0; c1 <= tmax + -1; ++c1) {
          for (c3 = 0; c3 <= ny + -1; ++c3) {
            (ey[0])[c3] = (c1);
          }
          for (c5 = 1; c5 <= ny + -1; ++c5) {
            (ex[0])[c5] = (((ex[0])[c5]) - (0.5 * (((hz[0])[c5]) - ((hz[0])[c5 - 1]))));
          }
          for (c5 = 0; c5 <= ny + -1; ++c5) {
            (hz[0])[c5] = (((hz[0])[c5]) - (0.7 * (((((ex[0])[c5 + 1]) - ((ex[0])[c5])) + ((ey[0 + 1])[c5])) - ((ey[0])[c5]))));
          }
        }
      }
      if (nx <= 0 && ny >= 2) {
        for (c1 = 0; c1 <= tmax + -1; ++c1) {
          for (c3 = 0; c3 <= ny + -1; ++c3) {
            (ey[0])[c3] = (c1);
          }
        }
      }
      if (nx == 1 && ny == 1) {
        for (c1 = 0; c1 <= tmax + -1; ++c1) {
          (ey[0])[0] = (c1);
          (hz[0])[0] = (((hz[0])[0]) - (0.7 * (((((ex[0])[0 + 1]) - ((ex[0])[0])) + ((ey[0 + 1])[0])) - ((ey[0])[0]))));
        }
      }
      if (nx <= 0 && ny == 1) {
        for (c1 = 0; c1 <= tmax + -1; ++c1) {
          (ey[0])[0] = (c1);
        }
      }
    }
  }
  
#pragma endscop
#ifdef PERFCTR
#endif
#ifdef TEST
#endif
  return 0;
}

