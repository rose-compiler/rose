// t0565.cc
// unhandled MemberDeclaration ambiguity

int f(int);

typedef int INT;

struct A {
  friend int ::f(int);

  // every C++ parser I've tried binds the "::" tightly, like "INT::f"
  //friend INT ::f(int);

  friend INT (::f)(int);

  // this line has the MemberDeclaration problem
  friend INT (::f(int));
};
