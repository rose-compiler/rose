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
{
      int c5;
      int c2;
      y[0][0] = r[0];
      beta[0] = 1;
      alpha[0] = r[0];
      for (c2 = 1; c2 <= 15; c2++) {
        beta[c2] = beta[c2 - 1] - alpha[c2 - 1] * alpha[c2 - 1] * beta[c2 - 1];
        sum[0][c2] = r[c2];
        for (c5 = 0; c5 <= c2 + -1; c5++) {
          sum[c5 + 1][c2] = sum[c5][c2] + r[c2 - c5 - 1] * y[c5][c2 - 1];
        }
        alpha[c2] = -sum[c2][c2] * beta[c2];
        for (c5 = 0; c5 <= c2 + -1; c5++) {
          y[c5][c2] = y[c5][c2 - 1] + alpha[c2] * y[c2 - c5 - 1][c2 - 1];
        }
        y[c2][c2] = alpha[c2];
      }
      for (c2 = 0; c2 <= 15; c2++) {
        out[c2] = y[c2][16 - 1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
