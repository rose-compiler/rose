void foo()
{
  while (1)
#pragma omp task
  {
  }
}
