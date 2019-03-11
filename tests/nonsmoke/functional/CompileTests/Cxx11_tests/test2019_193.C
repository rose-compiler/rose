struct X_ 
   {
     typedef long T_;
     short f(long n);
   };

typedef short T_;

T_ X_::f(T_ n)
   {
     return 7;
  /* n + 100; */ 
   }
//       ^::T_  (short)
//             ^ X_::T_  (long)

void foobar()
   {
     X_ x ;
     x.f(31);
   }

