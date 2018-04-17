// t0184.cc
// template member out-of-line definition

template <class S>
struct C {
  template <class T>
  int foo(T *t);
};

template <class S>
template <class T>
int C<S>::foo(T *t)
{
  return 5 + sizeof(t);
}
