#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef NR
# define NR 128
#endif
#ifndef NQ
# define NQ 128
#endif
#ifndef NP
# define NP 128
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
double A[128UL][128UL][128UL];
double sum[128UL][128UL][128UL];
double C4[128UL][128UL];
#else
#endif

inline void init_array()
{
  int i;
  int j;
  int k;
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    for (c1 = 0; c1 <= 127; ++c1) {
      for (c3 = 0; c3 <= 127; ++c3) {
        for (c5 = 0; c5 <= 127; ++c5) {
          ((A[c1])[c3])[c5] = (((((double )c1) * (c3)) + (c5)) / (128));
        }
      }
    }
    for (c1 = 0; c1 <= 127; ++c1) {
      for (c3 = 0; c3 <= 127; ++c3) {
        (C4[c1])[c3] = ((((double )c1) * (c3)) / (128));
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
  int k;
#ifndef POLYBENCH_DUMP_ARRAYS
  if ((argc > 42) && !(strcmp((argv[0]),"") != 0)) 
#endif
{
    for (i = 0; i < 128; i++) 
      for (j = 0; j < 128; j++) 
        for (k = 0; k < 128; k++) {
          fprintf(stderr,("%0.2lf "),(((A[i])[j])[k]));
          if (((((i * 128) + (j * 128)) + k) % 80) == 20) 
            fprintf(stderr,("\n"));
        }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int r;
  int q;
  int p;
  int s;
  int nr = 128;
  int nq = 128;
  int np = 128;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out A
{
    int ub1;
    int lb1;
    int c7;
    int c5;
    int c3;
    int c1;
    if (np >= 1 && nq >= 1 && nr >= 1) {
      for (c1 = 0; c1 <= nr + -1; ++c1) {
        for (c3 = 0; c3 <= nq + -1; ++c3) {
          for (c5 = 0; c5 <= np + -1; ++c5) {
            ((sum[c1])[c3])[c5] = (0);
            for (c7 = 0; c7 <= np + -1; ++c7) {
              ((sum[c1])[c3])[c5] = ((((sum[c1])[c3])[c5]) + ((((A[c1])[c3])[c7]) * ((C4[c7])[c5])));
            }
          }
          for (c5 = 0; c5 <= np + -1; ++c5) {
            ((A[c1])[c3])[c5] = (((sum[c1])[c3])[c5]);
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

