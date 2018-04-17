extern int min(int ,int );
                                                                                
void dgemm(double *a,double *b,double *c,int n)
{
  int _var_1;
  int _var_0;
  int i;
  int j;
  int k;
  for (_var_1 = 0; _var_1 <= -1 + n; _var_1 += 16) {
    for (_var_0 = 0; _var_0 <= -1 + n; _var_0 += 16) {
      for (i = 0; i <= -1 + n; i += 1) {
        for (k = _var_1; k <= min(-1 + n,_var_1 + 15); k += 1) {
          int dummy_1 = k * n + i;
          for (j = _var_0; j <= min(n + -16,_var_0); j += 16) {
            int _var_2 = (j);
            c[j * n + i] = c[j * n + i] + a[k * n + i] * b[j * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
            _var_2 = 1 + _var_2;
            c[_var_2 * n + i] = c[_var_2 * n + i] + a[k * n + i] * b[_var_2 * n + k];
          }
          for (; j <= min(-1 + n,_var_0 + 15); j += 1) {
            c[j * n + i] = c[j * n + i] + a[k * n + i] * b[j * n + k];
          }
        }
      }
    }
  }
}
