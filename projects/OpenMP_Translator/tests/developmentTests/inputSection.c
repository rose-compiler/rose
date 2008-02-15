/*
test case for sections
including clause handling, explicity section, combined parallel sections

by C. Liao
July 13, 2006
*/
#include "omp.h"
#include <assert.h>
#include <stdio.h>

int main(){
  int sum=7;
  int known_sum;
  int i;
  int k_3=7;
#pragma omp parallel
  {
#pragma omp sections reduction(+:sum) private(i) firstprivate(k_3) lastprivate(k_3) nowait
    {
#pragma omp section
      {
        for (i=1;i<400;i++){
          sum += i;
        }
       k_3+=1;
      }
#pragma omp section
      {
        for(i=400;i<700;i++)
          sum += i;
       k_3+=2;
      }
#pragma omp section
      {
        for(i=700;i<1000;i++)
           sum += i;
       k_3+=3;
      }

    }/* end of section reduction.*/
  }                      /* end of parallel */
  known_sum=(999*1000)/2+7;

// combined parallel sections
#pragma omp parallel sections
  {
#pragma omp section
  printf("sum is:%d\n",sum);
#pragma omp section
  printf("k_3 is:%d\n",k_3);
  }
  
  return 0;
}

