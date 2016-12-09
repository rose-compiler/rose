/*Example to show outlining vs. inlining*/

#ifdef _OPENMP
#include <omp.h>
#endif 

void do_sth(int a, int b, int c)
{
}

int main(void)
{
  int a,b,c;

#pragma omp parallel private(c) firstprivate(a)
  do_sth(a,b,c);
  
  return 0;
}

