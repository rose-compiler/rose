
int foo();
float * alloc();

void f() {
  int i,j,k;

  int n = foo();
  int m = foo();

  float * a = alloc();
  float ** b = alloc();

  for (k = 0; k < n*m; k++)
    a[k] = 0.;

  for (k = 0; k < n ; k++)
    b[k] = &a[k*m];

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      b[i][j] = b[i][j] + 1;
}

