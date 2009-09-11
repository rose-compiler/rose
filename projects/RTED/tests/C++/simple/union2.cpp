class Test {
  public:
  class UnionA {
  public:
    double *ptrA;
    int valueA;
  } unionA;

  Test() {}
  int i;
};


int main() {
  Test t;
  t.unionA.ptrA=new double;
  double* d = t.unionA.ptrA;
  *d = 25;
  t.unionA.valueA=23;
}
