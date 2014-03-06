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
{
      int c8;
      int c5;
      int c6;
      int c7;
      for (c5 = 0; c5 <= 15; c5++) {
        for (c6 = 0; c6 <= 15; c6++) {
          for (c7 = 0; c7 <= 15; c7++) {
            sum[c5][c6][c7] = 0;
          }
        }
      }
      for (c5 = 0; c5 <= 15; c5++) {
        for (c6 = 0; c6 <= 15; c6++) {
          for (c7 = 0; c7 <= 15; c7++) {
            for (c8 = 0; c8 <= 15; c8++) {
              sum[c5][c6][c7] = sum[c5][c6][c7] + A[c5][c6][c8] * C4[c8][c7];
            }
          }
        }
      }
      for (c5 = 0; c5 <= 15; c5++) {
        for (c6 = 0; c6 <= 15; c6++) {
          for (c7 = 0; c7 <= 15; c7++) {
            A[c5][c6][c7] = sum[c5][c6][c7];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
