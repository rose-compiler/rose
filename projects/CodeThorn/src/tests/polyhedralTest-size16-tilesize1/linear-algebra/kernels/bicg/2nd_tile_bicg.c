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
  int nx = 16;
  int ny = 16;
  double A[16][16];
  double s[16];
  double q[16];
  double p[16];
  double r[16];
  int i;
  int j;
  
#pragma scop
{
    int c2;
    int c1;
    
#pragma omp parallel for
{
      int c0;
#pragma omp parallel for
      for (c0 = 0; c0 <= 15; c0++) {
        q[c0] = 0;
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
          q[c0] = q[c0] + A[c0][c3] * p[c3];
        }
      }
    }
    
#pragma omp parallel for
{
      int c0;
#pragma omp parallel for
      for (c0 = 0; c0 <= 15; c0++) {
        s[c0] = 0;
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
          s[c0] = s[c0] + r[c3] * A[c3][c0];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
