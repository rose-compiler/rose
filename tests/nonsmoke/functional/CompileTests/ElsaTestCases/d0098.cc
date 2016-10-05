// can we find an "explicitly enum" enum in a superclass?

struct A {
  enum E {
    EGY,
    KETTO,
  };
};

struct B : A {
  void f() {
    enum E a;
  }
};
