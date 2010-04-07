/*
a lock should be shared. 
But a locally declared lock within a parallel region is private.
*/
#include <omp.h>
void foo()
{
#pragma omp parallel
  {
    omp_lock_t lck;
    omp_set_lock(&lck);
    printf("Thread = %d\n", omp_get_thread_num());
    omp_unset_lock(&lck);
  }
}
