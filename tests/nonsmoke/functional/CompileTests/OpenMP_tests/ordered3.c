#include <stdio.h>
#include <omp.h>

int main (void)
{
  int i,j, myval;
#pragma omp parallel for private(myval) ordered(2)
  for(i=1; i<=100; i++)
    for(j=1; j<=100; j++)
    {
      myval=i*j; 
#pragma omp ordered
      {
        printf("%d %d\n", i, myval);
      }
    }
  return 0;
}
