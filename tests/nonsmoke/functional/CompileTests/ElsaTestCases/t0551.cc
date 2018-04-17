// t0551.cc
// problem where we remember as non-dependent a particular
// template specialization, but we should have remembered
// the template itself

template <class T> 
void f(T *p);

struct C {
  template <class S>
  void m(S *);
};

template <class T>
struct A {
  struct B {};

  void foo()
  {
    B *b = 0;
    f(b);
    
    C c;
    c.m(b);
  }

  void bar();
};

void callfoo()
{
  A<char> a;
  a.foo();
}


void g(A<int>::B *b);
void g(A<char>::B *b);    //ERRORIFMISSING(1): this is the right one
void g(A<float>::B *b);

template <class T>
void A<T>::bar()
{
  // try this..
  B *b = 0;
  g(b);
}

void callbar()
{
  A<char> a;
  a.bar();
}

