// t0516.cc
// try to exercise applyArgumentMap
// extended version of t0515.cc

template <class, class, int>
struct A {
  typedef int Foo;
};

template <class, class, int>
struct A2 {
  // no 'Foo'
};

template <class T>
struct B {
  static int const bar = 0;
  typedef int Zoo;
  class C {
    static int const zerg = 0;
  };
};

template <class T>
struct B2 {
  // no 'bar'
};


double sqrt(double x);

template <class T>
typename A<int, int[4], ::B<T>::bar>::Foo sqrt(T x = 0);

double acosh(const double &x)
{
  // template's type is ok, but non-template is more specific
  return sqrt(x);
}


int f(int x);

template <class T>
typename A<int, int*, B2<T>::bar>::Foo f(T x);

int call_f(int x)
{
  // template type cannot be formed, no 'bar' in B2
  return f(x);
}


int g(int x);

template <class T>
typename A2<int, int&, B<T>::bar>::Foo g(T x);

int call_g(int x)
{
  // template type cannot be formed
  return g(x);
}



struct E {
  int x;
};

int h(int x);

int const zero = 0;

template <class T>
typename A<int E::*, int T::*, ::zero>::Foo h(T x);

int call_h(E e)
{
  // template type is ok
  return h(e);
}

int call2_h(int x)
{
  // template type is not ok
  return h(x);
}



struct F {
  typedef int Func(int);
};

int j(int x);

template <class T>
typename A<typename T::Func const, int, 0>::Foo j(T x);

int call_j(F f)
{
  // template type not ok because it attempts to cv-qualify a function
  // specified via DQT
  //ERROR(1): return j(f);
}

int call2_j(int x)
{
  // template type not ok because it wants int::Func
  j(x);
}


template <class T>
T const k();

void call_k()
{
  // attempts to cv-qualify a function specified via TypeVariable
  //ERROR(2): k<F::Func>();
}


// NOTE: "<:" is a token (alternate keyword for "["), so the
// space after "A<" is necessary ...

template <int n, class T>
A< ::B<T>,int,n> m();

void call_m()
{
  m<3, int>();
}



int p(int);

template <class T>
A<int,int, B<T>::Nonexist::bar> p(T);

void call_p()
{
  // template type cannot be formed because no 'Nonexist' in B<int>
  p(3);
}


int q(int);

template <class T>
A<int,int, B<T>::Zoo::gorf> q(T);

void call_q()
{
  // template type cannot be formed because 'B<int>::Zoo' not a class
  q(3);
}


int r(int);

template <class T>
A<int,int, B<T>::C::zerg> r(T,T);

void call_r()
{
  // template type is ok
  r(3,4);
}


int s(int);

template <class T>
A<int,int, B<T>::template C<int>::zerg > s(T);

void call_s()
{
  // template type is not ok b/c C isn't a template
  s(3);
}


template <class T>
struct G;

int t(int);

template <class T>
A<int,int, G<T>::zerg > t(T);

void call_t()
{
  // template type is not ok b/c G<int> is not defined
  t(3);
}




