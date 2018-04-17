int foo (int x )
{
#pragma omp task shared(x) mergeable
  {
    x++;
  }
#pragma omp taskwait
  return x; 
}
