// t0204.cc
// a non-dependent name is not visible at the point of
// instantiation, but it is visible within the original
// template definition

namespace N
{
  // name only visible in N
  typedef int anotherint;

  // this template uses 'anotherint'
  template <class T>
  struct C {
    anotherint *b;        // non-dependent name
  };
}

// but where we instantiate, 'anotherint' is not visible
N::C<char> c;
