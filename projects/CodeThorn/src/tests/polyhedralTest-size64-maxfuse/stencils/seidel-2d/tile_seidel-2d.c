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
{
      int c5;
      int c3;
      int c4;
      for (c3 = 0; c3 <= 1; c3++) {
        for (c4 = c3 + 1; c4 <= c3 + 14; c4++) {
          for (c5 = c3 + c4 + 1; c5 <= c3 + c4 + 14; c5++) {
            A[- 1 * c3 + c4][- 1 * c3 + - 1 * c4 + c5] = (A[- 1 * c3 + c4 - 1][- 1 * c3 + - 1 * c4 + c5 - 1] + A[- 1 * c3 + c4 - 1][- 1 * c3 + - 1 * c4 + c5] + A[- 1 * c3 + c4 - 1][- 1 * c3 + - 1 * c4 + c5 + 1] + A[- 1 * c3 + c4][- 1 * c3 + - 1 * c4 + c5 - 1] + A[- 1 * c3 + c4][- 1 * c3 + - 1 * c4 + c5] + A[- 1 * c3 + c4][- 1 * c3 + - 1 * c4 + c5 + 1] + A[- 1 * c3 + c4 + 1][- 1 * c3 + - 1 * c4 + c5 - 1] + A[- 1 * c3 + c4 + 1][- 1 * c3 + - 1 * c4 + c5] + A[- 1 * c3 + c4 + 1][- 1 * c3 + - 1 * c4 + c5 + 1]) / 9.0;
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
