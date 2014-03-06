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
    for (c0 = 0; c0 <= 1; c0++) {
      
#pragma omp parallel for private(c8)
{
        int c6;
        int c3;
        for (c6 = 0; c6 <= 15; c6++) {
          X[c6][16 - 0 - 2] = (X[c6][16 - 2 - 0] - X[c6][16 - 2 - 0 - 1] * A[c6][16 - 0 - 3]) / B[c6][16 - 3 - 0];
        }
        for (c3 = 1; c3 <= 13; c3++) {
          for (c6 = 0; c6 <= 15; c6++) {
            B[c6][c3] = B[c6][c3] - A[c6][c3] * A[c6][c3] / B[c6][c3 - 1];
            X[c6][c3] = X[c6][c3] - X[c6][c3 - 1] * A[c6][c3] / B[c6][c3 - 1];
          }
          for (c6 = 0; c6 <= 15; c6++) {
            X[c6][16 - c3 - 2] = (X[c6][16 - 2 - c3] - X[c6][16 - 2 - c3 - 1] * A[c6][16 - c3 - 3]) / B[c6][16 - 3 - c3];
          }
        }
        for (c3 = 14; c3 <= 15; c3++) {
          for (c6 = 0; c6 <= 15; c6++) {
            B[c6][c3] = B[c6][c3] - A[c6][c3] * A[c6][c3] / B[c6][c3 - 1];
            X[c6][c3] = X[c6][c3] - X[c6][c3 - 1] * A[c6][c3] / B[c6][c3 - 1];
          }
        }
      }
      
#pragma omp parallel for
{
        int c0;
#pragma omp parallel for
        for (c0 = 0; c0 <= 15; c0++) {
          X[c0][16 - 1] = X[c0][16 - 1] / B[c0][16 - 1];
        }
      }
      
#pragma omp parallel for private(c8)
{
        int c6;
        int c3;
        for (c6 = 0; c6 <= 15; c6++) {
          X[16 - 2 - 0][c6] = (X[16 - 2 - 0][c6] - X[16 - 0 - 3][c6] * A[16 - 3 - 0][c6]) / B[16 - 2 - 0][c6];
        }
        for (c3 = 1; c3 <= 13; c3++) {
          for (c6 = 0; c6 <= 15; c6++) {
            B[c3][c6] = B[c3][c6] - A[c3][c6] * A[c3][c6] / B[c3 - 1][c6];
            X[c3][c6] = X[c3][c6] - X[c3 - 1][c6] * A[c3][c6] / B[c3 - 1][c6];
          }
          for (c6 = 0; c6 <= 15; c6++) {
            X[16 - 2 - c3][c6] = (X[16 - 2 - c3][c6] - X[16 - c3 - 3][c6] * A[16 - 3 - c3][c6]) / B[16 - 2 - c3][c6];
          }
        }
        for (c3 = 14; c3 <= 15; c3++) {
          for (c6 = 0; c6 <= 15; c6++) {
            B[c3][c6] = B[c3][c6] - A[c3][c6] * A[c3][c6] / B[c3 - 1][c6];
            X[c3][c6] = X[c3][c6] - X[c3 - 1][c6] * A[c3][c6] / B[c3 - 1][c6];
          }
        }
      }
      
#pragma omp parallel for
{
        int c0;
#pragma omp parallel for
        for (c0 = 0; c0 <= 15; c0++) {
          X[16 - 1][c0] = X[16 - 1][c0] / B[16 - 1][c0];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
