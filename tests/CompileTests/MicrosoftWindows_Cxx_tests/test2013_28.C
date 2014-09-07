// This test code test the casting of const for both primative types and for class types.

int &
foobar( const int & x )
   {
      return (int &) x;
   }

#if 1
class X
   {
     public:
          void foo();
   };

void foobar ( const X & x )
   {
     return ((X &) x).foo();
   }
#endif

