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
  int UB;
  
#pragma scop
{
    int c0;
    int c1;
    if (UB >= 3) {
      for (c0 = 0; c0 <= 1; c0++) {
        B[1] = 0.33333 * (A[1 - 1] + A[1] + A[1 + 1]);
        for (c1 = 2 * c0 + 2; c1 <= 2 * c0 + UB + -2; c1++) {
          B[-2 * c0 + c1] = 0.33333 * (A[-2 * c0 + c1 - 1] + A[-2 * c0 + c1] + A[-2 * c0 + c1 + 1]);
          A[-2 * c0 + c1 + -1] = B[-2 * c0 + c1 + -1];
        }
        A[UB + -2] = B[UB + -2];
      }
    }
  }
  
#pragma endscop
  POLYBENCH_FREE_ARRAY(B);
  return 0;
}
