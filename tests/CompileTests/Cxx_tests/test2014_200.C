template <typename T>
class X
   {
     public: 
          T foo() {}
   };

// template void X<int>::foo(int t);

// Use X::foo(int) so that we can force it to be instantiated as
// an alternative to explicit template instatiation directive.
void foobar()
   {
     X<int> x;
     x.foo();
   }
