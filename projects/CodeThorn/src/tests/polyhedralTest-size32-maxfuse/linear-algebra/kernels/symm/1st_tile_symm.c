#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
/**
 * symm.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double alpha;
  double beta;
  double C[32][32];
  double A[32][32];
  double B[32][32];
  int i;
  int j;
  int k;
  double acc;
  
#pragma scop
{
    int c2;
    int c0;
    int c1;
    for (c0 = 0; c0 <= 31; c0++) {
      for (c1 = 0; c1 <= 1; c1++) {
        acc = 0;
        C[c0][c1] = beta * C[c0][c1] + alpha * A[c0][c0] * B[c0][c1] + alpha * acc;
      }
      for (c1 = 2; c1 <= 31; c1++) {
        C[0][c1] += alpha * A[0][c0] * B[c0][c1];
        acc = 0;
        acc += B[0][c1] * A[0][c0];
        for (c2 = 1; c2 <= c1 + -2; c2++) {
          C[c2][c1] += alpha * A[c2][c0] * B[c0][c1];
          acc += B[c2][c1] * A[c2][c0];
        }
        C[c0][c1] = beta * C[c0][c1] + alpha * A[c0][c0] * B[c0][c1] + alpha * acc;
      }
    }
  }
  
#pragma endscop
  return 0;
}
