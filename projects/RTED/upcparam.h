#include <upc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define NT 4                 /* default number of threads */

void check_thr(int num_thr)  /* function to check number of threads */
{
   if(THREADS!=num_thr){
      if(MYTHREAD == 0)
         printf("Wrong number of threads. THREADS = %d\n", THREADS);
      exit(1);
   }
}
  

/* function that returns an integer zero value which can not be calculated at compile time */
int zero(){
   return (int) (sin(0.1*MYTHREAD)/2.3);
}

