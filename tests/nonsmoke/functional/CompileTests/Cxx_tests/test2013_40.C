#include <typeinfo>

struct Base {};
struct Derived : Base {};

// struct Poly_Base {virtual void Member(){}};
// struct Poly_Derived: Poly_Base {};

int main() 
   {
  // non-polymorphic types:
     Derived derived;
     Base* pbase = &derived;

     bool test1 = ( typeid(int)==typeid(int) );
     bool test2 = ( typeid(derived)==typeid(*pbase) );
   }

