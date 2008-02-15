// t0384.cc
// global function is friend

// experimenting with how :: is parsed in relation to identifiers

// icc wants this
void f();
int f2();

class C {};

C g();

class C g2();


typedef int INT;

INT h();

INT *j();


enum E { blah };

enum E k();


namespace N {
  class Foo {};
};


struct A {
  friend void ::f();
  friend int ::f2();

  // TODO: make Elsa reject the illegal forms below

  // illegal
  //friend C ::g();

  // illegal
  //friend class C ::g2();

  // illegal
  //friend INT ::h();

  friend INT* ::j();

  // illegal
  //friend enum E ::k();
  
  friend class N::Foo;
};

class QCollection
{
  typedef void *Item;
  void inSort (QCollection::Item);
};
