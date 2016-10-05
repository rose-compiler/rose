#include <stdio.h>
#include <omp.h>
int main(void)
{
  int i=0,t_id;
#pragma omp parallel for ordered private (t_id)
  for (i=0;i<100;i++)
  {
    t_id= omp_get_thread_num();

#pragma omp ordered
    {
      printf("I am i=%d in thread %d\n",i,t_id);
    }
  }
  return 0;
}
