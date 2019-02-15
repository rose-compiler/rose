
template <typename T>
void foo() {
  auto v = T::v;
}

struct A {
  static float v;
};

struct B {
  static double v;
};

void bar() {
  foo<A>();
  foo<B>();
}

