#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * gemver.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double u1[16];
  double v1[16];
  double u2[16];
  double v2[16];
  double w[16];
  double x[16];
  double y[16];
  double z[16];
  int i;
  int j;
  
#pragma scop
{
    int c4;
    int c2;
    int c1;
    
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 2; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c5;
          if (c1 <= 2) {
            for (c5 = 7 * c1; c5 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c5++) {
              A[c2][c5] = A[c2][c5] + u1[c2] * v1[c5] + u2[c2] * v2[c5];
              x[c5] = x[c5] + beta * A[c2][c5] * y[c2];
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 2; c1++) {
      
#pragma ivdep
      
#pragma vector always
      
#pragma simd
{
        int c0;
        int c2;
        if (c1 <= 2) {
#pragma omp parallel for private(c2)
          for (c0 = ((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5))); c0 <= ((3 < (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5))?3 : (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5)))); c0++) {
            for (c2 = (5 * c0 > 7 * c1?5 * c0 : 7 * c1); c2 <= ((((15 < 5 * c0 + 4?15 : 5 * c0 + 4)) < 7 * c1 + 6?((15 < 5 * c0 + 4?15 : 5 * c0 + 4)) : 7 * c1 + 6)); c2++) {
              x[c2] = x[c2] + z[c2];
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 2; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c3;
          int c0;
          if (c1 <= 2) {
#pragma omp parallel for private(c3)
            for (c0 = ((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5))); c0 <= ((3 < (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5))?3 : (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5)))); c0++) {
              for (c3 = (5 * c0 > 7 * c1?5 * c0 : 7 * c1); c3 <= ((((15 < 5 * c0 + 4?15 : 5 * c0 + 4)) < 7 * c1 + 6?((15 < 5 * c0 + 4?15 : 5 * c0 + 4)) : 7 * c1 + 6)); c3++) {
                w[c3] = w[c3] + alpha * A[c3][c2] * x[c2];
              }
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
