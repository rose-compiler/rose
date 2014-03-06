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
  int n = 32;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[32];
  double B[32];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c0;
    int c2;
    for (c0 = 1; c0 <= 33; c0++) {
      if (c0 >= 31) {
        if ((c0 + 1) % 2 == 0) {
          A[30] = B[30];
        }
      }
      for (c2 = (0 > (((c0 + -30) * 2 < 0?-(-(c0 + -30) / 2) : ((2 < 0?(-(c0 + -30) + - 2 - 1) / - 2 : (c0 + -30 + 2 - 1) / 2))))?0 : (((c0 + -30) * 2 < 0?-(-(c0 + -30) / 2) : ((2 < 0?(-(c0 + -30) + - 2 - 1) / - 2 : (c0 + -30 + 2 - 1) / 2))))); c2 <= ((1 < (((c0 + -2) * 2 < 0?((2 < 0?-((-(c0 + -2) + 2 + 1) / 2) : -((-(c0 + -2) + 2 - 1) / 2))) : (c0 + -2) / 2))?1 : (((c0 + -2) * 2 < 0?((2 < 0?-((-(c0 + -2) + 2 + 1) / 2) : -((-(c0 + -2) + 2 - 1) / 2))) : (c0 + -2) / 2)))); c2++) {
        B[c0 + -2 * c2] = 0.33333 * (A[c0 + -2 * c2 - 1] + A[c0 + -2 * c2] + A[c0 + -2 * c2 + 1]);
        A[c0 + -2 * c2 + -1] = B[c0 + -2 * c2 + -1];
      }
      if (c0 <= 3) {
        if ((c0 + 1) % 2 == 0) {
          B[1] = 0.33333 * (A[1 - 1] + A[1] + A[1 + 1]);
        }
      }
    }
  }
  
#pragma endscop
  POLYBENCH_FREE_ARRAY(B);
  return 0;
}
