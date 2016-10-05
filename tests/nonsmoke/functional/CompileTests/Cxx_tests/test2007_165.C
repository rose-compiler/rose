// #include <iostream>

namespace A
   {
     template<typename T>
     void foobar(T t);
   }

namespace A
   {
// template<typename T>
// struct X { T t; };
     template<typename T>
     void foobar(T t) {};
   }

// X<long> x;

void foo()
   {
     A::foobar(1);
   }
