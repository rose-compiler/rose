#if 0
// Test code for default parameter specification in templated functions!
template<typename T>
class Y
   {
     public:
          Y(T i = 1);
   };

// Definition of member function outside of class is fine (works)!
template<typename T>
Y<T>::Y<T>( T i)
   {
   }
#endif


// Test use of default parameters in template member function declarations
template<typename T>
class Z
   {
     public:
          Z(T i = 1) {};
   };

// Use of templated class using constructor specificed with default argument
Z<int> z;


class X
   {
     public:
          X(int i = 1) {};
   };

X x;

namespace A
   {
     int foo(int i = 0);
   }

int A::foo(int i)
   {
     return 42;
   }

int a = A::foo();
