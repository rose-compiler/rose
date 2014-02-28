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
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[16][16];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c0;
    int c1;
    int c2;
    for (c0 = 0; c0 <= 1; c0++) {
      for (c1 = c0 + 1; c1 <= c0 + 14; c1++) {
        for (c2 = c0 + c1 + 1; c2 <= c0 + c1 + 14; c2++) {
          A[-1 * c0 + c1][-1 * c0 + -1 * c1 + c2] = (A[-1 * c0 + c1 - 1][-1 * c0 + -1 * c1 + c2 - 1] + A[-1 * c0 + c1 - 1][-1 * c0 + -1 * c1 + c2] + A[-1 * c0 + c1 - 1][-1 * c0 + -1 * c1 + c2 + 1] + A[-1 * c0 + c1][-1 * c0 + -1 * c1 + c2 - 1] + A[-1 * c0 + c1][-1 * c0 + -1 * c1 + c2] + A[-1 * c0 + c1][-1 * c0 + -1 * c1 + c2 + 1] + A[-1 * c0 + c1 + 1][-1 * c0 + -1 * c1 + c2 - 1] + A[-1 * c0 + c1 + 1][-1 * c0 + -1 * c1 + c2] + A[-1 * c0 + c1 + 1][-1 * c0 + -1 * c1 + c2 + 1]) / 9.0;
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
