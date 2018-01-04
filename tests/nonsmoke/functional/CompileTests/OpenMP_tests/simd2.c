//Variable examples of using simd directives
// Mixed with omp for, how to respond to reduce/reduce conflicts
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

   #pragma omp for private(tmp) reduction(+:sum)
   for (i = 0; i < n; i++) {
      tmp = a[i] + b[i];
      sum += tmp;
   }
 
   return sum;
}


#define N 45
int a[N], b[N], c[N];

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


