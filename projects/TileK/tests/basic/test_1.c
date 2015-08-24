
#include <stdio.h>
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

void print_array(int n_0, int n_1, int m_0, int m_1, float ** A) {
  int i, j;
  for (i = n_0; i < n_1; i++) {
    for (j = m_0; j < m_1; j++) {
      printf(" | %f", A[i][j]);
    }
    printf(" |\n");
  }
  printf("\n");
}

void kernel_0(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m])
  {
  #pragma tilek loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[1](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

int main() {

  const int n = 16;
  const int m = 16;

  float ** a;

  {
    a = create_array(n, m);

    print_array(0, 10, 0, 10, a);

    kernel_0(n, m, a, 3.5);

    print_array(0, 10, 0, 10, a);

    free_array(a);
  }

  return 0;
}

