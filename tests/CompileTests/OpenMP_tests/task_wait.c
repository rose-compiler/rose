
/* input: result
 * 5 : 5
 * 10: 55
 * 20: 6756
 * 30: 832040
 * 40: 102334155
 * 50: runs for quite a while. for int type and return negative values
 *     using unsigned long int: 3 996 334 433
 *
 * Based on A.13.4c, p182 of OMP 3.0 spec.
 *
 * How much speedup can we get when omp taskwait is used??
 *
 * 9/15/2008
 */
#include <stdio.h>
#include <assert.h>

unsigned long int input = 40;

unsigned long int fib(unsigned long int n) {
  unsigned long int i, j;
  if (n<2)
    return n;
  else 
  {
    #pragma omp task shared(i)
     i=fib(n-1);
    #pragma omp task shared(j)
    j=fib(n-2);
    #pragma omp taskwait
     return i+j;
  }
}

int main()
{
  unsigned long int result = 0;
#pragma omp parallel
  {
   #pragma omp single
    {
      result = fib(input);
    }
  }
  printf("Fibonacci number for %d is:%u\n",input, result);
  assert(result == 102334155);
  return 0;
 }
