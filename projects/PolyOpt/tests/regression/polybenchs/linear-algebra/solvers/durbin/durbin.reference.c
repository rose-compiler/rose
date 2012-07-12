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
/* Default data type is int. */
#ifndef DATA_TYPE
# define DATA_TYPE int
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%d "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
int y[4000UL][4000UL];
int sum[4000UL][4000UL];
int beta[4000UL];
int alpha[4000UL];
//input
int r[4000UL];
//output
int out[4000UL];
#else
#endif

inline void init_array()
{
  int i;
{
    int ub1;
    int lb1;
    int c1;
    for (c1 = 0; c1 <= 3999; ++c1) {
      r[c1] = (((c1) * 3.14159265358979323846));
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
      fprintf(stderr,("%d "),(r[i]));
      if ((i % 80) == 20) 
        fprintf(stderr,("\n"));
    }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int i;
  int k;
  int n = 4000;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out out
{
    int ub1;
    int lb1;
    int c3;
    int c1;
    if (n >= 2) {
      (y[0])[0] = (r[0]);
      beta[0] = 1;
      alpha[0] = (r[0]);
      for (c1 = 1; c1 <= n + -1; ++c1) {
        beta[c1] = ((beta[c1 - 1]) - (((alpha[c1 - 1]) * (alpha[c1 - 1])) * (beta[c1 - 1])));
        (sum[0])[c1] = (r[c1]);
        for (c3 = 0; c3 <= c1 + -1; ++c3) {
          (sum[c3 + 1])[c1] = (((sum[c3])[c1]) + ((r[(c1 - c3) - 1]) * ((y[c3])[c1 - 1])));
        }
        alpha[c1] = (-((sum[c1])[c1]) * (beta[c1]));
        for (c3 = 0; c3 <= c1 + -1; ++c3) {
          (y[c3])[c1] = (((y[c3])[c1 - 1]) + ((alpha[c1]) * ((y[(c1 - c3) - 1])[c1 - 1])));
        }
        (y[c1])[c1] = (alpha[c1]);
      }
      for (c1 = 0; c1 <= n + -1; ++c1) {
        out[c1] = ((y[c1])[(4000 - 1)]);
      }
    }
    if (n == 1) {
      (y[0])[0] = (r[0]);
      beta[0] = 1;
      alpha[0] = (r[0]);
      out[0] = ((y[0])[(4000 - 1)]);
    }
    if (n <= 0) {
      (y[0])[0] = (r[0]);
      beta[0] = 1;
      alpha[0] = (r[0]);
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

