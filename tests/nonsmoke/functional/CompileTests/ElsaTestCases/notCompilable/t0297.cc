// t0297.cc
// overloaded operator()

struct A {
  int operator() ();            // line 5
  int operator() (int);         // line 6
  int operator() (int,int);     // line 7
};

struct B {};

void foo()
{
  A a;
  B b;

  __testOverload( a(),     5);
  __testOverload( a(3),    6);
  __testOverload( a(4,5),  7);

  //ERROR(1): a(6,7,8);       // doesn't match any overloaded instance

  //ERROR(2): b(5,6,7);       // no operator() defined
}


