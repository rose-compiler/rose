/*
test preprocessing info before and after a statement
*/
int main()
{
int i;
#pragma omp parallel
  {
#if defined(_OPENMP)
  #pragma omp master
    {
	i++;
    }
#endif
  }
  return 0;
}

