// #include <iostream>
// #include <typeinfo>

namespace std 
   {
     class type_info 
        {
          public:
               bool operator==(const type_info& __arg) const;
        };
   }

using namespace std;

class X
   {
     public:
          X& operator<<(bool b);
   };

// X& X::operator<<(bool b);

X x;

// struct Base {};
// struct Derived : Base {};

int main() 
   {
  // non-polymorphic types:
  // Derived derived;
  // Base* pbase = &derived;

  // bool test1 = ( typeid(int)==typeid(int) );
  // bool test2 = ( typeid(derived)==typeid(*pbase) );

  // Should be unparsed as: cout << ( typeid(derived)==typeid(*pbase) );
  // cout << ( typeid(derived)==typeid(*pbase) );
  // x << ( typeid(derived)== typeid(*pbase) );
  // x << ( typeid(int) == typeid(long) );
     x << ( true == false );
  // bool test3 = typeid(derived) == typeid(*pbase);
   }

