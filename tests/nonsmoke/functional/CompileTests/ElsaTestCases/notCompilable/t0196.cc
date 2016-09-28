// t0196.cc
// constructor call and 'typename'

class N {      // behaves like a namespace
public:
  class C {
  public:
    C();
  };
};

void f()
{
  // normally, 'typename' would be used only in the body of
  // a template, and in fact I don't know if it is legal to
  // use it outside a template; but I will assume that it is

  typename N::C();        // constructor call to make a temporary
}
