// t0284.cc
// out-of-line class definition

namespace N {
  typedef int Int;
  class A;
}

class N::A {
  Int i;          // refer back to N, even out of line
};

