/*
Array typed firstprivate variables:
 element-by-element copy.

Contributed by Pranav Tendulkar
pranav@ics.forth.gr

4/12/2010
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int array[100];
int main()
{
  // Sara 5/13/2013
  // Initialize all positions for run-time check
  int i;
  for(i=0;i<100;i++)
    array[i] = 10;
  
#pragma omp parallel firstprivate(array)
  {
    int i;
    for(i=0;i<100;i++)
      array[i] += i;
  }
  
  // Sara 5/13/2013
  // Check result since firsprivate copies were not properly handled
  for(i=0;i<100;i++)
      assert( array[i]==10 );
  
  return 0;
}

