// Test if clause handling
// number of threads should be set to 1 if the if-clause's expression evaluates to be false
#include <assert.h>
#include <stdio.h>
#include <omp.h>

int main(void)
{
  int i=0;
#pragma omp parallel if(i == 0)
  {
    printf("Mutual exclusive output 1.\n");
  }

#pragma omp parallel if(i != 0)
  {
#pragma omp single
    {
      assert (omp_get_num_threads() == 1 );
    }
    printf("Mutual exclusive output 2.\n");
  }
 
  return 0;
}
