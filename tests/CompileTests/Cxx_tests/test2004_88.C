// Test code attempts to use a vacuous destructor call 

  // DQ (11/19/2004): Temporarily commented out since this is a demonstrated bug now that we qualify everything!
#if 1
// Don't specify a destructor in this class (so the compiler will generate one)
class X {};

void destroy ( X* x_ptr )
   {
  // This should unparse to: "x_ptr->~X();"
     x_ptr->~X();
   }
#endif


