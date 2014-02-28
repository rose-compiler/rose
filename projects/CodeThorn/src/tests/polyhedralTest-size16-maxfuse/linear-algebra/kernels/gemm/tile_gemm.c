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
    int c2;
    int c1;
    int c0;
{
      int c5;
      int c3;
      int c4;
      for (c3 = 0; c3 <= 15; c3++) {
        for (c4 = 0; c4 <= 15; c4++) {
          C[c3][c4] *= beta;
          for (c5 = 0; c5 <= 15; c5++) {
            C[c3][c4] += alpha * A[c3][c5] * B[c5][c4];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
