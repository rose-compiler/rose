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
{
        int c3;
        nrm = 0;
        for (c3 = 0; c3 <= 15; c3++) {
          nrm += A[c3][k] * A[c3][k];
        }
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c1;
      int c4;
      if (k >= 0) {
        
#pragma omp parallel for
{
          int c3;
          for (c3 = 0; c3 <= ((15 < k?15 : k)); c3++) {
            Q[c3][k] = A[c3][k] / R[k][k];
          }
          for (c3 = (0 > k + 1?0 : k + 1); c3 <= 15; c3++) {
            Q[c3][k] = A[c3][k] / R[k][k];
            R[k][c3] = 0;
          }
        }
      }
      if (k <= - 1) {
        
#pragma omp parallel for
{
          int c3;
          if (k <= 14) {
            for (c3 = k + 1; c3 <= -1; c3++) {
              R[k][c3] = 0;
            }
            for (c3 = (0 > k + 1?0 : k + 1); c3 <= 15; c3++) {
              Q[c3][k] = A[c3][k] / R[k][k];
              R[k][c3] = 0;
            }
          }
        }
      }
      
#pragma omp parallel for private(c4)
{
        int c3;
        int c7;
        if (k <= 14) {
          for (c3 = 0; c3 <= 15; c3++) {
            for (c7 = k + 1; c7 <= 15; c7++) {
              R[k][c7] += Q[c3][k] * A[c3][c7];
            }
            for (c7 = k + 1; c7 <= 15; c7++) {
              A[c3][c7] = A[c3][c7] - Q[c3][k] * R[k][c7];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
