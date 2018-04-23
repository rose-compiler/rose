namespace Y
   {
   }

void foobar()
   {
  // Without global qualification.
     namespace Y = Y;

  // Repeated namespace alias (allowed).
     namespace Y = Y;
   }

