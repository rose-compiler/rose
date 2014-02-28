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
    int c2;
    int c3;
{
      int c5;
      int c1;
      int c6;
#pragma omp parallel for private(c6, c5)
      for (c1 = 1; c1 <= 15; c1++) {
        for (c5 = (c1 + 1 > 2 * c1 + -13?c1 + 1 : 2 * c1 + -13); c5 <= ((c1 + 15 < 2 * c1 + 13?c1 + 15 : 2 * c1 + 13)); c5++) {
          for (c6 = (((0 > c1 + -14?0 : c1 + -14)) > -1 * c1 + c5 + -14?((0 > c1 + -14?0 : c1 + -14)) : -1 * c1 + c5 + -14); c6 <= ((((1 < c1 + -1?1 : c1 + -1)) < -1 * c1 + c5 + -1?((1 < c1 + -1?1 : c1 + -1)) : -1 * c1 + c5 + -1)); c6++) {
            A[c1 + - 1 * c6][- 1 * c1 + c5 + - 1 * c6] = (A[c1 + - 1 * c6 - 1][- 1 * c1 + c5 + - 1 * c6 - 1] + A[c1 + - 1 * c6 - 1][- 1 * c1 + c5 + - 1 * c6] + A[c1 + - 1 * c6 - 1][- 1 * c1 + c5 + - 1 * c6 + 1] + A[c1 + - 1 * c6][- 1 * c1 + c5 + - 1 * c6 - 1] + A[c1 + - 1 * c6][- 1 * c1 + c5 + - 1 * c6] + A[c1 + - 1 * c6][- 1 * c1 + c5 + - 1 * c6 + 1] + A[c1 + - 1 * c6 + 1][- 1 * c1 + c5 + - 1 * c6 - 1] + A[c1 + - 1 * c6 + 1][- 1 * c1 + c5 + - 1 * c6] + A[c1 + - 1 * c6 + 1][- 1 * c1 + c5 + - 1 * c6 + 1]) / 9.0;
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
