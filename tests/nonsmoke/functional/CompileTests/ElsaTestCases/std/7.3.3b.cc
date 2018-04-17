// 7.3.3b.cc

// implied prerequisite; actual example doesn't include B
struct B {
  void f(char);
  void g(char);
  enum E { e };
  union { int x; };
};

class C {
  int g();
};

class D2 : public B {
  using B::f;              // OK: B is a base of D2
  using B::e;              // OK: e is an enumerator of base B
  using B::x;              // OK: x is a union member of base B
  //ERROR(1): using C::g;              // error: C isn't a base of D2
};
