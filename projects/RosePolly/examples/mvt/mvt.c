#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "timer.h"

extern int bar1();

int main()
{
    int i, j;

	int N = bar();

	double cpu_start, cpu_end;
	double gpu_start, gpu_end;

	int ** a;
	int * x1, * x2, * y_1, * y_2;
	int * cpu_x1, * cpu_x2;

	a = (int**)malloc(N*sizeof(int*));
	for ( i = 0 ; i < N ; i++ )
		a[i] = (int*)malloc(N*sizeof(int));

	x1 = (int*)malloc(N*sizeof(int));
	x2 = (int*)malloc(N*sizeof(int));
	y_1 = (int*)malloc(N*sizeof(int));
	y_2 = (int*)malloc(N*sizeof(int));
	cpu_x1 = (int*)malloc(N*sizeof(int));
	cpu_x2 = (int*)malloc(N*sizeof(int));

	for (i=0; i<N; i++) {
        	y_1[i] = i;
        	y_2[i] = i+1;
        	x1[i] = 0;
        	x2[i] = 0;

        	for (j=0; j<N; j++)
            		a[i][j] = i+j+1;
    	}

	cpu_start = rtclock();
	for (int i=0; i<N; i+=1) {
        	for (int j=0; j<N; j+=1) {
            		cpu_x1[i] = cpu_x1[i] + a[i][j] * y_1[j];
        	}
    	}

   	for (int i=0; i<N; i+=1) {
        	for (int j=0; j<N; j+=1) {
           		 cpu_x2[i] = cpu_x2[i] + a[j][i] * y_2[j];
        	}
    	}
	cpu_end = rtclock();

	  printf("CPU time -> %0.6lfs\n", cpu_end - cpu_start);


gpu_start = rtclock();
#pragma accelerate
{
    for (int i=0; i<N; i+=1) {
        for (int j=0; j<N; j+=1) {
            x1[i] = x1[i] + a[i][j] * y_1[j];
        }
    }

    for (int i=0; i<N; i+=1) {
        for (int j=0; j<N; j+=1) {
            x2[i] = x2[i] + a[j][i] * y_2[j];
        }
    }
}

gpu_end = rtclock();

printf("GPU time -> %0.6lfs\n", gpu_end - gpu_start);

for ( i = 0 ; i < N ; i++ )
	assert(cpu_x1[i]==x1[i]&&cpu_x2[i]==x2[i]);

for ( i=0 ; i < N ; i++ )
	free(a[i]);

free(a);
free(x1);
free(x2);
free(y_1);
free(y_2);
free(cpu_x1);
free(cpu_x2);

    return 0;
}
