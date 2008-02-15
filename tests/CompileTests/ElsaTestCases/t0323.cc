// t0323.cc
// trigger class instantiation during second pass tcheck of another class

template <class T>
struct A {
  A(int*);
};

struct B {
  A<int*> get() {
    int *x;
    return x;
  }
};
