struct A {};
template <class T> struct B {};
template <class T> struct C {
  operator void **();
};
template <class T> C<T> f(B<T>&) {}
struct D {
  static void g(A &, void **);
  static void g(char *, void **);
};
int main () {
  B<A> x;
  A y;
  D::g(y, f(x));
}
