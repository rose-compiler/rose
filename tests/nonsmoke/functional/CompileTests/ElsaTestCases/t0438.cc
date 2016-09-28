// t0438.cc
// DQT type specifier resolution

// dimensions to test:
//   * various levels of qualification
//   - stopping at another DQT vs. full resolution
//   - partial specs vs primaries vs complete specs
//   * nested templates
//   - type parameters vs. non-type parameters
//   - variations like arguments in the wrong order, wrong #, etc.
//   - default args? (no, subsequent tests...)


template <class T>
struct A {
  typedef int INT;
  
  INT a1();
  typename A<T>::INT a2();

  struct B {
    typedef float FLOAT;

    FLOAT b1();
    
    template <class V>
    struct D {
      typedef char CHAR;
      
      CHAR d1();
    };
    
    struct E {
      typedef short SHORT;
      
      SHORT e1();
    };
  };

  template <class U>
  struct C {
    typedef double DOUBLE;

    DOUBLE c1();
    typename A<T>::template C<U>::DOUBLE c2();
    typename ::A<T>::template C<U>::DOUBLE c3();

    template <class W>
    struct F {
      typedef long LONG;

      LONG f1();
    };

    struct G {
      typedef int *INTPTR;

      INTPTR g1();
    };
  };
};


// ---- A ----
template <class T1>
typename A<T1>::INT         A<T1>::a1()
{ return 1; }

template <class T2>
typename A<T2>::INT         A<T2>::a2()
{ return 1; }

// ---- B ----
template <class T2a>
typename A<T2a>::B::FLOAT   A<T2a>::B::b1()
{ return 1.1; }

// ---- D ----
template <class T1>
template <class V1>
typename A<T1>::B::template D<V1>::CHAR    A<T1>::B::D<V1>::d1()
{ return 'a'; }

// ---- E ----
template <class T1>
typename A<T1>::B::E::SHORT                A<T1>::B::E::e1()
{ return 123; }

// ---- C ----
template <class T1>
template <class U1>
typename A<T1>::template C<U1>::DOUBLE     A<T1>::C<U1>::c1()
{ return 1.2; }

template <class T2>
template <class U2>
double                                     A<T2>::C<U2>::c2()
{ return 1.3; }

template <class T3>
template <class U3>
typename ::A<T3>::template C<U3>::DOUBLE   A<T3>::C<U3>::c3()
{ return 1.3; }

// ---- F ----
template <class T1>
template <class U1>
template <class W1>
typename A<T1>::template C<U1>::template F<W1>::LONG    A<T1>::C<U1>::F<W1>::f1()
{ return 123456789; }

// ---- G ----
int x;
template <class T1>
template <class U1>
typename A<T1>::template C<U1>::G::INTPTR               A<T1>::C<U1>::G::g1()
{ return &x; }


void foo()
{
  A<int> a;
  a.a1();
  a.a2();

  A<int>::B b;
  b.b1();

  A<int>::B::D<int> d;
  d.d1();

  A<int>::B::E e;
  e.e1();

  A<int>::C<int> c;
  c.c1();
  c.c2();
  c.c3();
  
  A<int>::C<int>::F<int> f;
  f.f1();

  A<int>::C<int>::G g;
  g.g1();
}


// EOF
