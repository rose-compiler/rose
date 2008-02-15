// definition of a function without 'const' on a basic type

// originally found in package audacity_1.2.3-1

// a.ii:7:1: error: the name `S1::S1' is overloaded, but the type `()(int const number)' doesn't match any of the 2 declared overloaded instances

// ERR-MATCH: the name `.*?' is overloaded, but the type `.*?' doesn't match

struct S1 {
S1(int number);
};

S1::S1(int const number) {
}


struct S2 {
  void f(int number);
  void f();
};

void S2::f(int const number) {
}
