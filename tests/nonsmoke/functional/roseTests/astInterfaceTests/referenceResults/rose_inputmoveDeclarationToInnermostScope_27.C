
void foo(int length,double *dd,double *x,double *y,double *xx,double *yy)
{
  for (int i = 0; i < length; ++i) {
    int i1;
    int i2;
    double dx1;
    double dx2;
    double dy1;
    double dy2;
    double a1;
    double a2;
    i1 = i + 1;
    i2 = i1 + 100;
    dx1 = xx[i2] - x[i2];
    dy1 = yy[i2] - y[i2];
    dx2 = x[i1] - x[i2];
    dy2 = y[i1] - y[i2];
    a1 = dx2 * dy1 - dx1 * dy2;
    dx1 = x[i1] - xx[i1];
    dy1 = y[i1] - yy[i1];
    dx2 = xx[i2] - xx[i1];
    dy2 = yy[i2] - yy[i1];
    a2 = dx2 * dy1 - dx1 * dy2;
    dd[i1] = 0.5 * (a1 + a2);
  }
}
