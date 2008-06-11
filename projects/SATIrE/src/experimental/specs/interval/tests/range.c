int f(int a) {
	int j;
	int i = 42;
	if (a < 0) 
		i -= 12;

	for (j = 0; j < a; j++) {
		++i;
	}
	a = i << 2;
	return a;
}
