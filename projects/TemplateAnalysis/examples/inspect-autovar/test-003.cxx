
template <typename T>
struct A {
  static void foo() {
    T v;
  }
};

void bar() {
  A<float>::foo();
}


