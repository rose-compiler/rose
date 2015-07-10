
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
  #pragma tilek kernel data(A[0:n][0:m])
  {
  #pragma tilek loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[1](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

void kernel_1(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m])
  {
  #pragma tilek loop tile[1](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[0](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

void kernel_2(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m])
  {
  #pragma tilek loop tile[0](dynamic) tile[1](static, 2)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[2](dynamic) tile[3](static, 2)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

void kernel_3(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m])
  {
  #pragma tilek loop tile[0](dynamic) tile[2](static, 2)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[1](dynamic) tile[3](static, 2)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

void kernel_4(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m])
  {
  #pragma tilek loop tile[0](static, 2) tile[2](dynamic) tile[3](static, 4) 
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[1](static, 2) tile[4](dynamic)
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

    kernel_0(n, m, a, 3.5);

    free_array(a);
  }

  {
    a = create_array(n, m);

    kernel_1(n, m, a, 3.5);

    free_array(a);
  }

  {
    a = create_array(n, m);

    kernel_2(n, m, a, 3.5);

    free_array(a);
  }

  {
    a = create_array(n, m);

    kernel_3(n, m, a, 3.5);

    free_array(a);
  }

  {
    a = create_array(n, m);

    kernel_4(n, m, a, 3.5);

    free_array(a);
  }

  return 0;
}

