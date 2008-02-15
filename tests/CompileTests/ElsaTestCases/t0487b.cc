// t0487b.cc
// more complex version of t0487.cc, exposing need to
// translate the DQT at the level of CVAtomicType, not
// just AtomicType

template <class T>
void f(T*, typename T::INTC *, int);

struct C {
  typedef int const INTC;
};

void g(C *c, int const *i)
{
  f(c, i, 1);
}
