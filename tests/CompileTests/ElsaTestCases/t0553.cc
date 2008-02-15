// t0553.cc
// similar to t0551.cc but involving an intermediate template class

template <class T>
class C {
public:
  void f(T &);
};

template <class T>
class A {
public:
  struct B {
    T x;
  };

  void g(C<B> &c)
  {
    B b;
    c.f(b);
  }

  void h(int)
  {
    C<B> c;
    g(c);
  }
};




template void A<int>::h(int);
