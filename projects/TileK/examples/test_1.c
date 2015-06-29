
void stencil_0(int n, int m, int t, float *** A, float *** B, float alpha, float beta) {
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

void stencil_1(int n, int m, int t, float *** A, float *** B, float alpha, float beta) {
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

void stencil_2(int n, int m, int t, float *** A, float *** B, float alpha, float beta) {
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

void stencil_3(int n, int m, int t, float *** A, float *** B, float alpha, float beta) {
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

