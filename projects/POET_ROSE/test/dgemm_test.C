
void dgemm_test(const int M,const int N,const int K,const double alpha,const double *A,const int lda,const double *B,const int ldb,const double beta,double *C,const int ldc)
{
  int i;
  int j;
  int l; //@ BEGIN(decl=Stmt)
/*@; BEGIN(nest1=Nest) @*/
  for (j = 0; j <= -1 + N; j += 1) {
/*@; BEGIN(nest3=Nest) @*/
    for (i = 0; i <= -1 + M; i += 1) {
      C[(j * ldc) + i] = (beta * (C[(j * ldc) + i]));
/*@; BEGIN(nest2=Nest) @*/
      for (l = 0; l <= -1 + K; l += 1) {
        C[(j * ldc) + i] = ((C[(j * ldc) + i]) + ((alpha * (A[(l * lda) + i])) * (B[(j * ldb) + l])));
      }
    }
  }
}

