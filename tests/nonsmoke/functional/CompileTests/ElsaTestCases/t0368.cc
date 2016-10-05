// t0368.cc
// from Altac Edena
// error: during function template argument deduction: argument 1 `class B &' is incompatible with parameter, `A<T>'

template <class T>
class A {};

class B : public A<int> {};

typedef A<int> C;

template <class T>
void f(A<T>);

void g()
{
  A<int> a;
  f(a); // <== OK
  C c;
  f(c); // <== OK
  B b;
  f(b); // <== KO
}


template <class T>
void f2(A<T> &);

template <class T>
void f3(A<T> *);

void h()
{
  B b;
  f2(b);
  f3(&b);
}
