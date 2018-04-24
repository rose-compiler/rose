class A {
};

class B {
};

double f0(double a1, long double b, float a2) {
  double x;
  long double y;
  float z;
  return x;
};

/* => X, Y&, X& */
A f1(A a1, B b, A a2) {
  A x;
  B y;
  A z;
  return x;
};

/* => X, Y, const X& */
A f2(A* a1, B b, A a2) {
  A x;
  B y;
  A z;
  return x;
};

/* => X&, Y&, const X& */
void f3(A** a1, B** b, A a2) {
  A x;
  B y;
  A z;
};

int main() {
  return 0;
}
