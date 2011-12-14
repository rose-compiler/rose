#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef M
# define M 500
#endif
#ifndef N
# define N 500
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
double float_n = 321414134.01;
#ifndef POLYBENCH_TEST_MALLOC
double data[501UL][501UL];
double symmat[501UL][501UL];
double mean[501UL];
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
    for (c1 = 0; c1 <= 500; ++c1) {
      for (c3 = 0; c3 <= 500; ++c3) {
        (data[c1])[c3] = ((((double )c1) * (c3)) / (500));
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
    for (i = 0; i <= 500; i++) 
      for (j = 0; j <= 500; j++) {
        fprintf(stderr,("%0.2lf "),((symmat[i])[j]));
        if ((((i * 500) + j) % 80) == 20) 
          fprintf(stderr,("\n"));
      }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int i;
  int j;
  int j1;
  int j2;
  int m = 500;
  int n = 500;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out symmat
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (m >= 1 && n >= 1) {
      for (c1 = 1; c1 <= m; ++c1) {
        mean[c1] = 0.0;
        for (c3 = 1; c3 <= n; ++c3) {
          mean[c1] += ((data[c3])[c1]);
        }
        mean[c1] /= float_n;
      }
      for (c1 = 1; c1 <= n; ++c1) {
        for (c3 = 1; c3 <= m; ++c3) {
          (data[c1])[c3] -= (mean[c3]);
        }
      }
      for (c1 = 1; c1 <= m; ++c1) {
        for (c3 = c1; c3 <= m; ++c3) {
          (symmat[c1])[c3] = 0.0;
          for (c5 = 1; c5 <= n; ++c5) {
            (symmat[c1])[c3] += (((data[c5])[c1]) * ((data[c5])[c3]));
          }
          (symmat[c3])[c1] = ((symmat[c1])[c3]);
        }
      }
    }
    if (m >= 1 && n <= 0) {
      for (c1 = 1; c1 <= m; ++c1) {
        mean[c1] = 0.0;
        mean[c1] /= float_n;
      }
      for (c1 = 1; c1 <= m; ++c1) {
        for (c3 = c1; c3 <= m; ++c3) {
          (symmat[c1])[c3] = 0.0;
          (symmat[c3])[c1] = ((symmat[c1])[c3]);
        }
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

