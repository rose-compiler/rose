#include <stdlib.h>
#include <stdio.h>
#include "omp.h"


int main()
{
 int i = 0;
 
 int tid =0;


#pragma omp parallel 
{ 
 
   i++;  
   
   tid = omp_get_thread_num();
 }

 return 0;

}

