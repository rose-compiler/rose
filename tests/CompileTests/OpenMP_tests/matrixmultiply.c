/*
Naive matrix-matrix multiplication(mmm)
By C. Liao
*/
#include <stdio.h>
#include <assert.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#define N 512
#define M 512
#define K 512

#define REAL float 
int i,j,k;
REAL a[N][M],b[M][K],c[N][K], c2[N][K];
int init();
int mmm();
int mmm2();
int verify();

int main(void)
{
  init();
  mmm();
  mmm2();

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
    {
      c[i][j]=0.0;
      c2[i][j]=0.0;
    }
  return 0;
}

/*
TODO: try different i,j,k orders

a b     e f    a*e+ b*g , a*f+ b*h
c d  x  g h  = c*e+ d*g,  c*f+ d*h

*/

int mmm()
{
#pragma omp parallel for private(i,j,k) collapse(2)
  for (i = 0; i < N; i++)
    for (j = 0; j < M; j++)
      for (k = 0; k < K; k++)
        c[i][j]= c[i][j]+a[i][k]*b[k][j];

  return 0;
}

int mmm2()
{
  for (i = 0; i < N; i++)
    for (j = 0; j < M; j++)
      for (k = 0; k < K; k++)
        c2[i][j]= c2[i][j]+a[i][k]*b[k][j];

  return 0;
}


int verify()
{
  REAL sum=0.0, sum2=0.0;
  for (i=0;i<N;i++)
    for(j=0;j<K;j++)
    {
      sum+=c[i][j];
      sum2+=c2[i][j];
    }
  printf("sum of c[i][j] is %f\n",sum);
  printf("sum of c2[i][j] is %f\n",sum2);
  assert (sum == sum2);
  return 0;
}

