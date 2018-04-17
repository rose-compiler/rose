#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
  int array[100];
  int i;
  for(i=0;i<100;i++)
    array[i] = 10;
  
#pragma omp parallel firstprivate(array)
  {
    int i;
    for(i=0;i<100;i++)
      array[i] += i;
  }
  
  
  for(i=0;i<100;i++)
  {
      assert( array[i]==10 );
  }
  
  return 0;
}
