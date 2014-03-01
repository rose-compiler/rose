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
    int c6;
    int c2;
    int c3;
    int c4;
    int c1;
#pragma omp parallel for private(c4, c2, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 3; c2++) {
        for (c4 = 2 * c1; c4 <= 2 * c1 + 1; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 4 * c2; c6 <= 4 * c2 + 3; c6++) {
            C[c4][c6] *= beta;
          }
        }
      }
    }
#pragma omp parallel for private(c4, c3, c2, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 3; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c4 = 2 * c1; c4 <= 2 * c1 + 1; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c6 = 4 * c2; c6 <= 4 * c2 + 3; c6++) {
              C[c4][c6] += alpha * A[c4][c3] * A[c6][c3];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
