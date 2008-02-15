// t0394.cc
// simpler version of t0393.cc

enum E { foo=3 };

template <int n>
struct S {};

void foof()
{
  S<foo> s;
}
