#include <initializer_list>

void foobar()
   {
     struct A
        {
          A(std::initializer_list<char>) : b(1) { }
          A(std::initializer_list<int>) : b(2) { }
          A() : b(3) { }
          int b;
        };

     A a = { 'a' };
     a.b;
     A b = { 1, 2 };
     b.b;
     A c = { };
     c.b;
   }


