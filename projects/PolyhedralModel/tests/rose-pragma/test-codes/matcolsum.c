#include <stdlib.h>

void matcolsum(float * a, float ** b, size_t n) {
	size_t i, j;
	#pragma polyhedric_program
	for (i = 0; i < n; i++) {
		a[i] = 0.;
		for (j = 0; j < n; j++) {
			a[i] += b[i][j];
		}
	}
}
