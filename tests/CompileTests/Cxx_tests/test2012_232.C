void foo ()
   {
     int x;

  // BUG: the TRUE statment is output in the function scope and the true branch is empty.
     if (true)
          0;
       else
          1;

   }

