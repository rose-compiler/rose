

int main(int argc, char **argv) {
  int a[4] = {1,2,3,4};

#pragma omp parallel
  {
#pragma omp sections
  {
#pragma omp section
  {
    a[0] = 2;
    a[3] = 1;
  }
#pragma omp section
  {
    a[1] = 3;
    a[2] = 1;
  }
#pragma omp section
  {
    a[2] = 10;
    a[0] = 2;
  }
  }
  }
  return 0;
}
