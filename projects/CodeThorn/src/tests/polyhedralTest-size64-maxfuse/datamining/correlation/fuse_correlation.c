/**
 * correlation.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#   define N ARRAYSIZE
#   define M ARRAYSIZE
# define _PB_N ARRAYSIZE
# define _PB_M ARRAYSIZE

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16;
  int m = 16;
  double data[16][16];
  double symmat[16][16];
  double mean[16];
  double stddev[16];
  int i;
  int j;
  int j1;
  int j2;
  double float_n = 1.2;
  double eps = 0.1f;
#define sqrt_of_array_cell(x,j) sqrt(x[j])
  
#pragma scop
{
    int c0;
    int c3;
    for (c0 = 0; c0 <= 15; c0++) {
      mean[c0] = 0.0;
      for (c3 = 0; c3 <= 15; c3++) {
        mean[c0] += data[c3][c0];
      }
      mean[c0] /= float_n;
    }
  }
/* Determine standard deviations of column vectors of data matrix. */
  for (j = 0; j < 16; j++) {{
      int c2;
      stddev[j] = 0.0;
      for (c2 = 0; c2 <= 15; c2++) {
        stddev[j] += (data[c2][j] - mean[j]) * (data[c2][j] - mean[j]);
      }
      stddev[j] /= float_n;
    }
    stddev[j] = sqrt(stddev[j]);
{
/* The following in an inelegant but usual way to handle
	 near-zero std. dev. values, which below would cause a zero-
	 divide. */
      stddev[j] = (stddev[j] <= eps?1.0 : stddev[j]);
    }
  }
/* Center and reduce the column vectors. */
  for (i = 0; i < 16; i++) 
    for (j = 0; j < 16; j++) {
      data[i][j] -= mean[j];
      data[i][j] /= sqrt(float_n) * stddev[j];
    }
{
    int c0;
    int c2;
    int c1;
    symmat[16 - 1][16 - 1] = 1.0;
    for (c0 = 0; c0 <= 14; c0++) {
      symmat[c0][c0] = 1.0;
      for (c1 = c0 + 1; c1 <= 15; c1++) {
        symmat[c0][c1] = 0.0;
        for (c2 = 0; c2 <= 15; c2++) {
          symmat[c0][c1] += data[c2][c0] * data[c2][c1];
        }
        symmat[c1][c0] = symmat[c0][c1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
