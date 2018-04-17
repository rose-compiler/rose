#include <stdio.h>
#include <omp.h>

int main (void)
{

  int i,myval;
#pragma omp parallel for private(myval) ordered
  for(i=1; i<=100; i++){
    myval=i*i; 
#pragma omp ordered
    {
      printf("%d %d\n", i, myval);
    }
  }
  return 0;
}
