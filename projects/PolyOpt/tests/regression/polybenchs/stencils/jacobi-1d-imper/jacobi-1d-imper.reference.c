#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef TSTEPS
# define TSTEPS 10000
#endif
#ifndef N
# define N 4096
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
double A[4096UL];
double B[4096UL];
#else
#endif

inline void init_array()
{
  int i;
  int j;
{
    int ub1;
    int lb1;
    int c1;
    for (c1 = 0; c1 <= 4095; ++c1) {
      A[c1] = ((((double )c1) + (10)) / (4096));
      B[c1] = ((((double )c1) + (11)) / (4096));
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
    for (i = 0; i < 4096; i++) {
      fprintf(stderr,("%0.2lf "),(A[i]));
      if ((i % 80) == 20) 
        fprintf(stderr,("\n"));
    }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int t;
  int i;
  int j;
  int tsteps = 10000;
  int n = 4096;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out A
{
    int ub1;
    int lb1;
    int c3;
    int c1;
    if (n >= 4 && tsteps >= 1) {
      for (c1 = 0; c1 <= tsteps + -1; ++c1) {
        for (c3 = 2; c3 <= n + -2; ++c3) {
          B[c3] = (0.33333 * (((A[c3 - 1]) + (A[c3])) + (A[c3 + 1])));
        }
        for (c3 = 2; c3 <= n + -2; ++c3) {
          A[c3] = (B[c3]);
        }
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

