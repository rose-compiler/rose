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
    int c4;
    int c2;
    
#pragma ivdep
    
#pragma vector always
    
#pragma simd
{
      int c1;
#pragma ivdep
#pragma vector always
#pragma simd
      for (c1 = 0; c1 <= 15; c1++) {
        mean[c1] = 0.0;
      }
    }
    for (c2 = 0; c2 <= 15; c2++) {
      
#pragma ivdep
      
#pragma vector always
      
#pragma simd
{
        int c1;
#pragma ivdep
#pragma vector always
#pragma simd
        for (c1 = 0; c1 <= 15; c1++) {
          mean[c1] += data[c2][c1];
        }
      }
    }
    
#pragma ivdep
    
#pragma vector always
    
#pragma simd
{
      int c1;
#pragma ivdep
#pragma vector always
#pragma simd
      for (c1 = 0; c1 <= 15; c1++) {
        mean[c1] /= float_n;
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
    int c4;
    int c2;
    int c3;
{
      int c6;
      int c7;
      int c5;
      for (c7 = 0; c7 <= 14; c7++) {
        symmat[c7][c7] = 1.0;
      }
      for (c5 = 0; c5 <= 14; c5++) {
        for (c7 = c5 + 1; c7 <= 15; c7++) {
          symmat[c5][c7] = 0.0;
        }
      }
      for (c5 = 0; c5 <= 14; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c6 = 0; c6 <= 15; c6++) {
          for (c7 = c5 + 1; c7 <= 15; c7++) {
            symmat[c5][c7] += data[c6][c5] * data[c6][c7];
          }
        }
      }
      for (c5 = 0; c5 <= 14; c5++) {
        for (c7 = c5 + 1; c7 <= 15; c7++) {
          symmat[c7][c5] = symmat[c5][c7];
        }
      }
      symmat[16 - 1][16 - 1] = 1.0;
    }
  }
  
#pragma endscop
  return 0;
}
