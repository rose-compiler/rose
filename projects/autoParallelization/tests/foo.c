#if 0
void foo(double *o1, double *o2, double *a, double *b, double *c, int **idx, int len)
{
  int i ;
  for (i = 0; i < len; ++i) {
    const int *lidx = idx[i];
    double volnew_o8 = 0.5 * c[i];
    double volold_o8 = 0.5 * a[i] * b[i] ;
    o1[i] += volnew_o8;
    o2[i] += volold_o8;
  }
}
#endif

#if 1
void goo(double *o1, double *o2, double *a, double *b, double *c, int **idx, int len)
{
  int i ;
  for (i = 0; i < len; ++i) {
    int ii ;
    const int *lidx = idx[i];
    double volnew_o8 = 0.5 * c[i];
    double volold_o8 = 0.5 * a[i] * b[i] ;
    for (ii = 0; ii < 6; ++ii) {
      int llidx = lidx[ii];
      o1[llidx] += volnew_o8;
      o2[llidx] += volold_o8;
    }
  }
}
#endif
