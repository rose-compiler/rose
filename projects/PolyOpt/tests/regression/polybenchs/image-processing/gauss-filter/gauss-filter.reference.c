#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef M
# define M 1920
#endif
#ifndef N
# define N 1080
#endif
#ifndef T
# define T 1920
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
int tot[4UL];
int Gauss[4UL];
int g_tmp_image[1080UL][1920UL];
int g_acc1[1080UL][1920UL][4UL];
int g_acc2[1080UL][1920UL][4UL];
//input
int in_image[1080UL][1920UL];
//output
int gauss_image[1080UL][1920UL];
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
    for (c1 = 0; c1 <= 1079; ++c1) {
      for (c3 = 0; c3 <= 1919; ++c3) {
        (in_image[c1])[c3] = ((((int )c1) * c3) / 1920);
      }
    }
    for (c1 = 0; c1 <= 3; ++c1) {
      Gauss[c1] = c1;
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
    for (i = 0; i < 1080; i++) 
      for (j = 0; j < 1920; j++) {
        fprintf(stderr,("%d "),((gauss_image[i])[j]));
        if ((((i * 1080) + j) % 80) == 20) 
          fprintf(stderr,("\n"));
      }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int x;
  int y;
  int k;
  int t = 1920;
  int m = 1920;
  int n = 1080;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out gauss_image
{
    int ub1;
    int lb1;
    int c5;
    int c3;
    int c1;
    if (m >= 3 && n >= 4) {
      tot[0] = 0;
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c1 = 1; c1 <= n + -3; ++c1) {
        for (c3 = 0; c3 <= m + -1; ++c3) {
          ((g_acc1[c1])[c3])[0] = 0;
          for (c5 = t + -1; c5 <= t + 1; ++c5) {
            ((g_acc1[c1])[c3])[(c5 + 2) - t] = ((((g_acc1[c1])[c3])[(c5 + 1) - t]) + (((in_image[(c1 + c5) - t])[c3]) * (Gauss[(c5 - t) + 1])));
          }
          (g_tmp_image[c1])[c3] = ((((g_acc1[c1])[c3])[3]) / (tot[3]));
        }
      }
      for (c1 = 1; c1 <= n + -2; ++c1) {
        for (c3 = 1; c3 <= m + -2; ++c3) {
          ((g_acc2[c1])[c3])[0] = 0;
          for (c5 = t + -1; c5 <= t + 1; ++c5) {
            ((g_acc2[c1])[c3])[(c5 + 2) - t] = ((((g_acc2[c1])[c3])[(c5 + 1) - t]) + (((g_tmp_image[c1])[(c3 + c5) - t]) * (Gauss[(c5 - t) + 1])));
          }
          (gauss_image[c1])[c3] = ((((g_acc2[c1])[c3])[3]) / (tot[3]));
        }
      }
    }
    if (m >= 1 && m <= 2 && n >= 4) {
      tot[0] = 0;
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c1 = 1; c1 <= n + -3; ++c1) {
        for (c3 = 0; c3 <= m + -1; ++c3) {
          ((g_acc1[c1])[c3])[0] = 0;
          for (c5 = t + -1; c5 <= t + 1; ++c5) {
            ((g_acc1[c1])[c3])[(c5 + 2) - t] = ((((g_acc1[c1])[c3])[(c5 + 1) - t]) + (((in_image[(c1 + c5) - t])[c3]) * (Gauss[(c5 - t) + 1])));
          }
          (g_tmp_image[c1])[c3] = ((((g_acc1[c1])[c3])[3]) / (tot[3]));
        }
      }
    }
    if (m <= 0 && n >= 4) {
      tot[0] = 0;
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
    }
    if (m >= 3 && n == 3) {
      tot[0] = 0;
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c3 = 1; c3 <= m + -2; ++c3) {
        ((g_acc2[1])[c3])[0] = 0;
        for (c5 = t + -1; c5 <= t + 1; ++c5) {
          ((g_acc2[1])[c3])[(c5 + 2) - t] = ((((g_acc2[1])[c3])[(c5 + 1) - t]) + (((g_tmp_image[1])[(c3 + c5) - t]) * (Gauss[(c5 - t) + 1])));
        }
        (gauss_image[1])[c3] = ((((g_acc2[1])[c3])[3]) / (tot[3]));
      }
    }
    if (m <= 2 && n == 3) {
      tot[0] = 0;
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
    }
    if (n <= 2) {
      tot[0] = 0;
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
      for (c1 = t + -1; c1 <= t + 1; ++c1) {
        tot[(c1 + 2) - t] = ((tot[(c1 + 1) - t]) + (Gauss[(c1 - t) + 1]));
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

