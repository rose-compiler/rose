
void foo(int n, float * a, float * b) {
  int i;

  #pragma tooling grapher file("test.dot")
  for (i = 0; i < n; i++)
    a[i] += b[i];
}

