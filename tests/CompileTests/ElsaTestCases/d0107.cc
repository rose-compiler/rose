//  Assertion failed: MatchTypes: got a type variable on the left, file ../elsa/matchtype.cc line 406

struct A;

template<class T> struct B {};

template<class T> A &operator>> (A &s, B<T> &)
{
  T t;
  s >> t;
}
