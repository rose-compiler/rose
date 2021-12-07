#include <stdio.h>
#include <stdlib.h>
#define N 32 

void init(int *A){
	int i;
	for(i=0;i<N;i++){
		A[i] = rand() % 10;
	}
}

void kernel(int *A, int *B){
	int i,x;
	for(i=0;i<N;i++){
		x = A[i];
		B[i] = (x*(x*(16*x*x-20)*x+5));
	}
}

void print(int *A, int *B){
	int i;
	for(i=0;i<N;i++){
		printf("A[%d] = %d, B[%d] = %d\n", i, A[i], i, B[i]);
	}
}

int main()
{
	int *A;
	int *B;

	A = (int *)malloc(N*sizeof(int));
	B = (int *)malloc(N*sizeof(int));

	init(A);
	kernel(A,B);
	print(A,B);

	free(A);
	free(B);
	return 0;
}
