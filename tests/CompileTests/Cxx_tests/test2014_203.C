template <typename T>
class X
   {
     public: 
          class iterator {};

          iterator foo() {}
   };

// template X<int>::iterator X<int>::foo();

void foobar()
   {
     X<int> x;
     x.foo();
   }

