#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE
/**
 * seidel-2d.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int n = 64;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[64][64];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c0;
    int c3;
    int c2;
    for (c0 = 1; c0 <= 63; c0++) {
      for (c2 = (c0 + 1 > 2 * c0 + -61?c0 + 1 : 2 * c0 + -61); c2 <= ((c0 + 63 < 2 * c0 + 61?c0 + 63 : 2 * c0 + 61)); c2++) {
        for (c3 = (((0 > c0 + -62?0 : c0 + -62)) > -1 * c0 + c2 + -62?((0 > c0 + -62?0 : c0 + -62)) : -1 * c0 + c2 + -62); c3 <= ((((1 < c0 + -1?1 : c0 + -1)) < -1 * c0 + c2 + -1?((1 < c0 + -1?1 : c0 + -1)) : -1 * c0 + c2 + -1)); c3++) {
          A[c0 + -1 * c3][-1 * c0 + c2 + -1 * c3] = (A[c0 + -1 * c3 - 1][-1 * c0 + c2 + -1 * c3 - 1] + A[c0 + -1 * c3 - 1][-1 * c0 + c2 + -1 * c3] + A[c0 + -1 * c3 - 1][-1 * c0 + c2 + -1 * c3 + 1] + A[c0 + -1 * c3][-1 * c0 + c2 + -1 * c3 - 1] + A[c0 + -1 * c3][-1 * c0 + c2 + -1 * c3] + A[c0 + -1 * c3][-1 * c0 + c2 + -1 * c3 + 1] + A[c0 + -1 * c3 + 1][-1 * c0 + c2 + -1 * c3 - 1] + A[c0 + -1 * c3 + 1][-1 * c0 + c2 + -1 * c3] + A[c0 + -1 * c3 + 1][-1 * c0 + c2 + -1 * c3 + 1]) / 9.0;
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
