// cc.in17
// some constructors with member-init and try-block


class Foo {
public:
  int x, y;
  
  Foo() : x(3), y(4) {}
  Foo(int z);
  Foo(double d);
};

Foo::Foo(int z) : x( (z) & (z) ), y(4) {}

//ERROR(1): Foo::Foo(Foo *f) {}

Foo::Foo(double d)
try
{
  x;
}
catch (int &x)
{
  6;
  throw;
}
