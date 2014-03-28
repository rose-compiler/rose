// This is a simplification of test2013_16.C as part of debugging.

// This code appears in flex_string.hpp as part of boost.
// It is used in ROSE, so ROSE will not compile ROSE until
// we can handle this construction of switch statements.

// This is a variation of Duff's device.

void
duff_foobar()
   {
     int a;

  // ERROR: This is unparsed as: "switch(a){ case 0: while(a){ a = 7; } case 1: { } }"
     switch (a)
        {
          case 0:
               while (a)
                  {
                    case 1: a = 7;
                  }
        }
   }

