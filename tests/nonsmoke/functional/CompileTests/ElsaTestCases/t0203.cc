// t0203.cc
// tricky use of typename and lookups

namespace M
{
  template <class T>
  struct B {
    typedef int myint;
  };
}

namespace N
{
  using M::B;

  template <class T>
  struct C {
    typename B<T>::myint a;      // dependent name
  };
}

N::C<char> c;
