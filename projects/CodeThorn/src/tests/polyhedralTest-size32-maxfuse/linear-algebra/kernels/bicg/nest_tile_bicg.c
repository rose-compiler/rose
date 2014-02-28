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
    int c3;
    int c4;
    int c1;
    int c2;
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 15; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 2 * c1; c4 <= 2 * c1 + 1; c4++) {
        q[c4] = 0;
      }
    }
#pragma omp parallel for private(c2, c4, c3)
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 7; c2++) {
        for (c3 = 4 * c2; c3 <= 4 * c2 + 3; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c4 = 2 * c1; c4 <= 2 * c1 + 1; c4++) {
            q[c4] = q[c4] + A[c4][c3] * p[c3];
          }
        }
      }
    }
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 15; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c4 = 2 * c1; c4 <= 2 * c1 + 1; c4++) {
        s[c4] = 0;
      }
    }
#pragma omp parallel for private(c2, c4, c3)
    for (c1 = 0; c1 <= 15; c1++) {
      for (c2 = 0; c2 <= 7; c2++) {
        for (c3 = 4 * c2; c3 <= 4 * c2 + 3; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c4 = 2 * c1; c4 <= 2 * c1 + 1; c4++) {
            s[c4] = s[c4] + r[c3] * A[c3][c4];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
