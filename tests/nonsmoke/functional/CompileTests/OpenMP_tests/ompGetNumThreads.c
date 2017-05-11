/*

Contributed by pranav@ics.forth.gr

4/14/2010
*/
#include <stdio.h>
#include <omp.h>
// The non-existence of omp.h is essential to repeat the original bug
//#include <omp.h>

int main()
{
  int k;
  
#pragma omp parallel
  {   
#pragma omp master
    {   
      k = omp_get_num_threads();
      printf ("Number of Threads requested = %i\n",k);
    }   
  }   
  return 0;
}

