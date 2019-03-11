
void dgemvT(const int M,const int N,const double alpha,const double *A,const int lda,const double *X,const int incX,const double beta,double *Y,const int incY)
{
  int i;
  int j;
  double Y_buf0;
  for (i = 0; i <= -1 + M; i += 1) {
    Y[i] = beta * Y[i];
  }
  for (i = 0; i <= -1 + M; i += 1) {
    Y_buf0 = Y[i];
    for (j = 0; j <= -1 + N; j += 1) {
      Y_buf0 += A[i * lda + j] * X[j];
    }
    Y[i] = Y_buf0;
  }
}
