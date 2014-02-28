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
{
        int c5;
        int c4;
        if (c0 >= 1 && c0 <= 29) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c4 = ((c0 + 1) * 2 < 0?-(-(c0 + 1) / 2) : ((2 < 0?(-(c0 + 1) + - 2 - 1) / - 2 : (c0 + 1 + 2 - 1) / 2))); c4 <= ((15 < c0?15 : c0)); c4++) {
            A[c0 + - 1 * c4][c4] = A[c0 + - 1 * c4][c4] / A[c0 + - 1 * c4][c0 + - 1 * c4];
          }
#pragma ivdep
#pragma vector always
#pragma simd
          for (c4 = ((c0 + 1) * 2 < 0?-(-(c0 + 1) / 2) : ((2 < 0?(-(c0 + 1) + - 2 - 1) / - 2 : (c0 + 1 + 2 - 1) / 2))); c4 <= ((15 < c0?15 : c0)); c4++) {
            for (c5 = -1 * c4 + c0 + 1; c5 <= 15; c5++) {
              A[c5][c4] = A[c5][c4] - A[c5][c0 + - 1 * c4] * A[c0 + - 1 * c4][c4];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
