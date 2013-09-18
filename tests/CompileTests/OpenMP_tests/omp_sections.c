#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

void funcA();
void funcB();

int main()
{
#pragma omp parallel 
{
   #pragma omp sections
   {
      #pragma omp section
        (void) funcA();

      #pragma omp section
        (void) funcB();
   } /*-- End of sections block --*/

} /*-- End of parallel region --*/

   return(0);
}

void funcA()
{
   printf("In funcA: this section is executed by thread %d\n",
        omp_get_thread_num());
}
void funcB()
{
   printf("In funcB: this section is executed by thread %d\n",
        omp_get_thread_num());
}
