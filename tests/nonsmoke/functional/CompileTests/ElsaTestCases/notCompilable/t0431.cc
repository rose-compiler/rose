// t0431.cc
// overload resolution where args are classes in subtype relation

struct Base {};

struct Derived : Base {};

struct FurtherDerived : Derived {};

int foo(Base b);        // line 10
int foo(Derived d);     // line 11
   
//void __testOverload(int,int);

void bar()
{
  FurtherDerived d;
  foo(d);

  __testOverload(foo(d), 11);
  __testOverload(foo(FurtherDerived()), 11);
}


// EOF
