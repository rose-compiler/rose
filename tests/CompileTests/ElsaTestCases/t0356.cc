// t0356.cc
// argument-dependent lookup bug reported by Altac Edena
// this is very similar to the example of 3.4.2 para 2

namespace N {
  class B {};
  void g(B);
  int h;
  
  // interestingly, icc does not regard this as an error,
  // though gcc-3 does ...
  //ERROR(3): int k;
}

int k(N::B);

class A {
public:
  void f() {
    N::B b;
    g(b);

    //ERROR(1): g(b,1);

    //ERROR(2): h(b);
    
    k(b);
  }
};
