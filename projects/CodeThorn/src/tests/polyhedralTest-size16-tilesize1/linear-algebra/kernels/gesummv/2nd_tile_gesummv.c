#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * gesummv.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double alpha;
  double beta;
  double A[16][16];
  double B[16][16];
  double tmp[16];
  double x[16];
  double y[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c2;
    
#pragma omp parallel for
{
      int c0;
#pragma omp parallel for
      for (c0 = 0; c0 <= 15; c0++) {
        y[c0] = 0;
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
          y[c0] = B[c0][c3] * x[c3] + y[c0];
        }
      }
    }
    
#pragma omp parallel for
{
      int c0;
#pragma omp parallel for
      for (c0 = 0; c0 <= 15; c0++) {
        tmp[c0] = 0;
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
          tmp[c0] = A[c0][c3] * x[c3] + tmp[c0];
        }
      }
    }
    
#pragma omp parallel for
{
      int c0;
#pragma omp parallel for
      for (c0 = 0; c0 <= 15; c0++) {
        y[c0] = alpha * tmp[c0] + beta * y[c0];
      }
    }
  }
  
#pragma endscop
  return 0;
}
