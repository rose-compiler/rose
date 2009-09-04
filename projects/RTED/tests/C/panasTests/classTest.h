

class TestClass
{
  // this is the declaration of the pointers
private:
  double *ptrA;
public:
  TestClass();
  ~TestClass();
  void runA();
private:
  long double ptrB;
  double foo() {return *ptrA;}
  char ptrC;
 public:
  static int ptrD;
  static double foo3() {return ptrD;}
private:
  double foo2() {return *ptrA;}
  double *ptrE;
};

class Test3;

class Test2 {
  Test2(){};
  ~Test2(){};
  int a;
  Test3* test3;
};

class Test3 {
 private:
  int a;
};


class TestClass2
{
  // this is the declaration of the pointers
private:
  double *ptrA;
public:
  TestClass2(){};
  ~TestClass2(){};
};
