// cc.in51
// accessing object fields with qualified names

class A {
public:
  void f();
};

class B {
public:
  void f();
};

class C : public A, public B {};

int main()
{
  C c;

  //ERROR(1):*/ c.f();      // ambiguous

  c.A::f();   // ok
  c.B::f();   // ok

  return 0;
}
