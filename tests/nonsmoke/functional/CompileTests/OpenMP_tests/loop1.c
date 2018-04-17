#include <stdio.h>
#include <omp.h>

int main()
{
  int i,j;
//  int innerreps = 100;
#pragma omp parallel private(j)
  {   
 //   for (j=0; j<innerreps; j++)
    {   
#pragma omp for schedule(static,2)
      for (i=0; i<32; i++)
      {   
       printf ("thread %d is executing %d \n",omp_get_thread_num(),i);
  //      delay(500);
      }   
    }   
  }   
  return 0;
}

