#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
#ifndef N
# define N 1024
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
double x;
#ifndef POLYBENCH_TEST_MALLOC
double a[1024UL][1024UL];
double p[1024UL];
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
      p[c1] = (3.14159265358979323846 * (c1));
      for (c3 = 0; c3 <= 1023; ++c3) {
        (a[c1])[c3] = ((3.14159265358979323846 * (c1)) + ((2 * c3)));
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
    for (i = 0; i < 1024; i++) {
      for (j = 0; j < 1024; j++) {
        fprintf(stderr,("%0.2lf "),((a[i])[j]));
        if ((((i * 1024) + j) % 80) == 20) 
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
  int n = 1024;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out a
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (n >= 3) {
      x = ((a[0])[0]);
      p[0] = (1.0 / sqrt(x));
      for (c3 = 1; c3 <= n + -1; ++c3) {
        x = ((a[0])[c3]);
        (a[c3])[0] = (x * (p[0]));
      }
      for (c1 = 1; c1 <= n + -2; ++c1) {
        x = ((a[c1])[c1]);
        for (c3 = 0; c3 <= c1 + -1; ++c3) {
          x = (x - (((a[c1])[c3]) * ((a[c1])[c3])));
        }
        p[c1] = (1.0 / sqrt(x));
        for (c3 = c1 + 1; c3 <= n + -1; ++c3) {
          x = ((a[c1])[c3]);
          for (c5 = 0; c5 <= c1 + -1; ++c5) {
            x = (x - (((a[c3])[c5]) * ((a[c1])[c5])));
          }
          (a[c3])[c1] = (x * (p[c1]));
        }
      }
      x = ((a[n + -1])[n + -1]);
      for (c3 = 0; c3 <= n + -2; ++c3) {
        x = (x - (((a[n + -1])[c3]) * ((a[n + -1])[c3])));
      }
      p[n + -1] = (1.0 / sqrt(x));
    }
    if (n == 2) {
      x = ((a[0])[0]);
      p[0] = (1.0 / sqrt(x));
      x = ((a[0])[1]);
      (a[1])[0] = (x * (p[0]));
      x = ((a[1])[1]);
      x = (x - (((a[1])[0]) * ((a[1])[0])));
      p[1] = (1.0 / sqrt(x));
    }
    if (n == 1) {
      x = ((a[0])[0]);
      p[0] = (1.0 / sqrt(x));
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

