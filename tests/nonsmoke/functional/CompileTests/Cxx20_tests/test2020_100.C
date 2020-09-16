// DQ (7/21/2020): Moved function calls into a function.
void foobar()
   {
     new int + 1; // okay: parsed as (new int) + 1, increments a pointer returned by new int

  // DQ (7/21/2020): Failing case should not be tested
  // new int * 1; // error: parsed as (new int*) (1)
   }
