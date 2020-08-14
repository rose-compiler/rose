extern "C"
   {
     class X_
        {
          int i;
          friend int foo(X_ &);
          public:
          X_(int ii) : i(ii) { }
          static int j;
        };
   }

extern "C" int foo(X_ &x)
   {
     ++X_::j;
     return ++x.i;
   }

int X_::j = 0;


void foobar()
   {
     X_ x(2);
     foo(x);
     X_::j;
   }
