#include <typeinfo>

struct Base {};
struct Derived : Base {};

int main() 
   {
  // built-in types:
     int i;
     int * pi;
     typeid(int).name();
     typeid(i).name();
     typeid(pi).name();
     typeid(*pi).name();

     Derived derived;
     Base* pbase = &derived;

     bool test = ( typeid(derived)==typeid(*pbase) );
   }

