#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef NX
# define NX 8000
#endif
#ifndef NY
# define NY 8000
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
double A[8000UL][8000UL];
double x[8000UL];
double y[8000UL];
double tmp[8000UL];
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
    for (c1 = 0; c1 <= 7999; ++c1) {
      x[c1] = ((c1) * 3.14159265358979323846);
      for (c3 = 0; c3 <= 7999; ++c3) {
        (A[c1])[c3] = ((((double )c1) * (c3)) / (8000));
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
    for (i = 0; i < 8000; i++) {
      fprintf(stderr,("%0.2lf "),(y[i]));
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
  int nx = 8000;
  int ny = 8000;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out y
{
    int ub1;
    int lb1;
    int c3;
    int c1;
    if (nx >= 1 && ny >= 1) {
      for (c1 = 0; c1 <= nx + -1; ++c1) {
        y[c1] = (0);
      }
      for (c1 = 0; c1 <= ny + -1; ++c1) {
        tmp[c1] = (0);
        for (c3 = 0; c3 <= ny + -1; ++c3) {
          tmp[c1] = ((tmp[c1]) + (((A[c1])[c3]) * (x[c3])));
        }
        for (c3 = 0; c3 <= ny + -1; ++c3) {
          y[c3] = ((y[c3]) + (((A[c1])[c3]) * (tmp[c1])));
        }
      }
    }
    if (nx >= 1 && ny <= 0) {
      for (c1 = 0; c1 <= nx + -1; ++c1) {
        y[c1] = (0);
      }
    }
    if (nx <= 0 && ny >= 1) {
      for (c1 = 0; c1 <= ny + -1; ++c1) {
        tmp[c1] = (0);
        for (c3 = 0; c3 <= ny + -1; ++c3) {
          tmp[c1] = ((tmp[c1]) + (((A[c1])[c3]) * (x[c3])));
        }
        for (c3 = 0; c3 <= ny + -1; ++c3) {
          y[c3] = ((y[c3]) + (((A[c1])[c3]) * (tmp[c1])));
        }
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

