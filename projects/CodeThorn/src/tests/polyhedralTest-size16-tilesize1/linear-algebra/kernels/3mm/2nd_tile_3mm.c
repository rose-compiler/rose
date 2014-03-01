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
    int c2;
    int c1;
    int c5;
    
#pragma omp parallel for private(c2)
{
      int c3;
      int c4;
      for (c3 = 0; c3 <= 15; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 0; c4 <= 15; c4++) {
          G[c3][c4] = 0;
          F[c3][c4] = 0;
        }
      }
    }
    
#pragma omp parallel for private(c5, c2)
{
      int c4;
      int c3;
      int c0;
#pragma omp parallel for private(c3, c4)
      for (c0 = 0; c0 <= 15; c0++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c4 = 0; c4 <= 15; c4++) {
            F[c0][c3] += C[c0][c4] * D[c4][c3];
          }
        }
      }
    }
    
#pragma omp parallel for private(c2)
{
      int c0;
      int c3;
#pragma omp parallel for private(c3)
      for (c0 = 0; c0 <= 15; c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c3 = 0; c3 <= 15; c3++) {
          E[c0][c3] = 0;
        }
      }
    }
    
#pragma omp parallel for private(c5, c2)
{
      int c4;
      int c6;
      int c3;
      for (c3 = 0; c3 <= 15; c3++) {
        for (c4 = 0; c4 <= 15; c4++) {
          for (c6 = 0; c6 <= 15; c6++) {
            E[c3][c4] += A[c3][c6] * B[c6][c4];
            G[c3][c6] += E[c3][c4] * F[c4][c6];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
