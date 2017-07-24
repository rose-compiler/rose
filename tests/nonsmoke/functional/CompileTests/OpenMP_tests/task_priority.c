void foo (float *a, int N)
{
  int i;
#pragma omp parallel private(i)
#pragma omp single
  {
    for (i=0;i<N; i++) {
#pragma omp task priority(i)
      a[i]= 0.5 ;
    }
  }
}

