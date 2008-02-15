// cc.in70
// inherit from a typedef'd name!

class Foo {};

typedef Foo Bar;

class Baz : public Bar {
  //...
};

