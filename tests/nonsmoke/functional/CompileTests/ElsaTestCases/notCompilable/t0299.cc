// t0299.cc
// slightly tricky overload resolution

struct A {
  void foo() const;    // line 5
  void foo();          // line 6

  void bar() const;
};

void A::bar() const
{
  foo();       // should pick first one

  // again..
  __testOverload(foo(), 5);
}
