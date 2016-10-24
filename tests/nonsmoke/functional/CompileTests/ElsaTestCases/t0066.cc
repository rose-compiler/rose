// cc.in66
// problem with friends

class Foo {
public:
  friend class Bar;
  int f();
};

class Bar {
public:
  int x;
};

int Foo::f()
{
  Bar *b;
  return b->x;
}
