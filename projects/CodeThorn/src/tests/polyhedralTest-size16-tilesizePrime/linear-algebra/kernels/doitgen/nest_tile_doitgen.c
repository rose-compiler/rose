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
  int nr = 16;
  int nq = 16;
  int np = 16;
  double A[16][16][16];
  double sum[16][16][16];
  double C4[16][16];
  int r;
  int q;
  int p;
  int s;
  
#pragma scop
{
    int c6;
    int c1;
    int c2;
    int c3;
    int c7;
    int c5;
#pragma omp parallel for private(c5, c3, c2, c6)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c5 = 5 * c1; c5 <= ((15 < 5 * c1 + 4?15 : 5 * c1 + 4)); c5++) {
            for (c6 = 7 * c2; c6 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c6++) {
              sum[c5][c6][c3] = 0;
            }
          }
        }
      }
    }
#pragma omp parallel for private(c5, c7, c3, c2, c6)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c5 = 5 * c1; c5 <= ((15 < 5 * c1 + 4?15 : 5 * c1 + 4)); c5++) {
            for (c6 = 7 * c2; c6 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c6++) {
              for (c7 = 0; c7 <= 15; c7++) {
                sum[c5][c6][c3] = sum[c5][c6][c3] + A[c5][c6][c7] * C4[c7][c3];
              }
            }
          }
        }
      }
    }
#pragma omp parallel for private(c5, c3, c2, c6)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c5 = 5 * c1; c5 <= ((15 < 5 * c1 + 4?15 : 5 * c1 + 4)); c5++) {
            for (c6 = 7 * c2; c6 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c6++) {
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
