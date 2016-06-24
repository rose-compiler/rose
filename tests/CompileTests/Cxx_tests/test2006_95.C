// Test of use of C++ typeid function
/*
The following is the output of the above example:

ap: B
ar: B
cp: C
cr: C

Classes A and B are polymorphic; classes C and D are not. Although cp and cr refer to 
an object of type D, typeid(*cp) and typeid(cr) return objects that represent class C.
*/

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>
#include <typeinfo>
using namespace std;

struct A { virtual ~A() { } };
struct B : A { };

struct C { };
struct D : C { };

int main() {
  B bobj;
  A* ap = &bobj;
  A& ar = bobj;
  cout << "ap: " << typeid(*ap).name() << endl;
  cout << "ar: " << typeid(ar).name() << endl;

  D dobj;
  C* cp = &dobj;
  C& cr = dobj;
  cout << "cp: " << typeid(*cp).name() << endl;
  cout << "cr: " << typeid(cr).name() << endl;

  cout << "expression: " << typeid(true && false).name() << endl;
  bool t = true, f = false;
  cout << "expression: " << typeid(t && f).name() << endl;
  int less = -1, more = 1;
  cout << "expression: " << typeid(less < more).name() << endl;
  cout << "expression: " << typeid(less | more).name() << endl;
  cout << "expression: " << typeid(less + more).name() << endl;
}

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

