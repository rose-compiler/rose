namespace N {
  template <typename T0>
  void foo();

  template <typename T1>
  struct A {};
}

template<typename T2>
void bar() {
  N::foo<typename N::A<T2>::X::Y>();
}
