class X
   {
     public:
          void foo();
          friend void foobar(X&x);
   };

class Y
   {
     public:
          void foo();
          friend void foobar(Y& y);
   };

X x;
Y y;

void X::foo()
   {
     foobar(x);
   }

void foobar(X&x);

void Y::foo()
   {
     foobar(y);
   }

void foobar(Y&y);
