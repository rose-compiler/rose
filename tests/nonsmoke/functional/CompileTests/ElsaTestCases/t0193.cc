// t0193.cc
// Assertion failed: ctor0, file cc_tcheck.cc line 1488
// from ostream.i, around line 7010

// one problem: a disambiguating error in one place prevents all
// future additions to the environment from taking place

namespace std {
  template < typename _Category >
  struct iterator
  { };

  template < typename _Iterator >
  struct iterator_traits
  { };
}

// it seems that 'using' is a necessary part of this testcase...
using std::iterator_traits;
using std::iterator;

template < typename _Iterator >
class __normal_iterator :
  public iterator
    < typename iterator_traits
                 < _Iterator >
                 ::iterator_category >
{};

template < typename _Iterator >
inline __normal_iterator < _Iterator >
operator+ (int __n,
           const __normal_iterator < _Iterator > &__i)
{
  return __normal_iterator < _Iterator > (__i.base () + __n);
}
