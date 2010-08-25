/*
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is hereby granted,
 * provided that the above copyright notice appear and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that the name of CMU not
 * be used in advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "util.h"

void FLP02();
void FLP03();
void FLP30();
//void FLP31();
void FLP32();
void FLP33();
void FLP34();

void FLP() {
        FLP02();
        FLP03();
        FLP30();
//      FLP31();
        FLP32();
        FLP33();
        FLP34();
}


/* FLP02_A v.42 */

float FLP02_mean(int array[], int size) {
  int total = 0;
  int i;
  for (i = 0; i < size; i++) {
    total += array[i];
    printf("array[%d] = %f and total is %f\n", i, array[i] / 100.0, total / 100.0);
  }
  if (size != 0) {
    return total / size;
  } else {
    return 0.0;
  }
}

enum {array_size = 10};
int array_value = 1010;

void FLP02(void) {
  int array[array_size];
  float avg;
  int i;
  for (i = 0; i < array_size; i++) {
    array[i] = array_value;
  }

  avg = FLP02_mean( array, array_size);
  printf("mean is %f\n", avg / 100.0);
  if (((int)nearbyint(avg)) == array[0]) {
    printf("array[0] is the mean\n");
  } else {
    printf("array[0] is not the mean\n");
  }
}

/* FLP03_A v.40 */
/**
 * \bug Due to a bug in rose we can't use the pragma
 */
//
//#pragma STDC FENV_ACCESS ON
//
//void FLP03(void) {
//  double a = 1e-40, b, c = 0.1;
//  float x = 0, y;
//  int fpeRaised;
//  /* ... */
//
//  feclearexcept(FE_ALL_EXCEPT);
//  /* Store a into y is inexact and underflows: */
//  y = a;
//  fpeRaised = fetestexcept(FE_ALL_EXCEPT);
//  /* fpeRaised  has FE_INEXACT and FE_UNDERFLOW */
//
//  feclearexcept(FE_ALL_EXCEPT);
//
//  /* divide by zero operation */
//  b = y / x;
//  fpeRaised = fetestexcept(FE_ALL_EXCEPT);
//  /* fpeRaised has FE_DIVBYZERO */
//
//  feclearexcept(FE_ALL_EXCEPT);
//
//  c = sin(30) * a;
//  fpeRaised = fetestexcept(FE_ALL_EXCEPT);
//  /* fpeRaised has FE_INEXACT */
//
//  feclearexcept(FE_ALL_EXCEPT);
//  /* ... */
//
//  printf("FLP03 %f %f %d\n", b, c, fpeRaised);
//}
//
/* FLP30_A v.25 */

void FLP30(void) {
        int count;
        for (count = 1; count <= 10; count += 1) {
          const float x = count/10.0f;
          /* ... */
          printf("FLP30 %f\n", x);
        }

        for (count = 1; count <= 10; count += 1) {
          const double x = 100000000.0 + count;
          /* ... */
          printf("FLP30 %f\n", x);
        }
}

/* FLP31_A v.15 */

//void FLP31(void) {
//      double complex c = 2.0 + 4.0 * I;
//      /* ... */
//      double complex result = log2(creal(c));
//      printf("%f + %fi\n", creal(result), cimag(result));
//}

/* FLP32_A v.69 */

void FLP32(void) {
        const double x = 0.0f;
        const double y = 0.0f;
        double result;

        /* Set the value for x */

        if ( isnan(x) || isless(x,-1) || isgreater(x, 1) ){
                 /* handle domain error */
        }
        result = acos(x);

        if ( (x == 0.f) && (y == 0.f) ) {
                /* handle domain error */
        }
        result = atan2(y, x);

        if (isnan(x) || islessequal(x, 0)) {
                /* handle domain and range errors */
        }
        result = log(x);

        if (isless(x, 0)){
                /* handle domain error */
        }
        result = sqrt(x);
        printf("FL32 %f\n", result);
}

/* FLP33_A v.38 */

void FLP33(void) {
        const short a = 533;
        const int b = 6789;
        const long c = 466438237;

        float d = a / 7.0f; /* d is 76.14286 */
        double e = b / 30.; /* e is 226.3 */
        double f = (double)c * 789; /* f is 368019768993.0 */

        d /= 7; /* d is 76.14286 */
        e /= 30; /* e is 226.3 */
        f *= 789; /* f is 368019768993.0 */

}

/* FLP34_A v.43 */

void FLP34(void) {
        const long double ld = 0.0;
        const double d1 = 0.0;
        double d2 = 0.0;
        float f1 = 0.0f;
        float f2 = 0.0f;
        int i1;

        if (f1 > (float) INT_MAX || f1 < (float) INT_MIN) {
                /* Handle Error */
        }
        i1 = (int)f1;

        if (d1 > FLT_MAX || d1 < -FLT_MAX) {
                /* Handle error condition */
        }
        f1 = (float)d1;

        if (ld > FLT_MAX || ld < -FLT_MAX) {
                /* Handle error condition */
        }
        f2 = (float)ld;

        if (ld > DBL_MAX || ld < -DBL_MAX) {
                /* Handle error condition */
        }
        d2 = (double)ld;

        /* End {code} */

        printf("FLP34 %f %f %d\n", d2, f2, i1);
}
