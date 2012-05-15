#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <assert.h>

#ifdef PERFCTR
#include <papi.h>
#include "papi_defs.h"
#endif

#include "decls.h"

#include "util.h"

double t_start, t_end;

extern int bar();

int main()
{
	int t, i, j, k, l, m, n;

int ttmax = bar();
int nny = bar();
int nnx = bar();

	init_array() ;

#ifdef PERFCTR
    PERF_INIT;
#endif

	IF_TIME(t_start = rtclock());

#pragma rosePolly
{
    for(int t=0; t<ttmax; t++)  {
        for (int j=0; j<nny; j++)
            ey[0][j] = t;
        for (int i=1; i<nnx; i++)
            for (int j=0; j<nny; j++)
                ey[i][j] = ey[i][j] - 0.5*(hz[i][j]-hz[i-1][j]);
        for (int i=0; i<nnx; i++)
            for (int j=1; j<nny; j++)
                ex[i][j] = ex[i][j] - 0.5*(hz[i][j]-hz[i][j-1]);
        for (int i=0; i<nnx; i++)
            for (int j=0; j<nny; j++)
                hz[i][j]=hz[i][j]-0.7*(ex[i][j+1]-ex[i][j]+ey[i+1][j]-ey[i][j]);
    }
}

    IF_TIME(t_end = rtclock());
    IF_TIME(printf("%0.6lfs\n", t_end - t_start));

#ifdef PERFCTR
    PERF_EXIT;
#endif
#ifdef TEST
    print_array();
#endif
    return 0;
}
