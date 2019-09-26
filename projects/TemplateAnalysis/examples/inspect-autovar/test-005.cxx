
template <typename T>
struct A {
  void foo() {
    T v;
  }
};

template <typename T>
struct B {
  typedef T type;
};

void bar() {
  A<float> v;
  B< A<int> >::type w;
}

