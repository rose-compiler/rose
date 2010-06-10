#ifdef _OPENMP
#include <omp.h>
#endif

void c_print_results( )
{
  int num_threads, max_threads;


  max_threads = 1;
  num_threads = 1;

  /*   figure out number of threads used */
#ifdef _OPENMP
  max_threads = omp_get_max_threads();
#pragma omp parallel shared(num_threads)
  {   
#pragma omp master
    num_threads = omp_get_num_threads();
  }
#endif


}
