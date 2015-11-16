// This test code is testing only formatting, not correctness.

int xyz;

void foobar()
   {
  // The behavior of the token-based unparsing is different if the variable moved 
  // is the last variable in a declaration contianing multiple declarations.
     int abc,i,j,k;
     int m;
     for (j = 0; j < 42; j++)
        {
          i = 42;
       // This comment is lost in the generated code!
       // k = 42;
          m = 42;
          xyz = 42;
        }
   }
