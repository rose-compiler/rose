#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "timer.h"

extern int bar1();
extern int bar2();

int main()
{
    int i, j, t;

	double cpu_start, cpu_end;
	double gpu_start, gpu_end;

	int T = bar1();
	int N = bar2();

	int ** a, ** cpu_a;

	a = (int**)malloc(N*sizeof(int*));
	cpu_a = (int**)malloc(N*sizeof(int*));

	for ( i = 0 ; i < N ; i++ ) {
		a[i] = (int*)malloc(N*sizeof(int));
		cpu_a[i] = (int*)malloc(N*sizeof(int));
	}

	for (i=0; i<N; i++) {
        	for (j=0; j<N; j++) {
            		a[i][j] = i*i+j*j;
			cpu_a[i][j] = a[i][j];
        	}
    	}

	cpu_start = rtclock();
	for (t=0; t<=T-1; t+=1)  {
        	for (i=1; i<=N-2; i+=1)  {
            		for (j=1; j<=N-2; j+=1)  {
                		cpu_a[i][j] = (cpu_a[i-1][j-1] + cpu_a[i-1][j] + cpu_a[i-1][j+1] 
                        	+ cpu_a[i][j-1] + cpu_a[i][j] + cpu_a[i][j+1]
                        	+ cpu_a[i+1][j-1] + cpu_a[i+1][j] + cpu_a[i+1][j+1])/9;
            		}
        	}
    	}
	cpu_end = rtclock();

	  printf("CPU time -> %0.6lfs\n", cpu_end - cpu_start);

gpu_start = rtclock();

#pragma rosePolly
{
    for (int t=0; t<T; t++)  {
        for (int i=1; i<N-1; i++)  {
            for (int j=1; j<N-1; j++)  {
                a[i][j] = (a[i-1][j-1] + a[i-1][j] + a[i-1][j+1] 
                        + a[i][j-1] + a[i][j] + a[i][j+1]
                        + a[i+1][j-1] + a[i+1][j] + a[i+1][j+1])/9;
            }
        }
    }
}

gpu_end = rtclock();

printf("GPU time -> %0.6lfs\n", gpu_end - gpu_start);

for ( i = 1 ; i < N ; i++ )
	for ( j = 1 ; j < N ; j++ ) 
		assert(cpu_a[i][j]==a[i][j]);

for ( i = 0 ; i < N ; i++ ) {
	free(a[i]);
	free(cpu_a[i]);
}

free(a);
free(cpu_a);

    return 0;
}
