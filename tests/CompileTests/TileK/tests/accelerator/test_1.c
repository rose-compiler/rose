
void kernel_0(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m]) num_gangs[0](4) num_gangs[1](4) num_workers[0](8) num_workers[1](8)
  {
  #pragma tilek loop tile(gang, 0) tile(gang, 1) tile[0](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile(worker, 0) tile(worker, 1) tile[1](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

