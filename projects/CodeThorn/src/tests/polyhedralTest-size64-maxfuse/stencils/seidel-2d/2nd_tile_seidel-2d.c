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
{
      int c6;
      int c5;
      int c1;
#pragma omp parallel for private(c5, c6)
      for (c1 = 1; c1 <= 63; c1++) {
        for (c5 = (c1 + 1 > 2 * c1 + -61?c1 + 1 : 2 * c1 + -61); c5 <= ((c1 + 63 < 2 * c1 + 61?c1 + 63 : 2 * c1 + 61)); c5++) {
          for (c6 = (((0 > c1 + -62?0 : c1 + -62)) > -1 * c1 + c5 + -62?((0 > c1 + -62?0 : c1 + -62)) : -1 * c1 + c5 + -62); c6 <= ((((1 < c1 + -1?1 : c1 + -1)) < -1 * c1 + c5 + -1?((1 < c1 + -1?1 : c1 + -1)) : -1 * c1 + c5 + -1)); c6++) {
            A[c1 + - 1 * c6][- 1 * c1 + c5 + - 1 * c6] = (A[c1 + - 1 * c6 - 1][- 1 * c1 + c5 + - 1 * c6 - 1] + A[c1 + - 1 * c6 - 1][- 1 * c1 + c5 + - 1 * c6] + A[c1 + - 1 * c6 - 1][- 1 * c1 + c5 + - 1 * c6 + 1] + A[c1 + - 1 * c6][- 1 * c1 + c5 + - 1 * c6 - 1] + A[c1 + - 1 * c6][- 1 * c1 + c5 + - 1 * c6] + A[c1 + - 1 * c6][- 1 * c1 + c5 + - 1 * c6 + 1] + A[c1 + - 1 * c6 + 1][- 1 * c1 + c5 + - 1 * c6 - 1] + A[c1 + - 1 * c6 + 1][- 1 * c1 + c5 + - 1 * c6] + A[c1 + - 1 * c6 + 1][- 1 * c1 + c5 + - 1 * c6 + 1]) / 9.0;
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
