/**
 * sum2pi_x.c
 *
 * CRAFT demo app. Calculates pi*x in a computationally-heavy way that
 * demonstrates how to use CRAFT without being too complicated.
 *
 * September 2013
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* macros */
#define ABS(x) ( ((x) < 0.0) ? (-(x)) : (x) )

/* constants */
#define PI     3.1415926535897932384626433832795
#define EPS    5e-7

/* loop  iterations; OUTER is X */
#define INNER    25
#define OUTER    500000

/* 'real' is double-precision if not pre-defined */
#ifndef real
#define real double
#endif

/* sum type is the same as 'real' if not pre-defined */
#ifndef sum_type
#define sum_type real
#endif

real pi = PI;
real tmp, acc;
sum_type sum = 0.0;

real answer = (real)OUTER * PI;  /* correct answer */

void sum2pi_x()
{
    int i, j;
    for (i=0; i<OUTER; i++) {
        acc = 0.0;
        for (j=1; j<INNER; j++) {

            /* accumulatively calculate pi */
            tmp = (real)pi / pow(2.0,j);
            acc += tmp;
        }
        sum += acc;
    }
}

int main()
{
    printf("===  Sum2PI_X  ===\n");
    printf("sizeof(real)=%lu\n",     sizeof(real));
    printf("sizeof(sum_type)=%lu\n", sizeof(sum_type));

    sum2pi_x();

    double err = ABS((double)answer-(double)sum);

    printf("  RESULT:  %.6e\n", sum);
    printf("  CORRECT: %.6e\n", answer);
    printf("  ERROR:   %.6e\n", err);
    printf("  THRESH:  %.6e\n", EPS*answer);

    if ((double)err < (double)EPS*answer) {
        printf("SUM2PI_X - SUCCESSFUL!\n");
    } else {
        printf("SUM2PI_X - FAILED!!!\n");
    }

    return 0;
}

