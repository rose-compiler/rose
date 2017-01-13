// t0131.cc
// experiment for structural delta..

int q;

struct Foo {
  int x;
  int y;
  void f();
};

void Foo::f()
{
  x = 6;
  y = x;
}

int g(Foo *f)
{
  f->x = 7;
  f->f();
  if (f->y)
    q = 78;
  return f->y;
}

void h()
{
  while (1) {
    q = 8;
    q = 9;
  }
}
