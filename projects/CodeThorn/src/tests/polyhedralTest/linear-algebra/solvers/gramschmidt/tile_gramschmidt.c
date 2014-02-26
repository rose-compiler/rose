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
{
        int c1;
        nrm = 0;
        for (c1 = 0; c1 <= 31; c1++) {
          nrm += A[c1][k] * A[c1][k];
        }
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c5;
      int c2;
{
        int c0;
        int c1;
        for (c0 = 0; c0 <= ((31 < k?31 : k)); c0++) {
          Q[c0][k] = A[c0][k] / R[k][k];
        }
        for (c0 = k + 1; c0 <= -1; c0++) {
          R[k][c0] = 0;
          for (c1 = 0; c1 <= 31; c1++) {
            R[k][c0] += Q[c1][k] * A[c1][c0];
            A[c1][c0] = A[c1][c0] - Q[c1][k] * R[k][c0];
          }
        }
        for (c0 = (0 > k + 1?0 : k + 1); c0 <= 31; c0++) {
          Q[c0][k] = A[c0][k] / R[k][k];
          R[k][c0] = 0;
          for (c1 = 0; c1 <= 31; c1++) {
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
