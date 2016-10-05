
#include<string>

template <typename T>
class X
   {
     public: 
       // class iterator {};

       // typedef iterator* iterator_ptr;

       // Another case where the return type needs a "typename" keyword (for gnu g++ version 4.4.7)
          std::string & foo() {}
   };

// template X<int>::iterator X<int>::foo();

void foobar()
   {
     X<int> x;
     x.foo();
   }
