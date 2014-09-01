void goo(double *o1, double *c, int **idx, int len)
{
  int i ;
  for (i = 0; i < len; ++i) {
    int ii ;
    const int *lidx = idx[i];
    double volnew_o8 = 0.5 * c[i];
    for (ii = 0; ii < 6; ++ii) {
      int llidx = lidx[ii];
      o1[llidx] += volnew_o8;
    }
  }
}

