struct S {
  S (int);
};
struct T{
  T(S const &);
};
void f() {
  T x(S(1));
}
