

int main(int argc, char **argv) {
  int a[4] = {1,2,3,4};
  int b[4] = {1,2,3,4};

#pragma omp parallel
  {
#pragma omp sections
  {
#pragma omp section
  {
  for (int i = 0; i < 4; ++i) {
    a[i] = 3*a[i];
  }
  }
#pragma omp section
  {
    for (int i = 0; i < 4; ++i) {
      b[i] = 3*b[i];
    }
  }
  }
  }
  return 0;
}
