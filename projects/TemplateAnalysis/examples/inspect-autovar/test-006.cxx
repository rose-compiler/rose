
template <typename T>
void foo() {
  T v;
}

void bar() {
  auto lambda = [&](int v) {
    foo<int>();
  };
}

