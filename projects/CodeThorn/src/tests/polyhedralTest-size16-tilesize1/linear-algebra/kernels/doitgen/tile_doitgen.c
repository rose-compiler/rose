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
{
      int c9;
      int c8;
      int c3;
      int c2;
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c8 = 0; c8 <= 15; c8++) {
            sum[c2][c3][c8] = 0;
          }
          for (c8 = 0; c8 <= 15; c8++) {
            for (c9 = 0; c9 <= 15; c9++) {
              sum[c2][c3][c8] = sum[c2][c3][c8] + A[c2][c3][c9] * C4[c9][c8];
            }
          }
          for (c8 = 0; c8 <= 15; c8++) {
            A[c2][c3][c8] = sum[c2][c3][c8];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
