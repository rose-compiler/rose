// A test case with third party pragmas inside a parallel region
extern void quicksort();
int main()
{

#pragma omp parallel
  {
#pragma intel omp taskq
    {
#pragma intel omp task
      {
        quicksort();
      }
    }
  }

  return 0;
}
