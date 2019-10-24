
// Non-sense code meant to stress Typeforge's type-chain analysis

struct A {
  double * ptr;

  double * bar(int i, double, double *);
};

A & foo_1(A &, double *);

int main(int argc, char ** argv) {
  double arr_1[10];
  double arr_2[10];
  double arr_3[10];

  A a;
  A & r = foo_1(a, &(arr_1[3]));

  a.ptr = r.bar(3, arr_3[9], &(arr_2[3]));

  return 0;
}

A & foo_1(A & a, double * p) {
  a.ptr = p;
  return a;
}

double * A::bar(int i, double v, double * p) {
  ptr[i] = v + *p;
  return (ptr + i);
}

