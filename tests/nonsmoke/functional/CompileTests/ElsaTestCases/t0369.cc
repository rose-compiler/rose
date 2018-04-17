// t0369.cc
// from Altac Edena
// error: there is no function called `f'

struct A {
  A(int b, int a = f());

  void foo(int x, int y = f());

  int i_am_retarded()
  {
    return bar(2,3 /*implicit*/);
  }

  int bar(int x, int y, int z = f())
  {
    return x+y+z;
  }

  void h()
  {
    foo(3,4);
    foo(5 /*A::f() implicit*/);

    bar(3,4,5);
    bar(6,7 /*A::f() implicit*/);
  }

  static int f();
};

void g()
{
  A a(1,2);
  A a2(1 /*implicit*/);

  a.foo(3,4);
  a.foo(5 /*A::f() passed implicitly*/);

  a.bar(3,4,5);
  a.bar(6,7 /*A::f() implicit*/);
}


// ------ try with a templatized class ------
template <class T>
struct C {
  int foo(int x, int y = 3);
  int bar(int x, int y = 3) { return 1; }
};

void h()
{
  C<int> c;

  c.foo(2,3);
  c.foo(2 /*implicit*/);

  c.bar(2,3);
  c.bar(2 /*implicit*/);
}
