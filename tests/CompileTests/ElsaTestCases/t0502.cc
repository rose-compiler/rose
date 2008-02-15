// t0502.cc
// invoke method on non-dependent class with dependent base

template <class T>
struct A {
  struct B : T {};

  void f()
  {
    B b;
    b.foo();
  };
};

struct C {
  void foo();
};

void f()
{
  A<C> a;
  a.f();
}


// ---------------
// in contrast, this would be error; gcc diagnoses, icc does not
template <class T>
struct D {
  void f()
  {
    C c;                      // C has no dependent bases
    //ERROR(1): c.bar();      // and no method 'bar'
  };
};


// there is an intermediate variant where C is a nested class but
// still has no dependent bases; since gcc does not diagnose that
// case, I am not going to add a test requiring that Elsa diagnose it,
// even though right now it does (the standard allows both behaviors)

