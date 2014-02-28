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
    int c0;
    int c5;
    int c4;
    int c1;
    for (c0 = 0; c0 <= 15; c0++) {
      for (c1 = 0; c1 <= 15; c1++) {
        for (c4 = 0; c4 <= 15; c4++) {
          sum[c0][c1][c4] = 0;
        }
        for (c4 = 0; c4 <= 15; c4++) {
          for (c5 = 0; c5 <= 15; c5++) {
            sum[c0][c1][c4] = sum[c0][c1][c4] + A[c0][c1][c5] * C4[c5][c4];
          }
        }
        for (c4 = 0; c4 <= 15; c4++) {
          A[c0][c1][c4] = sum[c0][c1][c4];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
