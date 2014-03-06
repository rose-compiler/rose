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
    int c3;
{
      int c5;
      int c2;
      for (c2 = 0; c2 <= 15; c2++) {
        for (c5 = 0; c5 <= 15; c5++) {
          A[c2][c5] = A[c2][c5] + u1[c2] * v1[c5] + u2[c2] * v2[c5];
        }
      }
      for (c2 = 0; c2 <= 15; c2++) {
        for (c5 = 0; c5 <= 15; c5++) {
          x[c2] = x[c2] + beta * A[c5][c2] * y[c5];
        }
      }
      for (c2 = 0; c2 <= 15; c2++) {
        x[c2] = x[c2] + z[c2];
      }
      for (c2 = 0; c2 <= 15; c2++) {
        for (c5 = 0; c5 <= 15; c5++) {
          w[c2] = w[c2] + alpha * A[c2][c5] * x[c5];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
