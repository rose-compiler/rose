namespace A {
  void f() {
  }

  namespace B {
    void f() {
    }
  }
}

using namespace A::B;
using namespace A;

void f() {
}

int main() {
  A::B::f();
  A::f();
  return 0;
}

// ROSE unparses "using namespace A::B;" as "using namespace B;" instead of "using namespace A::B;".
