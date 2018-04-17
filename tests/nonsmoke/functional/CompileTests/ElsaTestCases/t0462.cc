// t0462.cc
// testing some of 14.8.2.1, template argument deduction

// if this primary is ever instantiated, it will generate an error;
// the idea is I will define specializations for each case that is
// *supposed* to be called
template <class T>
void f1(T)
{
  typename T::foo x;
}

template <>
void f1( int* )
{}

template <>
void f1( int (*)() )
{}

template <>
void f1( int )
{}


// test that the 'const' will be ignored during deduction
template <class T>
void f2(T const)
{
  typename T::foo x;
}

template <>
void f2(int const)      // i.e., f2<int>
{}

// just check that explicit specification of template args
template <>
void f2<float>(float const)
{}


// try with a parameter type that is a reference
template <class T>
void f3(T const &)
{
  typename T::foo x;
}

template <>
void f3(int const &)
{}


template <class T>
void f4(T volatile &)
{
  typename T::foo x;
}

template <>
void f4(int volatile &)
{}


template <class T>
void f5(T const *)
{
  typename T::foo x;
}

template <>
void f5(int const *)
{}


template <class T>
void f6(T volatile * const *)
{
  typename T::foo x;
}

template <>
void f6(int volatile * const *)
{}


struct A { int x; };

template <class T>
void f7(T const A::*)
{
  typename T::foo x;
}

template <>
void f7(int const A::*)
{}


template <class T>
class B {};

class C : public B<int> {};

template <class T>
class D : public B<T> {};

template <class T>
void f8(B<T>)
{
  typename T::foo x;
}

template <>
void f8(B<int>)
{}

template <class T>
void f9(B<T>*)
{
  typename T::foo x;
}

template <>
void f9(B<int>*)
{}



int main()
{
  // 14.8.2.1

  // para 2

  // bullet 1
  int a[5];
  f1(a);         // call f1< int* >

  // bullet 2
  int (*b)() = 0;
  f1(*b);        // call f1< int (*)() >

  // bullet 3
  int const c = 0;
  f1(c);         // call f1< int >, *not* f1< int const >
  f2(c);         // call f2< int >

  // para 2, second to last sentence
  int d = 0;
  f2(d);         // call f2< int >

  int volatile e = 0;
  f2(e);         // call f2< int >

  // para 3

  // bullet 1
  f3(d);         // call f3< int >, ok that param has extra qualifiers
  f4(d);         // call f4< int >

  // bullet 2
  int *g = 0;
  f5(g);         // call f5< int >, converting int* to int const *

  int **h = 0;
  f6(h);         // call f6< int >, converting int** to int volatile * const *

  int A::*i = 0;
  f7(i);         // call f7< int >, converting int A::* to int const A::*
  
  // bullet 3
  C j;
  f8(j);         // call f8<int>, converting C to B<int>
  f9(&j);        // similar, but converting pointers

  D<int> k;
  f8(k);         // call f8<int>, converting D<int> to B<int>
  f9(&k);        // similar, but converting pointers
  
  // The final part of para 3 mentions that the bullets are considered
  // only if type deduction would otherwise fail, and that multiple
  // possible deduced parameter types are cause for failure, but I do
  // not know how to write a test case that would expose the effect of
  // either rule.

  return 0;
}
