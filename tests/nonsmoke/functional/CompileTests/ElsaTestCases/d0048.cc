// d0048.cc:9:1: error: ambiguous overload, no function is better than all others
struct B {};

struct C : B {
  C & operator = (C const &s);
  C & operator = (B const &t);
};

struct D : C {};
