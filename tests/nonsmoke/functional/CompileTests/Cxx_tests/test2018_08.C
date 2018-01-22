void foobar()
   {
     int x = 1;

  // Demonstration of rare use of seperate scopes for the true and false branches (every if 
  // statement has 3 different scopes, but only 2 can be used to hold different declarations).
     if (x)
          int x = 2;
       else
          int x = 3;
   }
