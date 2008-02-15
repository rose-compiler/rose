// t0476.cc
// use of a qualified class type name inside its scope

template <class T>
struct A {
  typedef int INT;
  
  void foo(INT i);
};

template <class T>
void A<T>::foo(A::INT i)    // 'typename' keyword is *not* required here
{}
