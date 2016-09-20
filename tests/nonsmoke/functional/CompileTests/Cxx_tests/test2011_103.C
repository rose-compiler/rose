double abs(double x) {
  if (x > 0)
    return x;
  else
    return -x;
}

double vec_norm1(double* v, int x) {
  double sum = 0;
  for (int i = 0; i < x; ++i)
    sum += abs(v[i]);
  return sum;
}

#if 1
double mat_norm1(double* v, int m, int n) {
  double norm = -1./0;
  for (int i = 0; i < n; ++i) {
    double result = vec_norm1(v + i * m, m);
    double result2 = vec_norm1(v + (n - i + 1) * m, m);
    norm = (result > norm) ? result : norm;
  }
  return norm;
}

int main(int, char**) {
  double a[10][10];
  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 10; ++j)
      a[i][j] = (i == j ? 1. : 0.);
  return (!(mat_norm1(&a[0][0], 10, 10) == 1.));
}
#endif
