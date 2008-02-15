// t0423.cc
// test more dependent expression matching

template <int n>
struct A {};

template <int n, class T>
struct C {
  void f(A<n+1>);              // line 9
  void f(A<n-2>);              // line 10
  void f(A<~n>);               // line 11
  void f(A<sizeof(T)>);        // line 12
  void f(A<n+10>);             // line 13
};

template <int n, class T>
void C<n,T>::f(A<n+1>)
{}

template <int n, class T>
void C<n,T>::f(A<n-2>)
{}

template <int n, class T>
void C<n,T>::f(A<~n>)
{}

template <int n, class T>
void C<n,T>::f(A<sizeof(T)>)
{}

template <int n, class T>
void C<n,T>::f(A<n+10>)
{}


void foo()
{
  C<1,int> c;

  A<2> a2;
  A<-1> am1;
  A<~1> at1;
  A<sizeof(int)> asi;
  A<4> a4;
  A<11> a11;

  __testOverload( c.f(a2),  9);
  __testOverload( c.f(am1), 10);
  __testOverload( c.f(at1), 11);
  __testOverload( c.f(asi), 12);
  __testOverload( c.f(a4),  12);
  __testOverload( c.f(a11), 13);
  
  //ERROR(1): C<-6,int> c2;    // lines 12,13 are same type then
}
