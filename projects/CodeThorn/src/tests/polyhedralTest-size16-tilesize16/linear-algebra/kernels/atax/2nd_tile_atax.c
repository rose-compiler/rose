#   define NX ARRAYSIZE
#   define NY ARRAYSIZE
# define _PB_NX ARRAYSIZE
# define _PB_NY ARRAYSIZE
/**
 * atax.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double x[16];
  double y[16];
  double tmp[16];
  int i;
  int j;
  
#pragma scop
{
    int c2;
    int c4;
    
#pragma ivdep
    
#pragma vector always
    
#pragma simd
{
      int c1;
#pragma ivdep
#pragma vector always
#pragma simd
      for (c1 = 0; c1 <= 15; c1++) {
        tmp[c1] = 0;
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
          tmp[c1] = tmp[c1] + A[c1][c2] * x[c2];
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
        y[c1] = 0;
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
          y[c1] = y[c1] + A[c2][c1] * tmp[c2];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
