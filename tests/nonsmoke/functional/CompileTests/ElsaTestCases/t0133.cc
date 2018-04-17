// t0133.cc
// d0011.cc but with no templates

struct A {};
struct B {};
struct C {
  operator void **();
};
C f(B&);
struct D {
  static void g(A &, void **);
  static void g(char *, void **);
};
int main () {
  B x;
  A y;
  D::g(y, f(x));
}
