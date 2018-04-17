// t0573.cc
// overload resolution with templatized candidates confuses strict mode

template <class T>
struct A {};

struct B {};

template <class T>
struct C {
  void f(A<T>*);
  void f(B*);

  void foo(B *b)
  {
    f(b);
  }


  void g(typename T::CHAR);
  void g(typename T::SHORT);

  void goo(int i)
  {
    g(i);
  }
};
