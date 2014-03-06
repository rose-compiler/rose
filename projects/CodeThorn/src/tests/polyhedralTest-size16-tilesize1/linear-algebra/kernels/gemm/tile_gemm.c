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
    int c1;
    int c5;
{
      int c2;
      int c6;
      int c8;
      for (c2 = 0; c2 <= 15; c2++) {
        for (c6 = 0; c6 <= 15; c6++) {
          C[c2][c6] *= beta;
          for (c8 = 0; c8 <= 15; c8++) {
            C[c2][c6] += alpha * A[c2][c8] * B[c8][c6];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
