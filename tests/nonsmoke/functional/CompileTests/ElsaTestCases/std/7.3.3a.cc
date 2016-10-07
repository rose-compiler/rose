// 7.3.3a.cc

struct B {
  void f(char);
  void g(char);
  enum E { e };
  union { int x; };
};

struct D : B {
  using B::f;
  void f(int) { f('c'); }        // calls B::f(char)
  void g(int) { g('c'); }        // recursively calls D::g(int)
};
