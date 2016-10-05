// t0405.cc
// 'using' declaration pulling a struct tag from a struct

enum B { bad };

struct A {
  struct B {};
  int B();
  int B(int);

  void foo();
};

void A::foo()
{
  struct B b;
  B();
  B(3);
}

struct C : A {
  // pulls in the struct tag and the overloaded functions
  using A::B;

  void bar();
};

void C::bar()
{
  struct B b;
  B();
  B(3);
}

// EOF
