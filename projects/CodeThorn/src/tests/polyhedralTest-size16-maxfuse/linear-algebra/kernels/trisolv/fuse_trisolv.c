#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * trisolv.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double A[16][16];
  double x[16];
  double c[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c0;
    x[0] = c[0];
    x[0] = x[0] / A[0][0];
    for (c0 = 1; c0 <= 15; c0++) {
      x[c0] = c[c0];
      for (c1 = 0; c1 <= c0 + -1; c1++) {
        x[c0] = x[c0] - A[c0][c1] * x[c1];
      }
      x[c0] = x[c0] / A[c0][c0];
    }
  }
  
#pragma endscop
  return 0;
}
