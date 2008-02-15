// t0441a.cc
// simplified version of t0441.cc

struct Outer { 
  template <class T>
  struct A;
};

template <class T>
struct Outer::A
{};
