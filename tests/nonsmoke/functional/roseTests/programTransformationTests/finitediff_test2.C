void dgemm(int* a, int* b, int* c, int n) {
  for (int i = 0; i < n; ++i) {
    int j2 = 0;
    for (int j = 0; j < n; j += 16) {
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = 1 + j2;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = 1 + j2;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
      for (int k = 0; k < n; ++k) {
	c[i * n + k] = c[i * n + k] + a[i * n + j2] * b[j2 * n + k];
      }
      j2 = j2 + 1;
    }
  }
}
