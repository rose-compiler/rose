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
    for (c1 = 0; c1 <= 15; c1++) {
      x[c1] = c[c1];
    }
    for (c1 = 1; c1 <= 31; c1++) {
#pragma omp parallel for
      for (c2 = (1 > c1 + -15?1 : c1 + -15); c2 <= (((c1 + -2) * 2 < 0?((2 < 0?-((-(c1 + -2) + 2 + 1) / 2) : -((-(c1 + -2) + 2 - 1) / 2))) : (c1 + -2) / 2)); c2++) {
        x[c1 + -1 * c2] = x[c1 + -1 * c2] - A[c1 + -1 * c2][c2 + -1] * x[c2 + -1];
      }
      if (c1 == 31) {
        x[15] = x[15] / A[15][15];
      }
      if (c1 == 1) {
        x[0] = x[0] / A[0][0];
      }
      if (c1 % 2 == 0) {
        x[c1 / 2] = x[c1 / 2] - A[c1 / 2][(c1 + -2) / 2] * x[(c1 + -2) / 2];
      }
      if (c1 >= 3 && c1 <= 29) {
        if ((c1 + 1) % 2 == 0) {
          x[(c1 + -1) / 2] = x[(c1 + -1) / 2] / A[(c1 + -1) / 2][(c1 + -1) / 2];
          x[(c1 + 1) / 2] = x[(c1 + 1) / 2] - A[(c1 + 1) / 2][(c1 + -3) / 2] * x[(c1 + -3) / 2];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
