
struct X
   {
     X(double);
   };

void f(double adouble) 
   {
  // This is a function.
     X i(X(adouble));

  // This is a variable.
     X j((X(adouble)));
   }
