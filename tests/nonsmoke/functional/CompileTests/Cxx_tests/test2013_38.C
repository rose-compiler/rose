// type_info example
#include <iostream>
#include <typeinfo>

using namespace std;

struct Base {};
struct Derived : Base {};
struct Poly_Base {virtual void Member(){}};
struct Poly_Derived: Poly_Base {};

int main() {
  // built-in types:
  int i;
  int * pi;
  cout << "int is: " << typeid(int).name() << endl;
  cout << "  i is: " << typeid(i).name() << endl;
  cout << " pi is: " << typeid(pi).name() << endl;
  cout << "*pi is: " << typeid(*pi).name() << endl << endl;

  // non-polymorphic types:
  Derived derived;
  Base* pbase = &derived;
  cout << "derived is: " << typeid(derived).name() << endl;
  cout << " *pbase is: " << typeid(*pbase).name() << endl;
  cout << boolalpha << "same type? "; 

// DQ (1/26/2013): This is a presecidence problem that we need to fix and that is represented in test2013_43.C.
// cout << ( typeid(derived)==typeid(*pbase) ) << endl << endl;

  // polymorphic types:
  Poly_Derived polyderived;
  Poly_Base* ppolybase = &polyderived;
  cout << "polyderived is: " << typeid(polyderived).name() << endl;
  cout << " *ppolybase is: " << typeid(*ppolybase).name() << endl;
  cout << boolalpha << "same type? "; 
// cout << ( typeid(polyderived)==typeid(*ppolybase) ) << endl << endl;
}

