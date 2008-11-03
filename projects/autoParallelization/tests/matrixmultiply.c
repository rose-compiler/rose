/*
Naive matrix-matrix multiplication(mmm)
By C. Liao
*/
#define N 1000
#define M 1000
#define K 1000

int i,j,k;
double a[N][M],b[M][K],c[N][K];

int mmm()
{
//#pragma omp parallel for private(i,j,k) shared(a,b,c)
for (i = 0; i < N; i++)
  for (j = 0; j < M; j++)
    for (k = 0; k < K; k++)
       c[i][j]= c[i][j]+a[i][k]*b[k][j];
return 0;
}

