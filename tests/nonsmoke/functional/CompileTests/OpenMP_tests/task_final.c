int foo (int x )
{
#pragma omp task shared(x) mergeable final (x>0)
  {
    x++;
  }
#pragma omp taskwait
  return x; 
}
