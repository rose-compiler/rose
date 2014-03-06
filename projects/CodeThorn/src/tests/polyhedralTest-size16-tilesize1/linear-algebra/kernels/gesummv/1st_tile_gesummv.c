#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * gesummv.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double B[16][16];
  double tmp[16];
  double x[16];
  double y[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c2;
#pragma omp parallel for
    for (c1 = 0; c1 <= 15; c1++) {
      y[c1] = 0;
    }
#pragma omp parallel for private(c2)
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        y[c1] = B[c1][c2] * x[c2] + y[c1];
      }
    }
#pragma omp parallel for
    for (c1 = 0; c1 <= 15; c1++) {
      tmp[c1] = 0;
    }
#pragma omp parallel for private(c2)
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        tmp[c1] = A[c1][c2] * x[c2] + tmp[c1];
      }
    }
#pragma omp parallel for
    for (c1 = 0; c1 <= 15; c1++) {
      y[c1] = alpha * tmp[c1] + beta * y[c1];
    }
  }
  
#pragma endscop
  return 0;
}
