// test throw by value
struct A{};
void f() {
  A a;
  throw a;
}

struct B{};
void g() {
  throw B();
}
