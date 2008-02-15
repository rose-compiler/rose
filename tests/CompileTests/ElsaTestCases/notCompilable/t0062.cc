// cc.in62
// ambiguity from nsLDAPProtocolModule.i

typedef unsigned short u_short;

class Foo2 {};

class Foo {
public:
  // ambiguous, but resolution is to consider it a ctor decl
  Foo( u_short );

  // gcc issues an unusual warning, but appears to accept this as a
  // declaration of member "u_short"
  //Foo2( u_short );

  // make sure this works
  Foo *p1;
  Foo *p2, *p3;

  // unambiguous
  Foo(u_short, u_short);

  // parse error now
  //*ERROR(1):*/ Foo x;

  // should (eventually) provoke error about incomplete type
  //Foo *y, z;

  // but this has to still work!
  typedef Foo my_type;

  // so must this
  static Foo an_instance;

  // what about this?  parsed as a declaration of a static variable,
  // because "static" isn't among the modifiers allowed to preceed
  // a ctor declaration
  static Foo(u_short);
};
