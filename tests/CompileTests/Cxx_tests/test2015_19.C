void f() {
}

namespace A {
  void f() {
  }
}

using namespace A;

int main() {
  A::f();
  ::f();
  return 0;
} 

// ROSE unparses function main as "A::f(); f()" instead of "A::f(); ::f();".
