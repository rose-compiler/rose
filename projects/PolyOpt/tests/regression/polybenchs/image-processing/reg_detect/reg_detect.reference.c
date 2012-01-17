#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef LENGTH
# define LENGTH 64
#endif
#ifndef MAXGRID
# define MAXGRID 6
#endif
#ifndef MAXRGC
# define MAXRGC ((MAXGRID - 1) * (MAXGRID - 1) * MAXGRID)
#endif
#ifndef NITER
# define NITER 100000
#endif
/* Default data type is int. */
#ifndef DATA_TYPE
# define DATA_TYPE int
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%d "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
int s;
#ifndef POLYBENCH_TEST_MALLOC
int sum_tang[6UL][6UL];
int mean[6UL][6UL];
int diff[6UL][6UL][64UL];
int sum_diff[6UL][6UL][64UL];
//input
int tangent[150UL];
//output
int path[6UL][6UL];
#else
#endif

inline void init_array()
{
  int i;
{
    int ub1;
    int lb1;
    int c1;
    for (c1 = 0; c1 <= 149; ++c1) {
      tangent[c1] = (((int )c1) + 42);
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
    fprintf(stderr,("%d "),s);
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int t;
  int i;
  int j;
  int cnt;
  int length = 64;
  int maxgrid = 6;
  int niter = 100000;
/* Initialize array. */
  init_array();
/* Start timer. */
  s = 0;
  for (j = 0; j <= (maxgrid - 1); j++) {
    (sum_tang[j])[j] = (tangent[(maxgrid + 1) * j]);
    for (i = (j + 1); i <= (maxgrid - 1); i++) 
      (sum_tang[j])[i] = (((sum_tang[j])[i - 1]) + (tangent[i + (maxgrid * j)]));
  }
  
#pragma scop
  
#pragma live - out s
{
    int ub1;
    int lb1;
    int c7;
    int c5;
    int c3;
    int c1;
    if (length >= 2 && maxgrid >= 2 && niter >= 1) {
      for (c1 = 0; c1 <= niter + -1; ++c1) {
        for (c3 = 0; c3 <= maxgrid + -1; ++c3) {
          for (c5 = c3; c5 <= maxgrid + -1; ++c5) {
            for (c7 = 0; c7 <= length + -1; ++c7) {
              ((diff[c3])[c5])[c7] = ((sum_tang[c3])[c5]);
            }
          }
        }
        for (c3 = 0; c3 <= maxgrid + -1; ++c3) {
          for (c5 = c3; c5 <= maxgrid + -1; ++c5) {
            ((sum_diff[c3])[c5])[0] = (((diff[c3])[c5])[0]);
            for (c7 = 1; c7 <= length + -1; ++c7) {
              ((sum_diff[c3])[c5])[c7] = ((((sum_diff[c3])[c5])[c7 - 1]) + (((diff[c3])[c5])[c7]));
            }
            (mean[c3])[c5] = (((sum_diff[c3])[c5])[length - 1]);
          }
        }
        for (c3 = 0; c3 <= maxgrid + -1; ++c3) {
          (path[0])[c3] = ((mean[0])[c3]);
        }
        for (c3 = 1; c3 <= maxgrid + -1; ++c3) {
          for (c5 = c3; c5 <= maxgrid + -1; ++c5) {
            (path[c3])[c5] = (((path[c3 - 1])[c5 - 1]) + ((mean[c3])[c5]));
          }
        }
        s += ((path[maxgrid - 1])[1]);
      }
    }
    if (length >= 2 && maxgrid == 1 && niter >= 1) {
      for (c1 = 0; c1 <= niter + -1; ++c1) {
        for (c7 = 0; c7 <= length + -1; ++c7) {
          ((diff[0])[0])[c7] = ((sum_tang[0])[0]);
        }
        ((sum_diff[0])[0])[0] = (((diff[0])[0])[0]);
        for (c7 = 1; c7 <= length + -1; ++c7) {
          ((sum_diff[0])[0])[c7] = ((((sum_diff[0])[0])[c7 - 1]) + (((diff[0])[0])[c7]));
        }
        (mean[0])[0] = (((sum_diff[0])[0])[length - 1]);
        (path[0])[0] = ((mean[0])[0]);
        s += ((path[maxgrid - 1])[1]);
      }
    }
    if (length == 1 && maxgrid >= 2 && niter >= 1) {
      for (c1 = 0; c1 <= niter + -1; ++c1) {
        for (c3 = 0; c3 <= maxgrid + -1; ++c3) {
          for (c5 = c3; c5 <= maxgrid + -1; ++c5) {
            ((diff[c3])[c5])[0] = ((sum_tang[c3])[c5]);
          }
        }
        for (c3 = 0; c3 <= maxgrid + -1; ++c3) {
          for (c5 = c3; c5 <= maxgrid + -1; ++c5) {
            ((sum_diff[c3])[c5])[0] = (((diff[c3])[c5])[0]);
            (mean[c3])[c5] = (((sum_diff[c3])[c5])[length - 1]);
          }
        }
        for (c3 = 0; c3 <= maxgrid + -1; ++c3) {
          (path[0])[c3] = ((mean[0])[c3]);
        }
        for (c3 = 1; c3 <= maxgrid + -1; ++c3) {
          for (c5 = c3; c5 <= maxgrid + -1; ++c5) {
            (path[c3])[c5] = (((path[c3 - 1])[c5 - 1]) + ((mean[c3])[c5]));
          }
        }
        s += ((path[maxgrid - 1])[1]);
      }
    }
    if (length == 1 && maxgrid == 1 && niter >= 1) {
      for (c1 = 0; c1 <= niter + -1; ++c1) {
        ((diff[0])[0])[0] = ((sum_tang[0])[0]);
        ((sum_diff[0])[0])[0] = (((diff[0])[0])[0]);
        (mean[0])[0] = (((sum_diff[0])[0])[length - 1]);
        (path[0])[0] = ((mean[0])[0]);
        s += ((path[maxgrid - 1])[1]);
      }
    }
    if (length <= 0 && maxgrid >= 2 && niter >= 1) {
      for (c1 = 0; c1 <= niter + -1; ++c1) {
        for (c3 = 0; c3 <= maxgrid + -1; ++c3) {
          for (c5 = c3; c5 <= maxgrid + -1; ++c5) {
            ((sum_diff[c3])[c5])[0] = (((diff[c3])[c5])[0]);
            (mean[c3])[c5] = (((sum_diff[c3])[c5])[length - 1]);
          }
        }
        for (c3 = 0; c3 <= maxgrid + -1; ++c3) {
          (path[0])[c3] = ((mean[0])[c3]);
        }
        for (c3 = 1; c3 <= maxgrid + -1; ++c3) {
          for (c5 = c3; c5 <= maxgrid + -1; ++c5) {
            (path[c3])[c5] = (((path[c3 - 1])[c5 - 1]) + ((mean[c3])[c5]));
          }
        }
        s += ((path[maxgrid - 1])[1]);
      }
    }
    if (length <= 0 && maxgrid == 1 && niter >= 1) {
      for (c1 = 0; c1 <= niter + -1; ++c1) {
        ((sum_diff[0])[0])[0] = (((diff[0])[0])[0]);
        (mean[0])[0] = (((sum_diff[0])[0])[length - 1]);
        (path[0])[0] = ((mean[0])[0]);
        s += ((path[maxgrid - 1])[1]);
      }
    }
    if (maxgrid <= 0 && niter >= 1) {
      for (c1 = 0; c1 <= niter + -1; ++c1) {
        s += ((path[maxgrid - 1])[1]);
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

