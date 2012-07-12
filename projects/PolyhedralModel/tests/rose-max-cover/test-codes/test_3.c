
float ** add(float ** a, unsigned int n, unsigned int m, float val) {
	unsigned int i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			a[i][j] += val;
		}
	}
	return a;
}
