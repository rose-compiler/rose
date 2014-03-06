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
{
      int c6;
      int c2;
      int c8;
      for (c2 = 0; c2 <= 15; c2++) {
        for (c6 = 0; c6 <= 1; c6++) {
          acc = 0;
          C[c2][c6] = beta * C[c2][c6] + alpha * A[c2][c2] * B[c2][c6] + alpha * acc;
        }
        for (c6 = 2; c6 <= 15; c6++) {
          acc = 0;
          for (c8 = 0; c8 <= c6 + -2; c8++) {
            C[c8][c6] += alpha * A[c8][c2] * B[c2][c6];
            acc += B[c8][c6] * A[c8][c2];
          }
          C[c2][c6] = beta * C[c2][c6] + alpha * A[c2][c2] * B[c2][c6] + alpha * acc;
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
