#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * cholesky.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Variable declaration/allocation. */
  double A[16][16];
  double p[16];
  int i;
  int j;
  int k;
  double x;
  
#pragma scop
  for (i = 0; i < 16; ++i) {{
      int c2;
{
        int c3;
        x = A[i][i];
        for (c3 = 0; c3 <= i + -1; c3++) {
          x = x - A[i][c3] * A[i][c3];
        }
      }
    }
    p[i] = 1.0 / sqrt(x);
{
      int c0;
      int c1;
      int c2;
{
        int c6;
        int c5;
        int c8;
        int c4;
        if (i <= 14) {
          if (i <= 0) {
            for (c4 = ((i + -1022) * 1024 < 0?-(-(i + -1022) / 1024) : ((1024 < 0?(-(i + -1022) + - 1024 - 1) / - 1024 : (i + -1022 + 1024 - 1) / 1024))); c4 <= 0; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
              for (c5 = (1024 * c4 > i + 1?1024 * c4 : i + 1); c5 <= ((1 < 1024 * c4 + 1023?1 : 1024 * c4 + 1023)); c5++) {
                c8 = (c5 * 32 < 0?((32 < 0?-((-c5 + 32 + 1) / 32) : -((-c5 + 32 - 1) / 32))) : c5 / 32);
                A[c5][i] = x * p[i];
                x = A[i][c5];
              }
            }
          }
          if (i <= 0) {
            x = A[i][2];
            A[2][i] = x * p[i];
#pragma ivdep
#pragma vector always
#pragma simd
            for (c5 = 3; c5 <= 15; c5++) {
              x = A[i][c5];
              A[c5][i] = x * p[i];
            }
          }
          if (i >= 1) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c5 = i + 1; c5 <= 15; c5++) {
              x = A[i][c5];
              for (c6 = 2; c6 <= i + 1; c6++) {
                x = x - A[c5][c6 + - 2] * A[i][c6 + - 2];
              }
              A[c5][i] = x * p[i];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
