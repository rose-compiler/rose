// t0155.cc
// example from John Kodumal of a ctorVar not being set?
// now fixed

int z;

class C {
  int *y;
  public:
  C() { y = &z; }
  virtual int *foo() { return y; }
};

int main() {
  C *c = new C;
  int *x;
  x = c->foo();
}
