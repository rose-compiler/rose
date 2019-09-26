
template <typename T>
void foo() {
  T v;
}

template <typename F>
void bar(F && f) {
  f(2);
}

void foobar() {
  bar([&](auto v) {
    foo<decltype(v)>();
  });
}

