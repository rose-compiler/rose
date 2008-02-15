// t0499.cc
// ?: on enums const variable

enum E {a,b};
const E aa = a;

void f(E);

void foo()
{
  f(true? aa : a);
}
