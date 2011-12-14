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
double w;
#ifndef POLYBENCH_TEST_MALLOC
double a[1025UL][1025UL];
double x[1025UL];
double y[1025UL];
double b[1025UL];
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
    for (c1 = 0; c1 <= 1024; ++c1) {
      x[c1] = ((((double )c1) + (1)) / (1024));
      b[c1] = ((((double )c1) + (2)) / (1024));
      for (c3 = 0; c3 <= 1024; ++c3) {
        (a[c1])[c3] = (((((double )c1) * (c3)) + (1)) / (1024));
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
    for (i = 0; i <= 1024; i++) {
      fprintf(stderr,("%0.2lf "),(x[i]));
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
  int k;
  int n = 1024;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out x
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (n >= 2) {
      b[0] = 1.0;
      for (c3 = 1; c3 <= n; ++c3) {
        w = ((a[c3])[0]);
        (a[c3])[0] = (w / ((a[0])[0]));
      }
      for (c3 = 1; c3 <= n; ++c3) {
        w = ((a[0 + 1])[c3]);
        w = (w - (((a[0 + 1])[0]) * ((a[0])[c3])));
        (a[0 + 1])[c3] = w;
      }
      for (c1 = 1; c1 <= n + -1; ++c1) {
        for (c3 = c1 + 1; c3 <= n; ++c3) {
          w = ((a[c3])[c1]);
          for (c5 = 0; c5 <= c1 + -1; ++c5) {
            w = (w - (((a[c3])[c5]) * ((a[c5])[c1])));
          }
          (a[c3])[c1] = (w / ((a[c1])[c1]));
        }
        for (c3 = c1 + 1; c3 <= n; ++c3) {
          w = ((a[c1 + 1])[c3]);
          for (c5 = 0; c5 <= c1; ++c5) {
            w = (w - (((a[c1 + 1])[c5]) * ((a[c5])[c3])));
          }
          (a[c1 + 1])[c3] = w;
        }
      }
      y[0] = (b[0]);
      for (c1 = 1; c1 <= n; ++c1) {
        w = (b[c1]);
        for (c3 = 0; c3 <= c1 + -1; ++c3) {
          w = (w - (((a[c1])[c3]) * (y[c3])));
        }
        y[c1] = w;
      }
      x[n] = ((y[n]) / ((a[n])[n]));
      for (c1 = 0; c1 <= n + -1; ++c1) {
        w = (y[(n - 1) - c1]);
        for (c3 = -c1 + n; c3 <= n; ++c3) {
          w = (w - (((a[(n - 1) - c1])[c3]) * (x[c3])));
        }
        x[(n - 1) - c1] = (w / ((a[(n - 1) - c1])[(n - 1) - c1]));
      }
    }
    if (n == 1) {
      b[0] = 1.0;
      w = ((a[1])[0]);
      (a[1])[0] = (w / ((a[0])[0]));
      w = ((a[0 + 1])[1]);
      w = (w - (((a[0 + 1])[0]) * ((a[0])[1])));
      (a[0 + 1])[1] = w;
      y[0] = (b[0]);
      w = (b[1]);
      w = (w - (((a[1])[0]) * (y[0])));
      y[1] = w;
      x[n] = ((y[n]) / ((a[n])[n]));
      w = (y[(n - 1) - 0]);
      w = (w - (((a[(n - 1) - 0])[1]) * (x[1])));
      x[(n - 1) - 0] = (w / ((a[(n - 1) - 0])[(n - 1) - 0]));
    }
    if (n <= 0) {
      b[0] = 1.0;
      y[0] = (b[0]);
      x[n] = ((y[n]) / ((a[n])[n]));
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

