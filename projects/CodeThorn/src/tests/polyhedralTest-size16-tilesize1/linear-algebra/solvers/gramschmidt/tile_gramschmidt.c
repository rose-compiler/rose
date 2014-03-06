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
      int c1;
{
        int c2;
        nrm = 0;
        for (c2 = 0; c2 <= 15; c2++) {
          nrm += A[c2][k] * A[c2][k];
        }
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c1;
      int c3;
{
        int c2;
        int c5;
        for (c2 = 0; c2 <= 15; c2++) {
          Q[c2][k] = A[c2][k] / R[k][k];
        }
        for (c2 = k + 1; c2 <= 15; c2++) {
          R[k][c2] = 0;
          for (c5 = 0; c5 <= 15; c5++) {
            R[k][c2] += Q[c5][k] * A[c5][c2];
          }
          for (c5 = 0; c5 <= 15; c5++) {
            A[c5][c2] = A[c5][c2] - Q[c5][k] * R[k][c2];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
