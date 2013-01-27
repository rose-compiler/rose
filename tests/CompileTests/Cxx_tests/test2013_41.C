#include <iostream>
#include <typeinfo>

using namespace std;

// class X {};
// X& operator<<(bool b);
// X x;

struct Base {};
struct Derived : Base {};

int main() 
   {
  // non-polymorphic types:
     Derived derived;
     Base* pbase = &derived;

  // bool test1 = ( typeid(int)==typeid(int) );
  // bool test2 = ( typeid(derived)==typeid(*pbase) );

  // Should be unparsed as: cout << ( typeid(derived)==typeid(*pbase) );
     cout << ( typeid(derived)==typeid(*pbase) );
  // x << ( typeid(derived)==typeid(*pbase) );
   }

