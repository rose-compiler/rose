#include <stdio.h>

int x = 0;

int foobar()
   {
     return x + 7;
   }

// This is a test code for the Interl Pin support in ROSE,
// it is used as input to the ROSE pin tool (run by pin).
int
main()
   {
  // x = foobar();
     for (int i=0; i < 10; i++)
        {
          x++;
        }

     if (x < 0)
        {
          printf ("Took a rare branch! \n");
          x = foobar();
          x = foobar();
        }

     return 0;
   }
