// t0017.c
// tricky use of a name in a way that looks somewhat like
// a constructor declaration

typedef struct Foo *Foo;

struct Foo {
  Foo x;
  int q;
};
