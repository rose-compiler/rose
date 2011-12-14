#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef TSTEPS
# define TSTEPS 10
#endif
#ifndef N
# define N 1024
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
double X[1024UL][1024UL];
double A[1024UL][1024UL];
double B[1024UL][1024UL];
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
    for (c1 = 0; c1 <= 1023; ++c1) {
      for (c3 = 0; c3 <= 1023; ++c3) {
        (X[c1])[c3] = (((((double )c1) * (c3)) + (1)) / (1024));
        (A[c1])[c3] = (((((double )c1) * (c3)) + (2)) / (1024));
        (B[c1])[c3] = (((((double )c1) * (c3)) + (3)) / (1024));
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
    for (i = 0; i < 1024; i++) 
      for (j = 0; j < 1024; j++) {
        fprintf(stderr,("%0.2lf "),((A[i])[j]));
        if ((((i * 1024) + j) % 80) == 20) 
          fprintf(stderr,("\n"));
      }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int t;
  int i1;
  int i2;
  int n = 1024;
  int tsteps = 10;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out X
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (n >= 3 && tsteps >= 1) {
      for (c1 = 0; c1 <= tsteps + -1; ++c1) {
        for (c3 = 0; c3 <= n + -1; ++c3) {
          for (c5 = 1; c5 <= n + -1; ++c5) {
            (X[c3])[c5] = (((X[c3])[c5]) - ((((X[c3])[c5 - 1]) * ((A[c3])[c5])) / ((B[c3])[c5 - 1])));
            (B[c3])[c5] = (((B[c3])[c5]) - ((((A[c3])[c5]) * ((A[c3])[c5])) / ((B[c3])[c5 - 1])));
          }
        }
        for (c3 = 0; c3 <= n + -1; ++c3) {
          (X[c3])[n - 1] = (((X[c3])[n - 1]) / ((B[c3])[n - 1]));
        }
        for (c3 = 0; c3 <= n + -1; ++c3) {
          for (c5 = 0; c5 <= n + -3; ++c5) {
            (X[c3])[(n - c5) - 2] = ((((X[c3])[(n - 2) - c5]) - (((X[c3])[((n - 2) - c5) - 1]) * ((A[c3])[(n - c5) - 3]))) / ((B[c3])[(n - 3) - c5]));
          }
        }
        for (c3 = 1; c3 <= n + -1; ++c3) {
          for (c5 = 0; c5 <= n + -1; ++c5) {
            (X[c3])[c5] = (((X[c3])[c5]) - ((((X[c3 - 1])[c5]) * ((A[c3])[c5])) / ((B[c3 - 1])[c5])));
            (B[c3])[c5] = (((B[c3])[c5]) - ((((A[c3])[c5]) * ((A[c3])[c5])) / ((B[c3 - 1])[c5])));
          }
        }
        for (c3 = 0; c3 <= n + -1; ++c3) {
          (X[n - 1])[c3] = (((X[n - 1])[c3]) / ((B[n - 1])[c3]));
        }
        for (c3 = 0; c3 <= n + -3; ++c3) {
          for (c5 = 0; c5 <= n + -1; ++c5) {
            (X[(n - 2) - c3])[c5] = ((((X[(n - 2) - c3])[c5]) - (((X[(n - c3) - 3])[c5]) * ((A[(n - 3) - c3])[c5]))) / ((B[(n - 2) - c3])[c5]));
          }
        }
      }
    }
    if (n == 2 && tsteps >= 1) {
      for (c1 = 0; c1 <= tsteps + -1; ++c1) {
        for (c3 = 0; c3 <= 1; ++c3) {
          (X[c3])[1] = (((X[c3])[1]) - ((((X[c3])[1 - 1]) * ((A[c3])[1])) / ((B[c3])[1 - 1])));
          (B[c3])[1] = (((B[c3])[1]) - ((((A[c3])[1]) * ((A[c3])[1])) / ((B[c3])[1 - 1])));
        }
        for (c3 = 0; c3 <= 1; ++c3) {
          (X[c3])[n - 1] = (((X[c3])[n - 1]) / ((B[c3])[n - 1]));
        }
        for (c5 = 0; c5 <= 1; ++c5) {
          (X[1])[c5] = (((X[1])[c5]) - ((((X[1 - 1])[c5]) * ((A[1])[c5])) / ((B[1 - 1])[c5])));
          (B[1])[c5] = (((B[1])[c5]) - ((((A[1])[c5]) * ((A[1])[c5])) / ((B[1 - 1])[c5])));
        }
        for (c3 = 0; c3 <= 1; ++c3) {
          (X[n - 1])[c3] = (((X[n - 1])[c3]) / ((B[n - 1])[c3]));
        }
      }
    }
    if (n == 1 && tsteps >= 1) {
      for (c1 = 0; c1 <= tsteps + -1; ++c1) {
        (X[0])[n - 1] = (((X[0])[n - 1]) / ((B[0])[n - 1]));
        (X[n - 1])[0] = (((X[n - 1])[0]) / ((B[n - 1])[0]));
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

