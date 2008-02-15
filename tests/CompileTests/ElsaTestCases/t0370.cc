// t0370.cc
// another from A.E.

template <class T>
struct B {
  static const int _n = -1;
  B(int n = _n);
};

void f() {
  B<int> b;
}
