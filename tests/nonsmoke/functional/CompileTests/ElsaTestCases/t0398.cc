// t0398.cc
// testing default args of member functions of a template class
// derived from example in 14.7.1p12

template<class T>
struct S {
  S();

  // constructor (never defined)
  S(int, T x, T y = ydef(T()), T z = zdef(T()));

  // never defined
  void f(T x, T y = ydef(T()), T z = zdef(T()));

  // defined out of line below use
  void g(T x, T y = ydef(T()), T z = zdef(T()));

  // defined out of line above use
  void g2(T x, T y = ydef(T()), T z = zdef(T()));

  // defined inline
  void h(T x, T y = ydef(T()), T z = zdef(T())) {}

  // static (not defined)
  static void i(T x, T y = ydef(T()), T z = zdef(T()));
};

template<class T>
void S<T>::g2(T x, T y, T z) {}

class  A { };

A zdef(A);

void foo(A a, A b, A c)
{
  S<A> s;

  s.f(a, b, c);             // no default argument instantiation
  s.f(a, b);                // default argument z = zdef(T()) instantiated
  //ERROR(1): s.f(a);       // ill-formed; ydef is not declared

  s.g(a,b,c);
  s.g(a,b);
  //ERROR(2): s.g(a);       // error

  s.g2(a,b,c);
  s.g2(a,b);
  //ERROR(3): s.g2(a);      // error

  s.h(a,b,c);
  s.h(a,b);
  //ERROR(4): s.h(a);       // error

  s.i(a,b,c);
  s.i(a,b);
  //ERROR(5): s.i(a);       // error

  // invoke constructor via IN_ctor
  S<A> s1(3, a,b,c);
  S<A> s2(3, a,b);
  //ERROR(6): S<A> s3(3, a);
  
  // invoke constructor via E_constructor
  0, S<A>(3, a,b,c);
  0, S<A>(3, a,b);
  //ERROR(7): 0, S<A>(3, a);

  // invoke constructor via E_new
  new S<A>(3, a,b,c);
  new S<A>(3, a,b);
  //ERROR(8): new S<A>(3, a);
  
  // there is also invocation via member initializer, but that should
  // be covered if the above cases are since they all use the same code
}

template<class T>
void S<T>::g(T x, T y, T z) {}
