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
      int c0;
{
        int c1;
        nrm = 0;
        for (c1 = 0; c1 <= 15; c1++) {
          nrm += A[c1][k] * A[c1][k];
        }
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c2;
      int c5;
{
        int c1;
        int c0;
        for (c0 = 0; c0 <= ((15 < k?15 : k)); c0++) {
          Q[c0][k] = A[c0][k] / R[k][k];
        }
        for (c0 = k + 1; c0 <= -1; c0++) {
          R[k][c0] = 0;
          for (c1 = 0; c1 <= 15; c1++) {
            R[k][c0] += Q[c1][k] * A[c1][c0];
            A[c1][c0] = A[c1][c0] - Q[c1][k] * R[k][c0];
          }
        }
        for (c0 = (0 > k + 1?0 : k + 1); c0 <= 15; c0++) {
          Q[c0][k] = A[c0][k] / R[k][k];
          R[k][c0] = 0;
          for (c1 = 0; c1 <= 15; c1++) {
            R[k][c0] += Q[c1][k] * A[c1][c0];
            A[c1][c0] = A[c1][c0] - Q[c1][k] * R[k][c0];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
