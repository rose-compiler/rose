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
    int c2;
    int c1;
#pragma omp parallel for private(c2)
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        A[c2][c1] = A[c2][c1] + u1[c2] * v1[c1] + u2[c2] * v2[c1];
        x[c1] = x[c1] + beta * A[c2][c1] * y[c2];
      }
    }
#pragma omp parallel for
    for (c1 = 0; c1 <= 15; c1++) {
      x[c1] = x[c1] + z[c1];
    }
#pragma omp parallel for private(c2)
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        w[c1] = w[c1] + alpha * A[c1][c2] * x[c2];
      }
    }
  }
  
#pragma endscop
  return 0;
}
