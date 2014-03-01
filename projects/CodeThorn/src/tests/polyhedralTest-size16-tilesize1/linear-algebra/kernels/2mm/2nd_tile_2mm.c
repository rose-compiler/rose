#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
#   define NK ARRAYSIZE
#   define NL ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
# define _PB_NK ARRAYSIZE
# define _PB_NL ARRAYSIZE
/**
 * 2mm.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nl = 16;
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double tmp[16][16];
  double A[16][16];
  double B[16][16];
  double C[16][16];
  double D[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c2;
    int c1;
    int c5;
    
#pragma omp parallel for private(c2)
{
      int c4;
      int c3;
      for (c3 = 0; c3 <= 15; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 0; c4 <= 15; c4++) {
          D[c3][c4] *= beta;
          tmp[c3][c4] = 0;
        }
      }
    }
    
#pragma omp parallel for private(c5, c2)
{
      int c3;
      int c4;
      int c6;
      for (c3 = 0; c3 <= 15; c3++) {
        for (c4 = 0; c4 <= 15; c4++) {
          for (c6 = 0; c6 <= 15; c6++) {
            tmp[c3][c4] += alpha * A[c3][c6] * B[c6][c4];
            D[c3][c6] += tmp[c3][c4] * C[c4][c6];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
