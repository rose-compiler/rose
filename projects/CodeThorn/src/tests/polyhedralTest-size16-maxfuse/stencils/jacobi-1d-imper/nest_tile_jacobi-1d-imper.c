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
    int c3;
    int c0;
    int c2;
    for (c0 = 0; c0 <= 4; c0++) {
      for (c2 = (0 > 2 * c0 + -7?0 : 2 * c0 + -7); c2 <= ((1 < 2 * c0?1 : 2 * c0)); c2++) {
        if (c0 == 0 && c2 == 0) {
          B[1] = 0.33333 * (A[1 - 1] + A[1] + A[1 + 1]);
        }
        for (c3 = (4 * c0 > 2 * c2 + 2?4 * c0 : 2 * c2 + 2); c3 <= ((4 * c0 + 3 < 2 * c2 + 14?4 * c0 + 3 : 2 * c2 + 14)); c3++) {
          B[-2 * c2 + c3] = 0.33333 * (A[-2 * c2 + c3 - 1] + A[-2 * c2 + c3] + A[-2 * c2 + c3 + 1]);
          A[-2 * c2 + c3 + -1] = B[-2 * c2 + c3 + -1];
        }
        if (c0 == c2 + 3) {
          A[14] = B[14];
        }
      }
      if (c0 == 0) {
        B[1] = 0.33333 * (A[1 - 1] + A[1] + A[1 + 1]);
      }
    }
  }
  
#pragma endscop
  POLYBENCH_FREE_ARRAY(B);
  return 0;
}
