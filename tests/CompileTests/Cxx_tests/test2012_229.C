void foo ()
   {
     int x;

  // BUG: the TRUE statement is output in the function scope and the true branch is empty.
     if (true)
          int x; 
   }

