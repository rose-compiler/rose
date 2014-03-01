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
    int c4;
    int c0;
    int c3;
{
      int c6;
      int c7;
      int c1;
      for (c1 = 0; c1 <= 15; c1++) {
        mean[c1] = 0.0;
        for (c6 = 0; c6 <= 15; c6++) {
          mean[c1] += data[c6][c1];
        }
        mean[c1] /= float_n;
        for (c6 = 0; c6 <= c1 + -1; c6++) {
          data[c6][c1] -= mean[c1];
        }
        for (c6 = c1; c6 <= 15; c6++) {
          symmat[c1][c6] = 0.0;
          data[c6][c1] -= mean[c1];
        }
        for (c6 = 0; c6 <= c1; c6++) {
          for (c7 = 0; c7 <= 15; c7++) {
            symmat[c6][c1] += data[c7][c6] * data[c7][c1];
          }
        }
        for (c6 = 0; c6 <= c1; c6++) {
          symmat[c1][c6] = symmat[c6][c1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
