struct A {};
template <class T> struct B {};
struct C {
  operator void **();
};
template <class T> C f(B<T>&) {}
struct D {
  static void g(A &, void **);
  static void g(char *, void **);
};
int main () {
  B<A> x;
  A y;
  D::g(y, f(x));
}
