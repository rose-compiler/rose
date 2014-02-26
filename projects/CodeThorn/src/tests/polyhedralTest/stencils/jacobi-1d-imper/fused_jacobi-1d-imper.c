/**
 * jacobi-1d-imper.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is double, default size is 100x10000. */
#include "jacobi-1d-imper.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 10;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[10];
  double B[10];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c3;
    for (c1 = 0; c1 <= 1; c1++) {
      for (c3 = 1; c3 <= 8; c3++) {
        B[c3] = 0.33333 * (A[c3 - 1] + A[c3] + A[c3 + 1]);
      }
      for (c3 = 1; c3 <= 8; c3++) {
        A[c3] = B[c3];
      }
    }
  }
  
#pragma endscop
  POLYBENCH_FREE_ARRAY(B);
  return 0;
}
