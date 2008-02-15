// t0274.cc
// namespace names vs. template param names
// variation on cppstd 14.6.1p6.cc

namespace N {
  // all uses of "C" below will cause an error if
  // lookup erroneously finds this one
  int C;

  template <class T> 
  class B {
    void f(T);
  };
}

template <class C>
void N::B<C>::f(C param)
{
  C b;                          // C is the template parameter, not N::C
}

template class N::B<int>;
