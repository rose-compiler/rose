// t0447.cc
// invoke static methods, in proximity to a DQT


template <class T>
struct A {
};

template <>
struct A<int> {
  static int f();
};

template <class T>
void foo(T *t)
{
  A<typename T::INT>::f();
}

struct B {
  typedef int INT;
};

void bar()
{
  B *bp = 0;
  foo(bp);
}

