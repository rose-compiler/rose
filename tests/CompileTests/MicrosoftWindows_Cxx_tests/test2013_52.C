// This is a simplification of test2013_17.C as part of debugging.
// This is a variation of Duff's device.

// NOTE: This test case will work properly if the first statement in the swithc body is a case statement.

void
duff_foobar()
   {
     int a;

  // Error: this unparses as: "switch(a){ case 1: while(a){ a = 7; } }"
     switch (a)
        {
          while (a)
             {
          case 1: a = 7; a = 8;
             }
        }
   }

