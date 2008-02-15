// t0221.cc
// simple use of dependent name from pseudo-instantiation

template <class S>
class A {
  typedef S *someType;
  
  enum E { someValue=4 };
};

template <class T>
class B {
  typename A<T>::someType x;         // 'someType' is a dependent name
};

void f()
{
  // so 'b.x' has type 'int*'
  B<int> b;

  __getStandardConversion(b.x, (int*&)0, 0 /*SC_IDENTITY*/);
}


template <class T>
int foo(T *t)
{
  return A<T>::someValue;            // 'someValue' is a dependent name
}

int g()
{
  float *x;
  return foo(x);
}


// EOF
