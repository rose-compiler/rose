//Variable examples of using simd directives
void foo (int n, double *a, double* b)
{
#pragma omp simd
  for (int i=0; i<n; i++)
    a[i]=b[i];
}

void foo2 (int n, double *a, double* b)
{
#pragma omp simd safelen(16)
  for (int i=0; i<n; i++)
    a[i]=b[i];
}

#if 0
void foo3 (int n, double *a, double* b)
{
  int j=0;
#pragma omp simd simdlen(16) linear(i,1)
  for (int i=0; i<n; i++,j++)
  {
    a[i]=b[i]+j;
  }
}

#define N 45
int a[N], b[N], c[N];

void foo4(int i, double* P)
{
  int j; 
#pragma omp simd lastprivate(j)
  for (i = 0; i < 999; ++i) {
    j = P[i];
  }
}
void work( double **a, double **b, double **c, int n )
{
  int i, j;
  double tmp;
#pragma omp for simd collapse(2) private(tmp)
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      tmp = a[i][j] + b[i][j];
      c[i][j] = tmp;
    }
  }  
}

// declare simd can show up several times!
#pragma omp declare simd linear(p:1)
float bar(int * p) {
   *p = *p +10;
   return *p; 
}

#endif
