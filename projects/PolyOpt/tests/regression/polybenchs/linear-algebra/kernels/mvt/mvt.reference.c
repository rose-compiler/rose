#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef N
# define N 4000
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
double A[4000UL][4000UL];
double x1[4000UL];
double y_1[4000UL];
double x2[4000UL];
double y_2[4000UL];
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
    for (c1 = 0; c1 <= 3999; ++c1) {
      x1[c1] = (((double )c1) / (4000));
      x2[c1] = ((((double )c1) + (1)) / (4000));
      y_1[c1] = ((((double )c1) + (3)) / (4000));
      y_2[c1] = ((((double )c1) + (4)) / (4000));
      for (c3 = 0; c3 <= 3999; ++c3) {
        (A[c1])[c3] = ((((double )c1) * (c3)) / (4000));
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
    for (i = 0; i < 4000; i++) {
      fprintf(stderr,("%0.2lf "),(x1[i]));
      fprintf(stderr,("%0.2lf "),(x2[i]));
      if (((2 * i) % 80) == 20) 
        fprintf(stderr,("\n"));
    }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int i;
  int j;
  int n = 4000;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out x1, x2
{
    int ub1;
    int lb1;
    int c3;
    int c1;
    if (n >= 1) {
      for (c1 = 0; c1 <= n + -1; ++c1) {
        for (c3 = 0; c3 <= n + -1; ++c3) {
          x1[c1] = ((x1[c1]) + (((A[c1])[c3]) * (y_1[c3])));
        }
      }
      for (c1 = 0; c1 <= n + -1; ++c1) {
        for (c3 = 0; c3 <= n + -1; ++c3) {
          x2[c1] = ((x2[c1]) + (((A[c3])[c1]) * (y_2[c3])));
        }
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

