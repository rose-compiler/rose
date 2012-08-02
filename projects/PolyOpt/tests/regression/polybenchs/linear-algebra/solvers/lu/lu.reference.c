#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
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
double A[1024UL][1024UL];
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
        (A[c1])[c3] = (((((double )c1) * (c3)) + (1)) / (1024));
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
  int i;
  int j;
  int k;
  int n = 1024;
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
    if (n >= 2) {
      for (c1 = 0; c1 <= n + -2; ++c1) {
        for (c3 = c1 + 1; c3 <= n + -1; ++c3) {
          (A[c1])[c3] = (((A[c1])[c3]) / ((A[c1])[c1]));
        }
        for (c3 = c1 + 1; c3 <= n + -1; ++c3) {
          for (c5 = c1 + 1; c5 <= n + -1; ++c5) {
            (A[c3])[c5] = (((A[c3])[c5]) - (((A[c3])[c1]) * ((A[c1])[c5])));
          }
        }
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

