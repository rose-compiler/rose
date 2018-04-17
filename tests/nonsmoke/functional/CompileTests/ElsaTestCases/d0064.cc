// test lots of combinations of forwarding
template<class T, int I> T f(T x, T i[I]);
template<class T, int I> T f(T x, T i[I]);
template<class T, int I> T f(T x, T i[I]) {
  return x;
}
template<class T, int I> T f(T x, T i[I]);
template<class T, int I> T f(T x, T i[I]);

template<class T, int I> struct A;
template<class T, int I> struct A;
template<class T, int I> struct A {
  T x;
  A() : x(I) {}
};
template<class T, int I> struct A;
template<class T, int I> struct A;

// Hmm, you just can't do this
//  template<class T, int I> T g(A<T*, I+1> a, (*f1)f<T[], I+2>, I z);
template<class T, int I> T g(A<T*, I> a, T i[I]);
template<class T, int I> T g(A<T*, I> a, T i[I]);
template<class T, int I> T g(A<T*, I> a, T i[I]) {
  return i[0];
}
template<class T, int I> T g(A<T*, I> a, T i[I]);
template<class T, int I> T g(A<T*, I> a, T i[I]);

int main() {
  int x = 3;
  int y[4];
  f<int, 4>(x, y);
  A<int*, 0/*NULL*/> b;
  g(b, y);
}
