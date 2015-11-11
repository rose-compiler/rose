 #define MSIZE 500
int n;
int m;
int mits;
double tol;
double relax = 1.0;
double alpha = 0.0543;
double u[500][500];
double f[500][500];
double uold[500][500];
double dx;
double dy;

void initialize()
{
  dx = 2.0 / (n - 1);
  dy = 2.0 / (m - 1);
  for (int i = 0; i < n; i++) 
    for (int j = 0; j < m; j++) {
      int xx = (int )(- 1.0 + dx * ((double )(i - 1)));
      int yy = (int )(- 1.0 + dy * ((double )(j - 1)));
      u[i][j] = 0.0;
      f[i][j] = - 1.0 * alpha * (1.0 - (xx * xx)) * (1.0 - (yy * yy)) - 2.0 * (1.0 - (xx * xx)) - 2.0 * (1.0 - (yy * yy));
    }
}

void foo(int len,float *compression,float *vnewc,float *delvc,float *compHalfStep)
{
/* declared once, used once */
  for (int zidx = 0; zidx < len; ++zidx) {
    float vchalf;
    compression[zidx] = (0.1 / vnewc[zidx] - 1.0);
    vchalf = (vnewc[zidx] - delvc[zidx] * 0.5);
    compHalfStep[zidx] = (1.0 / vchalf - 1.0);
  }
}
