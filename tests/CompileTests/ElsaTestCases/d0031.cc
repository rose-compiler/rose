// how come no FullExpression ?
struct A{
  operator int();
};
void f() {
  int x;
  x = A();
}
