#include <stdio.h>

#if defined(_OPENMP)
#include <omp.h>
#endif /* _OPENMP */

#define NUMELEMENT 10000000
static double a[NUMELEMENT];

static void init(void)
{
  int i=0,j;
   i=i+5; 
/*assume onthread 1,3 */
#pragma omp for
  for (i=0;i<NUMELEMENT;i++)
   {
    a[i]=(double)i/2.0;
    a[i]=(double)i/2.0;
    a[i]=(double)i/2.0;
    a[i]=(double)i/2.0;
   }
/*default team, on all threads*/
#pragma omp single 
  {
    j=omp_get_thread_num();
   printf("I am the single one: %d\n",j );
  }
}

int main(void)
{
#pragma omp parallel
 {
  init();
 }
 return 0;
}

