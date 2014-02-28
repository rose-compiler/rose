#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * trisolv.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16;
/* Variable declaration/allocation. */
  double A[16][16];
  double x[16];
  double c[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c2;
    int c3;
    for (c1 = 0; c1 <= 1; c1++) {
      for (c3 = 13 * c1; c3 <= ((15 < 13 * c1 + 12?15 : 13 * c1 + 12)); c3++) {
        x[c3] = c[c3];
      }
    }
    for (c1 = 0; c1 <= 16; c1++) {
      if (c1 <= 12) {
#pragma omp parallel for private(c3)
        for (c2 = c1 + -1; c2 <= (((11 * c1 + -12) * 10 < 0?((10 < 0?-((-(11 * c1 + -12) + 10 + 1) / 10) : -((-(11 * c1 + -12) + 10 - 1) / 10))) : (11 * c1 + -12) / 10)); c2++) {
          for (c3 = (c2 > 13 * c1 + -13 * c2?c2 : 13 * c1 + -13 * c2); c3 <= ((15 < 13 * c1 + -13 * c2 + 12?15 : 13 * c1 + -13 * c2 + 12)); c3++) {
            x[c3] = x[c3] - A[c3][c2 + -1] * x[c2 + -1];
          }
        }
      }
      if (c1 == 0) {
        x[0] = x[0] / A[0][0];
      }
      c2 = (1 > (((13 * c1 + -1) * 14 < 0?-(-(13 * c1 + -1) / 14) : ((14 < 0?(-(13 * c1 + -1) + - 14 - 1) / - 14 : (13 * c1 + -1 + 14 - 1) / 14))))?1 : (((13 * c1 + -1) * 14 < 0?-(-(13 * c1 + -1) / 14) : ((14 < 0?(-(13 * c1 + -1) + - 14 - 1) / - 14 : (13 * c1 + -1 + 14 - 1) / 14)))));
      if (c2 <= (((13 * c1 + 11) * 14 < 0?((14 < 0?-((-(13 * c1 + 11) + 14 + 1) / 14) : -((-(13 * c1 + 11) + 14 - 1) / 14))) : (13 * c1 + 11) / 14))) {
        if (c1 <= ((14 * c2 * 13 < 0?((13 < 0?-((-(14 * c2) + 13 + 1) / 13) : -((-(14 * c2) + 13 - 1) / 13))) : 14 * c2 / 13))) {
          x[c2] = x[c2] - A[c2][c2 + -1] * x[c2 + -1];
        }
        if (c1 == 16 && c2 == 15) {
          x[15] = x[15] / A[15][15];
        }
        if (c2 <= 14) {
          x[c2] = x[c2] / A[c2][c2];
          x[c2 + 1] = x[c2 + 1] - A[c2 + 1][c2 + -1] * x[c2 + -1];
        }
        for (c3 = c2 + 2; c3 <= ((15 < 13 * c1 + -13 * c2 + 12?15 : 13 * c1 + -13 * c2 + 12)); c3++) {
          x[c3] = x[c3] - A[c3][c2 + -1] * x[c2 + -1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
