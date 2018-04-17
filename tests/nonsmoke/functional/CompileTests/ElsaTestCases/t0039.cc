// cc.in39
// problem with fwd decls inside classes

class Foo {
  class Bar;
};

class Bar {
  int x;
};
