void dgemvT(const int M, const int N, const double alpha, const double *A, const int lda, const double *X, const int incX, const double beta, double *Y, const int incY)
{
   int i, j;                                            
   for (i = 0; i < M; i += 1)
   {
     Y[i] = beta * Y[i];
     for (j = 0; j < N; j += 1)
     {
         Y[i] += A[i*lda+j] * X[j];  
      }
   }
}

