void foobar()
   {
     int x = 0;
xxx: if (x)
          x = 1;
// Fortunately, this is not a legal place for label.
// xxx: else
       else
          x = 2;
   }
