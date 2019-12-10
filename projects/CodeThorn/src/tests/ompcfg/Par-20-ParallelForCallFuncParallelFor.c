

int foo(int *A, int l) {
#pragma omp parallel for
  for (int j=0; j < l; ++j) {
    A[j] = 3 * A[j];
  }
  return l;
}

int main(int argc, char **argv) {
  int a[4] = {1,2,3,4};
  int b[4] = {0, 0, 0, 0};

#pragma omp parallel
  {
#pragma omp for
  for (int i = 0; i < 4; ++i) {
    b[i] = foo(a, 4);
  }
  }

  return 0;
}
