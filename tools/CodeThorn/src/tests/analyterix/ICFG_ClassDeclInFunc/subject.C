void f() {
  class B {

  };
}

void f2() {
  int i;
  i = 1;
  class C {

  };
}

int main() {
  class A { 
   public:
    void mf() {
      int i;
      i = 2;
    }
    int i;
  };
  A a;
  a.mf();

  f();

  f2();
}

