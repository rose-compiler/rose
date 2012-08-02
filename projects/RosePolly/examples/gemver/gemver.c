#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "timer.h"

#define alpha 1
#define beta 1

extern int bar1();

main()
{
    	int i, j;

	double cpu_start, cpu_end;
	double gpu_start, gpu_end;

	int N = bar();

	int ** A, ** B;
	int * u1, * u2, * v1, * v2, * x, * y, * z, * w;
	int ** cpu_B;
	int * cpu_x, * cpu_w;

	A = (int**)malloc(N*sizeof(int*));
	B = (int**)malloc(N*sizeof(int*));
	cpu_B = (int**)malloc(N*sizeof(int*));

	u1 = (int*)malloc(N*sizeof(int));
	u2 = (int*)malloc(N*sizeof(int));
	v1 = (int*)malloc(N*sizeof(int));
	v2 = (int*)malloc(N*sizeof(int));
	x = (int*)malloc(N*sizeof(int));
	y = (int*)malloc(N*sizeof(int));
	z = (int*)malloc(N*sizeof(int));
	w = (int*)malloc(N*sizeof(int));
	cpu_x = (int*)malloc(N*sizeof(int));
	cpu_w = (int*)malloc(N*sizeof(int));

	for ( i = 0 ; i < N ; i++ ) {
		A[i] = (int*)malloc(N*sizeof(int));
		B[i] = (int*)malloc(N*sizeof(int));
		cpu_B[i] = (int*)malloc(N*sizeof(int));
	}

    for (i=0; i<N; i++) {
        u1[i] = i;
        u2[i] = (i+1)/N/2;
        v1[i] = (i+1)/N/4;
        v2[i] = (i+1)/N/6;
        y[i] = (i+1)/N/8;
        z[i] = (i+1)/N/9;
        x[i] = 0;
        w[i] = 0;
	cpu_x[i] = x[i];
	cpu_w[i] = w[i];
        for (j=0; j<N; j++) {
            A[i][j] = (i*j)/N;
        }
    }

cpu_start = rtclock();

    for (i=0; i<N; i+=1)
        for (j=0; j<N; j+=1)
            cpu_B[i][j] = A[i][j] + u1[i]*v1[j] + u2[i]*v2[j];

    for (i=0; i<N; i+=1)
        for (j=0; j<N; j+=1)
            cpu_x[i] = cpu_x[i] + beta* B[j][i]*y[j];


    for (i=0; i<N; i+=1)
        cpu_x[i] = cpu_x[i] + z[i];

    for (i=0; i<N; i+=1)
        for (j=0; j<N; j+=1)
            cpu_w[i] = cpu_w[i] + alpha* cpu_B[i][j]*cpu_x[j];

cpu_end = rtclock();

	  printf("CPU time -> %0.6lfs\n", cpu_end - cpu_start);


gpu_start = rtclock();
#pragma rosePolly
{
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            B[i][j] = A[i][j] + u1[i]*v1[j] + u2[i]*v2[j];

    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            x[i] = x[i] + beta* B[j][i]*y[j];


    for (int i=0; i<N; i++)
        x[i] = x[i] + z[i];

    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            w[i] = w[i] + alpha* B[i][j]*x[j];
}
gpu_end = rtclock();

printf("GPU time -> %0.6lfs\n", gpu_end - gpu_start);

for ( i = 0 ; i < N ; i++ ) {
	assert(cpu_x[i]==x[i]&&cpu_w[i]==w[i]);
	for ( j = 0 ; j < N ; j++ ) 
		assert(cpu_B[i][j]==B[i][j]);
}

for ( i = 0 ; i < N ; i++ ) {
	free(A[i]);
	free(B[i]);
	free(cpu_B[i]);
}

free(A);
free(B);
free(cpu_B);
free(u1);
free(u2);
free(v1);
free(v2);
free(x);
free(cpu_x);
free(y);
free(w);
free(cpu_w);


return 0;
}
