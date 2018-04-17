// t0496.cc
// function-style cast with hidden dependent dest type

struct Q {
  Q(int,int);
  operator int();
};

template <class T>
struct A {
  typedef Q some_type;
};

template <class T>
struct B {
  typedef typename A<T>::some_type another_type;
  int f(int i);
};

template <class T>
int B<T>::f(int i)
{
  return another_type(i,i);
}

void foo()
{
  B<int> b;
  b.f(2);
}
