#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "instrument.h"
/* Default problem size. */
#ifndef CZ
# define CZ 256
#endif
#ifndef CYM
# define CYM 256
#endif
#ifndef CXM
# define CXM 256
#endif
/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif
/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
double ch;
double mui;
#ifndef POLYBENCH_TEST_MALLOC
double Ex[257UL][257UL][257UL];
double Ey[257UL][257UL][257UL];
double Bza[257UL][257UL][257UL];
double Hz[257UL][257UL][257UL];
double clf[257UL][257UL];
double tmp[257UL][257UL];
double Ry[257UL][257UL];
double Ax[257UL][257UL];
double cymh[257UL];
double cyph[257UL];
double cxmh[257UL];
double cxph[257UL];
double czm[257UL];
double czp[257UL];
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
    mui = (2341);
    ch = (42);
    for (c1 = 0; c1 <= 256; ++c1) {
      czm[c1] = ((((double )c1) + (1)) / (256));
      czp[c1] = ((((double )c1) + (2)) / (256));
    }
    for (c1 = 0; c1 <= 256; ++c1) {
      cxmh[c1] = ((((double )c1) + (3)) / (256));
      cxph[c1] = ((((double )c1) + (4)) / (256));
    }
    for (c1 = 0; c1 <= 256; ++c1) {
      cymh[c1] = ((((double )c1) + (5)) / (256));
      cyph[c1] = ((((double )c1) + (6)) / (256));
    }
    for (c1 = 0; c1 <= 256; ++c1) {
      for (c3 = 0; c3 <= 256; ++c3) {
        (Ry[c1])[c3] = (((((double )c1) * (c3)) + (10)) / (256));
        (Ax[c1])[c3] = (((((double )c1) * (c3)) + (11)) / (256));
        for (c5 = 0; c5 <= 256; ++c5) {
          ((Ex[c1])[c3])[c5] = ((((((double )c1) * (c3)) + (c5)) + (1)) / (256));
          ((Ey[c1])[c3])[c5] = ((((((double )c1) * (c3)) + (c5)) + (2)) / (256));
          ((Hz[c1])[c3])[c5] = ((((((double )c1) * (c3)) + (c5)) + (3)) / (256));
        }
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
    for (i = 0; i <= 256; i++) 
      for (j = 0; j <= 256; j++) 
        for (k = 0; k <= 256; k++) {
          fprintf(stderr,("%0.2lf "),(((Bza[i])[j])[k]));
          fprintf(stderr,("%0.2lf "),(((Ex[i])[j])[k]));
          fprintf(stderr,("%0.2lf "),(((Ey[i])[j])[k]));
          fprintf(stderr,("%0.2lf "),(((Hz[i])[j])[k]));
          if ((((i * 256) + j) % 80) == 20) 
            fprintf(stderr,("\n"));
        }
    fprintf(stderr,("\n"));
  }
}


int main(int argc,char **argv)
{
  int iz;
  int iy;
  int ix;
  int Cz = 256;
  int Cym = 256;
  int Cxm = 256;
/* Initialize array. */
  init_array();
/* Start timer. */
  
#pragma scop
  
#pragma live - out Bza, Hz
  for (iz = 0; iz < Cz; iz++) {{
      int ub1;
      int lb1;
      int c3;
      int c1;
      if (Cxm >= 1 && Cym >= 1) {
        for (c1 = 0; c1 <= Cym + -1; ++c1) {
          for (c3 = 0; c3 <= Cxm + -1; ++c3) {
            (clf[iz])[c1] = ((((((Ex[iz])[c1])[c3]) - (((Ex[iz])[c1 + 1])[c3])) + (((Ey[iz])[c1])[c3 + 1])) - (((Ey[iz])[c1])[c3]));
            (tmp[iz])[c1] = ((((cymh[c1]) / (cyph[c1])) * (((Bza[iz])[c1])[c3])) - ((ch / (cyph[c1])) * ((clf[iz])[c1])));
            ((Hz[iz])[c1])[c3] = (((((cxmh[c3]) / (cxph[c3])) * (((Hz[iz])[c1])[c3])) + (((mui * (czp[iz])) / (cxph[c3])) * ((tmp[iz])[c1]))) - (((mui * (czm[iz])) / (cxph[c3])) * (((Bza[iz])[c1])[c3])));
            ((Bza[iz])[c1])[c3] = ((tmp[iz])[c1]);
          }
          (clf[iz])[c1] = ((((((Ex[iz])[c1])[Cxm]) - (((Ex[iz])[c1 + 1])[Cxm])) + ((Ry[iz])[c1])) - (((Ey[iz])[c1])[Cxm]));
          (tmp[iz])[c1] = ((((cymh[c1]) / (cyph[c1])) * (((Bza[iz])[c1])[Cxm])) - ((ch / (cyph[c1])) * ((clf[iz])[c1])));
          ((Hz[iz])[c1])[Cxm] = (((((cxmh[Cxm]) / (cxph[Cxm])) * (((Hz[iz])[c1])[Cxm])) + (((mui * (czp[iz])) / (cxph[Cxm])) * ((tmp[iz])[c1]))) - (((mui * (czm[iz])) / (cxph[Cxm])) * (((Bza[iz])[c1])[Cxm])));
          ((Bza[iz])[c1])[Cxm] = ((tmp[iz])[c1]);
        }
      }
      if (Cxm <= 0 && Cym >= 1) {
        for (c1 = 0; c1 <= Cym + -1; ++c1) {
          (clf[iz])[c1] = ((((((Ex[iz])[c1])[Cxm]) - (((Ex[iz])[c1 + 1])[Cxm])) + ((Ry[iz])[c1])) - (((Ey[iz])[c1])[Cxm]));
          (tmp[iz])[c1] = ((((cymh[c1]) / (cyph[c1])) * (((Bza[iz])[c1])[Cxm])) - ((ch / (cyph[c1])) * ((clf[iz])[c1])));
          ((Hz[iz])[c1])[Cxm] = (((((cxmh[Cxm]) / (cxph[Cxm])) * (((Hz[iz])[c1])[Cxm])) + (((mui * (czp[iz])) / (cxph[Cxm])) * ((tmp[iz])[c1]))) - (((mui * (czm[iz])) / (cxph[Cxm])) * (((Bza[iz])[c1])[Cxm])));
          ((Bza[iz])[c1])[Cxm] = ((tmp[iz])[c1]);
        }
      }
    }
{
      int ub1;
      int lb1;
      int c1;
      if (Cxm >= 1) {
        for (c1 = 0; c1 <= Cxm + -1; ++c1) {
          (clf[iz])[iy] = ((((((Ex[iz])[Cym])[c1]) - ((Ax[iz])[c1])) + (((Ey[iz])[Cym])[c1 + 1])) - (((Ey[iz])[Cym])[c1]));
          (tmp[iz])[iy] = ((((cymh[Cym]) / (cyph[iy])) * (((Bza[iz])[iy])[c1])) - ((ch / (cyph[iy])) * ((clf[iz])[iy])));
          ((Hz[iz])[Cym])[c1] = (((((cxmh[c1]) / (cxph[c1])) * (((Hz[iz])[Cym])[c1])) + (((mui * (czp[iz])) / (cxph[c1])) * ((tmp[iz])[iy]))) - (((mui * (czm[iz])) / (cxph[c1])) * (((Bza[iz])[Cym])[c1])));
          ((Bza[iz])[Cym])[c1] = ((tmp[iz])[iy]);
        }
        (clf[iz])[iy] = ((((((Ex[iz])[Cym])[Cxm]) - ((Ax[iz])[Cxm])) + ((Ry[iz])[Cym])) - (((Ey[iz])[Cym])[Cxm]));
        (tmp[iz])[iy] = ((((cymh[Cym]) / (cyph[Cym])) * (((Bza[iz])[Cym])[Cxm])) - ((ch / (cyph[Cym])) * ((clf[iz])[iy])));
        ((Hz[iz])[Cym])[Cxm] = (((((cxmh[Cxm]) / (cxph[Cxm])) * (((Hz[iz])[Cym])[Cxm])) + (((mui * (czp[iz])) / (cxph[Cxm])) * ((tmp[iz])[iy]))) - (((mui * (czm[iz])) / (cxph[Cxm])) * (((Bza[iz])[Cym])[Cxm])));
        ((Bza[iz])[Cym])[Cxm] = ((tmp[iz])[iy]);
      }
      if (Cxm <= 0) {
        (clf[iz])[iy] = ((((((Ex[iz])[Cym])[Cxm]) - ((Ax[iz])[Cxm])) + ((Ry[iz])[Cym])) - (((Ey[iz])[Cym])[Cxm]));
        (tmp[iz])[iy] = ((((cymh[Cym]) / (cyph[Cym])) * (((Bza[iz])[Cym])[Cxm])) - ((ch / (cyph[Cym])) * ((clf[iz])[iy])));
        ((Hz[iz])[Cym])[Cxm] = (((((cxmh[Cxm]) / (cxph[Cxm])) * (((Hz[iz])[Cym])[Cxm])) + (((mui * (czp[iz])) / (cxph[Cxm])) * ((tmp[iz])[iy]))) - (((mui * (czm[iz])) / (cxph[Cxm])) * (((Bza[iz])[Cym])[Cxm])));
        ((Bza[iz])[Cym])[Cxm] = ((tmp[iz])[iy]);
      }
    }
  }
  
#pragma endscop
/* Stop and print timer. */
  print_array(argc,argv);
  return 0;
}

