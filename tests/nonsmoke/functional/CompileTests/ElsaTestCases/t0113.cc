// t0113.cc
// disambiguate dependent scope qual

template <class T>
class C {};

template <class T>
int f()
{
  typedef C<T> CT;
  return CT::func1(T::func2(), T::func3());   // ambiguous: func or constructor
}

