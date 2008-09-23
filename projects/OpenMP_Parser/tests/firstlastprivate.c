#include <stdio.h>
#include <omp.h>
#include <sys/times.h>
#include <time.h>

#ifndef CLK_TCK
#define CLK_TCK 60
#endif


double get_seconds() /*routine to read time*/
{
  struct tms rusage;
  times(&rusage);
  return (double) (rusage.tms_utime)/CLK_TCK;
}


int main()
{
  int i,sum=0;
  int num_steps=10000000;
  double sec0, sec;


  sec=0.0;
  sec0=get_seconds();
  omp_set_num_threads(4);

#pragma omp parallel for firstprivate (sum) lastprivate (sum)
for(i=1;i<=num_steps;i++)
{
  sum=sum + i;
}
 sec=sec+(get_seconds() -sec0);
 printf("sum of %d = %d  spent time:%f seconds\n",num_steps,sum, sec);

 return 1;
}

