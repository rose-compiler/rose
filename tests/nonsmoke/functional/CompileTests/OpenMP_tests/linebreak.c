/*
 * Test for line continuation within pragmas
 *
 * Liao 2/5/2010
 *
 * */
void foo1(int a[])
{
  int i,j;

#pragma omp parallel \
  private (j) \
  shared (a)
  {
    a[i]=j;

  }
}

