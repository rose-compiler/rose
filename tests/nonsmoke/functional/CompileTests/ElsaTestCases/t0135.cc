// t0135.cc
// experiments with operator<
          
enum E {};

struct A {
  operator short ();
  operator E ();
};

void operator+ (E,E);

void f()
{
  A a;
  E e;

  // candidate: operator<(int,int)
  //   arg1: IC_AMBIGUOUS: can't decide between A->short->int and A->E->int
  //   arg2: SC_INT_PROM
  //
  // candidate: operator<(E,E)
  //   arg1: IC_USER_DEFINED: A->E
  //   arg2: SC_IDENTITY
  //
  // winner by virtue of arg2:
  //   candidate: operator<(E,E)
  a + e;
}



struct B {
  operator E();
};

void g()
{
  A a;
  B b;

  // candidate: operator<(int,int)
  //   arg1: IC_AMBIGUOUS: can't decide between A->short->int and A->E->int
  //   arg2: IC_USER_DEFINED: B->E->int
  //
  // candidate: operator<(E,E)
  //   arg1: IC_USER_DEFINED: A->E
  //   arg2: IC_USER_DEFINED: B->E
  //
  // winner by virtue of arg2:
  //   candidate: operator<(E,E)
  a + b;
}

