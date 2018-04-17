// t0357.cc
// trying to demonstrate effect of this-> elaboration 
// for parenthesized functions

class C {
public:
  int foo();
  void bar();
};

void C::bar()
{
  foo();

  (foo)();

  // this syntax causes a segfault with cc_tcheck.cc version 1.475
  ((((foo))))();
}


