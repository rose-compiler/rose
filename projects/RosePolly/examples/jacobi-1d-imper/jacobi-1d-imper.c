#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "decls.h"
#include "util.h"

double t_start, t_end;

extern int bar();

int main()
{
    int t, i, j;

	int TT = bar();
	int NN = bar();

    init_array();

    IF_TIME(t_start = rtclock());

#pragma rosePolly
{
    for (int t = 0; t < TT; t++) {
        for (int i = 2; i < NN - 1; i++) {
            b[i] = 0.33333 * (a[i-1] + a[i] + a[i + 1]);
        }
        for (int j = 2; j < NN - 1; j++) {
            a[j] = b[j];
        }
    }
}

    IF_TIME(t_end = rtclock());
    IF_TIME(printf("%0.6lfs\n", t_end - t_start));

#ifdef TEST
    print_array();
#endif

    return 0;
}
