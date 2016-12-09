#include<stdio.h>

int main()
   {
  // int i;
#pragma omp parallel
        {
          printf("Hello.\n");
        }

     return 0;
   }
