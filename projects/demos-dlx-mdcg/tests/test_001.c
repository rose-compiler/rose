
void foo(int n, float * a, float * b) {
  int i;

  #pragma logger log message("foo(%d)") where(both) cond(n, 20) params(n)
  for (i = 0; i < n; i++)
    a[i] += b[i];
}

