
template <typename T>
struct A {
  void foo() {
    T v;
  }
};

void bar() {
  A<float> a;
}


