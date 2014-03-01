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
    int c1;
{
      int c5;
      int c4;
      int c6;
      for (c4 = 0; c4 <= 15; c4++) {
        mean[c4] = 0.0;
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = 0; c5 <= 15; c5++) {
          mean[c4] += data[c5][c4];
        }
      }
      for (c4 = 0; c4 <= 15; c4++) {
        mean[c4] /= float_n;
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = 0; c5 <= 15; c5++) {
          data[c4][c5] -= mean[c5];
        }
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = c4; c5 <= 15; c5++) {
          symmat[c4][c5] = 0.0;
        }
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = c4; c5 <= 15; c5++) {
          for (c6 = 0; c6 <= 15; c6++) {
            symmat[c4][c5] += data[c6][c4] * data[c6][c5];
          }
        }
      }
      for (c4 = 0; c4 <= 15; c4++) {
        for (c5 = c4; c5 <= 15; c5++) {
          symmat[c5][c4] = symmat[c4][c5];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
