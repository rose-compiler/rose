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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "gemver.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double A[32][32];
  double u1[32];
  double v1[32];
  double u2[32];
  double v2[32];
  double w[32];
  double x[32];
  double y[32];
  double z[32];
  int i;
  int j;
  
#pragma scop
{
    int c3;
    int c0;
    for (c0 = 0; c0 <= 31; c0++) {
      for (c3 = 0; c3 <= 31; c3++) {
        A[c3][c0] = A[c3][c0] + u1[c3] * v1[c0] + u2[c3] * v2[c0];
        x[c0] = x[c0] + beta * A[c3][c0] * y[c3];
      }
      x[c0] = x[c0] + z[c0];
      for (c3 = 0; c3 <= 31; c3++) {
        w[c3] = w[c3] + alpha * A[c3][c0] * x[c0];
      }
    }
  }
  
#pragma endscop
  return 0;
}
