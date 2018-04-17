// t0098.cc
// overloaded fn, one is virtual another is static

class A {
  virtual int foo(int);
  static int foo(int,int);
};

int A::foo(int,int)
{}
