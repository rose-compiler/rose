/**
 * adi.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 * Alternating Direction Implicit solver: 
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
    int c2;
    if (n >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = 0; c1 <= n + -1; c1++) {
        for (c2 = 0; c2 <= n + -1; c2++) {
          X[c1][c2] = (((double )c1) * (c2 + 1) + 1) / n;
          A[c1][c2] = (((double )c1) * (c2 + 2) + 2) / n;
          B[c1][c2] = (((double )c1) * (c2 + 3) + 3) / n;
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
    for (c0 = 0; c0 <= 9; c0++) {
#pragma omp parallel for private(c8)
      for (c2 = 0; c2 <= 499; c2++) {
        for (c8 = 1; c8 <= 499; c8++) {
          B[c2][c8] = B[c2][c8] - A[c2][c8] * A[c2][c8] / B[c2][c8 - 1];
        }
        for (c8 = 1; c8 <= 499; c8++) {
          X[c2][c8] = X[c2][c8] - X[c2][c8 - 1] * A[c2][c8] / B[c2][c8 - 1];
        }
        for (c8 = 0; c8 <= 497; c8++) {
          X[c2][500 - c8 - 2] = (X[c2][500 - 2 - c8] - X[c2][500 - 2 - c8 - 1] * A[c2][500 - c8 - 3]) / B[c2][500 - 3 - c8];
        }
      }
#pragma omp parallel for
      for (c2 = 0; c2 <= 499; c2++) {
        X[c2][500 - 1] = X[c2][500 - 1] / B[c2][500 - 1];
      }
#pragma omp parallel for private(c8)
      for (c2 = 0; c2 <= 499; c2++) {
        for (c8 = 1; c8 <= 499; c8++) {
          B[c8][c2] = B[c8][c2] - A[c8][c2] * A[c8][c2] / B[c8 - 1][c2];
        }
        for (c8 = 1; c8 <= 499; c8++) {
          X[c8][c2] = X[c8][c2] - X[c8 - 1][c2] * A[c8][c2] / B[c8 - 1][c2];
        }
        for (c8 = 0; c8 <= 497; c8++) {
          X[500 - 2 - c8][c2] = (X[500 - 2 - c8][c2] - X[500 - c8 - 3][c2] * A[500 - 3 - c8][c2]) / B[500 - 2 - c8][c2];
        }
      }
#pragma omp parallel for
      for (c2 = 0; c2 <= 499; c2++) {
        X[500 - 1][c2] = X[500 - 1][c2] / B[500 - 1][c2];
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
