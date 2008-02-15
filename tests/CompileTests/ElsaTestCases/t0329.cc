// t0329.cc
// access a class member using a typedef in a qualifier

struct Other {
  int foo();
};

struct A {
  int foo();
  typedef Other A3;
  typedef A Myself;
};

typedef A A2;
typedef A A3;

void f()
{
  A a;                          

  // use the class name directly
  a.A::foo();
                   
  // use a global typedef alias
  a.A2::foo();

  // use a member alias
  a.Myself::foo();

  // error: try to access a non-member
  //ERROR(1): a.Other::foo();
  //ERROR(2): a.::Other::foo();

  // error: lookup in class != lookup in global
  //--nerfed--ERROR(3): a.A3::foo();
  // nerfed b/c it doesn't work; see t0330.cc

  // only use lookup in global
  a.::A3::foo();
}
