/**
 * mvt.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "mvt.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
/* Variable declaration/allocation. */
  double A[32][32];
  double x1[32];
  double x2[32];
  double y_1[32];
  double y_2[32];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c0;
    for (c0 = 0; c0 <= 31; c0++) {
      for (c1 = 0; c1 <= 31; c1++) {
        x1[c0] = x1[c0] + A[c0][c1] * y_1[c1];
        x2[c0] = x2[c0] + A[c1][c0] * y_2[c1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
