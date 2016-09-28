// t0223.cc
// return by value an instance of a PseudoInstantiation

template <class T>
struct C {
  C(T *x);    
};

template <class T>
C<T> foo(T *x)
{
  return C<T>(x);
}
