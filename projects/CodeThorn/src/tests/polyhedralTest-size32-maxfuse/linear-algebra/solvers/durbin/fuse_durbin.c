#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * durbin.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double y[16][16];
  double sum[16][16];
  double alpha[16];
  double beta[16];
  double r[16];
  double out[16];
  int i;
  int k;
  
#pragma scop
{
    int c2;
    int c3;
    y[0][0] = r[0];
    beta[0] = 1;
    alpha[0] = r[0];
    for (c2 = 1; c2 <= 15; c2++) {
      sum[0][c2] = r[c2];
    }
    for (c2 = 1; c2 <= 15; c2++) {
      sum[0 + 1][c2] = sum[0][c2] + r[c2 - 0 - 1] * y[0][c2 - 1];
      if (c2 >= 2) {
        sum[1 + 1][c2] = sum[1][c2] + r[c2 - 1 - 1] * y[1][c2 - 1];
        beta[c2] = beta[c2 - 1] - alpha[c2 - 1] * alpha[c2 - 1] * beta[c2 - 1];
      }
      if (c2 == 1) {
        beta[1] = beta[1 - 1] - alpha[1 - 1] * alpha[1 - 1] * beta[1 - 1];
      }
      for (c3 = 2; c3 <= c2 + -1; c3++) {
        sum[c3 + 1][c2] = sum[c3][c2] + r[c2 - c3 - 1] * y[c3][c2 - 1];
      }
      alpha[c2] = -sum[c2][c2] * beta[c2];
      y[c2][c2] = alpha[c2];
      for (c3 = c2; c3 <= 2 * c2 + -1; c3++) {
        y[-1 * c2 + c3][c2] = y[-1 * c2 + c3][c2 - 1] + alpha[c2] * y[c2 - (-1 * c2 + c3) - 1][c2 - 1];
      }
    }
    for (c2 = 0; c2 <= 15; c2++) {
      out[c2] = y[c2][16 - 1];
    }
  }
  
#pragma endscop
  return 0;
}
