#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef M
# define M 512
#endif
#ifndef N
# define N 512
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
double nrm;
#ifndef POLYBENCH_TEST_MALLOC
double A[512UL][512UL];
double R[512UL][512UL];
double Q[512UL][512UL];
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
    for (c1 = 0; c1 <= 511; ++c1) {
      for (c3 = 0; c3 <= 511; ++c3) {
        (A[c1])[c3] = ((((double )c1) * (c3)) / (512));
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
        fprintf(stderr,("%0.2lf "),((A[i])[j]));
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
  int m = 512;
  int n = 512;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out A
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (m >= 1 && n >= 2) {
      for (c1 = 0; c1 <= n + -2; ++c1) {
        nrm = (0);
        for (c3 = 0; c3 <= m + -1; ++c3) {
          nrm += (((A[c3])[c1]) * ((A[c3])[c1]));
        }
        (R[c1])[c1] = sqrt(nrm);
        for (c3 = 0; c3 <= m + -1; ++c3) {
          (Q[c3])[c1] = (((A[c3])[c1]) / ((R[c1])[c1]));
        }
        for (c3 = c1 + 1; c3 <= n + -1; ++c3) {
          (R[c1])[c3] = (0);
          for (c5 = 0; c5 <= m + -1; ++c5) {
            (R[c1])[c3] += (((Q[c5])[c1]) * ((A[c5])[c3]));
          }
          for (c5 = 0; c5 <= m + -1; ++c5) {
            (A[c5])[c3] = (((A[c5])[c3]) - (((Q[c5])[c1]) * ((R[c1])[c3])));
          }
        }
      }
      nrm = (0);
      for (c3 = 0; c3 <= m + -1; ++c3) {
        nrm += (((A[c3])[n + -1]) * ((A[c3])[n + -1]));
      }
      (R[n + -1])[n + -1] = sqrt(nrm);
      for (c3 = 0; c3 <= m + -1; ++c3) {
        (Q[c3])[n + -1] = (((A[c3])[n + -1]) / ((R[n + -1])[n + -1]));
      }
    }
    if (m >= 1 && n == 1) {
      nrm = (0);
      for (c3 = 0; c3 <= m + -1; ++c3) {
        nrm += (((A[c3])[0]) * ((A[c3])[0]));
      }
      (R[0])[0] = sqrt(nrm);
      for (c3 = 0; c3 <= m + -1; ++c3) {
        (Q[c3])[0] = (((A[c3])[0]) / ((R[0])[0]));
      }
    }
    if (m <= 0 && n >= 2) {
      for (c1 = 0; c1 <= n + -2; ++c1) {
        nrm = (0);
        (R[c1])[c1] = sqrt(nrm);
        for (c3 = c1 + 1; c3 <= n + -1; ++c3) {
          (R[c1])[c3] = (0);
        }
      }
      nrm = (0);
      (R[n + -1])[n + -1] = sqrt(nrm);
    }
    if (m <= 0 && n == 1) {
      nrm = (0);
      (R[0])[0] = sqrt(nrm);
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

