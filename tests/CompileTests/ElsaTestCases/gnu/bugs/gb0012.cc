// gb0012.cc
// "template <>" not required to specialize a member

template <class T>
struct A {
  int f(int);
  int g(int);
};
  

// the standard requires this, but gcc does not
//template <>

int A<int>::f(int)
{
  return 1;
}


// similarly when using a typedef (see also in/t0554.cc, in/t0555.cc)
typedef A<int> A_int;
int A_int::g(int)
{
  return 2;
}
