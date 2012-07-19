
// This works fine, but fails to unparse.
// class {};

// Function introduced so that we could define union in non-global scope
void foo ()
   {
  // This works fine, but fails to unparse.
  // class {};

  // DQ (7/14/2012): This causes an error.
  // This does not unparse to be ANYTHING, it is ignored (perhaps as it should be)
     union {};

  // union { int x; int y; };
   }

