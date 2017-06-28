#define NT 512 

void spotrf(float);
void ssyrk (float , float);
void strsm(float , float);
void sgemm(float , float, float);

void blocked_cholesky( int NB, float A[NB][NB] ) {
  int i, j, k;

  for (k=0; k<NB; k++) {
#pragma omp task depend(inout:A[k][k])
    spotrf (A[k][k]) ;
    for (i=k+1; i<NT; i++)
#pragma omp task depend(in:A[k][k]) depend(inout:A[k][i])
      strsm (A[k][k], A[k][i]);
    // update trailing submatrix
    for (i=k+1; i<NT; i++) {
      for (j=k+1; j<i; j++)
#pragma omp task depend(in:A[k][i],A[k][j]) depend(inout:A[j][i])
          sgemm( A[k][i], A[k][j], A[j][i]);
#pragma omp task depend(in:A[k][i]) depend(inout:A[i][i])
      ssyrk (A[k][i], A[i][i]);
    }
  }
}
