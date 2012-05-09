#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "timer.h"

extern int bar1();

int main()
{
	int i, j, k;

	double cpu_start, cpu_end;
	double gpu_start, gpu_end;

	int N = bar1();

	int ** a, ** cpu_a, ** L, ** U;
	
	a = (int**)malloc(N*sizeof(int*));
	cpu_a = (int**)malloc(N*sizeof(int*));

	L = (int**)malloc(N*sizeof(int*));
	U = (int**)malloc(N*sizeof(int*));

	for ( i = 0 ; i < N ; i++ ) {
		a[i] = (int*)malloc(N*sizeof(int));
		cpu_a[i] = (int*)malloc(N*sizeof(int));
		L[i] = (int*)malloc(N*sizeof(int));
		U[i] = (int*)malloc(N*sizeof(int));
	}


	for (i=0; i<N; i++) {
        	for (j=0; j<N; j++) {
            		L[i][j] = 0;
            		U[i][j] = 0;
        	}
    	}

    	for (i=0; i<N; i++) {
        	for (j=0; j<=i; j++) {
            		L[i][j] = i+j+1;
            		U[j][i] = i+j+1;
        	}
    	}


    	for (i=0; i<N; i++) {
        	for (j=0; j<N; j++) {
            		for (k=0; k<N; k++) {
                		a[i][j] += L[i][k]*U[k][j];
				cpu_a[i][j] = a[i][j];
           		 }
        	}
    	}


	cpu_start = rtclock();
	for (int k=0; k<N; k+=1) {
        	for (int j=k+1; j<N; j+=1)   {
            		cpu_a[k][j] = cpu_a[k][j]/cpu_a[k][k];
        	}
        	for(int i=k+1; i<N; i+=1)    {
            		for (int j=k+1; j<N; j+=1)   {
                		cpu_a[i][j] = cpu_a[i][j] - cpu_a[i][k]*cpu_a[k][j];
            		}
        	}
    	}
	cpu_end = rtclock();

printf("CPU time -> %0.6lfs\n", cpu_end - cpu_start);


gpu_start = rtclock();
#pragma rosePolly
{
    for (int k=0; k<N; k++) {
        for (int j=k+1; j<N; j++)   {
            a[k][j] = a[k][j]/a[k][k];
        }
        for(int i=k+1; i<N; i++)    {
            for (int j=k+1; j<N; j++)   {
                a[i][j] = a[i][j] - a[i][k]*a[k][j];
            }
        }
    }
}

gpu_end = rtclock();

printf("GPU time -> %0.6lfs\n", gpu_end - gpu_start);

for ( i = 0 ; i < N ; i++ )
	for ( j = 0 ; j < N ; j++ ) 
		assert(cpu_a[i][j]==a[i][j]);

for ( i = 0 ; i < N ; i++ ) {
	free(a[i]);
	free(cpu_a[i]);
	free(L[i]);
	free(U[i]);
}

free(a);
free(cpu_a);
free(L);
free(U);


    return 0;
}
