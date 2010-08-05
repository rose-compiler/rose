// A test input for private OpenMP lock violation
//
// a lock must be shared for the parallel region using it
// A lock declared within a parallel region is private to the region,
// using it is a violation
// Liao, 6/10/2009
extern void skip(int );
extern void work(int );
#include <omp.h>
#include <stdio.h>
int main()
{
  omp_lock_t lck;
  int id;

  omp_set_num_threads(4);
  omp_init_lock(&lck);
#pragma omp parallel private(id)
  {
    omp_lock_t lck; // wrong!!
    id = omp_get_thread_num();
    omp_set_lock(&lck);
    printf("My thread id is %d.\n", id);
    /* only one thread at a time can execute this printf */
    omp_unset_lock(&lck);
    while (! omp_test_lock(&lck)) {
      skip(id); /* we do not yet have the lock, so we must do something else */
    }
    work(id); /* we now have the lock and can do the work */
    omp_unset_lock(&lck);
  }
  omp_destroy_lock(&lck);
}

