#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

	const int LENGTH = 2000;

	printf("Allocating arrays of size %d elements.\n", LENGTH);
#pragma 0
	double* a = (double*) malloc(sizeof(double) * LENGTH);
#pragma 0
	double* b = (double*) malloc(sizeof(double) * LENGTH);
#pragma 0
	double* fast_c = (double*) malloc(sizeof(double) * LENGTH);

#pragma default 1
//	mlm_set_pool(1);

	printf("Allocation for fast_c is %llu\n", (unsigned long long int) fast_c);
	double* c = (double*) malloc(sizeof(double) * LENGTH);
	printf("Done allocating arrays.\n");

	int i;
	for(i = 0; i < LENGTH; ++i) {
		a[i] = i;
		b[i] = LENGTH - i;
		c[i] = 0;
	}

	// Issue a memory copy
	memcpy(fast_c, c, sizeof(double) * LENGTH);

	printf("Perfoming the fast_c compute loop...\n");
	#pragma omp parallel for
	for(i = 0; i < LENGTH; ++i) {
		fast_c[i] = 2.0 * a[i] + 1.5 * b[i];
	}

	// Now copy results back
        memcpy(c, fast_c, sizeof(double) * LENGTH);

	double sum = 0;
	for(i = 0; i < LENGTH; ++i) {
		sum += c[i];
	}

	printf("Sum of arrays is: %f\n", sum);
	printf("Freeing arrays...\n");

        free(a);
        free(b);	
	free(c);
	free(fast_c);

	printf("Done.\n");
}
