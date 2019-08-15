class A {
};

class B {
};

void f0(double a1, long double b, float a2) {
  double x;
  long double y;
  float z;
};

A f1(A a1, B b, A a2) {
  A x;
  B y;
  A z;
  return z;
};

A f2(A* a1, B b, A** a2) {
  A* x;
  B y;
  A z;
  return z;
};

void f3(A a1, B b, A a2) {
  A x;
  B y;
  A z;
};

void f4(A a1, B b, A a2) {
  A x;
  B y;
  A z;
};

void f5(A** a1, B** b, A** a2) {
  A** x;
  B** y;
  A z;
};

int main() {
  return 0;
}
