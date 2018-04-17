/*
test predefined macro names 
Liao, 6/19/2008
*/
#include <stdio.h>
#ifdef __UPC_DYNAMIC_THREADS__
  shared int array[100*THREADS];
#endif

#ifdef __UPC_STATIC_THREADS__
  shared int array[100];
#endif

int main()
{
  printf ("Predefined macro name: __UPC__ has value of %d\n",__UPC__);  
  printf ("Predefined macro name: __UPC_VERSION__ has value of %d\n",__UPC_VERSION__);  

#ifdef __UPC_DYNAMIC_THREADS__
 printf ("Predefined macro name:__UPC_DYNAMIC_THREADS__ has value of %d\n",__UPC_DYNAMIC_THREADS__);  
#endif

#ifdef __UPC_STATIC_THREADS__
 printf ("Predefined macro name:__UPC_STATIC_THREADS__  has value of %d\n",__UPC_STATIC_THREADS__ );  
 printf ("Predefined macro name: THREADS has value of %d\n", THREADS);  
#endif
  printf ("array should be defined by either one, with size of : %d\n", sizeof (array));

  return 0;
}
