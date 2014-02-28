#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
/**
 * gramschmidt.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int ni = 16;
  int nj = 16;
/* Variable declaration/allocation. */
  double A[16][16];
  double R[16][16];
  double Q[16][16];
  int i;
  int j;
  int k;
  double nrm;
  
#pragma scop
  for (k = 0; k < 16; k++) {{
      int c2;
{
        int c3;
        nrm = 0;
        for (c3 = 0; c3 <= 15; c3++) {
          nrm += A[c3][k] * A[c3][k];
        }
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c4;
      int c1;
      int c8;
      if (k >= 0) {
        
#pragma omp parallel for private(c8)
        for (c1 = 0; c1 <= 2; c1++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c0;
            int c2;
            if (c1 <= 2 && 7 * c1 <= k) {
#pragma omp parallel for private(c2)
              for (c0 = ((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5))); c0 <= ((((3 < ((k * 5 < 0?((5 < 0?-((-k + 5 + 1) / 5) : -((-k + 5 - 1) / 5))) : k / 5))?3 : ((k * 5 < 0?((5 < 0?-((-k + 5 + 1) / 5) : -((-k + 5 - 1) / 5))) : k / 5)))) < (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5))?((3 < ((k * 5 < 0?((5 < 0?-((-k + 5 + 1) / 5) : -((-k + 5 - 1) / 5))) : k / 5))?3 : ((k * 5 < 0?((5 < 0?-((-k + 5 + 1) / 5) : -((-k + 5 - 1) / 5))) : k / 5)))) : (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5)))); c0++) {
                for (c2 = (5 * c0 > 7 * c1?5 * c0 : 7 * c1); c2 <= ((((((15 < k?15 : k)) < 5 * c0 + 4?((15 < k?15 : k)) : 5 * c0 + 4)) < 7 * c1 + 6?((((15 < k?15 : k)) < 5 * c0 + 4?((15 < k?15 : k)) : 5 * c0 + 4)) : 7 * c1 + 6)); c2++) {
                  Q[c2][k] = A[c2][k] / R[k][k];
                }
              }
            }
          }
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c3;
            if (c1 <= 2 && k <= 14 && k <= 7 * c1 + 5) {
              for (c3 = (7 * c1 > k + 1?7 * c1 : k + 1); c3 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c3++) {
                Q[c3][k] = A[c3][k] / R[k][k];
                R[k][c3] = 0;
              }
            }
          }
        }
      }
      if (k <= - 1) {
        
#pragma omp parallel for private(c8)
        for (c1 = ((k + - 5) * 7 < 0?-(-(k + - 5) / 7) : ((7 < 0?(-(k + - 5) + - 7 - 1) / - 7 : (k + - 5 + 7 - 1) / 7))); c1 <= 2; c1++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c2;
            int c0;
            if (c1 <= -1 && k <= 7 * c1 + 5) {
#pragma omp parallel for private(c2)
              for (c0 = ((((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5)))) > (((k + -3) * 5 < 0?-(-(k + -3) / 5) : ((5 < 0?(-(k + -3) + - 5 - 1) / - 5 : (k + -3 + 5 - 1) / 5))))?(((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5)))) : (((k + -3) * 5 < 0?-(-(k + -3) / 5) : ((5 < 0?(-(k + -3) + - 5 - 1) / - 5 : (k + -3 + 5 - 1) / 5))))); c0 <= (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5)); c0++) {
                for (c2 = (((5 * c0 > 7 * c1?5 * c0 : 7 * c1)) > k + 1?((5 * c0 > 7 * c1?5 * c0 : 7 * c1)) : k + 1); c2 <= ((5 * c0 + 4 < 7 * c1 + 6?5 * c0 + 4 : 7 * c1 + 6)); c2++) {
                  R[k][c2] = 0;
                }
              }
            }
          }
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c0;
            if (c1 >= 0 && c1 <= 2) {
              for (c0 = 7 * c1; c0 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c0++) {
                Q[c0][k] = A[c0][k] / R[k][k];
                R[k][c0] = 0;
              }
            }
          }
        }
      }
      if (k <= 14) {
        
#pragma omp parallel for private(c8, c4)
        for (c1 = ((k + - 5) * 7 < 0?-(-(k + - 5) / 7) : ((7 < 0?(-(k + - 5) + - 7 - 1) / - 7 : (k + - 5 + 7 - 1) / 7))); c1 <= 2; c1++) {
          for (c4 = 0; c4 <= 15; c4++) {
            
#pragma ivdep
            
#pragma vector always
            
#pragma simd
{
              int c0;
              int c2;
              if (c1 <= 2 && k <= 14 && k <= 7 * c1 + 5) {
#pragma omp parallel for private(c2)
                for (c0 = ((((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5)))) > (((k + -3) * 5 < 0?-(-(k + -3) / 5) : ((5 < 0?(-(k + -3) + - 5 - 1) / - 5 : (k + -3 + 5 - 1) / 5))))?(((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5)))) : (((k + -3) * 5 < 0?-(-(k + -3) / 5) : ((5 < 0?(-(k + -3) + - 5 - 1) / - 5 : (k + -3 + 5 - 1) / 5))))); c0 <= ((3 < (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5))?3 : (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5)))); c0++) {
                  for (c2 = (((5 * c0 > 7 * c1?5 * c0 : 7 * c1)) > k + 1?((5 * c0 > 7 * c1?5 * c0 : 7 * c1)) : k + 1); c2 <= ((((15 < 5 * c0 + 4?15 : 5 * c0 + 4)) < 7 * c1 + 6?((15 < 5 * c0 + 4?15 : 5 * c0 + 4)) : 7 * c1 + 6)); c2++) {
                    R[k][c2] += Q[c4][k] * A[c4][c2];
                  }
                }
              }
            }
          }
          for (c4 = 0; c4 <= 15; c4++) {
            
#pragma ivdep
            
#pragma vector always
            
#pragma simd
{
              int c0;
              int c2;
              if (c1 <= 2 && k <= 14 && k <= 7 * c1 + 5) {
#pragma omp parallel for private(c2)
                for (c0 = ((((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5)))) > (((k + -3) * 5 < 0?-(-(k + -3) / 5) : ((5 < 0?(-(k + -3) + - 5 - 1) / - 5 : (k + -3 + 5 - 1) / 5))))?(((7 * c1 + -4) * 5 < 0?-(-(7 * c1 + -4) / 5) : ((5 < 0?(-(7 * c1 + -4) + - 5 - 1) / - 5 : (7 * c1 + -4 + 5 - 1) / 5)))) : (((k + -3) * 5 < 0?-(-(k + -3) / 5) : ((5 < 0?(-(k + -3) + - 5 - 1) / - 5 : (k + -3 + 5 - 1) / 5))))); c0 <= ((3 < (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5))?3 : (((7 * c1 + 6) * 5 < 0?((5 < 0?-((-(7 * c1 + 6) + 5 + 1) / 5) : -((-(7 * c1 + 6) + 5 - 1) / 5))) : (7 * c1 + 6) / 5)))); c0++) {
                  for (c2 = (((5 * c0 > 7 * c1?5 * c0 : 7 * c1)) > k + 1?((5 * c0 > 7 * c1?5 * c0 : 7 * c1)) : k + 1); c2 <= ((((15 < 5 * c0 + 4?15 : 5 * c0 + 4)) < 7 * c1 + 6?((15 < 5 * c0 + 4?15 : 5 * c0 + 4)) : 7 * c1 + 6)); c2++) {
                    A[c4][c2] = A[c4][c2] - Q[c4][k] * R[k][c2];
                  }
                }
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
