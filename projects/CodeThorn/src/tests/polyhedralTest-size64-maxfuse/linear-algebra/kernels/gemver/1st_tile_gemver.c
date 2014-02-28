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
  int n = 64;
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double A[64][64];
  double u1[64];
  double v1[64];
  double u2[64];
  double v2[64];
  double w[64];
  double x[64];
  double y[64];
  double z[64];
  int i;
  int j;
  
#pragma scop
{
    int c4;
    int c2;
    int c1;
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 63; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
          A[c2][c4] = A[c2][c4] + u1[c2] * v1[c4] + u2[c2] * v2[c4];
          x[c4] = x[c4] + beta * A[c2][c4] * y[c2];
        }
      }
    }
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 7; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
        x[c4] = x[c4] + z[c4];
      }
    }
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 63; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
          w[c4] = w[c4] + alpha * A[c4][c2] * x[c2];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
