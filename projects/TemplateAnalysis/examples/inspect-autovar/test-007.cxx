
template <typename T>
void foo() {
  T v;
}

template <typename F>
void bar(F && f) {
  f(2);
}

struct A {
  template <typename T>
  void operator () (T v) {
    foo<T>();
  }
};

void foobar() {
  A a;
  bar(a);
}

