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

int main()
{
	int t, i, j, k, l, m, n;


	init_array() ;

#ifdef PERFCTR
    PERF_INIT;
#endif

	IF_TIME(t_start = rtclock());

#pragma scop
    for(t=0; t<tmax; t++)  {
        for (j=0; j<ny; j++)
            ey[0][j] = t;
        for (i=1; i<nx; i++)
            for (j=0; j<ny; j++)
                ey[i][j] = ey[i][j] - 0.5*(hz[i][j]-hz[i-1][j]);
        for (i=0; i<nx; i++)
            for (j=1; j<ny; j++)
                ex[i][j] = ex[i][j] - 0.5*(hz[i][j]-hz[i][j-1]);
        for (i=0; i<nx; i++)
            for (j=0; j<ny; j++)
                hz[i][j]=hz[i][j]-0.7*(ex[i][j+1]-ex[i][j]+ey[i+1][j]-ey[i][j]);
    }
#pragma endscop

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

