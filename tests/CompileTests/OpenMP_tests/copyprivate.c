/*
A variable is both threadprivate and copyprivate.
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
float x=0.0;
int y=0;
#pragma omp threadprivate(x,y)
 
void init ()
{
  #pragma omp single copyprivate(x,y)
  {
    x=1.0;
    y=1;
  }
 printf("x=%f, y=%d\n",x,y);
}
 
int main (int argc, char * argv[])
{
#ifdef _OPENMP
omp_set_num_threads(4);
#endif
#pragma omp parallel
  {
    init ();
  }
  return 0;
}
