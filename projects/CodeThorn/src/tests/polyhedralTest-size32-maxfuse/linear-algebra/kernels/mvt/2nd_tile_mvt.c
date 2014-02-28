#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * mvt.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int n = 32;
/* Variable declaration/allocation. */
  double A[32][32];
  double x1[32];
  double x2[32];
  double y_1[32];
  double y_2[32];
  int i;
  int j;
  
#pragma scop
{
    int c2;
    int c1;
    int c4;
    
#pragma omp parallel for private(c4, c2)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 31; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c5;
          for (c5 = 8 * c1; c5 <= 8 * c1 + 7; c5++) {
            x1[c5] = x1[c5] + A[c5][c2] * y_1[c2];
            x2[c5] = x2[c5] + A[c2][c5] * y_2[c2];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
