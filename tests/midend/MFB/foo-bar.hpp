
namespace A {

  struct foo {
    int x;
    foo f();
  };

  foo y;

}

namespace B {
  void bar(A::foo arg);
}
