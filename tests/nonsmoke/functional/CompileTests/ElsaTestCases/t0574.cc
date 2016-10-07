// t0574.cc
// trouble disambiguating a conversion

struct A {
  A(A const &);    // would be implicitly defined even if not explicitly present
  A(int *);
};

class B {
public:
  operator A();
private:
  // GCC and ICC accept the "a2 = b" line with this entry
  // private, so they must be selecting the other one
  operator int*();
};


void foo(B &b)
{
  // GCC and ICC agree that this is ambiguous
  //ERROR(1): A a1(b);

  // but this is not; see 8.5p14
  A a2 = b;
}



// this is related in that the same resolution mechanism is used,
// though no conversion operator is used; I am using this to test
// that the proper AST nodes are inserted as a transformation
// (this test comes from Oink, which wants to see that 'x' can
// flow into 'z')
struct C {
  C(int y) {
    int /*$untainted*/ z = y;
  }
  operator bool () { return true; }
};

int main() {
  int /*$tainted*/ x;
  if (C a = x) {}
}
