
struct X {
      void foobar();
};

void X::foobar(){};

void foo()
   {
     X x;
     x.foobar();
   }
