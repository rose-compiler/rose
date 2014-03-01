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
    int c6;
    int c1;
    int c0;
    for (c0 = 0; c0 <= 15; c0++) {
      for (c1 = 0; c1 <= 15; c1++) {
        D[c0][c1] *= beta;
        tmp[c0][c1] = 0;
        for (c6 = 0; c6 <= 15; c6++) {
          tmp[c0][c1] += alpha * A[c0][c6] * B[c6][c1];
        }
        for (c6 = 0; c6 <= c1; c6++) {
          D[c0][c6] += tmp[c0][c1 + -1 * c6] * C[c1 + -1 * c6][c6];
        }
      }
      for (c1 = 16; c1 <= 30; c1++) {
        for (c6 = c1 + -15; c6 <= 15; c6++) {
          D[c0][c6] += tmp[c0][c1 + -1 * c6] * C[c1 + -1 * c6][c6];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
