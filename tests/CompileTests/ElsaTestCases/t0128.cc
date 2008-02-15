// t0128.cc
// bind a 'C' to a 'A const &'
// isolated by dsw

struct A {};
struct C : A {};
C g();
void f (const A & x);
void f ();
int main () {
  f (g ());
}
