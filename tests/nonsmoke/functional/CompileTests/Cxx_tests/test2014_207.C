template <typename T>
class X
   {
     public: 
          class iterator {};

          iterator foo(int i = 7) {}
   };

// template X<int>::iterator X<int>::foo();

void foobar()
   {
     X<int> x;
     x.foo();
   }

