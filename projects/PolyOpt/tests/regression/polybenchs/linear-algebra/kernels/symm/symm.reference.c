#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef N
# define N 512
#endif
#ifndef M
# define M 512
#endif
/* Default data type is double (dsymm). */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
double alpha;
double beta;
double acc;
#ifndef POLYBENCH_TEST_MALLOC
double A[512UL][512UL];
double B[512UL][512UL];
double C[512UL][512UL];
#else
#endif

inline void init_array()
{
  int i;
  int j;
{
    int ub1;
    int lb1;
    int c3;
    int c1;
    alpha = (12435);
    beta = (4546);
    for (c1 = 0; c1 <= 511; ++c1) {
      for (c3 = 0; c3 <= 511; ++c3) {
        (A[c1])[c3] = ((((double )c1) * (c3)) / (512));
      }
    }
    for (c1 = 0; c1 <= 511; ++c1) {
      for (c3 = 0; c3 <= 511; ++c3) {
        (B[c1])[c3] = (((((double )c1) * (c3)) + (1)) / (512));
        (C[c1])[c3] = (((((double )c1) * (c3)) + (2)) / (512));
      }
    }
  }
}

/* Define the live-out variables. Code is not executed unless
   POLYBENCH_DUMP_ARRAYS is defined. */

inline void print_array(int argc,char **argv)
{
  int i;
  int j;
#ifndef POLYBENCH_DUMP_ARRAYS
  if ((argc > 42) && !(strcmp((argv[0]),"") != 0)) 
#endif
{
    for (i = 0; i < 512; i++) 
      for (j = 0; j < 512; j++) {
        fprintf(stderr,("%0.2lf "),((C[i])[j]));
        if ((((i * 512) + j) % 80) == 20) 
          fprintf(stderr,("\n"));
      }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int i;
  int j;
  int k;
  int n = 512;
  int m = 512;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out C
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (m >= 1 && n >= 3) {
      for (c1 = 0; c1 <= m + -1; ++c1) {
        for (c3 = 0; c3 <= 1; ++c3) {
          acc = (0);
          (C[c1])[c3] = (((beta * ((C[c1])[c3])) + ((alpha * ((A[c1])[c1])) * ((B[c1])[c3]))) + (alpha * acc));
        }
        for (c3 = 2; c3 <= n + -1; ++c3) {
          acc = (0);
          for (c5 = 0; c5 <= c3 + -2; ++c5) {
            (C[c5])[c3] += ((alpha * ((A[c5])[c1])) * ((B[c1])[c3]));
            acc += (((B[c5])[c3]) * ((A[c5])[c1]));
          }
          (C[c1])[c3] = (((beta * ((C[c1])[c3])) + ((alpha * ((A[c1])[c1])) * ((B[c1])[c3]))) + (alpha * acc));
        }
      }
    }
    if (m >= 1 && n >= 1 && n <= 2) {
      for (c1 = 0; c1 <= m + -1; ++c1) {
        for (c3 = 0; c3 <= n + -1; ++c3) {
          acc = (0);
          (C[c1])[c3] = (((beta * ((C[c1])[c3])) + ((alpha * ((A[c1])[c1])) * ((B[c1])[c3]))) + (alpha * acc));
        }
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

