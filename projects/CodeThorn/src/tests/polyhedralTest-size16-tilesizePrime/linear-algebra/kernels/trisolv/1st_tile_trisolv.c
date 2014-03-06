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
    for (c1 = 0; c1 <= 2; c1++) {
      for (c3 = 7 * c1; c3 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c3++) {
        x[c3] = c[c3];
      }
    }
    for (c1 = 0; c1 <= 17; c1++) {
#pragma omp parallel for private(c3)
      for (c2 = (1 > c1 + -2?1 : c1 + -2); c2 <= (((7 * c1 + -2) * 8 < 0?((8 < 0?-((-(7 * c1 + -2) + 8 + 1) / 8) : -((-(7 * c1 + -2) + 8 - 1) / 8))) : (7 * c1 + -2) / 8)); c2++) {
        for (c3 = 7 * c1 + -7 * c2; c3 <= ((15 < 7 * c1 + -7 * c2 + 6?15 : 7 * c1 + -7 * c2 + 6)); c3++) {
          x[c3] = x[c3] - A[c3][c2 + -1] * x[c2 + -1];
        }
      }
      if (c1 == 0) {
        x[0] = x[0] / A[0][0];
      }
      if ((7 * c1 + 6) % 8 == 0) {
        x[(7 * c1 + 6) / 8] = x[(7 * c1 + 6) / 8] - A[(7 * c1 + 6) / 8][(7 * c1 + -2) / 8] * x[(7 * c1 + -2) / 8];
      }
      c2 = (1 > (((7 * c1 + -1) * 8 < 0?-(-(7 * c1 + -1) / 8) : ((8 < 0?(-(7 * c1 + -1) + - 8 - 1) / - 8 : (7 * c1 + -1 + 8 - 1) / 8))))?1 : (((7 * c1 + -1) * 8 < 0?-(-(7 * c1 + -1) / 8) : ((8 < 0?(-(7 * c1 + -1) + - 8 - 1) / - 8 : (7 * c1 + -1 + 8 - 1) / 8)))));
      if (c2 <= (((7 * c1 + 5) * 8 < 0?((8 < 0?-((-(7 * c1 + 5) + 8 + 1) / 8) : -((-(7 * c1 + 5) + 8 - 1) / 8))) : (7 * c1 + 5) / 8))) {
        if (c1 <= ((8 * c2 * 7 < 0?((7 < 0?-((-(8 * c2) + 7 + 1) / 7) : -((-(8 * c2) + 7 - 1) / 7))) : 8 * c2 / 7))) {
          x[c2] = x[c2] - A[c2][c2 + -1] * x[c2 + -1];
        }
        if (c1 == 17 && c2 == 15) {
          x[15] = x[15] / A[15][15];
        }
        if (c2 <= 14) {
          x[c2] = x[c2] / A[c2][c2];
          x[c2 + 1] = x[c2 + 1] - A[c2 + 1][c2 + -1] * x[c2 + -1];
        }
        for (c3 = c2 + 2; c3 <= ((15 < 7 * c1 + -7 * c2 + 6?15 : 7 * c1 + -7 * c2 + 6)); c3++) {
          x[c3] = x[c3] - A[c3][c2 + -1] * x[c2 + -1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
