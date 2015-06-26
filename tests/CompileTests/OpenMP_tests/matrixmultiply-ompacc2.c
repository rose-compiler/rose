/*
Naive matrix-matrix multiplication(mmm)
multiple GPUs, standard OpenMP 4.0 directives
By C. Liao
*/
#include <stdio.h>
#include <assert.h>
#include <omp.h>
#define N 1024 
#define M 1024
#define K 1024

#define REAL float 
int i,j,k;
REAL a[N][M],b[M][K],c[N][K], c2[N][K];
int init();
int mmm();
int mmm2();
int verify();

#define MAX_GPU_COUNT 4

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
  int GPU_N = 0, idev;
  int n = N; 
  cudaGetDeviceCount(&GPU_N);
  printf("CUDA-capable device count: %i\n", GPU_N);
    if (GPU_N > MAX_GPU_COUNT)
    {
        GPU_N = MAX_GPU_COUNT;
    }
  assert (GPU_N>0 && GPU_N<=MAX_GPU_COUNT);

  omp_set_num_threads(GPU_N);
#pragma omp parallel for shared (GPU_N, a, b, c, n) private(idev)
  for (idev = 0; idev < GPU_N; idev++)
  {
    int tid = omp_get_thread_num();
    cudaSetDevice(tid);
    int size = n / GPU_N;
    int offset = size * tid;
    if(tid < n%GPU_N)
    {
      size++;
    }
    if(tid >= n%GPU_N)
      offset += n%GPU_N;
    else
      offset += tid;
    printf("thread %d working on GPU devices %d with size %d copying data from y_ompacc with offset %d\n",tid, tid, size,offset);
    int i, j, k;

#pragma omp target device (tid) map(tofrom:c[offset:size][0:n]), map(to:a[offset:size][0:n],b[0:n][0:n], offset,size,n)
#pragma omp parallel for private(i,j,k) shared (a,b,c, n, offset, size)
    for (i = offset; i < offset + size; i++)
      for (j = 0; j < M; j++)
        for (k = 0; k < K; k++)
          c[i][j]= c[i][j]+a[i][k]*b[k][j];

  }
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

