/* Test multiple reduction clauses with different reduction operations
 */
#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 4

int main ()
{
 int i,total=1000000, yy=10000,zz=10000;
 double res=0.0;

 omp_set_num_threads(NUM_THREADS);

 #pragma omp parallel for reduction(+:res) reduction(*:yy,zz)
 for (i=0; i<= total; i++){
  res = res + i;
  res = res - 2*i;
  yy *=1;
  zz*=1;
  }
 
 printf("the sum of 1000000 is :%.0f\n", res);
 return 0;
}
