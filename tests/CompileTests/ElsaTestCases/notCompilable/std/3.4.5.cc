// 3.4.5.cc
// example showing qualified field access

struct A {
  int a;
};
struct B : virtual A {};
struct C : B {};
struct D : B {};
struct E : public C, public D {};
struct F : public A{};

void f()
{
  E e;
  e.B::a = 0;        // OK, only one A::a in E

  F f;
  f.A::a = 1;        // OK, A::a is a member of F
}
