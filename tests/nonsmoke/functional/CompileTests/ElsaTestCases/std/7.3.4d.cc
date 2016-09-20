// 7.3.4d.cc

namespace A {
  class X { };
  extern "C"   int g();
  extern "C++" int h();
}
namespace B {
  void X(int);
  extern "C"   int g();
  extern "C++" int h();
}
using namespace A;
using namespace B;

void f() {
  //ERROR(1): X(1);    // error: name X found in two namespaces
  g();                 // okay: name g refers to the same entity
  //ERROR(2): h();     // error: name h found in two namespaces
}
