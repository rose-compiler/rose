class A {
};

class B {
};

void f0(double a1, long double b, float a2) {
  double x;
  long double y;
  float z;
};

/* => X, Y, X */
A f1(A a1, B b, A a2) {
  A x;
  B y;
  A z;
  return z;
};

/* => X, _, const X* */
A f2(A* a1, B b, A** a2) {
  A* x;
  B y;
  A z;
  return z;
};

/* => X&, Y*, X& */
void f3(A a1, B b, A a2) {
  A x;
  B y;
  A z;
};

/* => const X&, Y&, const X& */
void f4(A a1, B b, A a2) {
  A x;
  B y;
  A z;
};

/* => const X&, Y&, const X& */
void f5(A** a1, B** b, A** a2) {
  A** x;
  B** y;
  A z;
};

int main() {
  return 0;
}
