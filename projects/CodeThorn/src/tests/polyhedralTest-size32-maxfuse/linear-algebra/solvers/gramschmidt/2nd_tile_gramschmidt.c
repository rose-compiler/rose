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
  int ni = 32;
  int nj = 32;
/* Variable declaration/allocation. */
  double A[32][32];
  double R[32][32];
  double Q[32][32];
  int i;
  int j;
  int k;
  double nrm;
  
#pragma scop
  for (k = 0; k < 32; k++) {{
      int c2;
{
        int c3;
        nrm = 0;
        for (c3 = 0; c3 <= 31; c3++) {
          nrm += A[c3][k] * A[c3][k];
        }
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c1;
      int c8;
      int c4;
      if (k >= 0) {
        
#pragma omp parallel for private(c8)
        for (c1 = 0; c1 <= 3; c1++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c0;
            int c2;
            if (8 * c1 <= k) {
#pragma omp parallel for private(c2)
              for (c0 = 4 * c1; c0 <= ((((k * 2 < 0?((2 < 0?-((-k + 2 + 1) / 2) : -((-k + 2 - 1) / 2))) : k / 2)) < 4 * c1 + 3?((k * 2 < 0?((2 < 0?-((-k + 2 + 1) / 2) : -((-k + 2 - 1) / 2))) : k / 2)) : 4 * c1 + 3)); c0++) {
                for (c2 = 2 * c0; c2 <= ((k < 2 * c0 + 1?k : 2 * c0 + 1)); c2++) {
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
            if (k <= 8 * c1 + 6) {
              for (c3 = (8 * c1 > k + 1?8 * c1 : k + 1); c3 <= 8 * c1 + 7; c3++) {
                Q[c3][k] = A[c3][k] / R[k][k];
                R[k][c3] = 0;
              }
            }
          }
        }
      }
      if (k <= - 1) {
        
#pragma omp parallel for private(c8)
        for (c1 = ((k + - 6) * 8 < 0?-(-(k + - 6) / 8) : ((8 < 0?(-(k + - 6) + - 8 - 1) / - 8 : (k + - 6 + 8 - 1) / 8))); c1 <= 3; c1++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c2;
            int c0;
            if (c1 <= -1 && k <= 8 * c1 + 6) {
#pragma omp parallel for private(c2)
              for (c0 = (((k * 2 < 0?-(-k / 2) : ((2 < 0?(-k + - 2 - 1) / - 2 : (k + 2 - 1) / 2)))) > 4 * c1?((k * 2 < 0?-(-k / 2) : ((2 < 0?(-k + - 2 - 1) / - 2 : (k + 2 - 1) / 2)))) : 4 * c1); c0 <= 4 * c1 + 3; c0++) {
                for (c2 = (2 * c0 > k + 1?2 * c0 : k + 1); c2 <= 2 * c0 + 1; c2++) {
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
            if (c1 >= 0) {
              for (c0 = 8 * c1; c0 <= 8 * c1 + 7; c0++) {
                Q[c0][k] = A[c0][k] / R[k][k];
                R[k][c0] = 0;
              }
            }
          }
        }
      }
      
#pragma omp parallel for private(c4, c8)
      for (c1 = ((k + - 6) * 8 < 0?-(-(k + - 6) / 8) : ((8 < 0?(-(k + - 6) + - 8 - 1) / - 8 : (k + - 6 + 8 - 1) / 8))); c1 <= 3; c1++) {
        for (c4 = 0; c4 <= 31; c4++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c2;
            int c0;
            if (k <= 8 * c1 + 6) {
#pragma omp parallel for private(c2)
              for (c0 = (((k * 2 < 0?-(-k / 2) : ((2 < 0?(-k + - 2 - 1) / - 2 : (k + 2 - 1) / 2)))) > 4 * c1?((k * 2 < 0?-(-k / 2) : ((2 < 0?(-k + - 2 - 1) / - 2 : (k + 2 - 1) / 2)))) : 4 * c1); c0 <= 4 * c1 + 3; c0++) {
                for (c2 = (2 * c0 > k + 1?2 * c0 : k + 1); c2 <= 2 * c0 + 1; c2++) {
                  R[k][c2] += Q[c4][k] * A[c4][c2];
                }
              }
            }
          }
        }
        for (c4 = 0; c4 <= 31; c4++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c2;
            int c0;
            if (k <= 8 * c1 + 6) {
#pragma omp parallel for private(c2)
              for (c0 = (((k * 2 < 0?-(-k / 2) : ((2 < 0?(-k + - 2 - 1) / - 2 : (k + 2 - 1) / 2)))) > 4 * c1?((k * 2 < 0?-(-k / 2) : ((2 < 0?(-k + - 2 - 1) / - 2 : (k + 2 - 1) / 2)))) : 4 * c1); c0 <= 4 * c1 + 3; c0++) {
                for (c2 = (2 * c0 > k + 1?2 * c0 : k + 1); c2 <= 2 * c0 + 1; c2++) {
                  A[c4][c2] = A[c4][c2] - Q[c4][k] * R[k][c2];
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
