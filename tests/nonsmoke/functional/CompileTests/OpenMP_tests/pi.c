#include <omp.h>
#include <stdio.h>

#include <math.h>
#include <assert.h>

/*
Example use: 
  printf(" checking error diff ratio \n");
  diff_ratio (error, error_ref, 5); // 6 is better, 7 is very restrictive 
*/
// value, reference value, and the number of significant digits to be ensured.
double diff_ratio (double val, double ref, int significant_digits)
{
  assert (significant_digits>=1);
  double diff_ratio = fabs(val - ref )/fabs(ref);
  double upper_limit = pow (0.1, significant_digits); // 1.0/(double(10^significant_digits)) ;
//  printf("value :%E  ref_value: %E  diff_ratio: %E upper_limit: %E \n",val, ref, diff_ratio, upper_limit);
  // ensure the number of the significant digits to be the same 
  if (diff_ratio >= upper_limit)
    printf("value :%E  ref_value: %E  diff_ratio: %E >= upper_limit: %E \n",val, ref, diff_ratio, upper_limit);
  assert ( diff_ratio < upper_limit);
  return diff_ratio;
}


int num_steps = 10000;
int main()
{
  double x=0;
  double sum = 0.0, pi;
  int i;
  double  step = 1.0/(double) num_steps;
#pragma omp parallel for private(i,x) reduction(+:sum) schedule(static)
  for (i=0; i<num_steps; i=i+1)
  {
    x=(i+0.5)*step;
    sum = sum + 4.0/(1.0+x*x);
  }
  pi=step*sum;
  printf("%f, diff_ratio=%f\n", pi, diff_ratio (pi, 3.141593,6));
  return 0; 
} 
