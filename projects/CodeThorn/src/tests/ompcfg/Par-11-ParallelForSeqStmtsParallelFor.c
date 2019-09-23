

int main(int argc, char **argv) {
  int a[4] = {1,2,3,4};
  int b[4] = {0, 0, 0, 0};

#pragma omp parallel
  {
#pragma omp for
  for (int i = 0; i < 4; ++i) {
    a[i] = 3*a[i];
  }
  }
  a[0] = 1;
  int k = a[1] + a[0] * 4;

#pragma omp parallel
  {
#pragma omp for
    for (int i = 0; i < 4; ++i) {
      a[i] = i + a[i];
    }
  }

  return 0;
}
