// test pass by value
class A{};
void g(A a) {
}
void f() {
  A a2;
  g(a2);
}
