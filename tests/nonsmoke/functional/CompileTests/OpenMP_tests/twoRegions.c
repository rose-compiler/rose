int N=100;
int b[100],c[100];
int j;

void foo()
{
#pragma omp parallel for shared(b) firstprivate(c)
  for (j=0; j<N; j++)
  {   
    b[j] = c[j];
  }
#pragma omp parallel for shared(b) firstprivate(c)
  for (j=0; j<N; j++)
  {   
    b[j] = c[j];
  }
}
