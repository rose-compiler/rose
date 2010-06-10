void function(int n, int * a, int * b) {
	int i, j;
	
	for (i = 0; i < n; i++) {
		a[i] = 0;
		for (j = 0; j < n; j++) {
			a[i] = a[i] + b[i * n + j];
		}
	}
}
	
