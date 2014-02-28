#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * lu.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Variable declaration/allocation. */
  double A[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c2;
    int c0;
    int c1;
    for (c0 = 1; c0 <= 29; c0++) {
#pragma omp parallel for private(c2)
      for (c1 = ((c0 + 1) * 2 < 0?-(-(c0 + 1) / 2) : ((2 < 0?(-(c0 + 1) + - 2 - 1) / - 2 : (c0 + 1 + 2 - 1) / 2))); c1 <= ((15 < c0?15 : c0)); c1++) {
        A[c0 + -1 * c1][c1] = A[c0 + -1 * c1][c1] / A[c0 + -1 * c1][c0 + -1 * c1];
        for (c2 = c0 + -1 * c1 + 1; c2 <= 15; c2++) {
          A[c2][c1] = A[c2][c1] - A[c2][c0 + -1 * c1] * A[c0 + -1 * c1][c1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
