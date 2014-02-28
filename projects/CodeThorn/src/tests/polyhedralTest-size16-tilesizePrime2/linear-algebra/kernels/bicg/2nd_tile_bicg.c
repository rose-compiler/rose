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
    int c4;
    int c2;
    int c1;
    
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 1; c1++) {
      
#pragma ivdep
      
#pragma vector always
      
#pragma simd
{
        int c0;
        int c2;
        if (c1 <= 1) {
#pragma omp parallel for private(c2)
          for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?1 : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
            for (c2 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c2 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c1 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c1 + 12)); c2++) {
              q[c2] = 0;
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c0;
          int c3;
          if (c1 <= 1) {
#pragma omp parallel for private(c3)
            for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?1 : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
              for (c3 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c3 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c1 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c1 + 12)); c3++) {
                q[c3] = q[c3] + A[c3][c2] * p[c2];
              }
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c4)
    for (c1 = 0; c1 <= 1; c1++) {
      
#pragma ivdep
      
#pragma vector always
      
#pragma simd
{
        int c0;
        int c2;
        if (c1 <= 1) {
#pragma omp parallel for private(c2)
          for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?1 : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
            for (c2 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c2 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c1 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c1 + 12)); c2++) {
              s[c2] = 0;
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c4)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c3;
          int c0;
          if (c1 <= 1) {
#pragma omp parallel for private(c3)
            for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?1 : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
              for (c3 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c3 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c1 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c1 + 12)); c3++) {
                s[c3] = s[c3] + r[c2] * A[c2][c3];
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
