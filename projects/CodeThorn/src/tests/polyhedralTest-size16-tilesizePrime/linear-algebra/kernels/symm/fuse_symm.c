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
  int ni = 16;
  int nj = 16;
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double C[16][16];
  double A[16][16];
  double B[16][16];
  int i;
  int j;
  int k;
  double acc;
  
#pragma scop
{
    int c1;
    int c3;
    int c5;
    for (c1 = 0; c1 <= 15; c1++) {
      for (c3 = 0; c3 <= 1; c3++) {
        acc = 0;
        C[c1][c3] = beta * C[c1][c3] + alpha * A[c1][c1] * B[c1][c3] + alpha * acc;
      }
      for (c3 = 2; c3 <= 15; c3++) {
        acc = 0;
        for (c5 = 0; c5 <= c3 + -2; c5++) {
          C[c5][c3] += alpha * A[c5][c1] * B[c1][c3];
          acc += B[c5][c3] * A[c5][c1];
        }
        C[c1][c3] = beta * C[c1][c3] + alpha * A[c1][c1] * B[c1][c3] + alpha * acc;
      }
    }
  }
  
#pragma endscop
  return 0;
}
