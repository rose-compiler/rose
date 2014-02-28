#   define NI ARRAYSIZE
# define _PB_NI ARRAYSIZE
/**
 * trmm.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int ni = 16;
/* Variable declaration/allocation. */
  double alpha;
  double A[16][16];
  double B[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c0;
    int c1;
    int c3;
{
      int c6;
      int c4;
      int c2;
#pragma omp parallel for private(c4, c6)
      for (c2 = 1; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 0; c4 <= 15; c4++) {
          for (c6 = c4; c6 <= c2 + c4 + -1; c6++) {
            B[c2][c4] += alpha * A[c2][c6 + - 1 * c4] * B[c4][c6 + - 1 * c4];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
