// DQ (7/21/2020): Moved function calls into a function.
void foobar()
   {
  // DQ (7/21/2020): Failing case should not be tested
  // new int(*[10])(); // error: parsed as (new int) (*[10]) ()
     new (int (*[10])()); // okay: allocates an array of 10 pointers to functions
   }

