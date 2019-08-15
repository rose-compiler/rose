template <typename T>
struct A {
  void foo() {}
};

int main() {
  A<int> a;
  a.foo();
  return 0;
}
