#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
/**
 * syrk.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double C[16][16];
  double A[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c1;
    int c3;
    int c2;
    
#pragma omp parallel for private(c2)
{
      int c3;
      int c0;
#pragma omp parallel for private(c3)
      for (c0 = 0; c0 <= 15; c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c3 = 0; c3 <= 15; c3++) {
          C[c0][c3] *= beta;
        }
      }
    }
    
#pragma omp parallel for private(c2, c3)
{
      int c4;
      int c0;
      int c5;
#pragma omp parallel for private(c5, c4)
      for (c0 = 0; c0 <= 15; c0++) {
        for (c4 = 0; c4 <= 15; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c5 = 0; c5 <= 15; c5++) {
            C[c0][c4] += alpha * A[c0][c5] * A[c4][c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
