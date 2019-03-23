namespace N {
  template <typename T0>
  struct A {};
}

template<typename T1>
void foo() {
  N::A<T1> a;
}
