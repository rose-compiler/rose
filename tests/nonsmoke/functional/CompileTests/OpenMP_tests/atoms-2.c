#include <omp.h>
int atom()
{
  int i=0;
#pragma omp parallel
  if (omp_get_thread_num() ==10 )
  {
    i =10;
  }
  return i;
}
