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
#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[16];
  double B[16];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c0;
    int c1;
{
      int c2;
      int c5;
      for (c2 = 0; c2 <= 1; c2++) {
        for (c5 = 2 * c2 + 2; c5 <= 2 * c2 + 14; c5++) {
          B[- 2 * c2 + c5] = 0.33333 * (A[- 2 * c2 + c5 - 1] + A[- 2 * c2 + c5] + A[- 2 * c2 + c5 + 1]);
        }
        B[1] = 0.33333 * (A[1 - 1] + A[1] + A[1 + 1]);
        A[14] = B[14];
        for (c5 = 2 * c2 + 2; c5 <= 2 * c2 + 14; c5++) {
          A[- 2 * c2 + c5 + - 1] = B[- 2 * c2 + c5 + - 1];
        }
      }
    }
  }
  
#pragma endscop
  POLYBENCH_FREE_ARRAY(B);
  return 0;
}
