// t0485.cc
// explicit specialization of member template

struct A {
  template <class T>
  T f(T);
};

template <class T>
T A::f(T)
{
  return 1;
}

template <>
int A::f(int);

template <>
int A::f<int>(int);

template <>
int A::f<int>(int)
{
  return 2;
}

void foo()
{
  A a;
  int x = 0;
  float y = 0;
  
  a.f(x);      // uses specialization
  a.f(y);      // uses primary
}
