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
    int c1;
    int c5;
    int c3;
    for (c1 = 0; c1 <= 15; c1++) {
      mean[c1] = 0.0;
      for (c3 = 0; c3 <= 15; c3++) {
        mean[c1] += data[c3][c1];
      }
      mean[c1] /= float_n;
    }
    for (c1 = 0; c1 <= 15; c1++) {
      for (c3 = 0; c3 <= 15; c3++) {
        data[c1][c3] -= mean[c3];
      }
    }
    for (c1 = 0; c1 <= 15; c1++) {
      for (c3 = c1; c3 <= 15; c3++) {
        symmat[c1][c3] = 0.0;
        for (c5 = 0; c5 <= 15; c5++) {
          symmat[c1][c3] += data[c5][c1] * data[c5][c3];
        }
        symmat[c3][c1] = symmat[c1][c3];
      }
    }
  }
  
#pragma endscop
  return 0;
}
