// t0191.cc
// segfault: base->templateInfo->instantiations
//
// similar to t0189.cc, but it is a nested class of a
// template class
//
// needed for ostream

template <class T>
struct A {
  struct B;       // forward decl of nested class
};

template <class T>
struct A<T>::B {
  int x;
};

int foo()
{
  A<int>::B b;
  return b.x;
}
