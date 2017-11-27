#include <omp.h>

void something_useful ( void );
void something_critical ( void );
void foo ( omp_lock_t * lock, int n )
{
  int i;

  for ( i = 0; i < n; i++ )
#pragma omp task
  {
    something_useful();
    while ( !omp_test_lock(lock) ) {
#pragma omp taskyield
    }
    something_critical();
    omp_unset_lock(lock);
  }
}
