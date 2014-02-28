#   define N ARRAYSIZE
#   define M ARRAYSIZE
# define _PB_N ARRAYSIZE
# define _PB_M ARRAYSIZE
/**
 * covariance.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int n = 16;
  int m = 16;
  double float_n = 1.2;
  double data[m][n];
  double symmat[m][n];
  double mean[m];
  int i;
  int j;
  int j1;
  int j2;
  
#pragma scop
{
    int c3;
    int c2;
    int c6;
    for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c6 = 0; c6 <= c2; c6++) {
        symmat[c6][c2] = 0.0;
      }
    }
#pragma ivdep
#pragma vector always
#pragma simd
    for (c6 = 0; c6 <= 15; c6++) {
      mean[c6] = 0.0;
    }
    for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c6 = 0; c6 <= 15; c6++) {
        mean[c6] += data[c2][c6];
      }
    }
#pragma ivdep
#pragma vector always
#pragma simd
    for (c6 = 0; c6 <= 15; c6++) {
      mean[c6] /= float_n;
    }
    for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c6 = 0; c6 <= 15; c6++) {
        data[c6][c2] -= mean[c2];
      }
    }
    for (c2 = 0; c2 <= 15; c2++) {
      for (c3 = 0; c3 <= 15; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c6 = 0; c6 <= c2; c6++) {
          symmat[c6][c2] += data[c3][c6] * data[c3][c2];
        }
      }
    }
    for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c6 = 0; c6 <= c2; c6++) {
        symmat[c2][c6] = symmat[c6][c2];
      }
    }
  }
  
#pragma endscop
  return 0;
}
