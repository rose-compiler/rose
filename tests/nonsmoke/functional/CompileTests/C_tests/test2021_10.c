void foobar()
   {
  // DQ (11/13/2021): This is a problem for the new multifile handling which names anonymous type declarations.
     typeof(
         ({ 
             int __ptr = 7; 
             42;
          })) sssss = 12;
   }
