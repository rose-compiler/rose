// t0190.cc
// constructor-style cast to an enumerated type that is inside a namespace;
// this triggers construction of its fully qualified name, which used to
// segfault because of an odd bug that pointlessly(?) made a certain scope
// pointer NULL whenever it didn't correspond to a class

namespace N          // not a class scope
{
  enum E { v };      // must use a typedef to name 'E'

  E foo()
  {
    return E(0);     // constructor-style cast
  }
}
