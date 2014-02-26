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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "bicg.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int nx = 32;
  int ny = 32;
  double A[32][32];
  double s[32];
  double q[32];
  double p[32];
  double r[32];
  int i;
  int j;
  
#pragma scop
{
    int c4;
    int c2;
    int c1;
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 3; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
        q[c4] = 0;
      }
    }
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 31; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
          q[c4] = q[c4] + A[c4][c2] * p[c2];
        }
      }
    }
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 3; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
        s[c4] = 0;
      }
    }
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 31; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
          s[c4] = s[c4] + r[c2] * A[c2][c4];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
