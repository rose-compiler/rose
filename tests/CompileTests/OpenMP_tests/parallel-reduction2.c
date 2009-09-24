#include<assert.h> 
#include<omp.h> 
#include<stdio.h> 

int main(void)
{
  int i =100, sum=100;
  int thread_num;
#pragma omp parallel reduction(+:sum)
  {
#pragma omp single
   {
     thread_num = omp_get_num_threads();
   }
    sum += i;
  }
  printf("thread num=%d sum =%d\n", thread_num, sum);
  assert(sum == (i*thread_num + 100));

  sum = 100;
#pragma omp parallel private(sum)
  {
#pragma omp single
   {
     thread_num = omp_get_num_threads();
   }
    sum += i;
  }
  printf("thread num=%d sum =%d\n", thread_num, sum);
  assert(sum != (i*thread_num + 100));


  return 0;
}
