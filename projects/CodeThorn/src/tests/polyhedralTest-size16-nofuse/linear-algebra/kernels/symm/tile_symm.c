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
    int c0;
    int c2;
{
      int c3;
      int c5;
      int c4;
      for (c3 = 0; c3 <= 15; c3++) {
        acc = 0;
        C[c3][0] = beta * C[c3][0] + alpha * A[c3][c3] * B[c3][0] + alpha * acc;
        C[0][2] += alpha * A[0][c3] * B[c3][2];
        acc = 0;
        C[c3][1] = beta * C[c3][1] + alpha * A[c3][c3] * B[c3][1] + alpha * acc;
        acc = 0;
        acc += B[0][2] * A[0][c3];
        C[c3][2] = beta * C[c3][2] + alpha * A[c3][c3] * B[c3][2] + alpha * acc;
        for (c4 = 3; c4 <= 15; c4++) {
          C[1][c4] += alpha * A[1][c3] * B[c3][c4];
          C[0][c4] += alpha * A[0][c3] * B[c3][c4];
          acc = 0;
          acc += B[0][c4] * A[0][c3];
          acc += B[1][c4] * A[1][c3];
          for (c5 = 3; c5 <= c4 + -1; c5++) {
            C[c5 + -1][c4] += alpha * A[c5 + -1][c3] * B[c3][c4];
            acc += B[c5 + -1][c4] * A[c5 + -1][c3];
          }
          C[c3][c4] = beta * C[c3][c4] + alpha * A[c3][c3] * B[c3][c4] + alpha * acc;
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
