namespace N {
  template <typename T0>
  void foo(T0 t0);

  template <typename T1>
  struct A {};
}

template<typename T2>
void bar() {
  using A = N::A<T2>;
  using X = typename A::X;
  N::foo<X>();
}
