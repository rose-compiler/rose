// t0495.cc
// function-style cast with ambiguous argument list

template <class T, class U>
struct A {
  static int i;
  typedef int some_type;
  A(int);
};

typedef int x;
typedef int y;

int f()
{
  return A<int,int>::some_type(A<x,y>::i);
}
