
void * malloc(unsigned int);

float ** f(unsigned int n, unsigned int m, float val) {
	int i, j;
	float ** a  = malloc(n * sizeof(float *));
	for (i = 0; i < n; i++) {
		a[i] = malloc(m * sizeof(float));
		for (j = 0; j < m; j++) {
			a[i][j] = val;
		}
	}
	return a;
}
