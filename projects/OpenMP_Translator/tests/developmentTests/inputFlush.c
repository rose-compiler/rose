// Test input for omp flush, considering several cases:
//
// a. omp flush, without any variables specified
// b. omp flush (var_list), with one or more variables specified
// c. orphaned omp flush
// d. #pragma omp flush has preprocessing info. attached
//
// Liao, 4/25/2008
#if defined(_OPENMP)
#include <omp.h>
#endif

double x=1.0;

int main(void)
{
 int a=0,b; 
#pragma omp parallel
{
#if 1
  #pragma omp flush (a,b)
  a=a+1;
  #pragma omp flush
  b=a;
#endif  
}  

return 0;
}

void foo()
{
#if 1  
#pragma omp flush(x)
  x = 2.0;
#endif  

}
