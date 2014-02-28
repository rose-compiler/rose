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
    int c1;
    int c2;
    
#pragma omp parallel for
{
      int c0;
#pragma omp parallel for
      for (c0 = 0; c0 <= 15; c0++) {
        mean[c0] = 0.0;
      }
    }
    
#pragma omp parallel for private(c2)
{
      int c0;
      int c3;
#pragma omp parallel for private(c3)
      for (c0 = 0; c0 <= 15; c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c3 = 0; c3 <= 15; c3++) {
          mean[c0] += data[c3][c0];
        }
      }
    }
    
#pragma omp parallel for
{
      int c0;
#pragma omp parallel for
      for (c0 = 0; c0 <= 15; c0++) {
        mean[c0] /= float_n;
      }
    }
  }
/* Determine standard deviations of column vectors of data matrix. */
  for (j = 0; j < 16; j++) {{
      int c4;
{
        int c5;
        stddev[j] = 0.0;
        for (c5 = 0; c5 <= 15; c5++) {
          stddev[j] += (data[c5][j] - mean[j]) * (data[c5][j] - mean[j]);
        }
        stddev[j] /= float_n;
      }
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
    int c3;
    int c1;
    int c2;
{
      symmat[16 - 1][16 - 1] = 1.0;
    }
    
#pragma omp parallel for private(c2)
{
      int c0;
      int c3;
#pragma omp parallel for private(c3)
      for (c0 = 0; c0 <= 14; c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c3 = c0 + 1; c3 <= 15; c3++) {
          symmat[c0][c3] = 0.0;
        }
      }
    }
    
#pragma omp parallel for private(c2, c3)
{
      int c5;
      int c4;
      int c0;
#pragma omp parallel for private(c4, c5)
      for (c0 = 0; c0 <= 14; c0++) {
        for (c4 = c0 + 1; c4 <= 15; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c5 = 0; c5 <= 15; c5++) {
            symmat[c0][c4] += data[c5][c0] * data[c5][c4];
          }
        }
      }
    }
    
#pragma omp parallel for private(c2)
{
      int c3;
      int c0;
#pragma omp parallel for private(c3)
      for (c0 = 0; c0 <= 14; c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c3 = c0 + 1; c3 <= 15; c3++) {
          symmat[c3][c0] = symmat[c0][c3];
        }
      }
    }
    
#pragma omp parallel for
{
      int c0;
#pragma omp parallel for
      for (c0 = 0; c0 <= 14; c0++) {
        symmat[c0][c0] = 1.0;
      }
    }
  }
  
#pragma endscop
  return 0;
}
