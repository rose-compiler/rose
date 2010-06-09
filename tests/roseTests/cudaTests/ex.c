
void kernel(float * in, float * out, int size, int i, int j) {
	out[i * size + j] = 4 * in[i * size + j] + in[i * size + (j + 1)] + in[i * size + (j - 1)] + in[(i + 1) * size + j] + in[(i - 1) * size + j];
}

void caller(float * in, float * out, int size) {
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			kernel(in, out, size, i, j);
		}
	}
}
