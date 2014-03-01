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
{
      int c3;
      int c7;
      int c2;
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          tmp[c2][c3] = 0;
          for (c7 = 0; c7 <= 15; c7++) {
            tmp[c2][c3] += alpha * A[c2][c7] * B[c7][c3];
          }
          D[c2][c3] *= beta;
          for (c7 = 0; c7 <= c3; c7++) {
            D[c2][c7] += tmp[c2][c3 + - 1 * c7] * C[c3 + - 1 * c7][c7];
          }
        }
        for (c3 = 16; c3 <= 30; c3++) {
          for (c7 = c3 + -15; c7 <= 15; c7++) {
            D[c2][c7] += tmp[c2][c3 + - 1 * c7] * C[c3 + - 1 * c7][c7];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
