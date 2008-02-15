// Strange example of typedef and declaration from EDG manual (section 11.3, page 270)

#include <stdio.h>

typedef int I;

int
main ()
   {
#if 1
     I(i) = 42;   // equivalent to "int i = 42;"
     int (j) = 43;
#else
     int i = 42;

  // This example from the EDG manual fails with the KCC compiler!
     I(i)++; // cast i to I and then increment
#endif

     printf ("i = %d j = %d \n",i,j);
     return 0;
   }

