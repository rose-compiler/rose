
namespace N
   {
     class ABC {};
   }

void foobar()
   {
  // Should unparse as: using void_t = void;
  // using DEF = N::ABC {};
     using DEF = N::ABC;
   }
