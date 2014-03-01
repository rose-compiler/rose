#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * gemver.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double alpha;
  double beta;
  double A[16][16];
  double u1[16];
  double v1[16];
  double u2[16];
  double v2[16];
  double w[16];
  double x[16];
  double y[16];
  double z[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c2;
{
      int c4;
      int c3;
      for (c3 = 0; c3 <= 15; c3++) {
        for (c4 = 0; c4 <= 15; c4++) {
          A[c3][c4] = A[c3][c4] + u1[c3] * v1[c4] + u2[c3] * v2[c4];
        }
      }
      for (c3 = 0; c3 <= 15; c3++) {
        for (c4 = 0; c4 <= 15; c4++) {
          x[c3] = x[c3] + beta * A[c4][c3] * y[c4];
        }
      }
      for (c3 = 0; c3 <= 15; c3++) {
        x[c3] = x[c3] + z[c3];
      }
      for (c3 = 0; c3 <= 15; c3++) {
        for (c4 = 0; c4 <= 15; c4++) {
          w[c3] = w[c3] + alpha * A[c3][c4] * x[c4];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
