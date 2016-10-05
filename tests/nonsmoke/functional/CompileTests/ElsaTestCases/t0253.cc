// t0253.cc
// uninstantiated template body,
// invoking a method on a concrete instantiation,
// of a template class with non-type parameter,
// that method being defined out of line

// needed for ostream, std::allocator::allocate()

template <int I>
class A {
public:
  void foo(int n);
};

template <int I>
void A<I>::foo(int n)
{ }

template <class T>
class B {
public:
  void bar(int n)
  {
    A<0>::foo(n);
  }
};
