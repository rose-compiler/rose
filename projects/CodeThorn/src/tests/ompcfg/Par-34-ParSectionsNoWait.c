
/*
 * This test case is currently not supported in ROSE.
 * 09/27/2019
 *
 * It is rejected for the nowait clause at the the section construct.
 */

int main(int argc, char **argv) {
  int a[4] = {1,2,3,4};
  int b[4] = {0, 0, 0, 0};

#pragma omp parallel
  {
#pragma omp sections nowait
    {
#pragma omp section
      {
  for (int i = 0; i < 4; ++i) {
    a[i] = 3*a[i];
  }
      }
#pragma omp section
      {
  for (int j = 0; j < 4; ++j) {
    b[j] = a[j];
  }
      }
    }
  }

  return 0;
}
