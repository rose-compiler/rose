// 7.3.4e.cc

namespace D {
  int d1;
  void f(char);
}
using namespace D;

int d1;                  // OK: no conflict with D::d1

namespace E {
  int e;
  void f(int);
}

namespace D {            // namespace extension
  int d2;
  using namespace E;
  void f(int);
}

void f()
{
  //ERROR(1): d1++;      // error: ambiguous ::d1 or D::d1?
  ::d1++;                // OK
  D::d1++;               // OK
  d2++;                  // OK: D::d2
  e++;                   // OK: E::e
  
  // I don't get these right because my lookup interfaces are
  // not capable of returning sets of declarations
  //f(1);                  // error: ambiguous: D::f(int) or E::f(int)?
  //f('a');                // OK: D::f(char)
}
