/* 
 * Example private of reference type or a variable used by address
 * 
 * */
#include<stdio.h>

int main ()
{
  int i =100;
  int &j=i;
#pragma omp parallel private(i)
  {
      printf ("%p %d \n", &i,j);
  }

  return 0;

}
