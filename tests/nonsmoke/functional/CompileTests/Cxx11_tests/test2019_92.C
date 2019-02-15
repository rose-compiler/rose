
struct S 
   {
     S();
   };

S* foo(int n) 
   {
  // This is only a problem when using an array.
     return new S[n];
   }

// S* s = new S[2];
