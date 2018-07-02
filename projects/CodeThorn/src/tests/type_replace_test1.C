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

typedef double** MULTIPOINTER;
typedef MULTIPOINTER ARRAYOFPOINTER[3][4][5];
typedef ARRAYOFPOINTER*** POINTERTOP;
typedef POINTERTOP TOPLEVEL[3][4][5];

void f6(double& a){
  double     b;
  double*    c;
  double**   d;
  double**** e;
  double f[4];
  double g[5][4];
  double h[6][4][5];
  double** (*i)[7][8];
  TOPLEVEL j;
  const double k = 5;
  double* restrict l;
  volatile double m;
}

int main() {
  return 0;
}
