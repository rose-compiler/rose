// 7.3.3f.cc

namespace A {
  int i;
}

namespace A1 {
  using A::i;
  using A::i;                     // OK: double declaration
}

void f()
{
  using A::i;
  //ERROR(1): using A::i;         // error: double declaration
}

class B {
public:
  int i;
};

class X : public B {
  using B::i;
  //ERROR(2): using B::i;         // error: double member declaration
};
