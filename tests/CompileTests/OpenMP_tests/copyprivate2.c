/*single and copyprivate*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
 
 
int main (int argc, char * argv[])
{
float x=0.0;
int y=0;
#ifdef _OPENMP
omp_set_num_threads(4);
#endif
#pragma omp parallel private(x,y)
  {
  #pragma omp single copyprivate(x,y)
  {
    x=546.0;
    y=777;
  }
 printf("x=%f, y=%d\n",x,y);
  }
  return 0;
}
