#   define NQ ARRAYSIZE
#   define NR ARRAYSIZE
#   define NP ARRAYSIZE
# define _PB_NQ ARRAYSIZE
# define _PB_NR ARRAYSIZE
# define _PB_NP ARRAYSIZE
/**
 * doitgen.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nr = 64;
  int nq = 64;
  int np = 64;
  double A[64][64][64];
  double sum[64][64][64];
  double C4[64][64];
  int r;
  int q;
  int p;
  int s;
  
#pragma scop
{
    int c5;
    int c7;
    int c6;
    int c1;
    int c2;
    int c3;
#pragma omp parallel for private(c3, c2, c6, c5)
    for (c1 = 0; c1 <= 31; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 63; c3++) {
          for (c5 = 2 * c1; c5 <= 2 * c1 + 1; c5++) {
            for (c6 = 4 * c2; c6 <= 4 * c2 + 3; c6++) {
              sum[c5][c6][c3] = 0;
            }
          }
        }
      }
    }
#pragma omp parallel for private(c3, c2, c6, c7, c5)
    for (c1 = 0; c1 <= 31; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 63; c3++) {
          for (c5 = 2 * c1; c5 <= 2 * c1 + 1; c5++) {
            for (c6 = 4 * c2; c6 <= 4 * c2 + 3; c6++) {
              for (c7 = 0; c7 <= 63; c7++) {
                sum[c5][c6][c3] = sum[c5][c6][c3] + A[c5][c6][c7] * C4[c7][c3];
              }
            }
          }
        }
      }
    }
#pragma omp parallel for private(c3, c2, c6, c5)
    for (c1 = 0; c1 <= 31; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 63; c3++) {
          for (c5 = 2 * c1; c5 <= 2 * c1 + 1; c5++) {
            for (c6 = 4 * c2; c6 <= 4 * c2 + 3; c6++) {
              A[c5][c6][c3] = sum[c5][c6][c3];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
