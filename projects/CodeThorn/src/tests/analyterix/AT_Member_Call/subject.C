void func(int) {

}

void func2(int) {

}

void func3(int) {

}

class A {
 public:
  int i;
  void (*fp)(int);

  void (A::*mfp)(int);

  void mf(int) {
    mf2(1);
  }

  void mf2(int) {

  }

  int operator ()(bool) {
    return 1;
  }

  A() : i(1), fp(func), mfp(&A::mf) {

  }

  static void smf() {

  }

  static void smf2() {

  }

};

int main() {
  A a;
  a.mf(0);

  A a2;
  a2.fp(1);

  A a3;
  a3(true);

  A a4;
  A& ar = a4;
  ar(false);

  A a5;
  a5.smf();

  A a6, a7;
  (a6.*a7.mfp)(3);

  A* ap2 = new A();
  ap2->fp(1);

  A* ap3 = new A();
  A* ap4 = new A();
  (ap3->*ap4->mfp)(2);

  A* ap5 = new A();
  ap2->smf();

  A::smf();
  (&A::smf2)();
}
