
#include <stdlib.h>

float ** create_array(int n, int m) {
  float ** a = malloc(n * sizeof(float *));
  float * a_ = malloc(n * m * sizeof(float));

  int i, j;

  for (i = 0; i < n; i++) {
    a[i] = a_ + i * m;
    for (j = 0; j < m; j++) {
      a[i][j] = i+j;
    }
  }

  return a;
}

void free_array(float ** a) {
  free(a[0]);
  free(a);
}

void kernel_0(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m]) num_gangs[0](4) num_gangs[1](4) num_workers[0](8) num_workers[1](8)
  {
  #pragma tilek loop tile(gang, 0) tile(worker, 0) tile[0](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile(gang, 1) tile(worker, 1) tile[1](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

int main() {

  const int n = 128;
  const int m = 128;

  float ** a;

  {
    a = create_array(n, m);

    kernel_0(n, m, a, 3.5);

    free_array(a);
  }

  return 0;
}

