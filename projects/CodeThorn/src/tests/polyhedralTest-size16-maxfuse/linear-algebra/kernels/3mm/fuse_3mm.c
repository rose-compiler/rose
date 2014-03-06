#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
#   define NK ARRAYSIZE
#   define NL ARRAYSIZE
#   define NM ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
# define _PB_NK ARRAYSIZE
# define _PB_NL ARRAYSIZE
# define _PB_NM ARRAYSIZE
/**
 * 3mm.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nm = 16;
  double E[16][16];
  double A[16][16];
  double B[16][16];
  double F[16][16];
  double C[16][16];
  double D[16][16];
  double G[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c6;
    int c1;
    int c0;
    for (c0 = 0; c0 <= 15; c0++) {
      for (c1 = c0; c1 <= c0 + 15; c1++) {
        G[c0][-1 * c0 + c1] = 0;
        F[c0][-1 * c0 + c1] = 0;
        for (c6 = 0; c6 <= 15; c6++) {
          F[c0][-1 * c0 + c1] += C[c0][c6] * D[c6][-1 * c0 + c1];
        }
        E[c0][-1 * c0 + c1] = 0;
        for (c6 = 0; c6 <= 15; c6++) {
          E[c0][-1 * c0 + c1] += A[c0][c6] * B[c6][-1 * c0 + c1];
        }
        for (c6 = 0; c6 <= c0; c6++) {
          G[c6][-1 * c0 + c1] += E[c6][c0 + -1 * c6] * F[c0 + -1 * c6][-1 * c0 + c1];
        }
      }
    }
    for (c0 = 16; c0 <= 30; c0++) {
      for (c1 = c0; c1 <= c0 + 15; c1++) {
        for (c6 = c0 + -15; c6 <= 15; c6++) {
          G[c6][-1 * c0 + c1] += E[c6][c0 + -1 * c6] * F[c0 + -1 * c6][-1 * c0 + c1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
