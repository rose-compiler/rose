
void kernel(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:n], b)
  {
  #pragma tilek loop tile[0](static, 2) tile[2](dynamic) tile[3](static, 4) 
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[1](static, 2) tile[4](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

