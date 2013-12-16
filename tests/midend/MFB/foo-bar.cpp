
#include "foo-bar.hpp"

namespace B {

void bar(A::foo arg) {
  int a;

  arg.x = A::y.x;
}

}
