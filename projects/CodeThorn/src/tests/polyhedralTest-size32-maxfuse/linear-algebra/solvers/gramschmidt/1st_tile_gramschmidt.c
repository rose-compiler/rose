#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
/**
 * gramschmidt.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int ni = 32;
  int nj = 32;
/* Variable declaration/allocation. */
  double A[32][32];
  double R[32][32];
  double Q[32][32];
  int i;
  int j;
  int k;
  double nrm;
  
#pragma scop
  for (k = 0; k < 32; k++) {{
      int c2;
      nrm = 0;
      for (c2 = 0; c2 <= 31; c2++) {
        nrm += A[c2][k] * A[c2][k];
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c1;
      int c8;
      int c4;
      if (k >= 0) {
#pragma omp parallel for private(c8)
        for (c1 = 0; c1 <= 3; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c8 = 8 * c1; c8 <= ((k < 8 * c1 + 7?k : 8 * c1 + 7)); c8++) {
            Q[c8][k] = A[c8][k] / R[k][k];
          }
#pragma ivdep
#pragma vector always
#pragma simd
          for (c8 = (8 * c1 > k + 1?8 * c1 : k + 1); c8 <= 8 * c1 + 7; c8++) {
            Q[c8][k] = A[c8][k] / R[k][k];
            R[k][c8] = 0;
          }
        }
      }
      if (k <= -1) {
#pragma omp parallel for private(c8)
        for (c1 = ((k + -6) * 8 < 0?-(-(k + -6) / 8) : ((8 < 0?(-(k + -6) + - 8 - 1) / - 8 : (k + -6 + 8 - 1) / 8))); c1 <= 3; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c8 = (8 * c1 > k + 1?8 * c1 : k + 1); c8 <= ((-1 < 8 * c1 + 7?-1 : 8 * c1 + 7)); c8++) {
            R[k][c8] = 0;
          }
#pragma ivdep
#pragma vector always
#pragma simd
          for (c8 = (0 > 8 * c1?0 : 8 * c1); c8 <= 8 * c1 + 7; c8++) {
            Q[c8][k] = A[c8][k] / R[k][k];
            R[k][c8] = 0;
          }
        }
      }
#pragma omp parallel for private(c4, c8)
      for (c1 = ((k + -6) * 8 < 0?-(-(k + -6) / 8) : ((8 < 0?(-(k + -6) + - 8 - 1) / - 8 : (k + -6 + 8 - 1) / 8))); c1 <= 3; c1++) {
        for (c4 = 0; c4 <= 31; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c8 = (8 * c1 > k + 1?8 * c1 : k + 1); c8 <= 8 * c1 + 7; c8++) {
            R[k][c8] += Q[c4][k] * A[c4][c8];
          }
        }
        for (c4 = 0; c4 <= 31; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c8 = (8 * c1 > k + 1?8 * c1 : k + 1); c8 <= 8 * c1 + 7; c8++) {
            A[c4][c8] = A[c4][c8] - Q[c4][k] * R[k][c8];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
