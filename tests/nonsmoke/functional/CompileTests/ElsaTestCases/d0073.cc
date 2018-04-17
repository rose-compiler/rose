struct A {};
struct B {};

// overloaded function f(); one is a template
void f(A &) {}
template<class T>
void f(B &) {}

int main() {
  A x;
  f(x);
}
