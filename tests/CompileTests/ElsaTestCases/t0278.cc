// t0278.cc
// from oink/test/template_class4_func_fwd2.cc

// test mutual resursion of function members of templatized classes

//  #include <iostream>
//  using namespace std;

template <class T>
struct A {
  T f(T x, T y);                // forward declaration
  T g(T x, T y);                // forward declaration
};

template <class T> 
T A<T>::f(T x, T y) 
{
  if (1) {                      // cqual doesn't see this is determined at compile time
    return x;                   // bad
  } else {
    return g(0, y);
  }
}

template <class T> 
T A<T>::g(T x, T y) 
{
  if (1) {                      // cqual doesn't see this is determined at compile time
    return 17;                  // irrelevant
  } else {
    return f(y, 0);
  }
}

int main() 
{
  int /*$tainted*/ x;
//    int x = 1;
  int x2 = x;                   // launder taintedness
  int /*$untainted*/ y;
//    int y = 2;
  A<int> a;
  y = a.f(0, x2);
//    cout << "y:" << y << endl;
  return 0;
}
