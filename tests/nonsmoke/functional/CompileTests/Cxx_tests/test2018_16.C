#include <stdio.h>

int foobar()
   {
     int z = 42;
     printf ("Starting foobar: z = %d \n",z);
     return z;
   }

int main()
   {
     int x = 0;
     printf ("Starting main: x = %d \n",x);
     switch (x)
        {
          int y = foobar();
          if (x)
               printf ("true case: x = %d \n",x);
            else
               printf ("false case: x = %d \n",x);
        }

     printf ("Leaving main: x = %d \n",x);
   }
