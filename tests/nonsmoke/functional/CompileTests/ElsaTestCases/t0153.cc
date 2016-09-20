// t0153.cc
// explicit ctor call expr, as RHS of assignment

struct A {
  A() {}
  A(int x) {}
};
int main() {
  int arg = 3;
  A a;
  (1, A(arg));   // test the ctor call alone
  a = A(arg);
}
