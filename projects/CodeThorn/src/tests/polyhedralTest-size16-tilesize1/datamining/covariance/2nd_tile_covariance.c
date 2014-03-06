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
    int c3;
    int c2;
    
#pragma omp parallel for private(c2)
{
      int c0;
      int c3;
#pragma omp parallel for private(c3)
      for (c0 = 0; c0 <= 15; c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c3 = c0; c3 <= 15; c3++) {
          symmat[c0][c3] = 0.0;
        }
      }
    }
    
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
      int c3;
      int c0;
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
          data[c0][c3] -= mean[c3];
        }
      }
    }
    
#pragma omp parallel for private(c2, c3)
{
      int c4;
      int c0;
      int c5;
#pragma omp parallel for private(c5, c4)
      for (c0 = 0; c0 <= 15; c0++) {
        for (c4 = c0; c4 <= 15; c4++) {
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
      for (c0 = 0; c0 <= 15; c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c3 = c0; c3 <= 15; c3++) {
          symmat[c3][c0] = symmat[c0][c3];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
