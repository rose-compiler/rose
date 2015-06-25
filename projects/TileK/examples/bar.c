
void bar(int n, int m, int p, int q, float ** a, float ** b, float ** c, float ** d) {
  int i, j, k;
  float ** e = malloc(n * q * sizeof(float));

  #pragma tilek kernel data(a[0:n][0:p], b[0:p][0:q], c[0:q][0:m], d[0:n][0:m], e[0:n][0:q])
  {
    #pragma tilek loop tile[2](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < q; j++) {
        e[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < p; k++)
          e[i][j] += a[i][k] * b[k][j];
      }
    #pragma tilek loop tile[2](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < m; j++) {
        d[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < q; k++)
          d[i][j] += e[i][k] * c[k][j];
      }
  }
}

