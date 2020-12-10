

int main(int argc, char **argv) {
  int a[4] = {1,2,0,4};

#pragma omp parallel
  {
#pragma omp for
  for (int i = 0; i < 4; ++i) {
    a[i] = 3*a[i];
    if (a[i] > 0) continue;
  }
  }

  return 0;
}
