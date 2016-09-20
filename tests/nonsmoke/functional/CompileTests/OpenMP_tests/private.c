/*
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 

float x;
int y;

int main (int argc, char * argv[])
{
#ifdef _OPENMP
   omp_set_num_threads(4);
#endif
    x=1.0;
    y=1;
#pragma omp parallel private(x)
  {
 printf("x=%f, y=%d\n",x,y);
  }

  return 0;
}

