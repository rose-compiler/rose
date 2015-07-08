
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

void kernel_0(int n, int m, int p, int q, float ** a, float ** b, float ** c, float ** d) {
  int i, j, k;
  float ** e = create_array(n, q);

  #pragma tilek kernel data(a[0:n][0:p], b[0:p][0:q], c[0:q][0:m], d[0:n][0:m], e[0:n][0:q])
  {
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < q; j++) {
        e[i][j] = 0;
        #pragma tilek loop tile[2](dynamic)
        for (k = 0; k < p; k++)
          e[i][j] += a[i][k] * b[k][j];
      }
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < m; j++) {
        d[i][j] = 0;
        #pragma tilek loop tile[2](dynamic)
        for (k = 0; k < q; k++)
          d[i][j] += e[i][k] * c[k][j];
      }
  }

  free_array(e);
}

void kernel_1(int n, int m, int p, int q, float ** a, float ** b, float ** c, float ** d) {
  int i, j, k;
  float ** e = create_array(n, q);

  #pragma tilek kernel data(a[0:n][0:p], b[0:p][0:q], c[0:q][0:m], d[0:n][0:m], e[0:n][0:q])
  {
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < q; j++) {
        e[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < p; k++)
          e[i][j] += a[i][k] * b[k][j];
      }
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < m; j++) {
        d[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < q; k++)
          d[i][j] += e[i][k] * c[k][j];
      }
  }

  free_array(e);
}

void kernel_2(int n, int m, int p, int q, float ** a, float ** b, float ** c, float ** d) {
  int i, j, k;
  float ** e = create_array(n, q);

  #pragma tilek kernel data(a[0:n][0:p], b[0:p][0:q], c[0:q][0:m], d[0:n][0:m], e[0:n][0:q])
  {
    #pragma tilek loop tile[10](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[11](dynamic)
      for (j = 0; j < q; j++) {
        e[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < p; k++)
          e[i][j] += a[i][k] * b[k][j];
      }
    #pragma tilek loop tile[0](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic)
      for (j = 0; j < m; j++) {
        d[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < q; k++)
          d[i][j] += e[i][k] * c[k][j];
      }
  }

  free_array(e);
}

void kernel_3(int n, int m, int p, int q, float ** a, float ** b, float ** c, float ** d) {
  int i, j, k;
  float ** e = create_array(n, q);

  #pragma tilek kernel data(a[0:n][0:p], b[0:p][0:q], c[0:q][0:m], d[0:n][0:m], e[0:n][0:q])
  {
    #pragma tilek loop tile[1](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[0](dynamic)
      for (j = 0; j < q; j++) {
        e[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < p; k++)
          e[i][j] += a[i][k] * b[k][j];
      }
    #pragma tilek loop tile[1](dynamic)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[0](dynamic)
      for (j = 0; j < m; j++) {
        d[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < q; k++)
          d[i][j] += e[i][k] * c[k][j];
      }
  }

  free_array(e);
}

void kernel_4(int n, int m, int p, int q, float ** a, float ** b, float ** c, float ** d) {
  int i, j, k;
  float ** e = create_array(n, q);

  #pragma tilek kernel data(a[0:n][0:p], b[0:p][0:q], c[0:q][0:m], d[0:n][0:m], e[0:n][0:q])
  {
    #pragma tilek loop tile[0](dynamic) tile[2](static, 4)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](static, 4) tile[3](dynamic)
      for (j = 0; j < q; j++) {
        e[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < p; k++)
          e[i][j] += a[i][k] * b[k][j];
      }
    #pragma tilek loop tile[0](dynamic) tile[2](static, 4)
    for (i = 0; i < n; i++)
      #pragma tilek loop tile[1](dynamic) tile[3](static, 4)
      for (j = 0; j < m; j++) {
        d[i][j] = 0;
        #pragma tilek loop tile[0](dynamic)
        for (k = 0; k < q; k++)
          d[i][j] += e[i][k] * c[k][j];
      }
  }

  free_array(e);
}

int main() {

  const int n = 16;
  const int m = 16;
  const int p = 16;
  const int q = 16;

  float ** a;
  float ** b;
  float ** c;
  float ** d;

  {
    a = create_array(n, p);
    b = create_array(p, q);
    c = create_array(q, m);
    d = create_array(n, m);

    kernel_0(n, m, p, q, a, b, c, d);

    free_array(a);
    free_array(b);
    free_array(c);
    free_array(d);
  }

  {
    a = create_array(n, p);
    b = create_array(p, q);
    c = create_array(q, m);
    d = create_array(n, m);

    kernel_1(n, m, p, q, a, b, c, d);

    free_array(a);
    free_array(b);
    free_array(c);
    free_array(d);
  }

  {
    a = create_array(n, p);
    b = create_array(p, q);
    c = create_array(q, m);
    d = create_array(n, m);

    kernel_2(n, m, p, q, a, b, c, d);

    free_array(a);
    free_array(b);
    free_array(c);
    free_array(d);
  }

  {
    a = create_array(n, p);
    b = create_array(p, q);
    c = create_array(q, m);
    d = create_array(n, m);

    kernel_3(n, m, p, q, a, b, c, d);

    free_array(a);
    free_array(b);
    free_array(c);
    free_array(d);
  }

  {
    a = create_array(n, p);
    b = create_array(p, q);
    c = create_array(q, m);
    d = create_array(n, m);

    kernel_4(n, m, p, q, a, b, c, d);

    free_array(a);
    free_array(b);
    free_array(c);
    free_array(d);
  }

  return 0;
}

