// Test of OpenMP runtime library calls can be handled by autoPar's diff mode
// autoPar has to pass -fopenmp to a backend compiler:
// Detect backend compiler used. 
// Pass proper flags accordingly. 
#include <stdio.h>  
#include <omp.h>  
  
int main()  
{  
#pragma omp parallel  
#pragma omp master  
  {  
    printf("%d\n", omp_get_num_threads());  
  }  
}

