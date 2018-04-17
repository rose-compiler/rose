// See test2007_33.C for an example of the exact opposent error.
// Likely the predicate in the condition is reversed!


int x = 1;

int
main()
   {
     int x = 2;
     int a;

  // Error: This will be unparsed as "a = x"
     a = ::x;

     return 0;
   }
