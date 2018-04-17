struct A {};
template <class T> struct B {};
template <class T> struct C {
  operator void **();
};
template <class T> C<T> f(B<T>&) {}
void g(void **, double);
void g(void **, int);
int main() {
  B<A> x;
  g(f(x), 1);
}
