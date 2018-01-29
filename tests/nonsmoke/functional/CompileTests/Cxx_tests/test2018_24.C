
// This code compiles as a C++11 file and only if the initializer_list header is include.
#include<initializer_list>

#define STD std::

struct A 
   {
     A(STD initializer_list<char>) : b(1) { }
     A(STD initializer_list<int>) : b(2) { }
     A() : b(3) { }
     int b;
   };
