// t0450.cc
// function-try-block for non-ctors

int nonmember()
try {
  return 2;
}
catch (int) {
  return 3;
}

struct A {
  int x;
  static int y;

  A();
  ~A();
  operator int ();
  void f();
};

A::A()
try
  : x(4)
{
  y++;
}
catch (int) {}

A::~A()
try
{
  y++;
}
catch (int) {}

A::operator int()
try
{
  y++;
  return 5;
}
catch (int) {
  return 6;
}

void A::f()
try
{
  y++;
}
catch (int) {}
