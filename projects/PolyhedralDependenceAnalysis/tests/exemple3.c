void function(int n, int ** a, int ** b, int ** c) {
	int i, j, k;
	
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			c[i][j] = 0;
			for (k = 0; k < n; k++) {
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
			}
		}
	}
}
	
