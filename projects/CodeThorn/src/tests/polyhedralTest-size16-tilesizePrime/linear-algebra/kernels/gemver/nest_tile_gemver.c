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
    int c4;
    int c2;
    int c3;
    int c1;
#pragma omp parallel for private(c3, c2, c4)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c3 = 7 * c2; c3 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c4 = 5 * c1; c4 <= ((15 < 5 * c1 + 4?15 : 5 * c1 + 4)); c4++) {
            A[c3][c4] = A[c3][c4] + u1[c3] * v1[c4] + u2[c3] * v2[c4];
            x[c4] = x[c4] + beta * A[c3][c4] * y[c3];
          }
        }
      }
    }
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 3; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 5 * c1; c4 <= ((15 < 5 * c1 + 4?15 : 5 * c1 + 4)); c4++) {
        x[c4] = x[c4] + z[c4];
      }
    }
#pragma omp parallel for private(c3, c2, c4)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c3 = 7 * c2; c3 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c4 = 5 * c1; c4 <= ((15 < 5 * c1 + 4?15 : 5 * c1 + 4)); c4++) {
            w[c4] = w[c4] + alpha * A[c4][c3] * x[c3];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
