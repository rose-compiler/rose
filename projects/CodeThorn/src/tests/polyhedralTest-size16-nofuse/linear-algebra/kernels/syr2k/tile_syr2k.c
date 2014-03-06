#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
/**
 * syr2k.c: This file is part of the PolyBench/C 3.2 test suite.
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
  
#pragma scop
{
    int c1;
    int c3;
    int c2;
{
      int c5;
      int c6;
      int c4;
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = 0; c5 <= 15; c5++) {
          C[c4][c5] *= beta;
        }
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = 0; c5 <= 15; c5++) {
          for (c6 = 0; c6 <= 15; c6++) {
            C[c4][c5] += alpha * A[c4][c6] * B[c5][c6];
            C[c4][c5] += alpha * B[c4][c6] * A[c5][c6];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
