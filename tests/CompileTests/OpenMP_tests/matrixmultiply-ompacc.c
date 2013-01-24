/*
Naive matrix-matrix multiplication(mmm)
By C. Liao
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#define N 1024 
#define M 1024
#define K 1024

#define REAL double
int i,j,k;
REAL a[N][M],b[M][K],c[N][K];
int init();
int mmm();
int verify();

int main(void)
{
init();
mmm();
return verify();
}
int init()
{
for (i=0;i<N;i++)
  for(j=0;j<M;j++)
    a[i][j]=3.0*i*j/N/M;

for (i=0;i<M;i++)
  for(j=0;j<K;j++)
    b[i][j]=5.0*j*i/N/M;

for (i=0;i<N;i++)
  for(j=0;j<K;j++)
    c[i][j]=0.0;
return 0;
}
/*
TODO: try different i,j,k orders

a b     e f    a*e+ b*g , a*f+ b*h
c d  x  g h  = c*e+ d*g,  c*f+ d*h

*/
int mmm()
{
  /* 
     If we define array shape [M] = [0:M], we could use the following simple format 
#pragma omp target data map(out:c[N][K]), map(in:a[N][M],b[M][K]
*/
  //#pragma omp target data map(out:c[0:N][0:K]), map(in:a[0:N][0:M],b[0:M][0:K]
  // Liao, I believe array c is both in and out. Also "omp target" should be used instead of "omp target data"
#pragma omp target map(inout:c[0:N][0:M]), map(in:a[0:N][0:M],b[0:M][0:K])
#pragma omp parallel for private(i,j,k)
    for (i = 0; i < N; i++)
    for (j = 0; j < M; j++)
    for (k = 0; k < K; k++)
    c[i][j]= c[i][j]+a[i][k]*b[k][j];

    return 0;
    }

int verify()
{
REAL sum=0.0;
for (i=0;i<N;i++)
  for(j=0;j<K;j++)
    sum+=c[i][j];
printf("sum of c[i][j] is %f\n",sum);
return 0;
}

