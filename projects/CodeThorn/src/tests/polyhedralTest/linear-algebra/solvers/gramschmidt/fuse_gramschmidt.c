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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 512. */
#include "gramschmidt.h"

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
      int c0;
      nrm = 0;
      for (c0 = 0; c0 <= 31; c0++) {
        nrm += A[c0][k] * A[c0][k];
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c5;
      int c2;
      for (c2 = 0; c2 <= ((31 < k?31 : k)); c2++) {
        Q[c2][k] = A[c2][k] / R[k][k];
      }
      for (c2 = k + 1; c2 <= -1; c2++) {
        R[k][c2] = 0;
      }
      for (c2 = (0 > k + 1?0 : k + 1); c2 <= 31; c2++) {
        Q[c2][k] = A[c2][k] / R[k][k];
        R[k][c2] = 0;
      }
      for (c2 = k + 1; c2 <= 31; c2++) {
        for (c5 = 0; c5 <= 31; c5++) {
          R[k][c2] += Q[c5][k] * A[c5][c2];
        }
        for (c5 = 0; c5 <= 31; c5++) {
          A[c5][c2] = A[c5][c2] - Q[c5][k] * R[k][c2];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
