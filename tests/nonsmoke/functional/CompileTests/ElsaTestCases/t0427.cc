// t0427.cc
// * 'using' declaration that nominates a class member
// * the member is a member template class

// icc and Elsa accept this input
// gcc rejects this input

struct B {
  template <class T>
  struct S {
    T x;
  };
};

// I added the intermediate class 'D1' so that this 'using'
// declaration is required (without it, 'S' does not refer
// to a template).  Without the intermediate layer, gcc
// accepts the code.
struct D1 : B {
  struct S {     // not a template
    int y;
  };
};

struct D2 : D1 {
  using B::S;      //ERRORIFMISSING(1): this is required
  
  int foo()
  {
    S<int> s;
    s.x = 5;
    return s.x;
  }
};
