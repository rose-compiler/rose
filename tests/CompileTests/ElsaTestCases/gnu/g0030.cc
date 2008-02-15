// g0030.cc
// testing "extern template"

template <class T>
struct A {
  int foo()
  {
    // This code is not valid when T = int, but I will not
    // notice because I won't actually instantiate it.
    typename T::Foo f;
    return f;
  }
};

extern template class A<int>;
