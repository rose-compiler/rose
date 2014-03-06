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
{
      int c5;
      int c1;
      for (c1 = 0; c1 <= 31; c1++) {
        for (c5 = 0; c5 <= 31; c5++) {
          A[c5][c1] = A[c5][c1] + u1[c5] * v1[c1] + u2[c5] * v2[c1];
          x[c1] = x[c1] + beta * A[c5][c1] * y[c5];
        }
        x[c1] = x[c1] + z[c1];
        for (c5 = 0; c5 <= 31; c5++) {
          w[c5] = w[c5] + alpha * A[c5][c1] * x[c1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
