#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE
/**
 * adi.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int tsteps = 2;
/* Variable declaration/allocation. */
  double X[16][16];
  double A[16][16];
  double B[16][16];
  int t;
  int i1;
  int i2;
  
#pragma scop
{
    int c0;
    int c2;
    int c8;
    int c15;
    for (c0 = 0; c0 <= 1; c0++) {
      
#pragma omp parallel for private(c15, c8)
      for (c2 = 0; c2 <= 1; c2++) {
        for (c8 = 1; c8 <= 15; c8++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c1;
            int c0;
            if (c2 <= 1) {
#pragma omp parallel for private(c1)
              for (c0 = ((13 * c2 + -10) * 11 < 0?-(-(13 * c2 + -10) / 11) : ((11 < 0?(-(13 * c2 + -10) + - 11 - 1) / - 11 : (13 * c2 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11))?1 : (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11)))); c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
                for (c1 = (11 * c0 > 13 * c2?11 * c0 : 13 * c2); c1 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c2 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c2 + 12)); c1++) {
                  B[c1][c8] = B[c1][c8] - A[c1][c8] * A[c1][c8] / B[c1][c8 - 1];
                }
              }
            }
          }
        }
        for (c8 = 1; c8 <= 15; c8++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c1;
            int c0;
            if (c2 <= 1) {
#pragma omp parallel for private(c1)
              for (c0 = ((13 * c2 + -10) * 11 < 0?-(-(13 * c2 + -10) / 11) : ((11 < 0?(-(13 * c2 + -10) + - 11 - 1) / - 11 : (13 * c2 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11))?1 : (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11)))); c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
                for (c1 = (11 * c0 > 13 * c2?11 * c0 : 13 * c2); c1 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c2 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c2 + 12)); c1++) {
                  X[c1][c8] = X[c1][c8] - X[c1][c8 - 1] * A[c1][c8] / B[c1][c8 - 1];
                }
              }
            }
          }
        }
        for (c8 = 0; c8 <= 13; c8++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c0;
            int c1;
            if (c2 <= 1) {
#pragma omp parallel for private(c1)
              for (c0 = ((13 * c2 + -10) * 11 < 0?-(-(13 * c2 + -10) / 11) : ((11 < 0?(-(13 * c2 + -10) + - 11 - 1) / - 11 : (13 * c2 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11))?1 : (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11)))); c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
                for (c1 = (11 * c0 > 13 * c2?11 * c0 : 13 * c2); c1 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c2 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c2 + 12)); c1++) {
                  X[c1][16 - c8 - 2] = (X[c1][16 - 2 - c8] - X[c1][16 - 2 - c8 - 1] * A[c1][16 - c8 - 3]) / B[c1][16 - 3 - c8];
                }
              }
            }
          }
        }
      }
      
#pragma omp parallel for private(c15)
      for (c2 = 0; c2 <= 1; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c1;
          int c0;
          if (c2 <= 1) {
#pragma omp parallel for private(c1)
            for (c0 = ((13 * c2 + -10) * 11 < 0?-(-(13 * c2 + -10) / 11) : ((11 < 0?(-(13 * c2 + -10) + - 11 - 1) / - 11 : (13 * c2 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11))?1 : (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11)))); c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
              for (c1 = (11 * c0 > 13 * c2?11 * c0 : 13 * c2); c1 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c2 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c2 + 12)); c1++) {
                X[c1][16 - 1] = X[c1][16 - 1] / B[c1][16 - 1];
              }
            }
          }
        }
      }
      
#pragma omp parallel for private(c15, c8)
      for (c2 = 0; c2 <= 1; c2++) {
        for (c8 = 1; c8 <= 15; c8++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c1;
            int c0;
            if (c2 <= 1) {
#pragma omp parallel for private(c1)
              for (c0 = ((13 * c2 + -10) * 11 < 0?-(-(13 * c2 + -10) / 11) : ((11 < 0?(-(13 * c2 + -10) + - 11 - 1) / - 11 : (13 * c2 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11))?1 : (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11)))); c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
                for (c1 = (11 * c0 > 13 * c2?11 * c0 : 13 * c2); c1 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c2 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c2 + 12)); c1++) {
                  B[c8][c1] = B[c8][c1] - A[c8][c1] * A[c8][c1] / B[c8 - 1][c1];
                }
              }
            }
          }
        }
        for (c8 = 1; c8 <= 15; c8++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c1;
            int c0;
            if (c2 <= 1) {
#pragma omp parallel for private(c1)
              for (c0 = ((13 * c2 + -10) * 11 < 0?-(-(13 * c2 + -10) / 11) : ((11 < 0?(-(13 * c2 + -10) + - 11 - 1) / - 11 : (13 * c2 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11))?1 : (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11)))); c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
                for (c1 = (11 * c0 > 13 * c2?11 * c0 : 13 * c2); c1 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c2 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c2 + 12)); c1++) {
                  X[c8][c1] = X[c8][c1] - X[c8 - 1][c1] * A[c8][c1] / B[c8 - 1][c1];
                }
              }
            }
          }
        }
        for (c8 = 0; c8 <= 13; c8++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c0;
            int c1;
            if (c2 <= 1) {
#pragma omp parallel for private(c1)
              for (c0 = ((13 * c2 + -10) * 11 < 0?-(-(13 * c2 + -10) / 11) : ((11 < 0?(-(13 * c2 + -10) + - 11 - 1) / - 11 : (13 * c2 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11))?1 : (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11)))); c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
                for (c1 = (11 * c0 > 13 * c2?11 * c0 : 13 * c2); c1 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c2 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c2 + 12)); c1++) {
                  X[16 - 2 - c8][c1] = (X[16 - 2 - c8][c1] - X[16 - c8 - 3][c1] * A[16 - 3 - c8][c1]) / B[16 - 2 - c8][c1];
                }
              }
            }
          }
        }
      }
      
#pragma omp parallel for private(c15)
      for (c2 = 0; c2 <= 1; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c1;
          int c0;
          if (c2 <= 1) {
#pragma omp parallel for private(c1)
            for (c0 = ((13 * c2 + -10) * 11 < 0?-(-(13 * c2 + -10) / 11) : ((11 < 0?(-(13 * c2 + -10) + - 11 - 1) / - 11 : (13 * c2 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11))?1 : (((13 * c2 + 12) * 11 < 0?((11 < 0?-((-(13 * c2 + 12) + 11 + 1) / 11) : -((-(13 * c2 + 12) + 11 - 1) / 11))) : (13 * c2 + 12) / 11)))); c0++) {
#pragma ivdep
#pragma vector always
#pragma simd
              for (c1 = (11 * c0 > 13 * c2?11 * c0 : 13 * c2); c1 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c2 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c2 + 12)); c1++) {
                X[16 - 1][c1] = X[16 - 1][c1] / B[16 - 1][c1];
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
