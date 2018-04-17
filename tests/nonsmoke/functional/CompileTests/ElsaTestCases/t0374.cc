// t0374.cc
// E.A.: user copy ctor should prevent compiler-supplied

struct B {
  B(const B&, int n = 0);
};

void f(const B& b) {
  B _b(b);
}
