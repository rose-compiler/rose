// Test upper case
//  6/11/2010
#ifdef _OPENMP
#include <omp.h>
#endif

void c_print_results( )
{
  int NUM_THREADS, max_threads;


  max_threads = 1;
  NUM_THREADS = 1;

  /*   figure out number of threads used */
#ifdef _OPENMP
  max_threads = omp_get_max_threads();
#pragma omp parallel num_threads(6) shared(NUM_THREADS)
  {   
#pragma omp master
    NUM_THREADS = omp_get_num_threads();
  }
#endif


}
