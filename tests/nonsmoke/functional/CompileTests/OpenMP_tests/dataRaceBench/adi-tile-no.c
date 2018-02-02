/**
 * adi.c: This file is part of the PolyBench/C 3.2 test suite.
 * Alternating Direction Implicit solver with tiling and nested SIMD.
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 * License: /LICENSE.OSU.txt
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
/* Default data type is double, default size is 10x1024x1024. */
#include "adi.h"
/* Array initialization. */

static void init_array(int n,double X[500 + 0][500 + 0],double A[500 + 0][500 + 0],double B[500 + 0][500 + 0])
{
  //int i;
  //int j;
{
    int c1;
    int c3;
    int c2;
    int c4;
    if (n >= 1) {
#pragma omp parallel for private(c4, c2, c3)
      for (c1 = 0; c1 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c1++) {
        for (c2 = 0; c2 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c2++) {
          for (c3 = 16 * c1; c3 <= ((16 * c1 + 15 < n + -1?16 * c1 + 15 : n + -1)); c3++) {
#pragma omp simd
            for (c4 = 16 * c2; c4 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c4++) {
              X[c3][c4] = (((double )c3) * (c4 + 1) + 1) / n;
              A[c3][c4] = (((double )c3) * (c4 + 2) + 2) / n;
              B[c3][c4] = (((double )c3) * (c4 + 3) + 3) / n;
            }
          }
        }
      }
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int n,double X[500 + 0][500 + 0])
{
  int i;
  int j;
  for (i = 0; i < n; i++) 
    for (j = 0; j < n; j++) {
      fprintf(stderr,"%0.2lf ",X[i][j]);
      if ((i * 500 + j) % 20 == 0) 
        fprintf(stderr,"\n");
    }
  fprintf(stderr,"\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_adi(int tsteps,int n,double X[500 + 0][500 + 0],double A[500 + 0][500 + 0],double B[500 + 0][500 + 0])
{
  //int t;
  //int i1;
  //int i2;
  
  //#pragma scop
{
    int c0;
    int c2;
    int c8;
    int c9;
    int c15;
    if (n >= 1 && tsteps >= 1) {
      for (c0 = 0; c0 <= tsteps + -1; c0++) {
        if (n >= 2) {
#pragma omp parallel for private(c15, c9, c8)
          for (c2 = 0; c2 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c2++) {
            for (c8 = 0; c8 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c8++) {
              for (c9 = (1 > 16 * c8?1 : 16 * c8); c9 <= ((16 * c8 + 15 < n + -1?16 * c8 + 15 : n + -1)); c9++) {
#pragma omp simd
                for (c15 = 16 * c2; c15 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c15++) {
                  B[c15][c9] = B[c15][c9] - A[c15][c9] * A[c15][c9] / B[c15][c9 - 1];
                }
              }
            }
            for (c8 = 0; c8 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c8++) {
              for (c9 = (1 > 16 * c8?1 : 16 * c8); c9 <= ((16 * c8 + 15 < n + -1?16 * c8 + 15 : n + -1)); c9++) {
#pragma omp simd
                for (c15 = 16 * c2; c15 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c15++) {
                  X[c15][c9] = X[c15][c9] - X[c15][c9 - 1] * A[c15][c9] / B[c15][c9 - 1];
                }
              }
            }
            for (c8 = 0; c8 <= (((n + -3) * 16 < 0?((16 < 0?-((-(n + -3) + 16 + 1) / 16) : -((-(n + -3) + 16 - 1) / 16))) : (n + -3) / 16)); c8++) {
              for (c9 = 16 * c8; c9 <= ((16 * c8 + 15 < n + -3?16 * c8 + 15 : n + -3)); c9++) {
#pragma omp simd
                for (c15 = 16 * c2; c15 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c15++) {
                  X[c15][n - c9 - 2] = (X[c15][n - 2 - c9] - X[c15][n - 2 - c9 - 1] * A[c15][n - c9 - 3]) / B[c15][n - 3 - c9];
                }
              }
            }
          }
        }
#pragma omp parallel for private(c15)
        for (c2 = 0; c2 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c2++) {
#pragma omp simd
          for (c15 = 16 * c2; c15 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c15++) {
            X[c15][n - 1] = X[c15][n - 1] / B[c15][n - 1];
          }
        }
        if (n >= 2) {
#pragma omp parallel for private(c15, c9, c8)
          for (c2 = 0; c2 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c2++) {
            for (c8 = 0; c8 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c8++) {
              for (c9 = (1 > 16 * c8?1 : 16 * c8); c9 <= ((16 * c8 + 15 < n + -1?16 * c8 + 15 : n + -1)); c9++) {
#pragma omp simd
                for (c15 = 16 * c2; c15 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c15++) {
                  B[c9][c15] = B[c9][c15] - A[c9][c15] * A[c9][c15] / B[c9 - 1][c15];
                }
              }
            }
            for (c8 = 0; c8 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c8++) {
              for (c9 = (1 > 16 * c8?1 : 16 * c8); c9 <= ((16 * c8 + 15 < n + -1?16 * c8 + 15 : n + -1)); c9++) {
#pragma omp simd
                for (c15 = 16 * c2; c15 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c15++) {
                  X[c9][c15] = X[c9][c15] - X[c9 - 1][c15] * A[c9][c15] / B[c9 - 1][c15];
                }
              }
            }
            for (c8 = 0; c8 <= (((n + -3) * 16 < 0?((16 < 0?-((-(n + -3) + 16 + 1) / 16) : -((-(n + -3) + 16 - 1) / 16))) : (n + -3) / 16)); c8++) {
              for (c9 = 16 * c8; c9 <= ((16 * c8 + 15 < n + -3?16 * c8 + 15 : n + -3)); c9++) {
#pragma omp simd
                for (c15 = 16 * c2; c15 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c15++) {
                  X[n - 2 - c9][c15] = (X[n - 2 - c9][c15] - X[n - c9 - 3][c15] * A[n - 3 - c9][c15]) / B[n - 2 - c9][c15];
                }
              }
            }
          }
        }
#pragma omp parallel for private(c15)
        for (c2 = 0; c2 <= (((n + -1) * 16 < 0?((16 < 0?-((-(n + -1) + 16 + 1) / 16) : -((-(n + -1) + 16 - 1) / 16))) : (n + -1) / 16)); c2++) {
#pragma omp simd
          for (c15 = 16 * c2; c15 <= ((16 * c2 + 15 < n + -1?16 * c2 + 15 : n + -1)); c15++) {
            X[n - 1][c15] = X[n - 1][c15] / B[n - 1][c15];
          }
        }
      }
    }
  }
  
//#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 500;
  int tsteps = 10;
/* Variable declaration/allocation. */
  double (*X)[500 + 0][500 + 0];
  X = ((double (*)[500 + 0][500 + 0])(polybench_alloc_data(((500 + 0) * (500 + 0)),(sizeof(double )))));
  ;
  double (*A)[500 + 0][500 + 0];
  A = ((double (*)[500 + 0][500 + 0])(polybench_alloc_data(((500 + 0) * (500 + 0)),(sizeof(double )))));
  ;
  double (*B)[500 + 0][500 + 0];
  B = ((double (*)[500 + 0][500 + 0])(polybench_alloc_data(((500 + 0) * (500 + 0)),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(n, *X, *A, *B);
/* Start timer. */
  polybench_timer_start();
  ;
/* Run kernel. */
  kernel_adi(tsteps,n, *X, *A, *B);
/* Stop and print timer. */
  polybench_timer_stop();
  ;
  polybench_timer_print();
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(n, *X);
/* Be clean. */
  free(((void *)X));
  ;
  free(((void *)A));
  ;
  free(((void *)B));
  ;
  return 0;
}
