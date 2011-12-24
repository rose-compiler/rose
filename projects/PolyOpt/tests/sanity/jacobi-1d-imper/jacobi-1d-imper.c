#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "decls.h"
#include "util.h"

double t_start, t_end;

int main()
{
    int t, i, j;

    init_array();

    IF_TIME(t_start = rtclock());

#pragma scop
    for (t = 0; t < T; t++) {
        for (i = 2; i < N - 1; i++) {
            b[i] = 0.33333 * (a[i-1] + a[i] + a[i + 1]);
        }
        for (j = 2; j < N - 1; j++) {
            a[j] = b[j];
        }
    }
#pragma endscop

    IF_TIME(t_end = rtclock());
    IF_TIME(printf("%0.6lfs\n", t_end - t_start));

#ifdef TEST
    print_array();
#endif

    return 0;
}
