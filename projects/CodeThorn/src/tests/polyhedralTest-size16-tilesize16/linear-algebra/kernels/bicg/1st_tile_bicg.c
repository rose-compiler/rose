#   define NX ARRAYSIZE
#   define NY ARRAYSIZE
# define _PB_NX ARRAYSIZE
# define _PB_NY ARRAYSIZE
/**
 * bicg.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nx = 16;
  int ny = 16;
  double A[16][16];
  double s[16];
  double q[16];
  double p[16];
  double r[16];
  int i;
  int j;
  
#pragma scop
{
    int c4;
    int c2;
#pragma ivdep
#pragma vector always
#pragma simd
    for (c4 = 0; c4 <= 15; c4++) {
      q[c4] = 0;
    }
    for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 0; c4 <= 15; c4++) {
        q[c4] = q[c4] + A[c4][c2] * p[c2];
      }
    }
#pragma ivdep
#pragma vector always
#pragma simd
    for (c4 = 0; c4 <= 15; c4++) {
      s[c4] = 0;
    }
    for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 0; c4 <= 15; c4++) {
        s[c4] = s[c4] + r[c2] * A[c2][c4];
      }
    }
  }
  
#pragma endscop
  return 0;
}
