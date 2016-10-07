// src/roseSupport/utility_functions.C:1358: SgStatement* rose::getNextStatement(SgStatement*): Assertion `false' failed.

// This test code also demonstrates the introduction of a larger 
// amount of new lines when using the -rose:unparse_tokens option.

void foobar()
   {
     double array[4];

     if (true) 
        {
       /* comment */
          array[2] = 5;

       skip:
          if (true) { }
        }
       else 
        {
          array[2] = 5;
        }
   }

