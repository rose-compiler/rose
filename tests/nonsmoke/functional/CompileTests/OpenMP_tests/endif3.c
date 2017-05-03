/* various cases of #if ...#endif
 * */
#include<stdio.h>
static int par_res;

int fib (int, int);
// Easiest case: the extend of #if ..#endif is explicit as {} is used
int fib0 (int n)
{
//#pragma omp parallel
#pragma omp single
  {
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
    par_res = fib(n,0);
#else
    par_res = fib0(n);
#endif
  }
  printf("Fibonacci result for %d is %d\n",n,par_res);
  return par_res;
}

// hard case for ROSE: #endif is attached to printf
// it should be moved to par_res = fib(n)
int fib1 (int n)
{
//#pragma omp parallel
#pragma omp single
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
    par_res = fib(n,0);
#else
    par_res = fib1(n);
#endif
//  printf("Fibonacci result for %d is %d\n",n,par_res);
   return par_res;
}



//TODO parallel joins the party
int fib2 (int n)
{
#pragma omp parallel
#pragma omp single
  { // this bracket is essential  now, since we don't use wave by default to decide the scope of #endif!!
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
  par_res = fib(n,0);
#else
  par_res = fib2(n);
#endif
  }
  return par_res;
}
