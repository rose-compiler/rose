
#include <stdlib.h>

float * create_vector(int n) {
  float * v = malloc(n * sizeof(float));

  int i;

  for (i = 0; i < n; i++)
    v[i] = i;

  return v;
}

void kernel_0(int n, float * v, float b) {
  int i;
  #pragma tilek kernel data(v[0:n]) num_threads(4)
  {
  #pragma tilek loop tile(thread) tile[1](dynamic)
  for (i = 0; i < n; i++)
      v[i]+=b;
  }
}

int main() {

  int n = 1024*1024;

  float * v;

  {
    v = create_vector(n);

    kernel_0(n, v, 3.5);

    free(v);
  }

  return 0;
}

