#include <stdlib.h>

float matsum(float ** a, size_t n) {
	size_t i, j;
	float res = 0;
	#pragma polyhedric_program
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			res += a[i][j];
		}
	}
	
	return res;
}

