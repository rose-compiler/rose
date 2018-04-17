/* A complex case of #if ..#endif
 * Extracted from BOTS
 *
 * Liao 2/4/2010
 * */
#include <stdio.h>
static int par_res;

extern int fib(int); 
void fib0 (int n)
{
        #pragma omp parallel
        #pragma omp single
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
        par_res = fib(n,0);
#else
        par_res = fib(n);
#endif
        printf("Fibonacci result for %d is %d\n",n,par_res);
}
