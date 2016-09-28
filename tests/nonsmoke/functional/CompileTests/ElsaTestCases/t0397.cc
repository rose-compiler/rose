// t0397.cc
// testing default args of non-member template functions
// derived from example in 14.7.1p12

// never defined
template<class T>
void f(T x, T y = ydef(T()), T z = zdef(T()));
                
// defined below use
template<class T>
void g(T x, T y = ydef(T()), T z = zdef(T()));
                    
// defined here
template<class T>
void h(T x, T y = ydef(T()), T z = zdef(T())) {}

class  A { };

A zdef(A);

void foo(A a, A b, A c) 
{
  f(a, b, c);             // no default argument instantiation
  f(a, b);                // default argument z = zdef(T()) instantiated
  //ERROR(1): f(a);       // ill-formed; ydef is not declared

  g(a,b,c);
  g(a,b);
  //ERROR(2): g(a);       // error

  h(a,b,c);
  h(a,b);
  //ERROR(3): h(a);       // error
}

template<class T>
void g(T x, T y, T z) {}
