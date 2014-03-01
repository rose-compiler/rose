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
    int c3;
    int c1;
    y[0][0] = r[0];
    beta[0] = 1;
    alpha[0] = r[0];
    for (c1 = 1; c1 <= 15; c1++) {
      beta[c1] = beta[c1 - 1] - alpha[c1 - 1] * alpha[c1 - 1] * beta[c1 - 1];
      sum[0][c1] = r[c1];
      for (c3 = 0; c3 <= c1 + -1; c3++) {
        sum[c3 + 1][c1] = sum[c3][c1] + r[c1 - c3 - 1] * y[c3][c1 - 1];
      }
      alpha[c1] = -sum[c1][c1] * beta[c1];
      for (c3 = 0; c3 <= c1 + -1; c3++) {
        y[c3][c1] = y[c3][c1 - 1] + alpha[c1] * y[c1 - c3 - 1][c1 - 1];
      }
      y[c1][c1] = alpha[c1];
    }
    for (c1 = 0; c1 <= 15; c1++) {
      out[c1] = y[c1][16 - 1];
    }
  }
  
#pragma endscop
  return 0;
}
