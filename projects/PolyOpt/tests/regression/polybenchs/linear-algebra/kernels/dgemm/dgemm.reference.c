#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef NI
# define NI 512
#endif
#ifndef NJ
# define NJ 512
#endif
#ifndef NK
# define NK 512
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
double alpha;
double beta;
#ifndef POLYBENCH_TEST_MALLOC
double C[512UL][512UL];
double A[512UL][512UL];
double B[512UL][512UL];
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
    alpha = (32412);
    beta = (2123);
    for (c1 = 0; c1 <= 511; ++c1) {
      for (c3 = 0; c3 <= 511; ++c3) {
        (A[c1])[c3] = ((((double )c1) * (c3)) / (512));
      }
    }
    for (c1 = 0; c1 <= 511; ++c1) {
      for (c3 = 0; c3 <= 511; ++c3) {
        (B[c1])[c3] = ((((double )c1) * (c3)) / (512));
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
    for (i = 0; i < 512; i++) {
      for (j = 0; j < 512; j++) {
        fprintf(stderr,("%0.2lf "),((C[i])[j]));
        if ((((i * 512) + j) % 80) == 20) 
          fprintf(stderr,("\n"));
      }
      fprintf(stderr,("\n"));
    }
  }
}


int main(int argc,char **argv)
{
  int i;
  int j;
  int k;
  int ni = 512;
  int nj = 512;
  int nk = 512;
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
    if (ni >= 1 && nj >= 1 && nk >= 1) {
      for (c1 = 0; c1 <= ni + -1; ++c1) {
        for (c3 = 0; c3 <= nj + -1; ++c3) {
          (C[c1])[c3] = (((C[c1])[c3]) * alpha);
          for (c5 = 0; c5 <= nk + -1; ++c5) {
            (C[c1])[c3] += ((beta * ((A[c1])[c5])) * ((B[c5])[c3]));
          }
        }
      }
    }
    if (ni >= 1 && nj >= 1 && nk <= 0) {
      for (c1 = 0; c1 <= ni + -1; ++c1) {
        for (c3 = 0; c3 <= nj + -1; ++c3) {
          (C[c1])[c3] = (((C[c1])[c3]) * alpha);
        }
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

