
template <typename T>
struct A {
  void foo() {
    T v;
  }

  static void bar();

  static char c;
};

A<float> bar_1() { }

void bar_2(A<char> c) { }

void bar_3() {
  A<int> a;
}

void bar_4() {
  A<double>::bar();
}

void bar_5() {
  char c = A<long>::c;
}

