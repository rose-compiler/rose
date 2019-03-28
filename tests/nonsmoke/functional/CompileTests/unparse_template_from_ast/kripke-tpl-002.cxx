template <typename T0>
struct A {};

template<typename T1>
void foo() {
  using Afoo = A<T1>;
}
