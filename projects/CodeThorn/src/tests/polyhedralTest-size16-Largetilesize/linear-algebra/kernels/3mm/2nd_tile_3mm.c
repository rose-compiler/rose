#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
#   define NK ARRAYSIZE
#   define NL ARRAYSIZE
#   define NM ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
# define _PB_NK ARRAYSIZE
# define _PB_NL ARRAYSIZE
# define _PB_NM ARRAYSIZE
/**
 * 3mm.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nk = 16;
  int nl = 16;
  int nm = 16;
  double E[16][16];
  double A[16][16];
  double B[16][16];
  double F[16][16];
  double C[16][16];
  double D[16][16];
  double G[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c10;
    int c2;
    int c5;
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
          G[c1][c2] = 0;
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
          F[c1][c2] = 0;
        }
      }
    }
    for (c2 = 0; c2 <= 15; c2++) {
      for (c5 = 0; c5 <= 15; c5++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c1;
#pragma ivdep
#pragma vector always
#pragma simd
          for (c1 = 0; c1 <= 15; c1++) {
            F[c1][c2] += C[c1][c5] * D[c5][c2];
          }
        }
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
          E[c1][c2] = 0;
        }
      }
    }
    for (c2 = 0; c2 <= 15; c2++) {
      for (c5 = 0; c5 <= 15; c5++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c1;
#pragma ivdep
#pragma vector always
#pragma simd
          for (c1 = 0; c1 <= 15; c1++) {
            E[c1][c2] += A[c1][c5] * B[c5][c2];
          }
        }
      }
      for (c5 = 0; c5 <= 15; c5++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c1;
#pragma ivdep
#pragma vector always
#pragma simd
          for (c1 = 0; c1 <= 15; c1++) {
            G[c1][c5] += E[c1][c2] * F[c2][c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
