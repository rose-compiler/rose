
#include <stdio.h>

void kernel(int n, int m, float ** a, float b);

int main(int argc, char ** argv) {

  const int n = 4;
  const int m = 6;

  int i, j;
  float ** a = malloc(n * sizeof(float *));
  float * a_ = malloc(n * m * sizeof(float));
  for (i = 0; i < n; i++) {
    a[i] = a_ + i * m;
    for (j = 0; j < m; j++) {
      a[i][j] = i+j;
    }
  }

  kernel(n, m, a, 3.5);

  int nb = n < 10 ? n : 6;
  int mb = m < 10 ? m : 6;
  for (i = 0; i < nb; i++) {
    for (j = 0; j < mb; j++) {
      printf("\t%f", a[i][j]);
    }
    printf("\n");
  }

  return 0;
}

