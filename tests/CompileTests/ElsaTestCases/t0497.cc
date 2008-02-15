// t0497.cc
// ambiguous argument to dependent base in template class ctor init

template <class T>
struct A {
  typedef int some_type;
  
  A(int,int);
};

template <class T>
struct B : A<T> {
  typedef typename A<T>::some_type my_type;

  B(int i) : A<T>(my_type(), i)
  {}
};

