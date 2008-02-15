// t0484.cc
// template member operator (from A.E.)

struct A {
  template <class T>
  A& operator<<(T t);
  
  // hmm.. I can't make a unary operator like this because it would
  // have no arguments with which to perform deduction, and there is
  // no way to explicitly specify template args for operator
  // invocations (w/o using explicit function call syntax, which is
  // not my concern at the moment)
};

void f(A& a, int b) {
  a.operator<<(b);
  a << b;
}
