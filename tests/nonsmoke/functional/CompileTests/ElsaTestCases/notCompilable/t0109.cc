// t0109.cc
// more template tests

template <int n
  //ERROR(1): , int n     // duplicate param
  >
class Foo {};

class Bar {};

class Zoo;     // incomplete class

void f()
{
  // <3> is a template argument
  new Foo< 3 > +4 > +5;

  // <3> is not a template argument
  new Bar< 3 > +4 > +5;
  
  // should be an error b/c Zoo is incomplete
  //ERROR(2): new Zoo;
}
