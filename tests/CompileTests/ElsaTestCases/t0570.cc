// t0570.cc
// matching with a DQT    

// see also t0488.cc

template <class T>
struct A {
  typedef int INT;
};

template <class T>
void f(typename A<T>::INT i, A<T> *a);

void foo()
{
  A<int> *a;
  int i;
  f(i, a);
}


      
// this ought to fail template argument deduction, but
// *not* by going into an infinite loop
template <class T>
void g(typename A<T>::INT i, typename A<T>::INT j);

void goo()
{
  int i, j;
  //ERROR(1): g(i, j);     // arg deduction should fail
  
  // but this is ok
  g<int>(i,j);
}
