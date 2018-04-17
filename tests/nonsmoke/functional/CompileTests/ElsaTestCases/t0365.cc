// t0365.cc
// another one from Altac Edena

namespace N {
  class A {};
  class B {};
  A& operator<<(A&, B&);
  A& f(A&, B&);
}

void g() {
  N::A a;
  N::B b;
  f(a, b);  // <=== OK
  a << b;   // <=== KO
}
