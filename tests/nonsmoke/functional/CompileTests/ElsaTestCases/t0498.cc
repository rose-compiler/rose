// t0498.cc
// use a template parameter in an E_constructor of default arg
// of method in nested template class

struct A {
  template <class T>
  struct B {
    void f(T t = T())
    {}
  };
};



