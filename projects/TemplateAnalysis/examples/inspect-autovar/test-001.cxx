
template <typename T>
struct A {
  static T v;
};

template <typename T>
void foo() {
  auto v = A<T>::v;
}

void bar() {
  foo<float>();
  foo<double>();
}

