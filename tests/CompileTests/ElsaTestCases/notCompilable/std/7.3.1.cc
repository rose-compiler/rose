// 7.3.1.cc

asm("collectLookupResults i=6 i=9");

namespace Outer {
  int i;                  // line 6
  namespace Inner {
    void f() { i++; }     // Outer::i
    int i;                // line 9
    void g() { i++; }     // Inner::i
  }
}

