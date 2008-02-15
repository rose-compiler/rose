struct A;
template<class T> struct B {};

template<class T> void foo (B<T> &);
template<class T> void foo (A*);

int main() {
  B<A> s;
  // this doubling is necessary to reproduce the bug
  foo(s);
  foo(s);
}
