// t0194.cc
// namespaces: say "using Foo" when Foo is a class template

namespace N {
  template <class T>
  struct C {
    T *p;
  };
};
  
using N::C;

// I should now be able to say C<int>, etc.  It's not clear exactly
// how the standard implies this; none of its examples explicitly
// show this usage.  Perhaps if I tracked down the precise meaning
// of the standard's usage of "name" I might find it; but for now
// I'm just relying on gcc's acceptance of the syntax.

int *f()
{
  C<int> c;
  return c.p;
}

// cppstd 7.3.3 para 5 disallows this
//ERROR(1): using N::C<float>;

