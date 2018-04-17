#include <stdio.h>
void foo(int dimensions)
{
   int numItems ;
   if (dimensions == 2) {
      numItems = 10 ;
   }
   else {
      numItems = 20 ;
   }
 
   double *buffer = new double[numItems] ;
   delete[] buffer ;
 
}
