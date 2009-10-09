/************************************************************
 Copyright (c) 2007 Iowa State University, Glenn Luecke, James Coyle,
 James Hoekstra, Marina Kraeva, Olga Weiss, Andre Wehe, Ying Xu,
 Melissa Yahya, Elizabeth Kleiman, Varun Srinivas, Alok Tripathi.
 All rights reserved.
 Licensed under the Educational Community License version 1.0.
 See the full agreement at http://rted.public.iastate.edu/ .
*************************************************************
  File name of the main program for this test: c_C_10_1_a_D.upc
  Description of the error from the test plan:
            Out-of-bounds shared memory access using indices.
  Test description: Read one element past the second dimension bound
                    but within an array of N*K elements.
                    The array is declared as shared [K] <type> (*arrA)[K],
                    and allocated as one piece of shared memory.
                    Read arrA[0][K] on thread 0.

  Requires relaxed mode to contain an error (yes or no): no
  Support files needed:                c_C_10_1_a_D_s.upc

  Date last modified:                  September 18, 2008
  Name of person writing this test:    Olga Weiss
  Name of person reviewing this test:  Ying Xu
****************************************************************/

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




//#include "upcparam.h"
#define K 4
#define N 2*K

void work_arr(shared [K] double (*arrA)[K], int i);

void work_arr(shared [K] double (*arrA)[K], int i)
{
  double varA;

  varA = arrA[0][i] + 1;   /*RTERR*/
  printf("varA = %d\n", (int)varA);
}

int main() {
  int i,j;
  shared [K] double (* arrA)[K];   /*DECLARE1*/

  check_thr(NT);

  arrA = (shared [K] double (*) [K])upc_all_alloc(N, K*sizeof(double)); /*VARALLOC*/
  upc_barrier;
  if(MYTHREAD==0){
     if (arrA==NULL){
         printf("Not enough memory to allocate arrA\n");
         upc_global_exit(1);
     }
  }
  upc_forall(i=0;i<N;i++;&arrA[i])
      for(j=0;j<K;j++)
          arrA[i][j] = 1000+i*100+j;

  upc_barrier;

  if(MYTHREAD == 0)
     work_arr(arrA, K);

  upc_barrier;

  if(MYTHREAD == 0)
  {
     for(i=0;i<N;i++)
     {
        printf("arrA[%d][0]=%d\n", i, (int)arrA[i][0]);
     }
     upc_free(arrA);
  }

  return 0;
}
