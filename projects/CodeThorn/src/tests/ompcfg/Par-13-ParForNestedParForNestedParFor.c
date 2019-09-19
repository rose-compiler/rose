

int main(int argc, char **argv) {
  int a[4] = {1,2,3,4};
  int b[4] = {1,1,1,1};
  int c[4] = {0,2,1,3};

  for (int i = 0; i < 1; ++i) {
    if (i < 2) {
      return -1;
    }
  }

#pragma omp parallel for
  for (int i = 0; i < 4; ++i) {
    a[i] = 3*a[i];
#pragma omp parallel for
    for(int j = 0; j < 4; ++j) {
      b[j] += a[i];
#pragma omp parallel for
      for(int k = 0; k < 4; ++k) {
        c[k] = a[i] * b[k] + c[k];
      }
    }
  }

  return 0;
}
