#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef TMAX
# define TMAX 50
#endif
#ifndef NX
# define NX 1024
#endif
#ifndef NY
# define NY 1024
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
double _fict_[50UL];
double ex[1024UL][1024UL];
double ey[1024UL][1024UL];
double hz[1024UL][1024UL];
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
      _fict_[c1] = ((double )c1);
    }
    for (c1 = 0; c1 <= 1023; ++c1) {
      for (c3 = 0; c3 <= 1023; ++c3) {
        (ex[c1])[c3] = (((((double )c1) * (c3)) + (1)) / (1024));
        (ey[c1])[c3] = (((((double )c1) * (c3)) + (2)) / (1024));
        (hz[c1])[c3] = (((((double )c1) * (c3)) + (3)) / (1024));
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
    for (i = 0; i < 1024; i++) 
      for (j = 0; j < 1024; j++) {
        fprintf(stderr,("%0.2lf "),((ex[i])[j]));
        fprintf(stderr,("%0.2lf "),((ey[i])[j]));
        fprintf(stderr,("%0.2lf "),((hz[i])[j]));
        if ((((i * 1024) + j) % 80) == 20) 
          fprintf(stderr,("\n"));
      }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int t;
  int i;
  int j;
  int tmax = 50;
  int nx = 1024;
  int ny = 1024;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out ex, ey, hz
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (nx >= 2 && ny >= 2 && tmax >= 1) {
      for (c1 = 0; c1 <= tmax + -1; ++c1) {
        for (c3 = 0; c3 <= ny + -1; ++c3) {
          (ey[0])[c3] = (_fict_[c1]);
        }
        for (c3 = 1; c3 <= nx + -1; ++c3) {
          for (c5 = 0; c5 <= ny + -1; ++c5) {
            (ey[c3])[c5] = (((ey[c3])[c5]) - (0.5 * (((hz[c3])[c5]) - ((hz[c3 - 1])[c5]))));
          }
        }
        for (c3 = 0; c3 <= nx + -1; ++c3) {
          for (c5 = 1; c5 <= ny + -1; ++c5) {
            (ex[c3])[c5] = (((ex[c3])[c5]) - (0.5 * (((hz[c3])[c5]) - ((hz[c3])[c5 - 1]))));
          }
        }
        for (c3 = 0; c3 <= nx + -2; ++c3) {
          for (c5 = 0; c5 <= ny + -2; ++c5) {
            (hz[c3])[c5] = (((hz[c3])[c5]) - (0.7 * (((((ex[c3])[c5 + 1]) - ((ex[c3])[c5])) + ((ey[c3 + 1])[c5])) - ((ey[c3])[c5]))));
          }
        }
      }
    }
    if (nx >= 2 && ny == 1 && tmax >= 1) {
      for (c1 = 0; c1 <= tmax + -1; ++c1) {
        (ey[0])[0] = (_fict_[c1]);
        for (c3 = 1; c3 <= nx + -1; ++c3) {
          (ey[c3])[0] = (((ey[c3])[0]) - (0.5 * (((hz[c3])[0]) - ((hz[c3 - 1])[0]))));
        }
      }
    }
    if (nx == 1 && ny >= 2 && tmax >= 1) {
      for (c1 = 0; c1 <= tmax + -1; ++c1) {
        for (c3 = 0; c3 <= ny + -1; ++c3) {
          (ey[0])[c3] = (_fict_[c1]);
        }
        for (c5 = 1; c5 <= ny + -1; ++c5) {
          (ex[0])[c5] = (((ex[0])[c5]) - (0.5 * (((hz[0])[c5]) - ((hz[0])[c5 - 1]))));
        }
      }
    }
    if (nx <= 0 && ny >= 2 && tmax >= 1) {
      for (c1 = 0; c1 <= tmax + -1; ++c1) {
        for (c3 = 0; c3 <= ny + -1; ++c3) {
          (ey[0])[c3] = (_fict_[c1]);
        }
      }
    }
    if (nx <= 1 && ny == 1 && tmax >= 1) {
      for (c1 = 0; c1 <= tmax + -1; ++c1) {
        (ey[0])[0] = (_fict_[c1]);
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

