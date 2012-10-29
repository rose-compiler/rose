// This example demonstates a label with a parent that is not a scope.
// This is the essential subset of test2012_100.c, and variations test2012_122-123.c

void foobar()
   {
     while (1)
        {
          switch(2)
             {
               case 0:
               case 1:
               case 2:
            // The default must have a single statement to reproduce the bug.
               default:
                f_b: if (3) 43;
//                  return;
             }
        }
   }

