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
#ifndef LENGTH
# define LENGTH 50
#endif
/* Default data type is int. */
#ifndef DATA_TYPE
# define DATA_TYPE int
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%d "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
int out;
#ifndef POLYBENCH_TEST_MALLOC
int sum_c[50UL][50UL][50UL];
int c[50UL][50UL];
//input
int W[50UL][50UL];
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
    for (c1 = 0; c1 <= 49; ++c1) {
      for (c3 = 0; c3 <= 49; ++c3) {
        (W[c1])[c3] = (((((int )c1) * c3) + 1) / 50);
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
    fprintf(stderr,("%d "),out);
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int iter;
  int i;
  int j;
  int k;
  int length = 50;
  int tsteps = 10000;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out out
{
    int ub1;
    int lb1;
    int c7;
    int c5;
    int c3;
    int c1;
    if (length >= 3 && tsteps >= 1) {
      out = 0;
      for (c1 = 0; c1 <= tsteps + -1; ++c1) {
        for (c3 = 0; c3 <= length + -1; ++c3) {
          for (c5 = 0; c5 <= length + -1; ++c5) {
            (c[c3])[c5] = 0;
          }
        }
        for (c3 = 0; c3 <= length + -3; ++c3) {
          ((sum_c[c3])[c3 + 1])[c3] = 0;
          (c[c3])[c3 + 1] = ((((sum_c[c3])[c3 + 1])[c3 + 1 - 1]) + ((W[c3])[c3 + 1]));
          for (c5 = c3 + 2; c5 <= length + -1; ++c5) {
            ((sum_c[c3])[c5])[c3] = 0;
            for (c7 = c3 + 1; c7 <= c5 + -1; ++c7) {
              ((sum_c[c3])[c5])[c7] = (((((sum_c[c3])[c5])[c7 - 1]) + ((c[c3])[c7])) + ((c[c7])[c5]));
            }
            (c[c3])[c5] = ((((sum_c[c3])[c5])[c5 - 1]) + ((W[c3])[c5]));
          }
        }
        ((sum_c[length + -2])[length + -1])[length + -2] = 0;
        (c[length + -2])[length + -1] = ((((sum_c[length + -2])[length + -1])[length + -1 - 1]) + ((W[length + -2])[length + -1]));
        out += ((c[0])[length - 1]);
      }
    }
    if (length == 2 && tsteps >= 1) {
      out = 0;
      for (c1 = 0; c1 <= tsteps + -1; ++c1) {
        for (c3 = 0; c3 <= 1; ++c3) {
          for (c5 = 0; c5 <= 1; ++c5) {
            (c[c3])[c5] = 0;
          }
        }
        ((sum_c[0])[1])[0] = 0;
        (c[0])[1] = ((((sum_c[0])[1])[1 - 1]) + ((W[0])[1]));
        out += ((c[0])[length - 1]);
      }
    }
    if (length == 1 && tsteps >= 1) {
      out = 0;
      for (c1 = 0; c1 <= tsteps + -1; ++c1) {
        (c[0])[0] = 0;
        out += ((c[0])[length - 1]);
      }
    }
    if (length <= 0 && tsteps >= 1) {
      out = 0;
      for (c1 = 0; c1 <= tsteps + -1; ++c1) {
        out += ((c[0])[length - 1]);
      }
    }
    if (tsteps <= 0) {
      out = 0;
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

