#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "timer.h"

extern int bar1();

int main()
{
    	int i, j;
	int N = bar1();

	double cpu_start, cpu_end;
	double gpu_start, gpu_end;

	int ** f, ** cpu_f;

	f = (int**)malloc(N*sizeof(int*));
	cpu_f = (int**)malloc(N*sizeof(int*));
	for ( i=0;i<N;i++ ) {
		f[i] = (int*)malloc(N*sizeof(int));
		cpu_f[i] = (int*)malloc(N*sizeof(int));
	}

	for (i=0; i<N; i++) {
        	for (j=0; j<N; j++) {
            		f[i][j] = i*i+j*j;
        	}
   	}

	cpu_start = rtclock();
	for (i=1; i<=N-2; i+=1)  {
        	for (j=1; j<=N-2; j+=1)  {
            		cpu_f[i][j] = cpu_f[j][i] + cpu_f[i][j-1];
        	}
    	}
	cpu_end = rtclock();

	  printf("CPU time -> %0.6lfs\n", cpu_end - cpu_start);

gpu_start = rtclock();

#pragma rosePolly
{
    for (int i=1; i<N-1; i++)  {
        for (int j=1; j<N-1; j++)  {
            f[i][j] = f[j][i] + f[i][j-1];
        }
    }
}

gpu_end = rtclock();

printf("GPU time -> %0.6lfs\n", gpu_end - gpu_start);

for ( i = 1 ; i < N ; i++ )
	for ( j = 1 ; j < N ; j++ ) 
		assert(cpu_f[i][j]==f[i][j]);

for (i=0;i<N;i++) {
	free(f[i]);
	free(cpu_f[i]);
}
free(f);
free(cpu_f);

    return 0;
}
