
void kernel_0(int n, int m, int t, float *** A, float *** B, float alpha, float beta) {
  int i, j, k;
  #pragma tilek kernel data(A[0:n][0:m][0:t], B[0:n][0:m][0:t], alpha, beta)
  {
  #pragma tilek loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[1](dynamic)
    for (j = 0; j < m; j++)
      #pragma tilek loop tile[2](dynamic)
      for (k = 0; k < t; k++)
        B[i][j][k] = alpha * A[i][j][k] + beta * (A[i-1][j][k] + A[i+1][j][k] + A[i][j-1][k] + A[i][j+1][k] + A[i][j][k-1] + A[i][j][k+1]);
  }
}

void kernel_1(int n, int m, int t, float *** A, float *** B, float alpha, float beta) {
  int i, j, k;
  #pragma tilek kernel data(A[0:n][0:m][0:t], B[0:n][0:m][0:t], alpha, beta)
  {
  #pragma tilek loop tile[2](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[0](dynamic)
    for (j = 0; j < m; j++)
      #pragma tilek loop tile[1](dynamic)
      for (k = 0; k < t; k++)
        B[i][j][k] = alpha * A[i][j][k] + beta * (A[i-1][j][k] + A[i+1][j][k] + A[i][j-1][k] + A[i][j+1][k] + A[i][j][k-1] + A[i][j][k+1]);
  }
}

void kernel_2(int n, int m, int t, float *** A, float *** B, float alpha, float beta) {
  int i, j, k;
  #pragma tilek kernel data(A[0:n][0:m][0:t], B[0:n][0:m][0:t], alpha, beta)
  {
  #pragma tilek loop tile[1](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[2](dynamic)
    for (j = 0; j < m; j++)
      #pragma tilek loop tile[1](dynamic)
      for (k = 0; k < t; k++)
        B[i][j][k] = alpha * A[i][j][k] + beta * (A[i-1][j][k] + A[i+1][j][k] + A[i][j-1][k] + A[i][j+1][k] + A[i][j][k-1] + A[i][j][k+1]);
  }
}

void kernel_3(int n, int m, int t, float *** A, float *** B, float alpha, float beta) {
  int i, j, k;
  #pragma tilek kernel data(A[0:n][0:m][0:t], B[0:n][0:m][0:t], alpha, beta)
  {
  #pragma tilek loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      #pragma tilek loop tile[2](dynamic)
      for (k = 0; k < t; k++)
        B[i][j][k] = alpha * A[i][j][k] + beta * (A[i-1][j][k] + A[i+1][j][k] + A[i][j-1][k] + A[i][j+1][k] + A[i][j][k-1] + A[i][j][k+1]);
  }
}

float *** create_array(int n, int m, int p) {
  float *** a = malloc(n * sizeof(float *));
  float ** a_ = malloc(n * m * sizeof(float *));
  float * a__ = malloc(n * m * p * sizeof(float));

  int i, j, k;

  for (i = 0; i < n; i++) {
    a[i] = a_ + i * m;
    for (j = 0; j < m; j++) {
      a[i][j] = a__ + i * m + j * p;
      for (k = 0; k < m; k++) {
        a[i][j][k] = i + j + k;
      }
    }
  }

  return a;
}

void free_array(float *** a) {
  free(a[0][0]);
  free(a[0]);
  free(a);
}

int main() {
  int n = 16;
  int m = 16;
  int t = 16;

  float alpha = 1.0;
  float beta = 2.548;

  float *** A;
  float *** B;

  {
    A = create_array(n, m, t);
    B = create_array(n, m, t);

    kernel_0(n, m, t, A, B, alpha, beta);

    free_array(A);
    free_array(B);
  }

  {
    A = create_array(n, m, t);
    B = create_array(n, m, t);

    kernel_1(n, m, t, A, B, alpha, beta);

    free_array(A);
    free_array(B);
  }

  {
    A = create_array(n, m, t);
    B = create_array(n, m, t);

    kernel_2(n, m, t, A, B, alpha, beta);

    free_array(A);
    free_array(B);
  }

  {
    A = create_array(n, m, t);
    B = create_array(n, m, t);

    kernel_3(n, m, t, A, B, alpha, beta);

    free_array(A);
    free_array(B);
  }

  return 0;
}

