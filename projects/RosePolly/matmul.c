#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "timer.h"

#define alpha 1
#define beta 1

extern int bar2();

int main()
{
    	int i, j, k;

	double cpu_start, cpu_end;
	double gpu_start, gpu_end;

	int N = bar2();

	int ** A, ** B, ** C;
	int ** cpu_C;

	A = (int**)malloc(N*sizeof(int*));
	B = (int**)malloc(N*sizeof(int*));
	C = (int**)malloc(N*sizeof(int*));
	cpu_C = (int**)malloc(N*sizeof(int*));

	for ( i = 0 ; i < N ; i++ ) {
		A[i] = (int*)malloc(N*sizeof(int));
		B[i] = (int*)malloc(N*sizeof(int));
		C[i] = (int*)malloc(N*sizeof(int));
		cpu_C[i] = (int*)malloc(N*sizeof(int));
	}

	for (i=0; i<N; i++) {
        	for (j=0; j<N; j++) {
            		A[i][j] = (i + j);
            		B[i][j] = (i*j);
            		C[i][j] = 0;
        	}
    	}
cpu_start = rtclock();
	for(i=0; i<N; i+=1)
        	for(j=0; j<N; j+=1)  
            		for(k=0; k<N; k+=1)
                		cpu_C[i][j] = beta*cpu_C[i][j] + alpha*A[i][k] * B[k][j];
cpu_end = rtclock();

	  printf("CPU time -> %0.6lfs\n", cpu_end - cpu_start);

gpu_start = rtclock();
#pragma rosePolly
{
    for(int i=0; i<N; i++)
        for(int j=0; j<N; j++)  
            for(int k=0; k<=N-1; k++)
					C[i][j] = beta*C[i][j] + alpha*A[i][k] * B[k][j];
}
gpu_end = rtclock();

printf("GPU time -> %0.6lfs\n", gpu_end - gpu_start);


for ( i = 0 ; i < N ; i++ )
	for ( j = 0 ; j < N ; j++ ) 
		assert(cpu_C[i][j]==C[i][j]);

for ( i = 0 ; i < N ; i++ ) {
	free(A[i]);
	free(B[i]);
	free(C[i]);
	free(cpu_C[i]);
}

free(A);
free(B);
free(C);
free(cpu_C);

    return 0;
}
