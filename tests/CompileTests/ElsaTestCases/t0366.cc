// t0366.cc
// testing some of the finer points of arg-dep operator lookup

namespace N {
  class A {};
  class B {};
  A& operator<<(A&, B&);
  A& f(A&, B&);

  struct C {        //ERRORIFMISSING(3): just testing multitest.pl
    void operator+(B&);
  };
}

// would make the use of '<<' below ambiguous
//ERROR(2): void operator<<(N::A&, N::B&);

void g() {
  N::A a;
  N::B b;
  f(a, b);  // <=== OK
  a << b;   // <=== KO
}

void h(N::C &c, N::B &b)
{
  // this tests that during 3.4.2 lookup we skip class members,
  // because otherwise we would find C::operator+ twice and
  // thus complain about an ambiguity
  c + b;
}

enum E { E_FOO };

struct D {
  D(E e);     // implicit conversion to D from E
};

void foo(D d);
void operator*(D d);

void j(D &d)
{
  E e = E_FOO;

  foo(d);
  foo(e);     // possible via implicit conversion

  *d;         // finds ::operator*(D)                                
  
  // icc recognizes this as illegal, gcc (3.4.0, 3.4.3) does not
  //ERROR(1): *e;         // finds nothing, since no class-typed args
}



