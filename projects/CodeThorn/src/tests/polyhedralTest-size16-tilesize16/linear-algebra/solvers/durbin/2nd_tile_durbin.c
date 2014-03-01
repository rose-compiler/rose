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
    int c1;
    int c2;
{
      int c4;
      int c3;
      y[0][0] = r[0];
      beta[0] = 1;
      alpha[0] = r[0];
      beta[1] = beta[1 - 1] - alpha[1 - 1] * alpha[1 - 1] * beta[1 - 1];
      for (c3 = 1; c3 <= 15; c3++) {
        sum[0][c3] = r[c3];
      }
      sum[0 + 1][1] = sum[0][1] + r[1 - 0 - 1] * y[0][1 - 1];
      alpha[1] = -sum[1][1] * beta[1];
      y[- 1 * 1 + 1][1] = y[- 1 * 1 + 1][1 - 1] + alpha[1] * y[1 - (- 1 * 1 + 1) - 1][1 - 1];
      y[1][1] = alpha[1];
      sum[0 + 1][2] = sum[0][2] + r[2 - 0 - 1] * y[0][2 - 1];
      sum[1 + 1][2] = sum[1][2] + r[2 - 1 - 1] * y[1][2 - 1];
      beta[2] = beta[2 - 1] - alpha[2 - 1] * alpha[2 - 1] * beta[2 - 1];
      alpha[2] = -sum[2][2] * beta[2];
      y[- 1 * 2 + 2][2] = y[- 1 * 2 + 2][2 - 1] + alpha[2] * y[2 - (- 1 * 2 + 2) - 1][2 - 1];
      y[2][2] = alpha[2];
      y[- 1 * 2 + 3][2] = y[- 1 * 2 + 3][2 - 1] + alpha[2] * y[2 - (- 1 * 2 + 3) - 1][2 - 1];
      for (c3 = 3; c3 <= 15; c3++) {
        sum[0 + 1][c3] = sum[0][c3] + r[c3 - 0 - 1] * y[0][c3 - 1];
        sum[1 + 1][c3] = sum[1][c3] + r[c3 - 1 - 1] * y[1][c3 - 1];
        beta[c3] = beta[c3 - 1] - alpha[c3 - 1] * alpha[c3 - 1] * beta[c3 - 1];
        for (c4 = 2; c4 <= c3 + -1; c4++) {
          sum[c4 + 1][c3] = sum[c4][c3] + r[c3 - c4 - 1] * y[c4][c3 - 1];
        }
        alpha[c3] = -sum[c3][c3] * beta[c3];
        y[- 1 * c3 + c3][c3] = y[- 1 * c3 + c3][c3 - 1] + alpha[c3] * y[c3 - (- 1 * c3 + c3) - 1][c3 - 1];
        y[c3][c3] = alpha[c3];
        for (c4 = c3 + 1; c4 <= 2 * c3 + -1; c4++) {
          y[- 1 * c3 + c4][c3] = y[- 1 * c3 + c4][c3 - 1] + alpha[c3] * y[c3 - (- 1 * c3 + c4) - 1][c3 - 1];
        }
      }
      for (c3 = 0; c3 <= 15; c3++) {
        out[c3] = y[c3][16 - 1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
