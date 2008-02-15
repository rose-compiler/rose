// t0090.cc
// more problems with static member defns

class A {
  static int foo(int);
  static int foo(int, int);
};

int A::foo(int)
{}
