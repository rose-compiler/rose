class A {
};

class B {
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
