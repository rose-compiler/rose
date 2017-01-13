// t0424.cc
// get a base class member template via 'using'

struct B {
  template <class T>
  void f(T t) {}
};

struct D : public B {
  using B::f;
  D() {
    f(1);
  }
};
