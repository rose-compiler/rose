struct A {};
struct C : A {};
C g();
void f (const A & x);
void f ();
int main () {
  f (g ());
}
