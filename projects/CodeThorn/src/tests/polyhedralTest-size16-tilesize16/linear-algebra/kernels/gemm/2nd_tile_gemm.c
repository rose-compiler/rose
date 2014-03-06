#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
#   define NK ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
# define _PB_NK ARRAYSIZE
/**
 * gemm.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nj = 16;
  int nk = 16;
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double C[16][16];
  double A[16][16];
  double B[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c3;
    int c4;
    int c2;
{
      int c6;
      int c7;
      int c5;
      for (c5 = 0; c5 <= 15; c5++) {
        for (c7 = 0; c7 <= 15; c7++) {
          C[c5][c7] *= beta;
        }
      }
      for (c5 = 0; c5 <= 15; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c6 = 0; c6 <= 15; c6++) {
          for (c7 = 0; c7 <= 15; c7++) {
            C[c5][c7] += alpha * A[c5][c6] * B[c6][c7];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
