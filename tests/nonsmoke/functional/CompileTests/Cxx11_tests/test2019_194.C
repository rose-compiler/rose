struct X 
   {
     typedef long T;
     short foo(long n);
   };

typedef short T;

T X::foo(T n)
// ^::T  (short)
// ^ X::T  (long)
   {
     return 7;
   }
