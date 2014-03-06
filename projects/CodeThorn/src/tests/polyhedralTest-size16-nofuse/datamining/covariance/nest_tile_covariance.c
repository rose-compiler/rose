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
    int c6;
    int c5;
    int c1;
    int c2;
    int c3;
#pragma omp parallel for private(c2, c5, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = ((c1 + -1) * 2 < 0?-(-(c1 + -1) / 2) : ((2 < 0?(-(c1 + -1) + - 2 - 1) / - 2 : (c1 + -1 + 2 - 1) / 2))); c2 <= 3; c2++) {
        for (c5 = (2 * c1 > 4 * c2?2 * c1 : 4 * c2); c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 2 * c1; c6 <= ((c5 < 2 * c1 + 1?c5 : 2 * c1 + 1)); c6++) {
            symmat[c6][c5] = 0.0;
          }
        }
      }
    }
#pragma omp parallel for private(c6)
    for (c1 = 0; c1 <= 7; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c6 = 2 * c1; c6 <= 2 * c1 + 1; c6++) {
        mean[c6] = 0.0;
      }
    }
#pragma omp parallel for private(c2, c5, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 3; c2++) {
        for (c5 = 4 * c2; c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 2 * c1; c6 <= 2 * c1 + 1; c6++) {
            mean[c6] += data[c5][c6];
          }
        }
      }
    }
#pragma omp parallel for private(c6)
    for (c1 = 0; c1 <= 7; c1++) {
#pragma ivdep
#pragma vector always
#pragma simd
      for (c6 = 2 * c1; c6 <= 2 * c1 + 1; c6++) {
        mean[c6] /= float_n;
      }
    }
#pragma omp parallel for private(c2, c5, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 3; c2++) {
        for (c5 = 4 * c2; c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 2 * c1; c6 <= 2 * c1 + 1; c6++) {
            data[c6][c5] -= mean[c5];
          }
        }
      }
    }
#pragma omp parallel for private(c3, c2, c5, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = ((c1 + -1) * 2 < 0?-(-(c1 + -1) / 2) : ((2 < 0?(-(c1 + -1) + - 2 - 1) / - 2 : (c1 + -1 + 2 - 1) / 2))); c2 <= 3; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c5 = (2 * c1 > 4 * c2?2 * c1 : 4 * c2); c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c6 = 2 * c1; c6 <= ((c5 < 2 * c1 + 1?c5 : 2 * c1 + 1)); c6++) {
              symmat[c6][c5] += data[c3][c6] * data[c3][c5];
            }
          }
        }
      }
    }
#pragma omp parallel for private(c2, c5, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = ((c1 + -1) * 2 < 0?-(-(c1 + -1) / 2) : ((2 < 0?(-(c1 + -1) + - 2 - 1) / - 2 : (c1 + -1 + 2 - 1) / 2))); c2 <= 3; c2++) {
        for (c5 = (2 * c1 > 4 * c2?2 * c1 : 4 * c2); c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 2 * c1; c6 <= ((c5 < 2 * c1 + 1?c5 : 2 * c1 + 1)); c6++) {
            symmat[c5][c6] = symmat[c6][c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
