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
{
      int c3;
      int c7;
      int c2;
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = c2; c3 <= c2 + 15; c3++) {
          E[c2][- 1 * c2 + c3] = 0;
          for (c7 = 0; c7 <= 15; c7++) {
            E[c2][- 1 * c2 + c3] += A[c2][c7] * B[c7][- 1 * c2 + c3];
          }
          F[c2][- 1 * c2 + c3] = 0;
          for (c7 = 0; c7 <= 15; c7++) {
            F[c2][- 1 * c2 + c3] += C[c2][c7] * D[c7][- 1 * c2 + c3];
          }
          G[c2][- 1 * c2 + c3] = 0;
          for (c7 = 0; c7 <= c2; c7++) {
            G[c7][- 1 * c2 + c3] += E[c7][c2 + - 1 * c7] * F[c2 + - 1 * c7][- 1 * c2 + c3];
          }
        }
      }
      for (c2 = 16; c2 <= 30; c2++) {
        for (c3 = c2; c3 <= c2 + 15; c3++) {
          for (c7 = c2 + -15; c7 <= 15; c7++) {
            G[c7][- 1 * c2 + c3] += E[c7][c2 + - 1 * c7] * F[c2 + - 1 * c7][- 1 * c2 + c3];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
