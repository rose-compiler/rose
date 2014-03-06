#   define NX ARRAYSIZE
#   define NY ARRAYSIZE
# define _PB_NX ARRAYSIZE
# define _PB_NY ARRAYSIZE
/**
 * atax.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nx = 16;
  int ny = 16;
  double A[16][16];
  double x[16];
  double y[16];
  double tmp[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c4;
    int c2;
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 2; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 7 * c1; c4 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c4++) {
        tmp[c4] = 0;
      }
    }
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 2; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 7 * c1; c4 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c4++) {
          tmp[c4] = tmp[c4] + A[c4][c2] * x[c2];
        }
      }
    }
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 2; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 7 * c1; c4 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c4++) {
        y[c4] = 0;
      }
    }
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 2; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 7 * c1; c4 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c4++) {
          y[c4] = y[c4] + A[c2][c4] * tmp[c2];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
