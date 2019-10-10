

int main(int argc, char **argv) {
  int a[4] = {1,2,3,4};

#pragma omp parallel
  {
#pragma omp sections
  {
#pragma omp section
  {
    a[0] = 2;
  }
#pragma omp section
  {
    a[1] = 3;
  }
#pragma omp section
  {
    a[2] = 10;
  }
  }
  }
  return 0;
}
