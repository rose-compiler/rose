void f();
struct A {
  A() {f();}
  char x;
  char &operator[] (unsigned int i) {
    return x;
  }
  void g(int i) {
    (*this)[i];
  }
};
