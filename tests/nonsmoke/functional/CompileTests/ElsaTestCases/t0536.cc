// t0536.cc
// typedef of a function pointer in a class

struct A {
  // ordinary pointer to member
  typedef void Func();

  struct B {
    Func *f;
  };
};

void foo(A::B *p)
{
  p->f();
}
