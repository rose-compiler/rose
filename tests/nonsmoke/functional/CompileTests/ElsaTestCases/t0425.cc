// t0425.cc
// use of self-name with different arguments as qualifier

template <class T, class U>
struct A {
  typename A<int, U>::INT a;
  typedef int INT;
};

template <class U>
struct A<int, U> {
  int x;
  typedef int INT;
};

A<char,char> a;
