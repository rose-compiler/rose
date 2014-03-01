#   define NQ ARRAYSIZE
#   define NR ARRAYSIZE
#   define NP ARRAYSIZE
# define _PB_NQ ARRAYSIZE
# define _PB_NR ARRAYSIZE
# define _PB_NP ARRAYSIZE
/**
 * doitgen.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nr = 16;
  int nq = 16;
  int np = 16;
  double A[16][16][16];
  double sum[16][16][16];
  double C4[16][16];
  int r;
  int q;
  int p;
  int s;
  
#pragma scop
{
    int c4;
    int c1;
    int c3;
    int c2;
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          sum[c1][c2][c3] = 0;
        }
      }
    }
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c4 = 0; c4 <= 15; c4++) {
            sum[c1][c2][c3] = sum[c1][c2][c3] + A[c1][c2][c4] * C4[c4][c3];
          }
        }
      }
    }
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          A[c1][c2][c3] = sum[c1][c2][c3];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
