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
    int c3;
    int c6;
    int c2;
    int c1;
    
#pragma omp parallel for private(c2, c6)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 13 * c1; c2 <= 15; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c0;
          int c3;
          if (13 * c1 <= c2) {
#pragma omp parallel for private(c3)
            for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((((c2 * 11 < 0?((11 < 0?-((-c2 + 11 + 1) / 11) : -((-c2 + 11 - 1) / 11))) : c2 / 11)) < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?((c2 * 11 < 0?((11 < 0?-((-c2 + 11 + 1) / 11) : -((-c2 + 11 - 1) / 11))) : c2 / 11)) : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
              for (c3 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c3 <= ((((c2 < 11 * c0 + 10?c2 : 11 * c0 + 10)) < 13 * c1 + 12?((c2 < 11 * c0 + 10?c2 : 11 * c0 + 10)) : 13 * c1 + 12)); c3++) {
                symmat[c3][c2] = 0.0;
              }
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c6)
    for (c1 = 0; c1 <= 1; c1++) {
      
#pragma ivdep
      
#pragma vector always
      
#pragma simd
{
        int c2;
        int c0;
        if (c1 <= 1) {
#pragma omp parallel for private(c2)
          for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?1 : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
            for (c2 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c2 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c1 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c1 + 12)); c2++) {
              mean[c2] = 0.0;
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c6)
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
                mean[c3] += data[c2][c3];
              }
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c6)
    for (c1 = 0; c1 <= 1; c1++) {
      
#pragma ivdep
      
#pragma vector always
      
#pragma simd
{
        int c2;
        int c0;
        if (c1 <= 1) {
#pragma omp parallel for private(c2)
          for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((1 < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?1 : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
            for (c2 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c2 <= ((((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) < 13 * c1 + 12?((15 < 11 * c0 + 10?15 : 11 * c0 + 10)) : 13 * c1 + 12)); c2++) {
              mean[c2] /= float_n;
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c6)
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
                data[c3][c2] -= mean[c2];
              }
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c6, c3)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 13 * c1; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c4;
            int c0;
            if (13 * c1 <= c2) {
#pragma omp parallel for private(c4)
              for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((((c2 * 11 < 0?((11 < 0?-((-c2 + 11 + 1) / 11) : -((-c2 + 11 - 1) / 11))) : c2 / 11)) < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?((c2 * 11 < 0?((11 < 0?-((-c2 + 11 + 1) / 11) : -((-c2 + 11 - 1) / 11))) : c2 / 11)) : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
                for (c4 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c4 <= ((((c2 < 11 * c0 + 10?c2 : 11 * c0 + 10)) < 13 * c1 + 12?((c2 < 11 * c0 + 10?c2 : 11 * c0 + 10)) : 13 * c1 + 12)); c4++) {
                  symmat[c4][c2] += data[c3][c4] * data[c3][c2];
                }
              }
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c6)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 13 * c1; c2 <= 15; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c3;
          int c0;
          if (13 * c1 <= c2) {
#pragma omp parallel for private(c3)
            for (c0 = ((13 * c1 + -10) * 11 < 0?-(-(13 * c1 + -10) / 11) : ((11 < 0?(-(13 * c1 + -10) + - 11 - 1) / - 11 : (13 * c1 + -10 + 11 - 1) / 11))); c0 <= ((((c2 * 11 < 0?((11 < 0?-((-c2 + 11 + 1) / 11) : -((-c2 + 11 - 1) / 11))) : c2 / 11)) < (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11))?((c2 * 11 < 0?((11 < 0?-((-c2 + 11 + 1) / 11) : -((-c2 + 11 - 1) / 11))) : c2 / 11)) : (((13 * c1 + 12) * 11 < 0?((11 < 0?-((-(13 * c1 + 12) + 11 + 1) / 11) : -((-(13 * c1 + 12) + 11 - 1) / 11))) : (13 * c1 + 12) / 11)))); c0++) {
              for (c3 = (11 * c0 > 13 * c1?11 * c0 : 13 * c1); c3 <= ((((c2 < 11 * c0 + 10?c2 : 11 * c0 + 10)) < 13 * c1 + 12?((c2 < 11 * c0 + 10?c2 : 11 * c0 + 10)) : 13 * c1 + 12)); c3++) {
                symmat[c2][c3] = symmat[c3][c2];
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
