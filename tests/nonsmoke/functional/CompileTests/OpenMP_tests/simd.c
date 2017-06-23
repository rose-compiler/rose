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

void foo3 (int n, double *a, double* b)
{
  int j=0;
#pragma omp simd simdlen(16)
  for (int i=0; i<n; i++,j++)
  {
    a[i]=b[i]+j;
  }
}

void foo32 (int n, double *a, double* b)
{
  int j=0, k=0;
#pragma omp simd linear(j,k)
  for (int i=0; i<n; i++,j++,k++)
  {
    a[i]=b[i]+j+k;
  }
}

void foo33 (int n, double *a, double* b)
{
  int j=0, k=0;
#pragma omp simd linear(j,k:1)
  for (int i=0; i<n; i++,j++,k++)
  {
    a[i]=b[i]+j+k;
  }
}

void fooAligned (int n, double *a, double* b)
{
  int j=0, k=0;
#pragma omp simd aligned(j,k)
  for (int i=0; i<n; i++,j++,k++)
  {
    a[i]=b[i]+j+k;
  }
}


void fooAligned2 (int n, double *a, double* b)
{
  int j=0, k=0;
#pragma omp simd aligned(j,k:1)
  for (int i=0; i<n; i++,j++,k++)
  {
    a[i]=b[i]+j+k;
  }
}

double work( double *a, double *b, int n )
{
   int i; 
   double tmp, sum;
   sum = 0.0;
   #pragma omp simd private(tmp) reduction(+:sum)
   for (i = 0; i < n; i++) {
      tmp = a[i] + b[i];
      sum += tmp;
   }
   return sum;
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

void work2( double **a, double **b, double **c, int n )
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

void work3( double **a, double **b, double **c, int n )
{
  int i, j;
  double tmp;
#pragma omp parallel for simd collapse(2) private(tmp)
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      tmp = a[i][j] + b[i][j];
      c[i][j] = tmp;
    }
  }  
}

// declare simd can show up several times!
#pragma omp declare simd simdlen(1) notinbranch
float bar(int * p) {
   *p = *p +10;
   return *p; 
}

// declare simd can show up several times!
#pragma omp declare simd linear(p:1)
#pragma omp declare simd uniform(p)
#pragma omp declare simd simdlen(1) notinbranch
float bar2(int * p) {
   *p = *p +10;
   return *p; 
}

