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
    int c3;
    for (c1 = 0; c1 <= 15; c1++) {
      tmp[c1] = 0;
      y[c1] = 0;
      for (c3 = 0; c3 <= 15; c3++) {
        tmp[c1] = A[c1][c3] * x[c3] + tmp[c1];
        y[c1] = B[c1][c3] * x[c3] + y[c1];
      }
      y[c1] = alpha * tmp[c1] + beta * y[c1];
    }
  }
  
#pragma endscop
  return 0;
}
