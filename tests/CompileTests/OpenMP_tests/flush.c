int main (void)
{
  int a = 1, b=2;
#pragma omp parallel if (a) num_threads(4)
  {
#pragma omp flush    
    if (a != 0)
      {
#pragma omp flush(a,b)
      }
    if (a != 0)
      {
#pragma omp barrier
      }
  }
  return 0;
}
