class X
   {
     public:
          void foo();
          friend void foobar();
   };

class Y
   {
     public:
          void foo();
          friend void foobar();
   };

void foobar();

void X::foo()
   {
     foobar();
   }

void Y::foo()
   {
     foobar();
   }

