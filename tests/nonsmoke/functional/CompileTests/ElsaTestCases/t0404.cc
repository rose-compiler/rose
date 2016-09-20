// t0404.cc
// lookup of mem-initializer-id


struct A {  
  typedef A AA;     // alias for 'A', within scope of 'A'

  A(int x);

  struct B;
};

typedef A AAA;      // alias for 'A' at global scope

struct A::B : A {
  B(int x);
  B(int x, int);
  B(int x, int, int);
};

A::B::B(int x)
  : A(x)
{}

// According to 12.6.2p2, 'AA' should be looked up in this sequence:
//   - A::B (scope of the constructor's class)
//   - global scope (scope containing the constructor's definition)
// and since neither contains it, it should be an error.  However, icc,
// gcc and Elsa all accept it, presumably because they all do the same
// lookup for mem-initializers as they do for identifiers in the body
// of the function.
//
// Actually, what is the "scope of the constructor's class"?  Is it
// the class itself, or the scope that would be nominally used for
// names appearing in class member functions?  If it means the latter,
// then all three parsers are correct, and this is legal.
A::B::B(int x, int)
  : AA(x)
{}

A::B::B(int x, int, int)
  : AAA(x)
{}

// EOF
