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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "trisolv.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
/* Variable declaration/allocation. */
  double A[32][32];
  double x[32];
  double c[32];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c0;
    x[0] = c[0];
    x[0] = x[0] / A[0][0];
    for (c0 = 1; c0 <= 31; c0++) {
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
