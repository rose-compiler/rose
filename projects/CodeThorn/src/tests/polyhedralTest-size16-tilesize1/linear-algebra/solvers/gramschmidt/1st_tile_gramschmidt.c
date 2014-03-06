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
  int ni = 16;
  int nj = 16;
/* Variable declaration/allocation. */
  double A[16][16];
  double R[16][16];
  double Q[16][16];
  int i;
  int j;
  int k;
  double nrm;
  
#pragma scop
  for (k = 0; k < 16; k++) {{
      int c2;
      nrm = 0;
      for (c2 = 0; c2 <= 15; c2++) {
        nrm += A[c2][k] * A[c2][k];
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c1;
      int c4;
      if (k >= 0) {
#pragma omp parallel for
        for (c1 = 0; c1 <= 15; c1++) {
          Q[c1][k] = A[c1][k] / R[k][k];
          if (c1 >= k + 1) {
            R[k][c1] = 0;
          }
        }
      }
      if (k <= -1) {
#pragma omp parallel for
        for (c1 = k + 1; c1 <= 15; c1++) {
          if (c1 >= 0) {
            Q[c1][k] = A[c1][k] / R[k][k];
          }
          R[k][c1] = 0;
        }
      }
#pragma omp parallel for private(c4)
      for (c1 = k + 1; c1 <= 15; c1++) {
        for (c4 = 0; c4 <= 15; c4++) {
          R[k][c1] += Q[c4][k] * A[c4][c1];
        }
        for (c4 = 0; c4 <= 15; c4++) {
          A[c4][c1] = A[c4][c1] - Q[c4][k] * R[k][c1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
