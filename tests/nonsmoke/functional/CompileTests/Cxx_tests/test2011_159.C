// This is a simpler case of a bug demonstrated in test2004_10.C
class var21
   {
  // No constructor exists...
   };

int foo()
   {
  // Here we call a constructor with "()" though the constructor is not defined (case of dik_zero).
     var21* test2 = new var21();

     return 0;
   }
