// t0569.cc
// arg-dep lookup finds a friend

template <class T>
void f(T &t)
{
  g(t,t);
}

namespace N
{
  struct A {
    struct B {
    };

    friend void g(B &a, B &b);
  };

  // to find this, must look at the definition namespace (N) of the
  // class (A) of which B is a member
  void h(A::B&);
}

void foo()
{
  N::A::B e;
  f(e);
  h(e);
}
