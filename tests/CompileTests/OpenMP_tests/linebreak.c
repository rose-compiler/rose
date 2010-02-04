void foo(int a[])
{
  int i,j;

#pragma omp task untied default (shared) \
  private (i,j) \
  shared (a)
  {
    a[i]=j;

  }
}
