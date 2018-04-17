// cc.in53
// problem with names of enums from within context of an inner class

namespace N {
  class Foo {
  public:
    enum Enum1 { E1_VAL = 3 };

    class Another {
    public:
      enum Enum2 { E2_VAL = 4 };
    };

    class Bar {
    public:
      int f();
      int g();
    };

  };

  int Foo::Bar::f()
  {
    int x;

    x = E1_VAL;      // ok
    //ERROR(1): x = E2_VAL;      // can't look into Foo::Another without qualifier

    return x;
  }
}

// three levels deep nesting of qualifiers; this in essence
// tests that the ArrayStackEmbed stuff works (deep in my
// implementation) even when we have to put some of the
// scope pointers on the heap
int N::Foo::Bar::g()
{
  return E1_VAL;
}
