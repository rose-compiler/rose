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
double alpha;
double beta;
#ifndef POLYBENCH_TEST_MALLOC
double A[4000UL][4000UL];
double B[4000UL][4000UL];
double x[4000UL];
double u1[4000UL];
double u2[4000UL];
double v2[4000UL];
double v1[4000UL];
double w[4000UL];
double y[4000UL];
double z[4000UL];
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
    alpha = (43532);
    beta = (12313);
    for (c1 = 0; c1 <= 3999; ++c1) {
      u1[c1] = (c1);
      u2[c1] = ((((c1 + 1) / 4000)) / 2.0);
      v1[c1] = ((((c1 + 1) / 4000)) / 4.0);
      v2[c1] = ((((c1 + 1) / 4000)) / 6.0);
      y[c1] = ((((c1 + 1) / 4000)) / 8.0);
      z[c1] = ((((c1 + 1) / 4000)) / 9.0);
      x[c1] = 0.0;
      w[c1] = 0.0;
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
      fprintf(stderr,("%0.2lf "),(w[i]));
      if ((i % 80) == 20) 
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
  
#pragma live - out w
{
    int ub1;
    int lb1;
    int c3;
    int c1;
    for (c1 = 0; c1 <= 3999; ++c1) {
      for (c3 = 0; c3 <= 3999; ++c3) {
        (A[c1])[c3] = ((((A[c1])[c3]) + ((u1[c1]) * (v1[c3]))) + ((u2[c1]) * (v2[c3])));
      }
    }
    for (c1 = 0; c1 <= 3999; ++c1) {
      for (c3 = 0; c3 <= 3999; ++c3) {
        x[c1] = ((x[c1]) + ((beta * ((A[c3])[c1])) * (y[c3])));
      }
    }
    for (c1 = 0; c1 <= 3999; ++c1) {
      x[c1] = ((x[c1]) + (z[c1]));
    }
    for (c1 = 0; c1 <= 3999; ++c1) {
      for (c3 = 0; c3 <= 3999; ++c3) {
        w[c1] = ((w[c1]) + ((alpha * ((A[c1])[c3])) * (x[c3])));
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

